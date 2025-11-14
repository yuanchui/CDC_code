/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pcap04.h
  * @brief          : Header for pcap04.c file.
  *                   PCAP04 capacitance-to-digital converter driver
  ******************************************************************************
  * @attention
  *
  * PCAP04 is a capacitance measurement IC with SPI interface
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PCAP04_H
#define __PCAP04_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* PCAP04 Opcodes */
#define PCAP04_I2C          0x28
#define REG                 0x44
#define WR_MEM              0xA0
#define WR_CONFIG           0xA3
#define POR                 0x88
#define INITIALIZE          0x8A
#define CDC_START           0x8C
#define RDC_START           0x8E
#define NV_STORE            0x96
#define TEST                0x7E
#define PCAP04_READ_RESULT  0x40

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern uint32_t Value[6];
extern uint8_t pcap04_use_float;  // Flag to enable float conversion (1=enabled, 0=disabled)

/* Exported functions prototypes ---------------------------------------------*/
char PCap04_Test(void);
void PCap04_PowerON_RESET(void);
void PCap04_CDCStart(void);
void PCap04_INITIALIZE(void);
void PCap04_Init(void);
void PCAP04_WriteFirmware(void);
void PCap04_Init_Tow(void);
uint32_t PCAP04_Read_CDC_Result_data(int Nun);
double integrated_data(uint32_t data);
void PCap04_SetMutualCapacitanceMode(void);
void PCap04_SetFloatConversion(uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif /* __PCAP04_H */

