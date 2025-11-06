/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usb_command.h
  * @brief   USB Command Processing Header
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_COMMAND_H
#define __USB_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* USB命令类型 */
#define CMD_SET_RATE    0x01  /* 设置扫描速率: SET_RATE:<ms> (例如: SET_RATE:100) */
#define CMD_FAST_MODE   0x02  /* 快速模式: FAST_MODE (收到就发送，一直循环) */
#define CMD_NORMAL_MODE 0x03  /* 普通模式: NORMAL_MODE (按设置速率循环) */
#define CMD_SINGLE_SCAN 0x04  /* 单次扫描: SINGLE_SCAN (只扫描一次) */
#define CMD_STOP        0x05  /* 停止扫描: STOP */
#define CMD_STATUS      0x06  /* 查询状态: STATUS */
#define CMD_HELP        0x07  /* 帮助信息: HELP */
#define CMD_SET_ROW     0x08  /* 设置行通道: SET_ROW:<n> (0-15) */
#define CMD_SET_COL     0x09  /* 设置列通道: SET_COL:<n> (0-15) */
#define CMD_GET_ROW     0x0A  /* 查询当前行通道: GET_ROW */
#define CMD_GET_COL     0x0B  /* 查询当前列通道: GET_COL */
#define CMD_SCAN_POINT  0x0C  /* 扫描指定点: SCAN_POINT:<row>:<col> */
#define CMD_MATRIX_INFO 0x0D  /* 查询矩阵信息: MATRIX_INFO */
#define CMD_SET_MODE    0x0E  /* 设置输出模式: SET_MODE:<raw|quant> */
#define CMD_SET_RANGE   0x0F  /* 设置量化范围: SET_RANGE:<min>:<max> */
#define CMD_SET_LEVEL   0x10  /* 设置量化档位: SET_LEVEL:<255|1023> */
#define CMD_SET_FORMAT  0x11  /* 设置输出格式: SET_FORMAT:<simple|table> */
#define CMD_START       0x12  /* 会话开始: START （发送 START 标志，允许传输） */
#define CMD_PCAP04_STATUS 0x13  /* 查询PCap04状态: PCAP04_STATUS */
#define CMD_PCAP04_TEST 0x14  /* 测试PCap04通信: PCAP04_TEST */

/* 工作模式 */
typedef enum {
  MODE_NORMAL = 0,    /* 普通模式：按设置速率循环扫描 */
  MODE_FAST = 1,      /* 快速模式：收到就发送，一直循环 */
  MODE_SINGLE = 2,    /* 单次模式：只扫描一次后停止 */
  MODE_STOP = 3       /* 停止模式：不扫描 */
} WorkMode_t;

/* 输出模式 */
typedef enum {
  OUTPUT_RAW = 0,      /* 原始值模式：输出原始电容值 */
  OUTPUT_QUANT = 1     /* 量化模式：输出量化后的值 */
} OutputMode_t;

/* 输出格式 */
typedef enum {
  FORMAT_SIMPLE = 0,   /* 简洁格式：X00Y00:值 (每行一个点) */
  FORMAT_TABLE = 1     /* 表格格式：带行列标记，用逗号分隔 */
} OutputFormat_t;

/* 量化档位 */
typedef enum {
  QUANT_LEVEL_255 = 255,   /* 量化到 0-255 */
  QUANT_LEVEL_1023 = 1023  /* 量化到 0-1023 */
} QuantLevel_t;

/* Exported variables --------------------------------------------------------*/
extern WorkMode_t g_work_mode;
extern uint32_t g_scan_delay_ms;
extern uint8_t g_current_row;  /* 当前行通道 (0-15) */
extern uint8_t g_current_col;  /* 当前列通道 (0-15) */
extern OutputMode_t g_output_mode;      /* 输出模式：原始值/量化值 */
extern OutputFormat_t g_output_format;  /* 输出格式：简洁格式/表格格式 */
extern uint32_t g_quant_min;            /* 量化最小值 */
extern uint32_t g_quant_max;            /* 量化最大值 */
extern QuantLevel_t g_quant_level;      /* 量化档位：255 或 1023 */
extern volatile uint8_t g_stream_enabled; /* 是否允许流式传输，会话由 START/STOP 控制 */

/* Exported functions prototypes ---------------------------------------------*/
void USB_Command_Init(void);
void USB_Command_Process(uint8_t *buf, uint32_t len);
uint8_t USB_Command_Parse(const char *cmd);

#ifdef __cplusplus
}
#endif

#endif /* __USB_COMMAND_H */

