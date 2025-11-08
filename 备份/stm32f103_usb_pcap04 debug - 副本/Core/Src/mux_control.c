/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mux_control.c
  * @brief   CD74HC4067SM96 Multiplexer Control Implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mux_control.h"

/******************************************************************************/
/*                              MUX Initialization                            */
/******************************************************************************/
void MUX_Init(void)
{
  /* 初始化时禁用所有多路复用器 */
  MUX_Disable_Column();
  MUX_Disable_Row();
}

/******************************************************************************/
/*                           Column Multiplexer Select                        */
/******************************************************************************/
void MUX_Select_Column(uint8_t channel)
{
  /* 限制通道范围 */
  if(channel >= MUX_CHANNELS) {
    channel = MUX_CHANNELS - 1;
  }
  
  /* CD74HC4067SM96 需要4个选择信号: S0-S3 */
  /* 列选16选1: SY0-SY3 (PA3,PA4,PA6,PA7), ENX (PA5) - 使能信号 */
  /* 注意：用户说ENX在PA5，ENY在PB11 */
  
  /* 设置选择信号 (S0-S3) */
  HAL_GPIO_WritePin(SY0_GPIO_Port, SY0_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S0 */
  HAL_GPIO_WritePin(SY1_GPIO_Port, SY1_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S1 */
  HAL_GPIO_WritePin(SY2_GPIO_Port, SY2_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S2 */
  HAL_GPIO_WritePin(SY3_GPIO_Port, SY3_Pin, (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S3 */
  
  /* 使能列选多路复用器 (ENX在PA5，低电平使能) */
  HAL_GPIO_WritePin(ENY_GPIO_Port, ENY_Pin, GPIO_PIN_RESET);  /* 注意：CubeMX配置中ENY在PA5 */
}

/******************************************************************************/
/*                            Row Multiplexer Select                         */
/******************************************************************************/
void MUX_Select_Row(uint8_t channel)
{
  /* 限制通道范围 */
  if(channel >= MUX_CHANNELS) {
    channel = MUX_CHANNELS - 1;
  }
  
  /* 行选16选1: SX0-SX3 (PB1,PB0,PB10,PB2), ENY (PB11) - 使能信号 */
  
  /* 设置选择信号 (S0-S3) */
  HAL_GPIO_WritePin(SX0_GPIO_Port, SX0_Pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S0 */
  HAL_GPIO_WritePin(SX1_GPIO_Port, SX1_Pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S1 */
  HAL_GPIO_WritePin(SX2_GPIO_Port, SX2_Pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S2 */
  HAL_GPIO_WritePin(SX3_GPIO_Port, SX3_Pin, (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);  /* S3 */
  
  /* 使能行选多路复用器 (ENY在PB11，低电平使能) */
  HAL_GPIO_WritePin(ENX_GPIO_Port, ENX_Pin, GPIO_PIN_RESET);  /* 注意：CubeMX配置中ENX在PB11 */
}

/******************************************************************************/
/*                          Disable Column Multiplexer                       */
/******************************************************************************/
void MUX_Disable_Column(void)
{
  /* 禁用列选多路复用器 (ENX在PA5，高电平禁用) */
  /* 注意：用户说ENX在PA5，CubeMX配置中ENY在PA5，所以使用ENY_GPIO_Port和ENY_Pin */
  HAL_GPIO_WritePin(ENY_GPIO_Port, ENY_Pin, GPIO_PIN_SET);
}

/******************************************************************************/
/*                          Disable Row Multiplexer                          */
/******************************************************************************/
void MUX_Disable_Row(void)
{
  /* 禁用行选多路复用器 (ENY在PB11，高电平禁用) */
  /* 注意：CubeMX配置中ENX在PB11，但用户说ENY在PB11，所以使用ENX_GPIO_Port和ENX_Pin */
  HAL_GPIO_WritePin(ENX_GPIO_Port, ENX_Pin, GPIO_PIN_SET);
}

