/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pcap04_spi.h"
#include "mux_control.h"
#include "matrix_scan.h"
#include "usbd_cdc_if.h"
#include "usb_command.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* PCap04 固件数据 (Standard Firmware - 548 bytes) */
uint8_t standard_fw[548] = {
  0x24, 0x05, 0xA0, 0x01, 0x20, 0x55, 0x42, 0x5C, 0x48, 0xB1, 0x07, 0x92, 0x02, 0x20, 0x13, 0x02,
  0x20, 0x93, 0x02, 0xB2, 0x02, 0x78, 0x20, 0x54, 0xB3, 0x06, 0x91, 0x00, 0x7F, 0x20, 0x86, 0x20,
  0x54, 0xB6, 0x03, 0x72, 0x62, 0x20, 0x54, 0xB7, 0x00, 0x00, 0x42, 0x5C, 0xA1, 0x00, 0x49, 0xB0,
  0x00, 0x49, 0x40, 0xAB, 0x5D, 0x92, 0x1C, 0x90, 0x02, 0x7F, 0x20, 0x86, 0x66, 0x67, 0x76, 0x77,
  0x66, 0x7A, 0xCF, 0xCD, 0xE6, 0x43, 0xF1, 0x44, 0x29, 0xE0, 0x7A, 0xDC, 0xE7, 0x41, 0x32, 0xAA,
  0x01, 0x99, 0xFD, 0x7B, 0x01, 0x7A, 0xCF, 0xEB, 0xE6, 0x43, 0xF1, 0x44, 0x29, 0xE0, 0x7A, 0xC1,
  0xE7, 0x41, 0x32, 0x6A, 0xDE, 0x44, 0x7A, 0xCF, 0xEA, 0xE6, 0x43, 0xF1, 0x44, 0x29, 0xE0, 0x6A,
  0xDF, 0x44, 0x7A, 0xC4, 0xE7, 0x41, 0x32, 0xAB, 0x05, 0x7A, 0xC1, 0xE1, 0x43, 0xE0, 0x3A, 0x7A,
  0xC0, 0xE1, 0x43, 0xE0, 0x3A, 0x02, 0x7A, 0xCF, 0xE6, 0xE6, 0x43, 0xF1, 0x44, 0x29, 0xE0, 0x7A,
  0xEF, 0x44, 0x02, 0x20, 0x9D, 0x84, 0x01, 0x21, 0x2E, 0x21, 0x74, 0x20, 0x37, 0xC8, 0x7A, 0xE7,
  0x43, 0x49, 0x11, 0x6A, 0xD4, 0x44, 0x7A, 0xC1, 0xD8, 0xE6, 0x43, 0xE9, 0x44, 0x1C, 0x43, 0x13,
  0xAB, 0x63, 0x6A, 0xDE, 0x41, 0xAB, 0x0B, 0x46, 0x46, 0x46, 0x7A, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xE3, 0x41, 0x32, 0x1C, 0x44, 0xE9, 0x13, 0x6A, 0xD4, 0x13, 0x41, 0xAA, 0xDF, 0x7A, 0xC5, 0xE1,
  0x43, 0x49, 0xE0, 0x34, 0x7A, 0xCF, 0xE3, 0xE6, 0x43, 0xF1, 0x44, 0x29, 0xE0, 0xDB, 0xC0, 0x27,
  0xE5, 0x6A, 0xDF, 0x43, 0x7A, 0xC8, 0xE7, 0x41, 0x30, 0xAB, 0x03, 0x86, 0x01, 0x92, 0x37, 0x7A,
  0xC6, 0xE7, 0x41, 0x7A, 0xFA, 0xE7, 0x43, 0xEA, 0x44, 0x7A, 0xC1, 0xE1, 0xE6, 0x43, 0xE9, 0x44,
  0x25, 0xE0, 0x7A, 0xC6, 0xE7, 0x41, 0x7A, 0xFA, 0xE7, 0x43, 0xEA, 0x44, 0x7A, 0xC0, 0xE7, 0x43,
  0xE9, 0x44, 0x25, 0xE0, 0x92, 0x10, 0x7A, 0xE1, 0x44, 0xE2, 0x44, 0xE3, 0x44, 0xE4, 0x44, 0xE5,
  0x44, 0xE6, 0x44, 0xE7, 0x44, 0xE8, 0x44, 0xC1, 0xD8, 0x24, 0x3E, 0x92, 0xFF, 0x02, 0x7A, 0xCF,
  0xD7, 0xE6, 0x43, 0xF1, 0x44, 0x7A, 0xD0, 0xE7, 0x43, 0x2A, 0x2A, 0x32, 0xAB, 0x03, 0x42, 0x5C,
  0x92, 0x03, 0x7A, 0xC0, 0xE1, 0x43, 0xD9, 0x27, 0x90, 0x6A, 0xDF, 0x43, 0x7A, 0xC8, 0xE7, 0x41,
  0x32, 0xAB, 0x03, 0x86, 0x01, 0x92, 0x11, 0x7A, 0xC2, 0x43, 0x7A, 0xE7, 0x44, 0x6A, 0xC6, 0x44,
  0x7A, 0xC3, 0x43, 0x7A, 0xE8, 0x44, 0x6A, 0xC7, 0x44, 0xC1, 0xD4, 0x24, 0x57, 0x7A, 0xC8, 0xE1,
  0x43, 0xE0, 0x3A, 0x02, 0x7A, 0xCF, 0xE7, 0xE6, 0x43, 0xF1, 0x44, 0x29, 0xE0, 0x7A, 0xC7, 0xE1,
  0x41, 0x6A, 0xD4, 0x45, 0x5A, 0x25, 0x36, 0x46, 0x46, 0x46, 0x46, 0x7A, 0xE9, 0x44, 0x7A, 0xC0,
  0xE7, 0x43, 0x55, 0x7A, 0xEA, 0x45, 0x7A, 0xE9, 0x51, 0x1C, 0x43, 0x6A, 0xCA, 0x44, 0x1D, 0x43,
  0x6A, 0xCB, 0x44, 0x7A, 0xC1, 0xCA, 0xE6, 0x43, 0xE9, 0x44, 0x7A, 0xC1, 0xE1, 0x43, 0x7A, 0xCC,
  0xE0, 0xE6, 0x41, 0x2C, 0x42, 0x7A, 0xC5, 0xE1, 0x43, 0x49, 0xE0, 0x34, 0x7A, 0xC1, 0xCC, 0xE6,
  0x43, 0xE9, 0x44, 0x7A, 0xC1, 0xE1, 0x43, 0x2C, 0x70, 0x7A, 0xCC, 0x43, 0x7A, 0xCF, 0x44, 0x7A,
  0xCD, 0x43, 0x7A, 0xCE, 0x44, 0x6A, 0xCA, 0x43, 0xC1, 0xCA, 0x7A, 0xE6, 0x41, 0xE9, 0x45, 0x2B,
  0xAE, 0xEE, 0x44, 0x7A, 0xC1, 0xCA, 0xE6, 0x43, 0xE9, 0x44, 0x7A, 0xC1, 0xE1, 0x43, 0x7A, 0xCC,
  0xEC, 0xE6, 0x41, 0x2C, 0x42, 0x7A, 0xC5, 0xE1, 0x43, 0x49, 0xE0, 0x34, 0x7A, 0xC1, 0xCC, 0xE6,
  0x43, 0xE9, 0x44, 0x7A, 0xC1, 0xE1, 0x43, 0x2C, 0x70, 0x7A, 0xCC, 0x43, 0x7A, 0xCF, 0x44, 0x7A,
  0xCD, 0x43, 0x7A, 0xCE, 0x44, 0x6A, 0xCB, 0x43, 0xC1, 0xCA, 0x7A, 0xE6, 0x41, 0xE9, 0x45, 0x2B,
  0xAE, 0xED, 0x44, 0x02
};

/* PCap04 配置数据 (Standard Configuration - 52 bytes) */
uint8_t standard_cfg_bytewise[52] = {
  0x1D, 0x00, 0x58, 0x10,
  0x10, 0x00, 0x0F, 0x20,
  0x00, 0xD0, 0x07, 0x00,
  0x00, 0x08, 0xFF, 0x03,
  0x00, 0x24, 0x00, 0x00,
  0x00, 0x01, 0x50, 0x30,
  0x73, 0x04, 0x50, 0x08,
  0x5A, 0x00, 0x82, 0x08,
  0x08, 0x00, 0x47, 0x40,
  0x00, 0x00, 0x00, 0x71,
  0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00
};

/* 矩阵数据 */
MatrixData_t matrix_data;
HAL_StatusTypeDef ret;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
  /* 配置IIC_EN引脚：根据USE_I2C_MODE宏自动设置 */
#if PCAP04_COMM_MODE_I2C
  PCap04_Set_IIC_EN(1);  /* I2C模式：高电平使能 */
#else
  PCap04_Set_IIC_EN(0);  /* SPI模式：低电平使能 */
#endif
  
  /* 等待USB连接 */
  HAL_Delay(1000);
  
  /* 初始化多路复用器 */
  Matrix_Scan_Init();
  
  /* PCap04 初始化序列 */
  /* 1. Power-On Reset (POR) */
  Write_Opcode(POR);
  HAL_Delay(500);
  
  /* 2. Initialize (INIT) */
  Write_Opcode(INIT);
  HAL_Delay(100);
  
  /* 3. 写入固件 */
  ret = PCap04_Memory_Access(WR_MEM, 0x00, standard_fw, 548);
  if(ret != HAL_OK) {
    /* 错误处理 */
  }
  HAL_Delay(100);
  
  /* 4. 写入配置 */
  ret = PCap04_Config_Access(WR_CONFIG, 0x00, standard_cfg_bytewise, 52);
  if(ret != HAL_OK) {
    /* 错误处理 */
  }
  HAL_Delay(100);
  
  /* 5. 再次初始化 */
  Write_Opcode(INIT);
  HAL_Delay(100);
  
  /* 初始化USB命令处理 */
  USB_Command_Init();
  
#if (USE_SIMULATION_MODE != 0)
  /* 初始化随机数生成器（用于模拟PCap04数据） */
  PCap04_Random_Init();
  
  /* USB输出初始化完成信息 */
  {
    uint8_t msg[] = "PCap04 Initialization Complete!\r\n"
                    "16x16 Matrix Scan Ready...\r\n"
                    "Note: Running in SIMULATION MODE (random data generation)\r\n"
                    "Type 'HELP' or '?' for available commands.\r\n\r\n";
    CDC_Transmit_FS(msg, sizeof(msg)-1);
    HAL_Delay(100);
  }
#else
  /* USB输出初始化完成信息 */
  {
    uint8_t msg[] = "PCap04 Initialization Complete!\r\n"
                    "16x16 Matrix Scan Ready...\r\n"
                    "Running in REAL MODE (SPI data acquisition)\r\n"
                    "Type 'HELP' or '?' for available commands.\r\n\r\n";
    CDC_Transmit_FS(msg, sizeof(msg)-1);
    HAL_Delay(100);
  }
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    static uint8_t session_open = 0;
    static uint32_t last_scan_ms = 0;
    static uint8_t scanning_in_progress = 0;

    /* 会话开始/结束标志管理（会话级别，由Matrix_Scan_And_Stream内部管理的START/END是数据级别的） */
    /* 注意：Matrix_Scan_And_Stream内部已经管理每次扫描的START/END，这里只管理会话状态 */
    if(g_stream_enabled && session_open == 0) {
      session_open = 1;
    } else if(!g_stream_enabled && session_open == 1) {
      session_open = 0;
    }

    /* 未启用流式传输则不传输 */
    if(!g_stream_enabled) {
      HAL_Delay(10);
      continue;
    }

    /* 根据工作模式执行扫描（非阻塞调度） */
    switch(g_work_mode) {
      case MODE_NORMAL:
        /* 普通模式：按设置速率循环扫描（非阻塞基于系统节拍） */
        if(!scanning_in_progress && (HAL_GetTick() - last_scan_ms >= g_scan_delay_ms)) {
          scanning_in_progress = 1;
          Matrix_Scan_And_Stream(&matrix_data);  /* 函数内部等待USB发送完成 */
          scanning_in_progress = 0;
          last_scan_ms = HAL_GetTick();
        }
        break;
        
      case MODE_FAST:
        /* 快速模式：尽可能快地扫描（避免阻塞延时） */
        if(!scanning_in_progress) {
          scanning_in_progress = 1;
          Matrix_Scan_And_Stream(&matrix_data);
          scanning_in_progress = 0;
        }
        break;
        
      case MODE_SINGLE:
        /* 单次模式：只扫描一次后停止；Matrix_Scan_And_Stream内部已经管理START/END */
        Matrix_Scan_And_Stream(&matrix_data);
        g_work_mode = MODE_STOP;  /* 扫描完成后停止 */
        g_stream_enabled = 0;      /* 禁用流式传输 */
        break;
        
      case MODE_STOP:
      default:
        /* 停止模式：不扫描，等待命令 */
        HAL_Delay(10);
        break;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
