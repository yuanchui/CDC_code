/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim1;

/* TIM1 init function */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  // Configure for 5ms interrupt: SystemClock = 72MHz, Prescaler = 7200-1, Period = 50-1
  // Timer frequency = 72MHz / 7200 / 50 = 20Hz = 50ms period
  // For 5ms: Prescaler = 720-1, Period = 500-1
  htim1.Init.Prescaler = 720 - 1;  // 72MHz / 720 = 100kHz
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 500 - 1;     // 100kHz / 500 = 200Hz = 5ms period
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  // Configure for base timer with interrupt
  // No need for OC and break/dead time configuration for interrupt mode
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
    
    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();
    
    /* TIM1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Start TIM1 with interrupt
  */
void TIM1_Start(void)
{
  HAL_TIM_Base_Start_IT(&htim1);
}

/**
  * @brief  Stop TIM1
  */
void TIM1_Stop(void)
{
  HAL_TIM_Base_Stop_IT(&htim1);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    extern USB_SystemStatus_t g_usb_status;
    
    // Increment timer counter
    g_usb_status.timer_counter++;
    
    // Check if scan should be triggered
    if (g_usb_status.is_running && 
        g_usb_status.timer_counter >= g_usb_status.timer_threshold) {
      g_usb_status.timer_counter = 0;
      
      // Trigger scan (will be handled in main loop or callback)
      // This flag can be checked in main loop
    }
  }
}

/* USER CODE END 1 */
