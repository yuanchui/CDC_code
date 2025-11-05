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
#define IIC_EN_Pin GPIO_PIN_8
#define IIC_EN_GPIO_Port GPIOA
#define USB_EN_Pin GPIO_PIN_15
#define USB_EN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
