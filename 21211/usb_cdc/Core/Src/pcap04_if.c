#include "pcap04_if.h"
#include "pcap04_assets.h"
#include "spi.h"
#include "usb_cmd.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include "main.h"

#define PCAP04_FLASH_ADDR        0x08080000UL
#define PCAP04_OPCODE_WRITE_REG  0xA0U
#define PCAP04_REG_QUEUE_CAP     64U
#define PCAP04_EVENT_QUEUE_CAP   4U

#define PCAP04_SPI_ASSERT_CS()   HAL_GPIO_WritePin(SPI_SSN_GPIO_Port, SPI_SSN_Pin, GPIO_PIN_RESET)
#define PCAP04_SPI_RELEASE_CS()  HAL_GPIO_WritePin(SPI_SSN_GPIO_Port, SPI_SSN_Pin, GPIO_PIN_SET)

typedef enum {
	PCAP_IDLE = 0,
	PCAP_REG_WRITE,
	PCAP_FW_WRITE
} pcap_state_t;

typedef struct {
	uint8_t addr;
	uint8_t value;
} reg_job_t;

static volatile pcap_state_t s_state = PCAP_IDLE;
static uint8_t s_regs[PCAP04_REG_COUNT];
static uint8_t s_regs_initialized = 0U;

static reg_job_t s_reg_queue[PCAP04_REG_QUEUE_CAP];
static uint8_t s_reg_head = 0U;
static uint8_t s_reg_tail = 0U;
static uint8_t s_reg_count = 0U;
static reg_job_t s_active_job = {0U, 0U};

static uint8_t s_spi_tx_buf[260];

static pcap04_event_t s_events[PCAP04_EVENT_QUEUE_CAP];
static uint8_t s_evt_head = 0U;
static uint8_t s_evt_tail = 0U;
static uint8_t s_evt_count = 0U;

static void push_event(pcap04_event_type_t type, uint8_t status, uint8_t addr, uint8_t value)
{
	if (s_evt_count >= PCAP04_EVENT_QUEUE_CAP)
	{
		return;
	}
	s_events[s_evt_tail].type = type;
	s_events[s_evt_tail].status = status;
	s_events[s_evt_tail].reg_addr = addr;
	s_events[s_evt_tail].reg_value = value;
	s_evt_tail = (uint8_t)((s_evt_tail + 1U) % PCAP04_EVENT_QUEUE_CAP);
	s_evt_count++;
}

static void queue_reg_job(uint8_t addr, uint8_t value)
{
	if (s_reg_count >= PCAP04_REG_QUEUE_CAP)
	{
		/* drop oldest to keep newest configuration */
		s_reg_head = (uint8_t)((s_reg_head + 1U) % PCAP04_REG_QUEUE_CAP);
		s_reg_count--;
	}
	s_reg_queue[s_reg_tail].addr = addr;
	s_reg_queue[s_reg_tail].value = value;
	s_reg_tail = (uint8_t)((s_reg_tail + 1U) % PCAP04_REG_QUEUE_CAP);
	s_reg_count++;
}

static void ensure_regs_initialized(void)
{
	if (s_regs_initialized != 0U)
	{
		return;
	}
	PCAP04_InitRegisters(s_regs);
	memcpy(s_regs, PCAP04_DefaultConfig, sizeof(PCAP04_DefaultConfig));
	(void)PCAP04_LoadFromFlash(s_regs, PCAP04_FLASH_ADDR);
	s_regs_initialized = 1U;
	PCAP04_SPI_RELEASE_CS();
}

void pcap04_if_init(void)
{
	/* 初始化寄存器镜像、SPI 状态与事件队列 */
	ensure_regs_initialized();
	s_state = PCAP_IDLE;
	s_reg_head = s_reg_tail = s_reg_count = 0U;
	s_evt_head = s_evt_tail = s_evt_count = 0U;
}

uint8_t pcap04_if_reg_set(uint8_t addr, const char* bitname, uint8_t value, uint8_t* old_value, uint8_t* new_value)
{
	/* 更新寄存器镜像并排队写操作，立即返回避免阻塞 */
	ensure_regs_initialized();
	if (addr >= PCAP04_REG_COUNT || bitname == NULL)
	{
		return 1U;
	}
	uint8_t previous = PCAP04_GetRegisterBit(s_regs, addr, bitname);
	uint8_t rc = PCAP04_SetRegisterBit(s_regs, addr, bitname, value);
	if (rc != 0U)
	{
		return rc;
	}
	uint8_t now = PCAP04_GetRegisterBit(s_regs, addr, bitname);
	if (old_value != NULL) *old_value = previous;
	if (new_value != NULL) *new_value = now;
	queue_reg_job(addr, s_regs[addr]);
	return 0U;
}

uint8_t pcap04_if_reg_get(uint8_t addr, const char* bitname, uint8_t* out_value)
{
	/* 从镜像读取寄存器字段数值 */
	ensure_regs_initialized();
	if (addr >= PCAP04_REG_COUNT || bitname == NULL || out_value == NULL)
	{
		return 1U;
	}
	*out_value = PCAP04_GetRegisterBit(s_regs, addr, bitname);
	return 0U;
}

uint8_t pcap04_if_reg_byte(uint8_t addr)
{
	ensure_regs_initialized();
	if (addr >= PCAP04_REG_COUNT)
	{
		return 0U;
	}
	return s_regs[addr];
}

const uint8_t* pcap04_if_reg_snapshot(void)
{
	ensure_regs_initialized();
	return s_regs;
}

uint8_t pcap04_if_sync_all(void)
{
	/* 将镜像中的全部寄存器重新排队写入，返回排队数量 */
	ensure_regs_initialized();
	uint8_t queued = 0U;
	for (uint8_t i = 0U; i < PCAP04_REGISTER_TABLE.registers_count; i++)
	{
		uint8_t addr = PCAP04_REGISTER_TABLE.registers[i].address;
		queue_reg_job(addr, s_regs[addr]);
		queued++;
	}
	return queued;
}

uint8_t pcap04_if_save_to_flash(void)
{
	ensure_regs_initialized();
	return PCAP04_SaveToFlash(s_regs, PCAP04_FLASH_ADDR);
}

uint8_t pcap04_if_load_from_flash(void)
{
	ensure_regs_initialized();
	return PCAP04_LoadFromFlash(s_regs, PCAP04_FLASH_ADDR);
}

uint8_t pcap04_if_fetch_event(pcap04_event_t* out)
{
	if (out == NULL || s_evt_count == 0U)
	{
		return 0U;
	}
	*out = s_events[s_evt_head];
	s_evt_head = (uint8_t)((s_evt_head + 1U) % PCAP04_EVENT_QUEUE_CAP);
	s_evt_count--;
	return 1U;
}

void pcap04_if_tick_5ms(void)
{
	if (s_state != PCAP_IDLE)
	{
		return;
	}
	if (s_reg_count > 0U)
	{
		s_active_job = s_reg_queue[s_reg_head];
		s_reg_head = (uint8_t)((s_reg_head + 1U) % PCAP04_REG_QUEUE_CAP);
		s_reg_count--;

		s_spi_tx_buf[0] = PCAP04_OPCODE_WRITE_REG;
		s_spi_tx_buf[1] = s_active_job.addr;
		s_spi_tx_buf[2] = s_active_job.value;
		PCAP04_SPI_ASSERT_CS();
		if (HAL_SPI_Transmit_IT(&hspi2, s_spi_tx_buf, 3U) == HAL_OK)
		{
			usb_cmd_pause_cdc();
			s_state = PCAP_REG_WRITE;
		}
		else
		{
			PCAP04_SPI_RELEASE_CS();
			push_event(PCAP04_EVENT_REG_SYNC_DONE, 1U, s_active_job.addr, s_active_job.value);
			s_state = PCAP_IDLE;
		}
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi != &hspi2)
	{
		return;
	}
	if (s_state == PCAP_REG_WRITE)
	{
		PCAP04_SPI_RELEASE_CS();
		s_state = PCAP_IDLE;
		usb_cmd_resume_cdc();
		push_event(PCAP04_EVENT_REG_SYNC_DONE, 0U, s_active_job.addr, s_active_job.value);
	}
}

int PCap04_Test(void)
{
	/* Placeholder non-blocking self-test stub */
	return 1;
}
