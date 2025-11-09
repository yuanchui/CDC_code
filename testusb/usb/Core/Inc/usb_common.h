/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_common.h
  * @brief          : USB Command/Control Protocol System
  *                   Unified command parsing and execution framework
  ******************************************************************************
  * @attention
  *
  * USB Command Protocol System for STM32F103 + USB2.0 + CD74HC4067 + PCAP04
  * 
  * Features:
  * - Unified command parsing with && support
  * - Queue execution with WAIT support
  * - Extensible command structure
  * - Status feedback system
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USB_COMMON_H
#define __USB_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  System operation mode
  */
typedef enum {
    USB_MODE_STOPPED = 0,      // Stopped
    USB_MODE_SINGLE_SCAN,       // Single scan mode
    USB_MODE_NORMAL,            // Normal continuous scan
    USB_MODE_FAST               // Fast continuous scan
} USB_SystemMode_t;

/**
  * @brief  Command execution result
  */
typedef enum {
    CMD_RESULT_OK = 0,          // Success
    CMD_RESULT_ERROR,            // General error
    CMD_RESULT_INVALID_PARAM,    // Invalid parameter
    CMD_RESULT_UNKNOWN_CMD,      // Unknown command
    CMD_RESULT_BUSY              // System busy
} CMD_Result_t;

/**
  * @brief  System status structure
  */
typedef struct {
    USB_SystemMode_t mode;       // Current operation mode
    uint32_t scan_rate_ms;       // Scan rate in milliseconds
    uint8_t current_row;         // Current scan row (0-15)
    uint8_t current_col;         // Current scan column (0-15)
    uint8_t matrix_rows;         // Matrix rows (max 16)
    uint8_t matrix_cols;         // Matrix columns (max 16)
    bool is_running;             // Is scanning active
    uint32_t timer_counter;      // Timer interrupt counter
    uint32_t timer_threshold;    // Timer threshold for scan trigger
} USB_SystemStatus_t;

/* Exported constants --------------------------------------------------------*/

#define USB_CMD_MAX_LEN           256     // Maximum command length
#define USB_CMD_QUEUE_SIZE        32      // Maximum queue size
#define USB_CMD_PARAM_MAX         8       // Maximum parameters per command
#define USB_MATRIX_MAX_SIZE       16      // Maximum matrix size

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern USB_SystemStatus_t g_usb_status;

/* Exported functions prototypes ---------------------------------------------*/

// Initialization
void USB_Common_Init(void);

// Command processing
CMD_Result_t USB_ProcessCommand(const char *cmd_line);
CMD_Result_t USB_ProcessCommandQueue(void);

// System control
void USB_SetMode(USB_SystemMode_t mode);
void USB_SetScanRate(uint32_t rate_ms);
void USB_Start(void);
void USB_Stop(void);
void USB_SingleScan(void);

// Channel control
void USB_SetRow(uint8_t row);
void USB_SetCol(uint8_t col);
uint8_t USB_GetRow(void);
uint8_t USB_GetCol(void);
void USB_ScanPoint(uint8_t row, uint8_t col);

// Status and info
USB_SystemStatus_t USB_GetStatus(void);
void USB_PrintStatus(void);
void USB_PrintHelp(void);
void USB_PrintMatrixInfo(void);
bool USB_CheckScanTrigger(void);  // Check if scan should be triggered (call from main loop)

// Response formatting
void USB_SendOK(const char *cmd);
void USB_SendError(uint8_t code, const char *info);
void USB_SendStatus(void);

// Queue management
void USB_QueueStart(void);
void USB_QueueEnd(void);
void USB_QueueWait(uint32_t ms);
bool USB_IsQueueActive(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_COMMON_H */

