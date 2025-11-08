/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usb_command.c
  * @brief   USB Command Processing Implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_command.h"
#include "usbd_cdc_if.h"
#include "matrix_scan.h"
#include "pcap04_spi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 前向声明 */
extern uint32_t Quantize_Value(uint32_t raw_value, uint32_t min_val, uint32_t max_val, uint16_t level);

/* Private variables ---------------------------------------------------------*/
WorkMode_t g_work_mode = MODE_NORMAL;
static WorkMode_t g_last_work_mode = MODE_NORMAL;  /* 保存上一次的工作模式，用于START时恢复 */
uint32_t g_scan_delay_ms = 100;  /* 默认100ms */
uint8_t g_current_row = 0;       /* 当前行通道 (0-15) */
uint8_t g_current_col = 0;       /* 当前列通道 (0-15) */
OutputMode_t g_output_mode = OUTPUT_RAW;  /* 默认原始值模式 */
OutputFormat_t g_output_format = FORMAT_TABLE;  /* 默认表格格式 */
uint32_t g_quant_min = 0;                  /* 量化最小值 */
uint32_t g_quant_max = 100000;             /* 量化最大值（默认10万） */
QuantLevel_t g_quant_level = QUANT_LEVEL_255;  /* 默认255档位 */
volatile uint8_t g_stream_enabled = 0; /* START/STOP 会话开关 */

/* Private function prototypes -----------------------------------------------*/
static void Send_Response(const char *msg);
static void Process_SetRate(const char *param);
static void Process_FastMode(void);
static void Process_NormalMode(void);
static void Process_SingleScan(void);
static void Process_Stop(void);
static void Process_Start(void);
static void Process_Status(void);
static void Process_Help(void);
static void Process_SetRow(const char *param);
static void Process_SetCol(const char *param);
static void Process_GetRow(void);
static void Process_GetCol(void);
static void Process_ScanPoint(const char *param);
static void Process_MatrixInfo(void);
static void Process_SetMode(const char *param);
static void Process_SetRange(const char *param);
static void Process_SetLevel(const char *param);
static void Process_SetFormat(const char *param);
static void Process_PCap04_Status(void);
static void Process_PCap04_Test(void);

/******************************************************************************/
/*                           USB Command Initialization                       */
/******************************************************************************/
void USB_Command_Init(void)
{
  g_work_mode = MODE_NORMAL;  /* 默认普通模式，开始扫描 */
  g_scan_delay_ms = 100;      /* 默认100ms扫描间隔 */
  g_current_row = 0;          /* 默认行通道0 */
  g_current_col = 0;          /* 默认列通道0 */
  g_output_mode = OUTPUT_RAW;  /* 默认原始值模式 */
  g_output_format = FORMAT_TABLE;  /* 默认表格格式 */
  g_quant_min = 0;            /* 量化最小值 */
  g_quant_max = 100000;        /* 量化最大值（默认10万） */
  g_quant_level = QUANT_LEVEL_255;  /* 默认255档位 */
}

/******************************************************************************/
/*                           Parse Command                                    */
/******************************************************************************/
uint8_t USB_Command_Parse(const char *cmd)
{
  if(cmd == NULL || strlen(cmd) == 0) {
    return 0;
  }
  
  /* 转换为大写以便比较 */
  char cmd_upper[64];
  int i;
  for(i = 0; i < strlen(cmd) && i < 63; i++) {
    cmd_upper[i] = (cmd[i] >= 'a' && cmd[i] <= 'z') ? (cmd[i] - 'a' + 'A') : cmd[i];
  }
  cmd_upper[i] = '\0';
  
  /* 查找冒号位置（参数分隔符） */
  char *colon = strchr(cmd_upper, ':');
  char *param = NULL;
  int cmd_len = strlen(cmd_upper);
  
  if(colon != NULL) {
    cmd_len = colon - cmd_upper;
    param = (char*)colon + 1;
  }
  
  /* 解析命令 */
  if(strncmp(cmd_upper, "SET_RATE", cmd_len) == 0 || strncmp(cmd_upper, "RATE", cmd_len) == 0) {
    Process_SetRate(param);
    return CMD_SET_RATE;
  }
  else if(strncmp(cmd_upper, "FAST_MODE", cmd_len) == 0 || strncmp(cmd_upper, "FAST", cmd_len) == 0) {
    Process_FastMode();
    return CMD_FAST_MODE;
  }
  else if(strncmp(cmd_upper, "NORMAL_MODE", cmd_len) == 0 || strncmp(cmd_upper, "NORMAL", cmd_len) == 0) {
    Process_NormalMode();
    return CMD_NORMAL_MODE;
  }
  else if(strncmp(cmd_upper, "SINGLE_SCAN", cmd_len) == 0 || strncmp(cmd_upper, "SINGLE", cmd_len) == 0) {
    Process_SingleScan();
    return CMD_SINGLE_SCAN;
  }
  else if(strncmp(cmd_upper, "STOP", cmd_len) == 0) {
    Process_Stop();
    return CMD_STOP;
  }
  else if(strncmp(cmd_upper, "START", cmd_len) == 0) {
    Process_Start();
    return CMD_START;
  }
  else if(strncmp(cmd_upper, "STATUS", cmd_len) == 0) {
    Process_Status();
    return CMD_STATUS;
  }
  else if(strncmp(cmd_upper, "HELP", cmd_len) == 0 || strncmp(cmd_upper, "?", cmd_len) == 0) {
    Process_Help();
    return CMD_HELP;
  }
  else if(strncmp(cmd_upper, "SET_ROW", cmd_len) == 0) {
    Process_SetRow(param);
    return CMD_SET_ROW;
  }
  else if(strncmp(cmd_upper, "SET_COL", cmd_len) == 0) {
    Process_SetCol(param);
    return CMD_SET_COL;
  }
  else if(strncmp(cmd_upper, "GET_ROW", cmd_len) == 0) {
    Process_GetRow();
    return CMD_GET_ROW;
  }
  else if(strncmp(cmd_upper, "GET_COL", cmd_len) == 0) {
    Process_GetCol();
    return CMD_GET_COL;
  }
  else if(strncmp(cmd_upper, "SCAN_POINT", cmd_len) == 0 || strncmp(cmd_upper, "POINT", cmd_len) == 0) {
    Process_ScanPoint(param);
    return CMD_SCAN_POINT;
  }
  else if(strncmp(cmd_upper, "MATRIX_INFO", cmd_len) == 0 || strncmp(cmd_upper, "INFO", cmd_len) == 0) {
    Process_MatrixInfo();
    return CMD_MATRIX_INFO;
  }
  else if(strncmp(cmd_upper, "SET_MODE", cmd_len) == 0 || strncmp(cmd_upper, "MODE", cmd_len) == 0) {
    Process_SetMode(param);
    return CMD_SET_MODE;
  }
  else if(strncmp(cmd_upper, "SET_RANGE", cmd_len) == 0 || strncmp(cmd_upper, "RANGE", cmd_len) == 0) {
    Process_SetRange(param);
    return CMD_SET_RANGE;
  }
  else if(strncmp(cmd_upper, "SET_LEVEL", cmd_len) == 0 || strncmp(cmd_upper, "LEVEL", cmd_len) == 0) {
    Process_SetLevel(param);
    return CMD_SET_LEVEL;
  }
  else if(strncmp(cmd_upper, "SET_FORMAT", cmd_len) == 0 || strncmp(cmd_upper, "FORMAT", cmd_len) == 0) {
    Process_SetFormat(param);
    return CMD_SET_FORMAT;
  }
  else if(strncmp(cmd_upper, "PCAP04_STATUS", cmd_len) == 0 || strncmp(cmd_upper, "PCAP_STATUS", cmd_len) == 0) {
    Process_PCap04_Status();
    return CMD_PCAP04_STATUS;
  }
  else if(strncmp(cmd_upper, "PCAP04_TEST", cmd_len) == 0 || strncmp(cmd_upper, "PCAP_TEST", cmd_len) == 0 || strncmp(cmd_upper, "TEST", cmd_len) == 0) {
    Process_PCap04_Test();
    return CMD_PCAP04_TEST;
  }
  
  return 0;
}

/******************************************************************************/
/*                           Process USB Command                              */
/******************************************************************************/
void USB_Command_Process(uint8_t *buf, uint32_t len)
{
  if(buf == NULL || len == 0) {
    return;
  }
  
  /* 确保字符串以null结尾 */
  char cmd[256];
  uint32_t copy_len = (len < 255) ? len : 255;
  memcpy(cmd, buf, copy_len);
  cmd[copy_len] = '\0';
  
  /* 去除换行符和回车符 */
  char *p = cmd;
  while(*p != '\0') {
    if(*p == '\r' || *p == '\n') {
      *p = '\0';
      break;
    }
    p++;
  }
  
  /* 去除前导空格 */
  while(*cmd == ' ' || *cmd == '\t') {
    memmove(cmd, cmd + 1, strlen(cmd));
  }
  
  /* 解析并处理命令 */
  if(strlen(cmd) > 0) {
    USB_Command_Parse(cmd);
  }
}

/******************************************************************************/
/*                           Command Handlers                                 */
/******************************************************************************/
static void Process_SetRate(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    uint32_t rate = strtoul(param, NULL, 10);
    if(rate > 0 && rate <= 10000) {  /* 限制在1-10000ms之间 */
      g_scan_delay_ms = rate;
      char msg[64];
      sprintf(msg, "OK: Scan rate set to %lu ms\r\n", g_scan_delay_ms);
      Send_Response(msg);
    } else {
      Send_Response("ERROR: Invalid rate (1-10000 ms)\r\n");
    }
  } else {
    char msg[64];
    sprintf(msg, "Current scan rate: %lu ms\r\n", g_scan_delay_ms);
    Send_Response(msg);
  }
}

static void Process_FastMode(void)
{
  g_work_mode = MODE_FAST;
  g_last_work_mode = MODE_FAST;  /* 保存工作模式 */
  g_scan_delay_ms = 0;  /* 快速模式：无延时 */
  Send_Response("OK: Fast mode enabled (continuous scan, no delay)\r\n");
}

static void Process_NormalMode(void)
{
  g_work_mode = MODE_NORMAL;
  g_last_work_mode = MODE_NORMAL;  /* 保存工作模式 */
  if(g_scan_delay_ms == 0) {
    g_scan_delay_ms = 100;  /* 恢复默认值 */
  }
  char msg[64];
  sprintf(msg, "OK: Normal mode enabled (scan rate: %lu ms)\r\n", g_scan_delay_ms);
  Send_Response(msg);
}

static void Process_SingleScan(void)
{
  g_work_mode = MODE_SINGLE;
  Send_Response("OK: Single scan mode enabled (will scan once then stop)\r\n");
}

static void Process_Stop(void)
{
  /* 保存当前工作模式（如果不是STOP模式） */
  if(g_work_mode != MODE_STOP) {
    g_last_work_mode = g_work_mode;
  }
  g_work_mode = MODE_STOP;
  g_stream_enabled = 0;
  /* 会话结束标志 */
  Send_Response("END\r\n");
  Send_Response("STOP\r\n");
}

static void Process_Start(void)
{
  /* 开始会话并允许传输 */
  g_stream_enabled = 1;
  /* 如果当前是STOP模式，恢复到之前的工作模式 */
  if(g_work_mode == MODE_STOP) {
    g_work_mode = g_last_work_mode;
    /* 如果之前没有保存的模式，默认使用NORMAL模式 */
    if(g_work_mode == MODE_STOP) {
      g_work_mode = MODE_NORMAL;
    }
  }
  Send_Response("START\r\n");
  Send_Response("OK: Streaming session started\r\n");
}

static void Process_Status(void)
{
  char msg[512];
  const char *mode_str;
  const char *output_mode_str;
  
  switch(g_work_mode) {
    case MODE_NORMAL: mode_str = "NORMAL"; break;
    case MODE_FAST: mode_str = "FAST"; break;
    case MODE_SINGLE: mode_str = "SINGLE"; break;
    case MODE_STOP: mode_str = "STOP"; break;
    default: mode_str = "UNKNOWN"; break;
  }
  
  output_mode_str = (g_output_mode == OUTPUT_RAW) ? "RAW" : "QUANT";
  const char *format_str = (g_output_format == FORMAT_SIMPLE) ? "SIMPLE" : "TABLE";
  
  sprintf(msg, "Status:\r\n"
               "  Work Mode: %s\r\n"
               "  Scan Delay: %lu ms\r\n"
               "  Current Row: %d\r\n"
               "  Current Col: %d\r\n"
               "  Matrix Size: 16x16\r\n"
               "  Output Mode: %s\r\n"
               "  Output Format: %s\r\n",
          mode_str, g_scan_delay_ms, g_current_row, g_current_col, output_mode_str, format_str);
  
  if(g_output_mode == OUTPUT_QUANT) {
    char range_msg[128];
    sprintf(range_msg, "  Quant Range: %lu - %lu\r\n"
                       "  Quant Level: 0-%d\r\n",
            g_quant_min, g_quant_max, (int)g_quant_level);
    strcat(msg, range_msg);
  }
  
  Send_Response(msg);
}

static void Process_Help(void)
{
  const char *help = 
    "Available Commands:\r\n"
    "\r\n"
    "Scan Control:\r\n"
    "  SET_RATE:<ms>     - Set scan rate in milliseconds (1-10000)\r\n"
    "  FAST_MODE         - Enable fast mode (continuous scan, no delay)\r\n"
    "  NORMAL_MODE       - Enable normal mode (scan at set rate)\r\n"
    "  SINGLE_SCAN       - Perform single scan then stop\r\n"
    "  START             - Begin streaming session (emit START, enable TX)\r\n"
    "  STOP              - Stop scanning (emit END, disable TX)\r\n"
    "\r\n"
    "Row/Column Control:\r\n"
    "  SET_ROW:<n>       - Set row channel (0-15)\r\n"
    "  SET_COL:<n>       - Set column channel (0-15)\r\n"
    "  GET_ROW           - Query current row channel\r\n"
    "  GET_COL           - Query current column channel\r\n"
    "  SCAN_POINT:<r>:<c> - Scan specific point (row:col)\r\n"
    "  MATRIX_INFO       - Show matrix information\r\n"
    "\r\n"
    "Output Mode Control:\r\n"
    "  SET_MODE:<raw|quant> - Set output mode (raw=original value, quant=quantized value)\r\n"
    "  SET_RANGE:<min>:<max> - Set quantization range (L-H)\r\n"
    "  SET_LEVEL:<255|1023> - Set quantization level (0-255 or 0-1023)\r\n"
    "  SET_FORMAT:<simple|table> - Set output format (simple=X00Y00:value, table=with headers)\r\n"
    "\r\n"
    "System:\r\n"
    "  STATUS            - Show current status\r\n"
    "  PCAP04_STATUS     - Show PCap04 sensor status\r\n"
    "  PCAP04_TEST       - Test PCap04 communication\r\n"
    "  HELP or ?         - Show this help\r\n"
    "\r\n"
    "Examples:\r\n"
    "  SET_RATE:50       - Set scan rate to 50ms\r\n"
    "  SET_ROW:5         - Set row channel to 5\r\n"
    "  GET_COL           - Query current column channel\r\n"
    "  SCAN_POINT:3:7    - Scan point at row 3, column 7\r\n"
    "  MATRIX_INFO       - Show matrix information\r\n"
    "  SET_MODE:quant    - Enable quantization mode\r\n"
    "  SET_RANGE:1000:50000 - Set quant range 1000-50000\r\n"
    "  SET_LEVEL:1023    - Set quant level to 0-1023\r\n";
  
  Send_Response(help);
}

/******************************************************************************/
/*                           New Command Handlers                             */
/******************************************************************************/
static void Process_SetRow(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    uint32_t row = strtoul(param, NULL, 10);
    if(row < 16) {
      g_current_row = (uint8_t)row;
      char msg[64];
      sprintf(msg, "OK: Row channel set to %d\r\n", g_current_row);
      Send_Response(msg);
    } else {
      Send_Response("ERROR: Invalid row (0-15)\r\n");
    }
  } else {
    char msg[64];
    sprintf(msg, "Current row channel: %d\r\n", g_current_row);
    Send_Response(msg);
  }
}

static void Process_SetCol(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    uint32_t col = strtoul(param, NULL, 10);
    if(col < 16) {
      g_current_col = (uint8_t)col;
      char msg[64];
      sprintf(msg, "OK: Column channel set to %d\r\n", g_current_col);
      Send_Response(msg);
    } else {
      Send_Response("ERROR: Invalid column (0-15)\r\n");
    }
  } else {
    char msg[64];
    sprintf(msg, "Current column channel: %d\r\n", g_current_col);
    Send_Response(msg);
  }
}

static void Process_GetRow(void)
{
  char msg[64];
  sprintf(msg, "Current row channel: %d\r\n", g_current_row);
  Send_Response(msg);
}

static void Process_GetCol(void)
{
  char msg[64];
  sprintf(msg, "Current column channel: %d\r\n", g_current_col);
  Send_Response(msg);
}

static void Process_ScanPoint(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    uint32_t row = 0, col = 0;
    char *colon = strchr(param, ':');
    
    if(colon != NULL) {
      /* 格式: row:col */
      char row_str[16], col_str[16];
      int row_len = colon - param;
      if(row_len > 0 && row_len < 16) {
        memcpy(row_str, param, row_len);
        row_str[row_len] = '\0';
        row = strtoul(row_str, NULL, 10);
      }
      
      strncpy(col_str, colon + 1, 15);
      col_str[15] = '\0';
      col = strtoul(col_str, NULL, 10);
      
      if(row < 16 && col < 16) {
        /* 单点输出需要 START/END 包围 */
        Send_Response("START\r\n");
        /* 扫描指定点 */
        uint32_t raw_value = Matrix_Scan_Point((uint8_t)row, (uint8_t)col);
        uint32_t output_value;
        
        /* 根据输出模式选择原始值或量化值 */
        if(g_output_mode == OUTPUT_QUANT) {
          /* 量化模式 */
          output_value = Quantize_Value(raw_value, g_quant_min, g_quant_max, (uint16_t)g_quant_level);
          char msg[256];
          sprintf(msg, "Point [%lu:%lu]\r\n"
                      "  Raw Value: %lu\r\n"
                      "  Quantized: %lu (0-%d, range: %lu-%lu)\r\n",
                  row, col, raw_value, output_value, (int)g_quant_level, g_quant_min, g_quant_max);
          Send_Response(msg);
        } else {
          /* 原始值模式 */
          char msg[128];
          sprintf(msg, "Point [%lu:%lu] = %lu (RAW)\r\n", row, col, raw_value);
          Send_Response(msg);
        }
        Send_Response("END\r\n");
      } else {
        Send_Response("ERROR: Invalid coordinates (0-15)\r\n");
      }
    } else {
      Send_Response("ERROR: Format is SCAN_POINT:<row>:<col>\r\n");
    }
  } else {
    Send_Response("ERROR: Missing parameters. Use SCAN_POINT:<row>:<col>\r\n");
  }
}

static void Process_MatrixInfo(void)
{
  char msg[512];
  sprintf(msg, 
    "========================================\r\n"
    "Matrix Information:\r\n"
    "========================================\r\n"
    "  Matrix Size:        16x16 (256 points)\r\n"
    "  Current Row Channel: %d\r\n"
    "  Current Col Channel: %d\r\n"
    "  Row Channel Range:  0-15\r\n"
    "  Col Channel Range:  0-15\r\n"
    "\r\n"
    "Hardware Configuration:\r\n"
    "  Multiplexer:        CD74HC4067SM96 (16:1)\r\n"
    "  Row Mux Channels:  SX0-SX3 (PB1,PB0,PB10,PB2), ENY (PB11)\r\n"
    "  Col Mux Channels:   SY0-SY3 (PA3,PA4,PA6,PA7), ENX (PA5)\r\n"
    "  Sensor:             PCap04 (Capacitance-to-Digital Converter)\r\n"
    "\r\n"
    "Communication:\r\n"
    "  Interface:          SPI2\r\n"
    "  Pins:               PB13(SCK), PB14(MISO), PB15(MOSI), PB12(SSN)\r\n"
    "  IIC_EN:             PA8 (Low=SPI, High=I2C)\r\n"
    "========================================\r\n",
    g_current_row, g_current_col);
  Send_Response(msg);
}

/******************************************************************************/
/*                           Quantization Mode Handlers                       */
/******************************************************************************/
static void Process_SetMode(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    char param_upper[16];
    int i;
    for(i = 0; i < strlen(param) && i < 15; i++) {
      param_upper[i] = (param[i] >= 'a' && param[i] <= 'z') ? (param[i] - 'a' + 'A') : param[i];
    }
    param_upper[i] = '\0';
    
    if(strcmp(param_upper, "RAW") == 0 || strcmp(param_upper, "ORIGINAL") == 0) {
      g_output_mode = OUTPUT_RAW;
      /* 返回模式信息 */
      Send_Response("OK: Output mode set to RAW (original values)\r\n");
      /* 发送START标志，表示可以开始数据传输，并启用流式传输 */
      Send_Response("START\r\n");
      g_stream_enabled = 1;  /* 启用数据传输 */
    }
    else if(strcmp(param_upper, "QUANT") == 0 || strcmp(param_upper, "QUANTIZE") == 0) {
      g_output_mode = OUTPUT_QUANT;
      /* 返回模式信息 */
      char msg[128];
      sprintf(msg, "OK: Output mode set to QUANTIZE (range: %lu-%lu, level: 0-%d)\r\n",
              g_quant_min, g_quant_max, (int)g_quant_level);
      Send_Response(msg);
      /* 发送START标志，表示可以开始数据传输，并启用流式传输 */
      Send_Response("START\r\n");
      g_stream_enabled = 1;  /* 启用数据传输 */
    }
    else {
      Send_Response("ERROR: Invalid mode. Use 'raw' or 'quant'\r\n");
    }
  } else {
    const char *mode_str = (g_output_mode == OUTPUT_RAW) ? "RAW" : "QUANT";
    char msg[64];
    sprintf(msg, "Current output mode: %s\r\n", mode_str);
    Send_Response(msg);
  }
}

static void Process_SetRange(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    uint32_t min_val = 0, max_val = 0;
    char *colon = strchr(param, ':');
    
    if(colon != NULL) {
      /* 格式: min:max */
      char min_str[32], max_str[32];
      int min_len = colon - param;
      if(min_len > 0 && min_len < 32) {
        memcpy(min_str, param, min_len);
        min_str[min_len] = '\0';
        min_val = strtoul(min_str, NULL, 10);
      }
      
      strncpy(max_str, colon + 1, 31);
      max_str[31] = '\0';
      max_val = strtoul(max_str, NULL, 10);
      
      if(max_val > min_val) {
        g_quant_min = min_val;
        g_quant_max = max_val;
        char msg[128];
        sprintf(msg, "OK: Quantization range set to %lu - %lu\r\n", g_quant_min, g_quant_max);
        Send_Response(msg);
      } else {
        Send_Response("ERROR: Max value must be greater than min value\r\n");
      }
    } else {
      Send_Response("ERROR: Format is SET_RANGE:<min>:<max>\r\n");
    }
  } else {
    char msg[128];
    sprintf(msg, "Current quantization range: %lu - %lu\r\n", g_quant_min, g_quant_max);
    Send_Response(msg);
  }
}

static void Process_SetLevel(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    uint32_t level = strtoul(param, NULL, 10);
    
    if(level == 255) {
      g_quant_level = QUANT_LEVEL_255;
      Send_Response("OK: Quantization level set to 0-255\r\n");
    }
    else if(level == 1023) {
      g_quant_level = QUANT_LEVEL_1023;
      Send_Response("OK: Quantization level set to 0-1023\r\n");
    }
    else {
      Send_Response("ERROR: Invalid level. Use 255 or 1023\r\n");
    }
  } else {
    char msg[64];
    sprintf(msg, "Current quantization level: 0-%d\r\n", (int)g_quant_level);
    Send_Response(msg);
  }
}

static void Process_SetFormat(const char *param)
{
  if(param != NULL && strlen(param) > 0) {
    char param_upper[16];
    int i;
    for(i = 0; i < strlen(param) && i < 15; i++) {
      param_upper[i] = (param[i] >= 'a' && param[i] <= 'z') ? (param[i] - 'a' + 'A') : param[i];
    }
    param_upper[i] = '\0';
    
    if(strcmp(param_upper, "SIMPLE") == 0) {
      g_output_format = FORMAT_SIMPLE;
      Send_Response("OK: Output format set to SIMPLE (X00Y00:value)\r\n");
    }
    else if(strcmp(param_upper, "TABLE") == 0) {
      g_output_format = FORMAT_TABLE;
      Send_Response("OK: Output format set to TABLE (with row/col headers, comma separated)\r\n");
    }
    else {
      Send_Response("ERROR: Invalid format. Use 'simple' or 'table'\r\n");
    }
  } else {
    const char *format_str = (g_output_format == FORMAT_SIMPLE) ? "SIMPLE" : "TABLE";
    char msg[64];
    sprintf(msg, "Current output format: %s\r\n", format_str);
    Send_Response(msg);
  }
}

/******************************************************************************/
/*                           PCap04 Status Handler                            */
/******************************************************************************/
static void Process_PCap04_Status(void)
{
  PCap04_Status_t status = PCap04_Get_Status();
  char msg[512];
  
  sprintf(msg, 
    "========================================\r\n"
    "PCap04 Status Information:\r\n"
    "========================================\r\n"
    "Communication: %s\r\n"
    "Initialized: %s\r\n"
    "Mode: %s\r\n"
    "\r\n"
    "Configuration Registers:\r\n"
    "  Config Reg0: 0x%08lX (%lu)\r\n"
    "  Config Reg1: 0x%08lX (%lu)\r\n"
    "\r\n"
    "Result Registers:\r\n"
    "  Result Reg0: 0x%08lX (%lu)\r\n"
    "  Result Reg1: 0x%08lX (%lu)\r\n",
    status.communication_ok ? "OK" : "FAIL",
    status.is_initialized ? "YES" : "NO (Need initialization)",
    status.is_simulation_mode ? "SIMULATION (Random Data)" : "REAL (SPI Data)",
    status.config_reg0, status.config_reg0,
    status.config_reg1, status.config_reg1,
    status.result_reg0, status.result_reg0,
    status.result_reg1, status.result_reg1);
  
  /* 如果未初始化，添加警告信息 */
  if(!status.is_initialized) {
    char warning_msg[256];
    sprintf(warning_msg,
      "\r\n"
      "WARNING: PCap04 not initialized!\r\n"
      "Possible causes:\r\n"
      "  - Device not powered on\r\n"
      "  - Initialization sequence not completed\r\n"
      "  - SPI communication error\r\n"
      "  - Hardware connection issue\r\n"
      "========================================\r\n");
    strcat(msg, warning_msg);
  } else {
    strcat(msg, "========================================\r\n");
  }
  
  Send_Response(msg);
}

/******************************************************************************/
/*                           PCap04 Test Handler                              */
/******************************************************************************/
static void Process_PCap04_Test(void)
{
  PCap04_TestResult_t test_result = PCap04_Test_Communication();
  char msg[256];
  
  if(test_result.test_result == 1) {
    /* 测试成功 */
    sprintf(msg,
      "========================================\r\n"
      "PCap04 Communication Test:\r\n"
      "========================================\r\n"
      "Result: PASS\r\n"
      "Received Value: 0x%02X (0x11 expected)\r\n"
      "Status: Communication OK\r\n"
      "========================================\r\n",
      test_result.received_value);
  } else {
    /* 测试失败 */
    sprintf(msg,
      "========================================\r\n"
      "PCap04 Communication Test:\r\n"
      "========================================\r\n"
      "Result: FAIL\r\n"
      "Received Value: 0x%02X (0x11 expected)\r\n"
      "Error: %s\r\n"
      "\r\n"
      "Possible causes:\r\n"
      "  - SPI not initialized\r\n"
      "  - Hardware connection issue\r\n"
      "  - PCap04 not responding\r\n"
      "  - SPI configuration error\r\n"
      "========================================\r\n",
      test_result.received_value,
      test_result.error_msg != NULL ? test_result.error_msg : "Unknown error");
  }
  
  Send_Response(msg);
}

/******************************************************************************/
/*                           Send Response                                    */
/******************************************************************************/
static void Send_Response(const char *msg)
{
  if(msg != NULL) {
    uint16_t len = strlen(msg);
    CDC_Transmit_FS((uint8_t*)msg, len);
  }
}

