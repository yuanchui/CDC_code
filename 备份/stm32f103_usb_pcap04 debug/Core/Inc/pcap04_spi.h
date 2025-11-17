/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pcap04_spi.h
  * @brief   PCap04 SPI Communication Interface Header
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PCAP04_SPI_H
#define __PCAP04_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"

/* Exported constants --------------------------------------------------------*/
#define LOW  ((uint8_t)0)
#define HIGH ((uint8_t)1)

/* 模拟模式配置 - 可以通过预编译宏控制是否使用随机数据或真实SPI数据
 * 
 * 使用方法：
 * 1. 使用模拟模式（随机数据）- 默认：保持 USE_SIMULATION_MODE 定义
 * 2. 使用真实SPI数据：
 *    - 方法A：注释掉下面的 #define USE_SIMULATION_MODE 1
 *    - 方法B：在编译选项中添加 -DUSE_SIMULATION_MODE=0
 *    - 方法C：在项目预处理器定义中移除 USE_SIMULATION_MODE
 */
#ifndef USE_SIMULATION_MODE
#define USE_SIMULATION_MODE 0  /* 默认使用模拟模式（随机数据），设置为0或注释掉以使用真实SPI */
#endif

/* PCap04 Opcodes - 参考数据手册 Figure 101 */
#define POR           0x88
#define INIT          0x8A
#define CDC_START     0x8C
#define RDC_START     0x8E
#define DSP_TRIG      0x8D
#define NV_STORE      0x96
#define NV_RECALL     0x99
#define NV_ERASE      0x9C
#define TEST_READ     0x7E
#define WR_MEM        0xA0
#define RD_MEM        0x20
#define WR_CONFIG     0xA3
#define RD_CONFIG     0x23
#define RD_RESULT     0x40

/* Exported functions prototypes ---------------------------------------------*/
/* SPI 辅助函数 */
void PCap04_Set_IIC_EN(uint8_t enable);  /* 设置 IIC_EN 引脚：1=I2C, 0=SPI */
void Set_SSN(uint8_t level);             /* 设置 SPI 片选信号（软件控制） */

/* SPI 写操作函数 */
void Write_Opcode(uint8_t one_byte);
void Write_Opcode2(uint8_t byte1, uint8_t byte2);
void Write_Dword(uint8_t opcode, uint8_t address, uint32_t dword);
void Write_Dword_Auto_Incr(uint8_t opcode, uint8_t from_addr, uint32_t *dword_array, uint8_t to_addr);

/* SPI 读操作函数 */
uint32_t Read_Dword(uint8_t rd_opcode, uint8_t address);

/* PCap04 专用函数 */
HAL_StatusTypeDef PCap04_Memory_Access(uint8_t opcode, uint16_t address, uint8_t *byte, uint16_t size);
HAL_StatusTypeDef PCap04_Config_Access(uint8_t opcode, uint8_t address, uint8_t *byte, uint8_t size);
uint32_t PCap04_Read_Result(uint8_t rd_opcode, uint8_t address);

/* 随机数生成函数（用于模拟模式） */
void PCap04_Random_Init(void);                    /* 初始化随机数生成器 */
uint32_t PCap04_Generate_Random(uint32_t min, uint32_t max);  /* 生成指定范围的随机数 */

/* PCap04 状态检测函数 */
typedef struct {
  uint8_t communication_ok;      /* 通信是否正常：0=失败, 1=正常 */
  uint8_t is_initialized;        /* 是否已初始化：0=未初始化, 1=已初始化 */
  uint8_t is_simulation_mode;   /* 是否为模拟模式：0=真实模式, 1=模拟模式 */
  uint32_t config_reg0;          /* 配置寄存器0的值 */
  uint32_t config_reg1;          /* 配置寄存器1的值 */
  uint32_t result_reg0;          /* 结果寄存器0的值 */
  uint32_t result_reg1;          /* 结果寄存器1的值 */
} PCap04_Status_t;

PCap04_Status_t PCap04_Get_Status(void);  /* 获取PCap04状态信息 */

/* PCap04 通信测试函数 */
typedef struct {
  uint8_t test_result;      /* 测试结果：0=失败, 1=成功 */
  uint8_t received_value;   /* 接收到的值 */
  const char *error_msg;    /* 错误信息（如果失败） */
} PCap04_TestResult_t;

PCap04_TestResult_t PCap04_Test_Communication(void);  /* 测试PCap04通信 */

#ifdef __cplusplus
}
#endif

#endif /* __PCAP04_SPI_H */

