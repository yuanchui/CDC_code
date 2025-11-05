/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : cd74hc4067.h
  * @brief          : Header for cd74hc4067.c file.
  *                   CD74HC4067 16-channel analog multiplexer driver
  ******************************************************************************
  * @attention
  *
  * CD74HC4067 is a 16-channel analog multiplexer/demultiplexer
  * Control pins: S0, S1, S2, S3 (4-bit binary selection)
  * Enable pins: ENX, ENY (active low)
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CD74HC4067_H
#define __CD74HC4067_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  Multiplexer operation modes
  */
typedef enum {
    MUX_MODE_DISABLED = 0,    // Both X and Y disabled
    MUX_MODE_X_ONLY,          // Only X channel enabled
    MUX_MODE_Y_ONLY,          // Only Y channel enabled
    MUX_MODE_XY_BOTH          // Both X and Y channels enabled
} MUX_Mode_t;

/**
  * @brief  Multiplexer status structure
  */
typedef struct {
    MUX_Mode_t mode;          // Current operation mode
    uint8_t x_channel;        // Current X channel (0-15)
    uint8_t y_channel;        // Current Y channel (0-15)
    uint8_t enx_state;        // ENX pin state (0=enabled, 1=disabled)
    uint8_t eny_state;        // ENY pin state (0=enabled, 1=disabled)
} MUX_Status_t;

/* Exported constants --------------------------------------------------------*/

#define MUX_CHANNEL_MAX     15    // Maximum channel number (0-15)

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

// Initialization
void MUX_Init(void);

// Channel selection functions
void MUX_SelectMode(MUX_Mode_t mode, uint8_t x_channel, uint8_t y_channel);
void MUX_SelectX(uint8_t channel);
void MUX_SelectY(uint8_t channel);
void MUX_SelectXY(uint8_t x_channel, uint8_t y_channel);

// Enable/Disable functions
void MUX_EnableX(void);
void MUX_DisableX(void);
void MUX_EnableY(void);
void MUX_DisableY(void);
void MUX_EnableBoth(void);
void MUX_DisableBoth(void);

// Scanning functions
void MUX_ScanRow(uint8_t row);           // Scan specific row (Y fixed, X scans 0-15)
void MUX_ScanColumn(uint8_t column);     // Scan specific column (X fixed, Y scans 0-15)
void MUX_ScanAll(void);                  // Scan all combinations (16x16 matrix)

// Status and utility functions
MUX_Status_t MUX_GetStatus(void);
void MUX_PrintStatus(void);
void MUX_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __CD74HC4067_H */
