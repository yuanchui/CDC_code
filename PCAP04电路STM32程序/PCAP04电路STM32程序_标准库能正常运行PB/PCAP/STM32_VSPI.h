
#ifndef __STM32_VSPI_H__
#define __STM32_VSPI_H__

#include "pcap.h"
#include "stm32f10x_conf.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "STM32_VSPI.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

  // PB14--MOSI,=1
  #define VSPI_MOSI_PORT      GPIOB
  #define VSPI_MOSI_PIN       GPIO_Pin_15
  #define VSPI_MOSI_H()       GPIO_SetBits(VSPI_MOSI_PORT,   VSPI_MOSI_PIN)   
  #define VSPI_MOSI_L()       GPIO_ResetBits(VSPI_MOSI_PORT, VSPI_MOSI_PIN)  

  // PB13--SCK,=1
  #define VSPI_SCL_PORT       GPIOB		//GPIOD
  #define VSPI_SCL_PIN        GPIO_Pin_13	//GPIO_Pin_2
  #define VSPI_SCL_H()        GPIO_SetBits(VSPI_SCL_PORT,   VSPI_SCL_PIN)    
  #define VSPI_SCL_L()        GPIO_ResetBits(VSPI_SCL_PORT, VSPI_SCL_PIN)   

  //PB15--MISO
  #define VSPI_MISO_PORT      GPIOB
  #define VSPI_MISO_PIN       GPIO_Pin_14
  #define VSPI_MISO_READ()    GPIO_ReadInputDataBit(VSPI_MISO_PORT, VSPI_MISO_PIN) 
  
  // º¯ÊýAddress_Read
  void VSPI_Configuration(void); 
  u8   VSPI_ReadByte(void);
  void VSPI_SendByte(u8 sdata);  
  u8   VSPI_ReadFirstByte(void);                          
  void VSPI_ReadData(u16 addr, u16 byte_cnt, u8 * d_data);	 
  void VSPI_WriteData(u16 addr, u16 byte_cnt, u8 * s_data);  
  u8   VSPI_ReadStatus(void);
  void VSPI_Send16Byte(u16  sdata);
	  
#endif
















