/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mux_control.h
  * @brief   CD74HC4067SM96 Multiplexer Control Header
  * 
  * CD74HC4067SM96 16选1多路复用器控制
  * 列选16选1: SY0-SY3 (PA3,PA4,PA6,PA7), ENX (PA5) - 用户说ENX在PA5
  * 行选16选1: SX0-SX3 (PB1,PB0,PB10,PB2), ENY (PB11) - 用户说ENY在PB11
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MUX_CONTROL_H
#define __MUX_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define MUX_CHANNELS 16

/* Exported functions prototypes ---------------------------------------------*/
void MUX_Init(void);
void MUX_Select_Column(uint8_t channel);  /* 列选16选1: 选择列 (0-15) */
void MUX_Select_Row(uint8_t channel);     /* 行选16选1: 选择行 (0-15) */
void MUX_Disable_Column(void);            /* 禁用列选 */
void MUX_Disable_Row(void);               /* 禁用行选 */

#ifdef __cplusplus
}
#endif

#endif /* __MUX_CONTROL_H */

