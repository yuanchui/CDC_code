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
#include "pcap04.h"
#include "cd74hc4067.h"
#include "usbd_cdc_if.h"
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
  HAL_GPIO_WritePin(IIC_EN_GPIO_Port, IIC_EN_Pin, GPIO_PIN_RESET);
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(IIC_EN_GPIO_Port, IIC_EN_Pin, GPIO_PIN_RESET);
	// Initialize CD74HC4067 multiplexer
	MUX_Init();
	
	// Wait for USB enumeration to complete (longer delay to allow Windows to detect)
	HAL_Delay(2000);
	
	// Delay printf calls until USB is enumerated
	// Note: printf uses USB CDC which needs to be enumerated first
	
	if(PCap04_Test() != 1)
	{
		// Don't use printf here - USB may not be ready yet
		// printf("通讯失败\n");
	}
	
	PCap04_Init_Tow();
	
	// Wait additional time for USB to be fully ready
	HAL_Delay(500);
	
	// Now it's safe to use USB_Printf after USB enumeration
	// CD74HC4067 Multiplexer Test Examples
	USB_Printf("=== CD74HC4067 Multiplexer Test ===\r\n");
	
	// Test 1: Single channel selection
	USB_Printf("\n1. Testing single channel selection:\r\n");
	MUX_SelectMode(MUX_MODE_X_ONLY, 5, 0);
	HAL_Delay(500);
	MUX_SelectMode(MUX_MODE_Y_ONLY, 0, 10);
	HAL_Delay(500);
	
	// Test 2: Both channels
	USB_Printf("\n2. Testing both channels:\r\n");
	MUX_SelectMode(MUX_MODE_XY_BOTH, 3, 7);
	HAL_Delay(500);
	
	// Test 3: Print status
	USB_Printf("\n3. Current status:\r\n");
	MUX_PrintStatus();
	
	USB_Printf("\n=== Starting PCAP04 measurement with multiplexer ===\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		// Example 1: Scan specific row (Y=2, X scans 0-15)
		USB_Printf("\n--- Scanning Row 2 ---\r\n");
		MUX_ScanRow(2);
		HAL_Delay(2000);
		
		// Example 2: Scan specific column (X=5, Y scans 0-15) 
		USB_Printf("\n--- Scanning Column 5 ---\r\n");
		MUX_ScanColumn(5);
		HAL_Delay(2000);
		
		// Example 3: Select specific position and read PCAP04
		USB_Printf("\n--- Reading PCAP04 at position [8,12] ---\r\n");
		MUX_SelectXY(8, 12);
		HAL_Delay(100);  // Settling time
		
		Value[0] = PCAP04_Read_CDC_Result_data(0);
		HAL_Delay(100);
		Value[1] = PCAP04_Read_CDC_Result_data(1);
		HAL_Delay(100);			
		Value[2] = PCAP04_Read_CDC_Result_data(2);
		HAL_Delay(100);				
		Value[3] = PCAP04_Read_CDC_Result_data(3);
		HAL_Delay(100);				

		USB_Printf("Position [8,12] - PCAP04 Data:\r\n");
		USB_Printf("Channel 0: %X\r\n", Value[0]);
		USB_Printf("Channel 1: %X\r\n", Value[1]);
		USB_Printf("Channel 2: %X\r\n", Value[2]);
		USB_Printf("Channel 3: %X\r\n", Value[3]);
		
		HAL_Delay(3000);
		
		// Example 4: Show current multiplexer status
		MUX_PrintStatus();
		HAL_Delay(2000);
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
