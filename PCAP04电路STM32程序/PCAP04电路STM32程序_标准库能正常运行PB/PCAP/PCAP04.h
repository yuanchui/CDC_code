#ifndef __PCAP04_H__
#define __PCAP04_H__
 
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pcap.h"
#include "w25q16.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

  // 引脚定义
  
  // CS--PB7
  #define PCAP04_CS_PORT            GPIOB
  #define PCAP04_CS_PIN             GPIO_Pin_12
  #define PCAP04_CS_H()             GPIO_SetBits(PCAP04_CS_PORT, PCAP04_CS_PIN)
  #define PCAP04_CS_L()             GPIO_ResetBits(PCAP04_CS_PORT, PCAP04_CS_PIN)
  
  #define PCap04_ReadByte        VSPI_ReadByte
  #define PCap04_WriteByte       VSPI_SendByte

  // 常量定义
  #define C_DotParm                 0.000000007450580596923828125   // 为pow(2,-27)的值
  #define R_DotParm					0.0000000298023223876953125		// 为pow(2,-25)的值
  #define C_Factor                 	10   		// 电容缩放因子
  #define R_Factor					1.339		// 电阻缩放因子
  
  // 常量
  #define PCAP04_TEST_CORRECT       0x11            // 测试读返回正确代码
  // 功能码
  #define PCAP04_POR   				      0x88            // 系统复位代码
  #define PCAP04_INIT   			      0x8A            // 芯片初始化代码
  #define PCAP04_CDC_START   	      0x8C            // 启动CDC功能代码
  #define PCAP04_RDC_START          0x8E            // 启动RDC功能代码
  #define PCAP04_TEST_READ          0x7E            // 测试代码
  #define PCAP04_READ_RESULT        0x40            // 读芯片转换结果代码
  #define PCAP04_WRITE_FIRMWARE     0xA0            // 写固件代码

  
  
  // 函数声明
  void PCAP04_Config(void);
  void PCAP04_Init(void);
  u8 PCAP04_ReadReg(u8 add);
  void PCAP04_WriteFirmware(void);
  double PCAP04_Read_CDC_Result(void);
  double PCAP04_Read_RDC_Result(void);
  void PCAP04_ReadFirmware(void);
  void PCAP04_WriteConfig(void);
  void PCAP04_ReadConfig(void);
  void PCAP04_Read_Result(void);
 u32 PCAP04_Read_CDC_Result_data(int data);
 void PCAP04_Readjcq(void);
 u32 PCAP04_Read_RDC_Result_data(void);
 void printf_integrated(void);
 double integrated_data(u32 data);
 char PCap04_Test(void);
#endif
