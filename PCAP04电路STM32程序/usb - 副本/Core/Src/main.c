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
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pcap04.h"
#include "cd74hc4067.h"
#include "usbd_cdc_if.h"
#include <stdio.h>  // For sprintf if needed (though USB_Printf handles formatting)
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
// These variables (Value, pcap04_use_float, integrated_data) are assumed to be
// declared as 'extern' in pcap04.h and defined in pcap04.c or main.c as globals.
// If not, you might need to declare them here or ensure they are properly linked.
// Example:
// uint32_t Value[4]; // If Value is not extern from pcap04.c
// extern int pcap04_use_float; // If pcap04_use_float is extern from pcap04.c
// extern float integrated_data(uint32_t raw_value); // If integrated_data is extern from pcap04.c
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// DWT delay function prototypes (should also be in main.h)
void DWT_Delay_Init(void);
void DWT_Delay_us(uint32_t us);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief Initializes the DWT Cycle Counter for microsecond delays.
  *        Must be called once before using DWT_Delay_us.
  *        Note: This implementation is common for STM32F1xx (Cortex-M3).
  *        The DWT->LAR register is not typically present or required to be unlocked
  *        for DWT access on these cores, hence its removal from the original suggestion.
  */
void DWT_Delay_Init(void) {
    /* Enable TRC (Trace Enable) in DEMCR */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* Reset DWT cycle counter */
    DWT->CYCCNT = 0;

    /* Enable DWT cycle counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
  * @brief Provides a microsecond delay using the DWT Cycle Counter.
  * @param us: Number of microseconds to delay.
  *        The accuracy depends on SystemCoreClock being correctly set.
  */
void DWT_Delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;
    // Calculate ticks for desired us delay. SystemCoreClock is in Hz.
    // Ticks per us = SystemCoreClock / 1,000,000
    // For example, if SystemCoreClock is 72MHz, then 72 ticks per microsecond.
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - startTick) < delayTicks);
}

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
  DWT_Delay_Init(); // Initialize DWT for microsecond delays
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  HAL_GPIO_WritePin(IIC_EN_GPIO_Port, IIC_EN_Pin, GPIO_PIN_RESET);
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

	// Initialize CD74HC4067 multiplexer
	MUX_Init();
	
	// Wait for USB enumeration to complete (longer delay to allow Windows to detect)
	HAL_Delay(2000);
	
	// Delay printf calls until USB is enumerated
	// Note: printf uses USB CDC which needs to be enumerated first
	
	if(PCap04_Test() != 1)
	{
		// Don't use printf here - USB may not be ready yet
		// You might want to blink an LED or use a UART for debug if USB fails
	}
	
	PCap04_Init_Tow();
	
	// Configure PCAP04 for mutual capacitance mode
	// PC0/PC1: Reference capacitance
	// PC2/PC3: Measurement capacitance
	// PC4/PC5: Disabled
//	PCap04_SetMutualCapacitanceMode(); // RESTORED: This line is crucial for PCAP04.
	HAL_Delay(50); // Small delay after configuration
	
	// Set float conversion flag (1=enabled, 0=disabled)
	PCap04_SetFloatConversion(1);  // Enable float conversion

	// Wait additional time for USB to be fully ready
	HAL_Delay(500);
	
	// Now it's safe to use USB_Printf after USB enumeration
	USB_Printf("=== Initializing PCAP04 and Multiplexer ===\r\n");
	MUX_PrintStatus(); // Show initial mux status
	USB_Printf("\n=== Starting PCAP04 measurement with multiplexer ===\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		USB_Printf("\n--- Starting 16x16 Matrix Scan (PC1 values) ---\r\n");
		
		int x, y;
		for (y = 0; y < 16; y++) // Iterate through Y-channels (rows)
		{
			for (x = 0; x < 16; x++) // Iterate through X-channels (columns)
			{
				// Select the current X and Y channels on the multiplexer
				MUX_SelectXY(x, y);
				
				// Short delay for multiplexer settling and PCAP04 to stabilize after channel switch
				// 2000 us (2 ms) is used here for stability. Adjust if needed.
				DWT_Delay_us(1000); 
				MUX_EnableBoth();
DWT_Delay_us(1000); 
				// Read PCAP04 data for all 4 channels
				// Assuming PCAP04_Read_CDC_Result_data triggers a conversion and returns the result.
				// We read all 4, but only print PC1 as requested.
//				Value[0] = PCAP04_Read_CDC_Result_data(0);
				Value[1] = PCAP04_Read_CDC_Result_data(1);
//				Value[2] = PCAP04_Read_CDC_Result_data(2);
//				Value[3] = PCAP04_Read_CDC_Result_data(3);
				MUX_DisableBoth();
				// Print only the PC1 measurement for the current (x, y) point
				if(pcap04_use_float)
				{
					// Display as floating point values (pF)
					USB_Printf("%.2f ", integrated_data(Value[1])); // Print PC1 value with a space
				}
				else
				{
					// Display as raw hex values
					USB_Printf("0x%08X ", Value[1]); // Print PC1 raw value with a space
				}
				
				// No DWT_Delay_us(100) here, to make the row print faster.
			}
			// After each row (16 columns) is printed, add a newline.
			USB_Printf("\r\n");
			// Add a small delay after printing a full row, to allow USB buffer to clear.
			
		}
		
		// After the 16x16 scan is complete
		USB_Printf("\n--- 16x16 Matrix Scan Complete (test) ---\r\n");
		
		// Long delay before starting the next full scan cycle
		HAL_Delay(2000); // Wait 5 seconds before repeating the entire scan (RESTORED)
	if(PCap04_Test() != 1)
	{
		USB_Printf("\n--- 1e232323---\r\n");
	}
	
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9; // PLL output 72MHz (8MHz HSE * 9)
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // System clock from PLL (72MHz)
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // HCLK = 72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1 = 36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2 = 72MHz

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5; // USB clock = 72MHz / 1.5 = 48MHz
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

