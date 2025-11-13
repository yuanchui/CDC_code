#include "usb_cmd.h"
#include "cmd_queue.h"
#include "usbd_cdc_if.h"
#include "pcap04_if.h"
#include "pcap04_register.h"
#include "mux_cd4067.h"

/*
 * 模块说明：
 * 该文件负责 USB CDC 指令链路的解析与调度，核心目标是保证所有 USB 收发、
 * 指令处理、SPI 外设访问都在中断或定时器回调内以非阻塞方式完成。
 * - usb_cmd_rx_push() 在 USB 中断中被调用，将数据写入软件双缓冲。
 * - usb_cmd_tick_5ms() 由 TIM1 5 ms 定时回调触发，完成解析、执行和回包。
 * - 指令支持优先级与 “&&” 组合，队列内会维持顺序和优先级。
 * - 针对 PCAP04 寄存器操作，结合 pcap04_register_def.c 生成详细回包说明。
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX_BUFFER_COUNT      2U
#define RESP_QUEUE_CAP       6U
#define RESP_MSG_MAX_LEN     160U

/* RX double buffer and flags */
static uint8_t rx_buf[RX_BUFFER_COUNT][64];
static volatile uint16_t rx_len[RX_BUFFER_COUNT] = {0U, 0U};
static volatile uint8_t rx_write_idx = 0U;
static volatile uint8_t rx_read_idx = 1U;
static volatile uint8_t rx_pending = 0U;

/* CDC pause flag with simple depth counter */
static volatile uint8_t cdc_paused = 0U;
static volatile uint8_t cdc_pause_depth = 0U;

/* Response queue */
static char resp_msgs[RESP_QUEUE_CAP][RESP_MSG_MAX_LEN];
static uint8_t resp_head = 0U;
static uint8_t resp_tail = 0U;
static uint8_t resp_count = 0U;

/* Helpers ------------------------------------------------------------------ */
static inline const char* skip_spaces(const char* s)
{
	while (s != NULL && *s != '\0' && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n'))
	{
		s++;
	}
	return s;
}

static void rtrim(char* s)
{
	if (s == NULL) return;
	int len = (int)strlen(s);
	while (len > 0)
	{
		char c = s[len - 1];
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			s[len - 1] = '\0';
			len--;
		}
		else
		{
			break;
		}
	}
}

static void ltrim(char* s)
{
	if (s == NULL) return;
	size_t len = strlen(s);
	size_t idx = 0U;
	while (idx < len && (s[idx] == ' ' || s[idx] == '\t' || s[idx] == '\r' || s[idx] == '\n'))
	{
		idx++;
	}
	if (idx > 0U)
	{
		memmove(s, s + idx, len - idx + 1U);
	}
}

static void trim(char* s)
{
	ltrim(s);
	rtrim(s);
}

static cmd_priority_t extract_priority(char* text)
{
	trim(text);
	if (text[0] == '[' && text[2] == ']' && text[1] != '\0')
	{
		char tag = (char)toupper((unsigned char)text[1]);
		size_t len = strlen(text);
		/* Remove prefix */
		memmove(text, text + 3, len - 2);
		trim(text);
		if (tag == 'H')
		{
			return CMD_PRIO_HIGH;
		}
	}
	return CMD_PRIO_LOW;
}

static uint8_t queue_response_fmt(const char* status, const char* tag, const char* fmt, ...)
{
	if (resp_count >= RESP_QUEUE_CAP)
	{
		return 0U;
	}

	char* slot = resp_msgs[resp_tail];
	int written = snprintf(slot, RESP_MSG_MAX_LEN, "[%s][%s] ", status, tag != NULL ? tag : "");
	if (written < 0)
	{
		return 0U;
	}

	size_t offset = (size_t)written;
	if (offset >= RESP_MSG_MAX_LEN)
	{
		offset = RESP_MSG_MAX_LEN - 1U;
	}

	va_list args;
	va_start(args, fmt);
	int appended = vsnprintf(&slot[offset], RESP_MSG_MAX_LEN - offset, fmt, args);
	va_end(args);

	if (appended < 0)
	{
		slot[offset] = '\0';
	}

	/* Ensure message ends with CRLF */
	size_t len = strlen(slot);
	if (len + 2U < RESP_MSG_MAX_LEN)
	{
		slot[len++] = '\r';
		slot[len++] = '\n';
		slot[len] = '\0';
	}

	resp_tail = (uint8_t)((resp_tail + 1U) % RESP_QUEUE_CAP);
	resp_count++;
	return 1U;
}

static void queue_ok(const char* tag, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buffer[RESP_MSG_MAX_LEN];
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	(void)queue_response_fmt("OK", tag != NULL ? tag : "CMD", "%s", buffer);
}

static void queue_err(const char* tag, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char buffer[RESP_MSG_MAX_LEN];
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	(void)queue_response_fmt("ERR", tag != NULL ? tag : "CMD", "%s", buffer);
}

static void pump_tx(void)
{
	if (resp_count == 0U)
	{
		return;
	}
	if (CDC_TxBusy_FS() != USBD_OK)
	{
		return;
	}

	const char* msg = resp_msgs[resp_head];
	uint16_t len = (uint16_t)strlen(msg);
	if (len == 0U)
	{
		resp_head = (uint8_t)((resp_head + 1U) % RESP_QUEUE_CAP);
		resp_count--;
		return;
	}

	if (CDC_FS_Send((const uint8_t*)msg, len) == USBD_OK)
	{
		resp_head = (uint8_t)((resp_head + 1U) % RESP_QUEUE_CAP);
		resp_count--;
	}
}

static const char* read_token(const char* src, char* dst, size_t dst_len)
{
	const char* p = skip_spaces(src);
	size_t idx = 0U;
	while (p != NULL && *p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && idx + 1U < dst_len)
	{
		dst[idx++] = (char)*p;
		p++;
	}
	dst[idx] = '\0';
	return p;
}

static int parse_u32(const char* token, uint32_t* out)
{
	if (token == NULL || out == NULL || *token == '\0')
	{
		return 0;
	}
	char* endptr = NULL;
	uint32_t value = (uint32_t)strtoul(token, &endptr, 0);
	if (endptr == token)
	{
		return 0;
	}
	*out = value;
	return 1;
}

static const PCAP04_Register_t* get_register_def(uint8_t addr)
{
	return PCAP04_GetRegisterDefinition(addr);
}

static const PCAP04_RegBit_t* find_bit_def(const PCAP04_Register_t* reg, const char* name)
{
	if (reg == NULL || name == NULL)
	{
		return NULL;
	}
	for (uint8_t i = 0U; i < reg->bits_count; i++)
	{
		const PCAP04_RegBit_t* bit = &reg->bits[i];
		if (strcmp(bit->name, name) == 0)
		{
			return bit;
		}
	}
	return NULL;
}

static const PCAP04_BitOption_t* find_bit_option(const PCAP04_RegBit_t* bit, uint8_t field_value)
{
	if (bit == NULL || bit->options == NULL)
	{
		return NULL;
	}
	uint8_t encoded = (uint8_t)(field_value << bit->bit_start);
	for (uint8_t i = 0U; i < bit->options_count; i++)
	{
		if (bit->options[i].value == encoded)
		{
			return &bit->options[i];
		}
	}
	return NULL;
}

static void queue_reg_bit_detail(uint8_t addr, const PCAP04_RegBit_t* bit, uint8_t field_value)
{
	if (bit == NULL)
	{
		return;
	}
	const PCAP04_BitOption_t* option = find_bit_option(bit, field_value);
	if (option != NULL)
	{
		queue_response_fmt("INFO", "REG",
		                   "0x%02X [%u:%u] %s=%u (%s - %s)",
		                   addr,
		                   (unsigned)bit->bit_start,
		                   (unsigned)bit->bit_end,
		                   bit->name,
		                   (unsigned)field_value,
		                   option->name,
		                   option->description);
	}
	else
	{
		queue_response_fmt("INFO", "REG",
		                   "0x%02X [%u:%u] %s=%u (%s)",
		                   addr,
		                   (unsigned)bit->bit_start,
		                   (unsigned)bit->bit_end,
		                   bit->name,
		                   (unsigned)field_value,
		                   bit->description);
	}
}

static void queue_reg_summary(uint8_t addr)
{
	const PCAP04_Register_t* def = get_register_def(addr);
	const uint8_t* mirror = pcap04_if_reg_snapshot();
	uint8_t value = (mirror != NULL) ? mirror[addr] : pcap04_if_reg_byte(addr);

	if (def == NULL)
	{
		queue_response_fmt("INFO", "REG", "0x%02X = 0x%02X (未定义)", addr, value);
		return;
	}

	queue_response_fmt("INFO", "REG",
	                   "0x%02X %s = 0x%02X (%s)",
	                   addr,
	                   def->name,
	                   value,
	                   def->description);

	for (uint8_t i = 0U; i < def->bits_count; i++)
	{
		const PCAP04_RegBit_t* bit = &def->bits[i];
		uint8_t field_value = PCAP04_GetBitValue(value, bit->bit_start, bit->bit_end);
		queue_reg_bit_detail(addr, bit, field_value);
	}
}

static void parse_and_enqueue(const uint8_t* data, uint16_t len)
{
	uint16_t i = 0U;
	while (i < len)
	{
		uint16_t start = i;
		while (i < len)
		{
			if ((i + 1U < len) && data[i] == '&' && data[i + 1U] == '&')
			{
				break;
			}
			i++;
		}
		uint16_t seg_len = (uint16_t)(i - start);
		if (seg_len > 0U)
		{
			char segment[CMD_QUEUE_ITEM_MAX_LEN];
			uint16_t copy_len = (seg_len >= CMD_QUEUE_ITEM_MAX_LEN) ? (CMD_QUEUE_ITEM_MAX_LEN - 1U) : seg_len;
			memcpy(segment, &data[start], copy_len);
			segment[copy_len] = '\0';

			trim(segment);
			cmd_priority_t prio = extract_priority(segment);
			if (segment[0] != '\0')
			{
				if (!cmd_queue_push((const uint8_t*)segment, (uint16_t)strlen(segment), prio))
				{
					queue_err("QUEUE", "command queue full, dropped '%s'", segment);
				}
			}
		}
		if ((i + 1U < len) && data[i] == '&' && data[i + 1U] == '&')
		{
			i += 2U;
		}
	}
}

/* Command handlers --------------------------------------------------------- */
static void handle_reg_command(const char* args);
static void handle_pcap_command(const char* args);
static void handle_scan_command(const char* args);

static void handle_single_command(const char* cmd)
{
	if (cmd == NULL)
	{
		return;
	}
	char keyword[16];
	const char* rest = read_token(cmd, keyword, sizeof(keyword));
	if (keyword[0] == '\0')
	{
		return;
	}

	for (char* p = keyword; *p != '\0'; ++p)
	{
		*p = (char)toupper((unsigned char)*p);
	}

	if (strcmp(keyword, "PAUSE") == 0)
	{
		usb_cmd_pause_cdc();
		queue_ok("CDC", "paused");
	}
	else if (strcmp(keyword, "RESUME") == 0)
	{
		usb_cmd_resume_cdc();
		queue_ok("CDC", "resumed");
	}
	else if (strcmp(keyword, "PCAP") == 0)
	{
		handle_pcap_command(rest);
	}
	else if (strcmp(keyword, "REG") == 0)
	{
		handle_reg_command(rest);
	}
	else if (strcmp(keyword, "SCAN") == 0)
	{
		handle_scan_command(rest);
	}
	else if (strcmp(keyword, "STATUS") == 0)
	{
		queue_ok("SYS", "cdc_paused=%u queue=%u rx_pending=%u", usb_cmd_is_cdc_paused(), cmd_queue_count(), rx_pending);
	}
	else
	{
		queue_err("CMD", "unknown keyword '%s'", keyword);
	}
}

static void handle_pcap_command(const char* args)
{
	char sub[16];
	const char* rest = read_token(args, sub, sizeof(sub));
	for (char* p = sub; *p != '\0'; ++p)
	{
		*p = (char)toupper((unsigned char)*p);
	}

	if (strcmp(sub, "TEST") == 0)
	{
		int rc = PCap04_Test();
		if (rc == 1)
		{
			queue_ok("PCAP", "self-test passed");
		}
		else
		{
			queue_err("PCAP", "self-test failed (%d)", rc);
		}
	}
	else if (strcmp(sub, "FW") == 0)
	{
		(void)queue_response_fmt("INFO", "PCAP", "firmware update scheduling not yet implemented");
	}
	else if (strcmp(sub, "SYNC") == 0 || strcmp(sub, "INIT") == 0)
	{
		uint8_t count = pcap04_if_sync_all();
		queue_ok("PCAP", "queued %u register writes", (unsigned)count);
	}
	else
	{
		queue_err("PCAP", "unknown sub-command '%s'", sub);
	}
}

static void handle_reg_command(const char* args)
{
	char action[16];
	const char* rest = read_token(args, action, sizeof(action));
	for (char* p = action; *p != '\0'; ++p)
	{
		*p = (char)toupper((unsigned char)*p);
	}

	if (strcmp(action, "SET") == 0)
	{
		char addr_tok[16];
		rest = read_token(rest, addr_tok, sizeof(addr_tok));
		uint32_t addr = 0U;
		if (!parse_u32(addr_tok, &addr) || addr >= PCAP04_REG_COUNT)
		{
			queue_err("REG", "invalid address '%s'", addr_tok);
			return;
		}

		char bitname[32];
		rest = read_token(rest, bitname, sizeof(bitname));
		if (bitname[0] == '\0')
		{
			queue_err("REG", "missing bit field name");
			return;
		}

		char value_tok[16];
		rest = read_token(rest, value_tok, sizeof(value_tok));
		uint32_t val = 0U;
		if (!parse_u32(value_tok, &val))
		{
			queue_err("REG", "invalid value '%s'", value_tok);
			return;
		}

		uint8_t old_val = 0U;
		uint8_t new_val = 0U;
		uint8_t rc = pcap04_if_reg_set((uint8_t)addr, bitname, (uint8_t)val, &old_val, &new_val);
		if (rc == 0U)
		{
		queue_ok("REG", "SET 0x%02X.%s: %u->%u", (unsigned)addr, bitname, (unsigned)old_val, (unsigned)new_val);
		const PCAP04_Register_t* def = get_register_def((uint8_t)addr);
		if (def != NULL)
		{
			const PCAP04_RegBit_t* bit_def = find_bit_def(def, bitname);
			if (bit_def != NULL)
			{
				queue_reg_bit_detail((uint8_t)addr, bit_def, new_val);
			}
		}
		}
		else
		{
			queue_err("REG", "SET failed rc=%u", rc);
		}
	}
	else if (strcmp(action, "GET") == 0)
	{
		char addr_tok[16];
		rest = read_token(rest, addr_tok, sizeof(addr_tok));
		uint32_t addr = 0U;
		if (!parse_u32(addr_tok, &addr) || addr >= PCAP04_REG_COUNT)
		{
			queue_err("REG", "invalid address '%s'", addr_tok);
			return;
		}

		char bitname[32];
		read_token(rest, bitname, sizeof(bitname));
		if (bitname[0] == '\0')
		{
			queue_err("REG", "missing bit field name");
			return;
		}

		uint8_t value = 0U;
		uint8_t rc = pcap04_if_reg_get((uint8_t)addr, bitname, &value);
		if (rc == 0U)
		{
		queue_ok("REG", "GET 0x%02X.%s=0x%02X", (unsigned)addr, bitname, value);
		const PCAP04_Register_t* def = get_register_def((uint8_t)addr);
		if (def != NULL)
		{
			const PCAP04_RegBit_t* bit_def = find_bit_def(def, bitname);
			if (bit_def != NULL)
			{
				queue_reg_bit_detail((uint8_t)addr, bit_def, value);
			}
		}
		}
		else
		{
			queue_err("REG", "GET failed rc=%u", rc);
		}
	}
	else if (strcmp(action, "PRINT") == 0)
	{
		char addr_tok[16];
		read_token(rest, addr_tok, sizeof(addr_tok));
		uint32_t addr = 0U;
		if (!parse_u32(addr_tok, &addr) || addr >= PCAP04_REG_COUNT)
		{
			queue_err("REG", "invalid address '%s'", addr_tok);
			return;
		}
		uint8_t reg_val = pcap04_if_reg_byte((uint8_t)addr);
	(void)reg_val;
	queue_reg_summary((uint8_t)addr);
	}
	else if (strcmp(action, "SNAPSHOT") == 0 || strcmp(action, "DUMP") == 0)
	{
		for (uint8_t i = 0U; i < PCAP04_REGISTER_TABLE.registers_count; i++)
		{
			queue_reg_summary(PCAP04_REGISTER_TABLE.registers[i].address);
		}
	}
	else if (strcmp(action, "SAVE") == 0)
	{
		uint8_t rc = pcap04_if_save_to_flash();
		if (rc == 0U)
		{
			queue_ok("REG", "configuration saved to flash");
		}
		else
		{
			queue_err("REG", "save to flash failed rc=%u", rc);
		}
	}
	else if (strcmp(action, "LOAD") == 0)
	{
		uint8_t rc = pcap04_if_load_from_flash();
		if (rc == 0U)
		{
			queue_ok("REG", "configuration loaded from flash");
		}
		else
		{
			queue_err("REG", "load from flash failed rc=%u", rc);
		}
	}
	else
	{
		queue_err("REG", "unknown action '%s'", action);
	}
}

static void handle_scan_command(const char* args)
{
	char mode_tok[16];
	const char* rest = read_token(args, mode_tok, sizeof(mode_tok));
	for (char* p = mode_tok; *p != '\0'; ++p)
	{
		*p = (char)toupper((unsigned char)*p);
	}

	if (strcmp(mode_tok, "FULL") == 0)
	{
		char period_tok[16];
		read_token(rest, period_tok, sizeof(period_tok));
		uint32_t period = 200U;
		if (period_tok[0] != '\0')
		{
			(void)parse_u32(period_tok, &period);
		}
		mux_set_mode(MUX_SCAN_FULL, 0U, 0U);
		mux_set_period_ms((uint16_t)(period == 0U ? 200U : period));
		mux_enable(1U);
		queue_ok("SCAN", "full scan every %u ms", (unsigned)period);
	}
	else if (strcmp(mode_tok, "POINT") == 0)
	{
		char row_tok[16];
		char col_tok[16];
		rest = read_token(rest, row_tok, sizeof(row_tok));
		read_token(rest, col_tok, sizeof(col_tok));
		uint32_t row = 0U, col = 0U;
		if (!parse_u32(row_tok, &row) || !parse_u32(col_tok, &col))
		{
			queue_err("SCAN", "POINT requires row/col");
			return;
		}
		mux_set_mode(MUX_SCAN_POINT, (uint8_t)row, (uint8_t)col);
		mux_enable(1U);
		queue_ok("SCAN", "point (%u,%u)", (unsigned)row, (unsigned)col);
	}
	else if (strcmp(mode_tok, "ROW") == 0)
	{
		char row_tok[16];
		read_token(rest, row_tok, sizeof(row_tok));
		uint32_t row = 0U;
		if (!parse_u32(row_tok, &row))
		{
			queue_err("SCAN", "ROW requires index");
			return;
		}
		mux_set_mode(MUX_SCAN_ROW, (uint8_t)row, 0U);
		mux_enable(1U);
		queue_ok("SCAN", "row %u", (unsigned)row);
	}
	else if (strcmp(mode_tok, "COL") == 0)
	{
		char col_tok[16];
		read_token(rest, col_tok, sizeof(col_tok));
		uint32_t col = 0U;
		if (!parse_u32(col_tok, &col))
		{
			queue_err("SCAN", "COL requires index");
			return;
		}
		mux_set_mode(MUX_SCAN_COL, 0U, (uint8_t)col);
		mux_enable(1U);
		queue_ok("SCAN", "column %u", (unsigned)col);
	}
	else if (strcmp(mode_tok, "EN") == 0)
	{
		char en_tok[8];
		read_token(rest, en_tok, sizeof(en_tok));
		uint32_t en = 0U;
		if (!parse_u32(en_tok, &en))
		{
			queue_err("SCAN", "EN requires 0 or 1");
			return;
		}
		mux_enable((uint8_t)(en != 0U));
		queue_ok("SCAN", "enable=%u", (unsigned)en);
	}
	else
	{
		queue_err("SCAN", "unknown mode '%s'", mode_tok);
	}
}

/* Public API ----------------------------------------------------------------*/
void usb_cmd_init(void)
{
	/* 初始化指令队列、PCAP04 接口、CD4067 扫描器 */
	cmd_queue_init();
	pcap04_if_init();
	mux_init();
	queue_response_fmt("INFO", "SYS", "command subsystem ready");
}

void usb_cmd_rx_push(const uint8_t* data, uint16_t length)
{
	/* USB 中断接收入口：仅复制数据并更新标志，避免重负载操作 */
	if (length == 0U || data == NULL) return;
	uint16_t copy_len = (length > sizeof(rx_buf[0])) ? (uint16_t)sizeof(rx_buf[0]) : length;
	if (rx_pending >= RX_BUFFER_COUNT)
	{
		rx_read_idx ^= 1U;
		rx_pending--;
	}
	memcpy(rx_buf[rx_write_idx], data, copy_len);
	rx_len[rx_write_idx] = copy_len;
	rx_write_idx ^= 1U;
	rx_pending++;
}

void usb_cmd_tick_5ms(void)
{
	/* 5 ms 周期调度：解析指令、执行命令、轮询外设、发送排队回复 */
	if (rx_pending > 0U)
	{
		uint8_t idx = rx_read_idx;
		parse_and_enqueue(rx_buf[idx], rx_len[idx]);
		rx_len[idx] = 0U;
		rx_read_idx ^= 1U;
		rx_pending--;
	}

	pump_tx();

	cmd_item_t item;
	if (cmd_queue_pop(&item))
	{
		char cmd_copy[CMD_QUEUE_ITEM_MAX_LEN];
		uint16_t copy_len = (item.length >= CMD_QUEUE_ITEM_MAX_LEN) ? (CMD_QUEUE_ITEM_MAX_LEN - 1U) : item.length;
		memcpy(cmd_copy, item.data, copy_len);
		cmd_copy[copy_len] = '\0';
		handle_single_command(cmd_copy);
	}

	pcap04_if_tick_5ms();
	mux_tick_5ms();

	/* Drain PCAP04 events */
	pcap04_event_t evt;
	while (pcap04_if_fetch_event(&evt))
	{
		switch (evt.type)
		{
		case PCAP04_EVENT_REG_SYNC_DONE:
			queue_ok("PCAP", "register 0x%02X synchronized (rc=%u)", evt.reg_addr, evt.status);
			break;
		case PCAP04_EVENT_FIRMWARE_DONE:
			if (evt.status == 0U)
			{
				queue_ok("PCAP", "firmware update complete");
			}
			else
			{
				queue_err("PCAP", "firmware update failed rc=%u", evt.status);
			}
			break;
		default:
			break;
		}
	}

	pump_tx();
}

void usb_cmd_pause_cdc(void)
{
	/* 累计暂停计数，确保关键操作期间停止 CDC 发送 */
	cdc_pause_depth++;
	cdc_paused = 1U;
}

void usb_cmd_resume_cdc(void)
{
	/* 恢复 CDC 发送，当暂停计数回落到 0 时才真正放开 */
	if (cdc_pause_depth > 0U)
	{
		cdc_pause_depth--;
	}
	if (cdc_pause_depth == 0U)
	{
		cdc_paused = 0U;
	}
}

uint8_t usb_cmd_is_cdc_paused(void)
{
	return cdc_paused;
}

#include "usb_cmd.h"
#include "cmd_queue.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include "pcap04_if.h"
#include "mux_cd4067.h"

/* RX double buffer and flags */
static uint8_t rx_buf[2][64];
static volatile uint16_t rx_len[2] = {0U, 0U};
static volatile uint8_t rx_write_idx = 0U;
static volatile uint8_t rx_read_idx = 1U;
static volatile uint8_t rx_pending = 0U;

/* CDC pause flag */
static volatile uint8_t cdc_paused = 0U;

static void parse_and_enqueue(const uint8_t* data, uint16_t len)
{
	/* Very simple parser: split by '&&', assign priority by prefix like "[H]" or "[L]". */
	uint16_t i = 0U;
	while (i < len)
	{
		/* Find segment end */
		uint16_t start = i;
		while (i < len)
		{
			if ((i + 1U < len) && data[i] == '&' && data[i + 1U] == '&')
			{
				break;
			}
			i++;
		}
		uint16_t seg_len = (uint16_t)(i - start);
		/* Determine priority */
		cmd_priority_t prio = CMD_PRIO_LOW;
		if (seg_len >= 3U && data[start] == '[' && data[start + 2U] == ']')
		{
			if (data[start + 1U] == 'H') prio = CMD_PRIO_HIGH;
			if (data[start + 1U] == 'L') prio = CMD_PRIO_LOW;
		}
		/* Enqueue pointer to segment (no copy to keep ISR light); consumer executes quickly */
		(void)cmd_queue_push(&data[start], seg_len, prio);
		/* Skip '&&' */
		if ((i + 1U < len) && data[i] == '&' && data[i + 1U] == '&')
		{
			i += 2U;
		}
	}
}

void usb_cmd_rx_push(const uint8_t* data, uint16_t length)
{
	if (length == 0U) return;
	/* Only accept up to 64B FS packet per RX slot */
	uint16_t copy_len = (length > sizeof(rx_buf[0])) ? (uint16_t)sizeof(rx_buf[0]) : length;
	if (rx_pending >= 2U)
	{
		/* Both buffers busy: drop oldest by advancing read index to keep latest */
		rx_read_idx ^= 1U;
		rx_pending--;
	}
	memcpy(rx_buf[rx_write_idx], data, copy_len);
	rx_len[rx_write_idx] = copy_len;
	rx_write_idx ^= 1U;
	rx_pending++;
}

void usb_cmd_tick_5ms(void)
{
	/* Move any RX'd segments into the command queue with priority and sequencing */
	if (rx_pending > 0U)
	{
		uint8_t idx = rx_read_idx;
		parse_and_enqueue(rx_buf[idx], rx_len[idx]);
		rx_len[idx] = 0U;
		rx_read_idx ^= 1U;
		rx_pending--;
	}

	/* Process at most one queued command per tick to bound latency */
	cmd_item_t item;
	if (cmd_queue_pop(&item))
	{
		/* Dispatch command. Minimal examples: control CDC pause, PCAP04 test, CRLF, etc. */
		if (item.length >= 4 && (memcmp(item.data, "PAUS", 4) == 0))
		{
			usb_cmd_pause_cdc();
		}
		else if (item.length >= 4 && (memcmp(item.data, "RESU", 4) == 0))
		{
			usb_cmd_resume_cdc();
		}
		else if (item.length >= 4 && (memcmp(item.data, "TEST", 4) == 0))
		{
			(void)PCap04_Test();
		}
		/* Echo by default to prove non-blocking path */
		if (!cdc_paused)
		{
			/* Try to transmit; if busy, retry next tick */
			if (CDC_TxBusy_FS() == USBD_OK)
			{
				(void)CDC_FS_Send(item.data, item.length);
			}
		}
	}
	/* Advance CD4067 scanning asynchronously */
	mux_tick_5ms();
}

void usb_cmd_pause_cdc(void)
{
	cdc_paused = 1U;
}

void usb_cmd_resume_cdc(void)
{
	cdc_paused = 0U;
}

uint8_t usb_cmd_is_cdc_paused(void)
{
	return cdc_paused;
}


