/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SY0_Pin GPIO_PIN_3
#define SY0_GPIO_Port GPIOA
#define SY1_Pin GPIO_PIN_4
#define SY1_GPIO_Port GPIOA
#define ENY_Pin GPIO_PIN_5
#define ENY_GPIO_Port GPIOA
#define SY2_Pin GPIO_PIN_6
#define SY2_GPIO_Port GPIOA
#define SY3_Pin GPIO_PIN_7
#define SY3_GPIO_Port GPIOA
#define SX1_Pin GPIO_PIN_0
#define SX1_GPIO_Port GPIOB
#define SX0_Pin GPIO_PIN_1
#define SX0_GPIO_Port GPIOB
#define SX3_Pin GPIO_PIN_2
#define SX3_GPIO_Port GPIOB
#define SX2_Pin GPIO_PIN_10
#define SX2_GPIO_Port GPIOB
#define ENX_Pin GPIO_PIN_11
#define ENX_GPIO_Port GPIOB
#define SPI_SSN_Pin GPIO_PIN_12
#define SPI_SSN_GPIO_Port GPIOB
#define IIC_EN_Pin GPIO_PIN_8
#define IIC_EN_GPIO_Port GPIOA
#define USB_EN_Pin GPIO_PIN_15
#define USB_EN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/******************************************************************************/
/* 通信方式选择：一键切换 I2C 或 SPI                                          */
/******************************************************************************/
/* 
 * 【一键切换通信方式】
 * 只需修改下面的宏定义即可：
 * - USE_I2C_MODE = 1: 使用 I2C 通信方式
 * - USE_I2C_MODE = 0: 使用 SPI 通信方式
 * 
 * IIC_EN 引脚会自动配置：
 * - I2C 模式：IIC_EN = 高电平（使能）
 * - SPI 模式：IIC_EN = 低电平（禁用）
 */
#define USE_I2C_MODE  0  /* 0=SPI模式, 1=I2C模式 */

#if USE_I2C_MODE
  #define PCAP04_COMM_MODE_I2C  1
  #define PCAP04_COMM_MODE_SPI  0
#else
  #define PCAP04_COMM_MODE_I2C  0
  #define PCAP04_COMM_MODE_SPI  1
#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
