/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pcap04_spi.c
  * @brief   PCap04 SPI Communication Interface Implementation
  * 
  * @verbatim
  * SPI Timings:
  *   SSN enable to valid latch clock = min. 1.7us (2.6us)
  *   SSN hold time after SCK falling = min. 2.2us (3.6us)
  * 
  * Timing in between the bytes:
  *   Transmitting incremental = no additional gap (recommended usage)
  *   Receiving incremental = approx. 1us (recommended usage)
  * @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "pcap04_spi.h"
#include "spi.h"
#include "usbd_cdc_if.h"

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi2;

/* 随机数生成器状态（用于模拟PCap04数据） */
static uint32_t g_random_seed = 1;

/* Private function prototypes -----------------------------------------------*/
/* 随机数生成器函数 */
static uint32_t Random_Generate(void);
static void Random_Init(uint32_t seed);

/******************************************************************************/
/*                              Set IIC_EN Pin                                */
/******************************************************************************/
void PCap04_Set_IIC_EN(uint8_t enable)
{
  if(enable) {
    HAL_GPIO_WritePin(IIC_EN_GPIO_Port, IIC_EN_Pin, GPIO_PIN_SET);   /* 使能 I2C */
  } else {
    HAL_GPIO_WritePin(IIC_EN_GPIO_Port, IIC_EN_Pin, GPIO_PIN_RESET);  /* 禁用 I2C, 使能 SPI */
  }
}

/******************************************************************************/
/*                              Set SSN Pin                                   */
/******************************************************************************/
void Set_SSN(uint8_t level)
{
  /* 注意：STM32F103使用PB12作为SPI2_NSS，需要配置为软件控制 */
  /* 在gpio.c中将PB12配置为GPIO_OUTPUT，这里直接控制 */
  if(level == LOW) {
    HAL_GPIO_WritePin(SPI_SSN_GPIO_Port, SPI_SSN_Pin, GPIO_PIN_RESET);
  }
  if(level == HIGH) {
    HAL_GPIO_WritePin(SPI_SSN_GPIO_Port, SPI_SSN_Pin, GPIO_PIN_SET);
  }
}

/******************************************************************************/
/*                            Write one byte Opcode                           */
/******************************************************************************/
void Write_Opcode(uint8_t one_byte)
{
  uint8_t timeout = 10;
  
  /* SSN默认拉低使能，只需确保为LOW */
  Set_SSN(LOW);
  
  /* 发送操作码 */
  HAL_SPI_Transmit(&hspi2, &one_byte, 1, timeout); 
  
  /* SSN默认拉低使能，不需要拉高 */
}

/******************************************************************************/
/*                               Write two bytes                              */
/******************************************************************************/
void Write_Opcode2(uint8_t byte1, uint8_t byte2)
{
  uint8_t timeout = 10;
  uint8_t spiTX[2];

  spiTX[0] = byte1;
  spiTX[1] = byte2;
      
  /* SSN默认拉低使能，只需确保为LOW */
  Set_SSN(LOW);
  
  /* 发送两个字节 */
  HAL_SPI_Transmit(&hspi2, spiTX, 2, timeout);
  
  /* SSN默认拉低使能，不需要拉高 */
}

/******************************************************************************/
/*                         Write one data double word                         */
/******************************************************************************/
void Write_Dword(uint8_t opcode, uint8_t address, uint32_t dword)
{
  uint8_t timeout = 10;
  uint8_t spiTX[6];

  spiTX[0] = opcode;
  spiTX[1] = address;
  spiTX[2] = (uint8_t)(dword >> 24);
  spiTX[3] = (uint8_t)(dword >> 16);
  spiTX[4] = (uint8_t)(dword >> 8);
  spiTX[5] = (uint8_t)(dword);
      
  /* SSN默认拉低使能，只需确保为LOW */
  Set_SSN(LOW);
  
  /* 发送寄存器地址和数据 */
  HAL_SPI_Transmit(&hspi2, spiTX, 6, timeout);
  
  /* SSN默认拉低使能，不需要拉高 */
}

/******************************************************************************/
/*                    Write double words auto incrementally                   */
/******************************************************************************/
void Write_Dword_Auto_Incr(uint8_t opcode, uint8_t from_addr, uint32_t *dword_array, uint8_t to_addr)
{
  uint8_t timeout = 10;
  uint8_t spiTX[4];
  uint32_t temp_u32 = 0;
  uint8_t spiTX_addr[2];

  spiTX_addr[0] = opcode;
  spiTX_addr[1] = from_addr;

  /* 1. Put SSN low - Activate */
  Set_SSN(LOW);

  /* 2.a Transmit register address */
  HAL_SPI_Transmit(&hspi2, spiTX_addr, 2, timeout); 
    
  /* 2.b Transmit register address incrementally */
  for (int i = from_addr; i <= to_addr; i++) {
    temp_u32 = *dword_array;
    spiTX[0] = (uint8_t)(temp_u32 >> 24);
    spiTX[1] = (uint8_t)(temp_u32 >> 16);
    spiTX[2] = (uint8_t)(temp_u32 >> 8);
    spiTX[3] = (uint8_t)(temp_u32);

    HAL_SPI_Transmit(&hspi2, spiTX, 4, timeout);

    dword_array++;
  }
  
  /* SSN默认拉低使能，不需要拉高 */
}

/******************************************************************************/
/*                              Read double word                              */
/******************************************************************************/
uint32_t Read_Dword(uint8_t rd_opcode, uint8_t address)
{
  uint8_t timeout = 10;
  uint8_t spiTX[2];
  uint8_t spiRX[4];
  uint32_t temp_u32 = 0;
  
  spiTX[0] = rd_opcode;
  spiTX[1] = address;
  
  /* SSN默认拉低使能，只需确保为LOW */
  Set_SSN(LOW);
  
  /* 发送寄存器地址 */
  HAL_SPI_Transmit(&hspi2, spiTX, 2, timeout);
  
  /* 读取四个字节 */
  HAL_SPI_Receive(&hspi2, spiRX, 4, timeout);
  
  /* SSN默认拉低使能，不需要拉高 */
  
  /* Concatenate of bytes (from MSB to LSB) */
  temp_u32 = ((uint32_t)spiRX[0] << 24) | ((uint32_t)spiRX[1] << 16) | ((uint32_t)spiRX[2] << 8) | (uint32_t)spiRX[3];
  
  return temp_u32;
}

/******************************************************************************/
/*                         PCap04 Memory Access                              */
/******************************************************************************/
HAL_StatusTypeDef PCap04_Memory_Access(uint8_t opcode, uint16_t address, uint8_t *byte, uint16_t size)
{
  /* 将字节数组转换为双字数组 */
  uint32_t dword_array[137];  /* 最大548字节 = 137双字 */
  uint16_t dword_count;
  uint8_t i;
  
  if(size == 0 || size > 548) {
    return HAL_ERROR;
  }
  
  /* 计算需要的双字数量（向上取整） */
  dword_count = (size + 3) / 4;
  
  /* 将字节数组转换为双字数组（小端格式） */
  for(i = 0; i < dword_count; i++) {
    uint16_t byte_idx = i * 4;
    dword_array[i] = 0;
    
    if(byte_idx < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx]) << 0;
    }
    if(byte_idx + 1 < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx + 1]) << 8;
    }
    if(byte_idx + 2 < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx + 2]) << 16;
    }
    if(byte_idx + 3 < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx + 3]) << 24;
    }
  }
  
  /* 使用自动递增写入 */
  Write_Dword_Auto_Incr(opcode, (uint8_t)address, dword_array, (uint8_t)(address + dword_count - 1));
  
  return HAL_OK;
}

/******************************************************************************/
/*                         PCap04 Config Access                               */
/******************************************************************************/
HAL_StatusTypeDef PCap04_Config_Access(uint8_t opcode, uint8_t address, uint8_t *byte, uint8_t size)
{
  /* 将字节数组转换为双字数组 */
  uint32_t dword_array[13];  /* 配置寄存器通常是13个双字 */
  uint8_t dword_count;
  uint8_t i;
  
  if(size == 0 || size > 52) {
    return HAL_ERROR;
  }
  
  /* 计算需要的双字数量（向上取整） */
  dword_count = (size + 3) / 4;
  
  /* 将字节数组转换为双字数组（小端格式） */
  for(i = 0; i < dword_count && i < 13; i++) {
    uint8_t byte_idx = i * 4;
    dword_array[i] = 0;
    
    if(byte_idx < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx]) << 0;
    }
    if(byte_idx + 1 < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx + 1]) << 8;
    }
    if(byte_idx + 2 < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx + 2]) << 16;
    }
    if(byte_idx + 3 < size) {
      dword_array[i] |= ((uint32_t)byte[byte_idx + 3]) << 24;
    }
  }
  
  /* 使用自动递增写入 */
  Write_Dword_Auto_Incr(opcode, address, dword_array, (uint8_t)(address + dword_count - 1));
  
  return HAL_OK;
}

/******************************************************************************/
/*                         Random Number Generator                             */
/******************************************************************************/
/**
  * @brief  简单的线性同余生成器（LCG）用于生成伪随机数
  * @note   公式: seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF
  * @retval 随机数（0 到 2^31-1）
  */
static uint32_t Random_Generate(void)
{
  /* 线性同余生成器 (LCG) */
  /* 使用公式: (a * seed + c) mod m */
  /* 其中 a = 1103515245, c = 12345, m = 2^31 */
  g_random_seed = ((uint64_t)g_random_seed * 1103515245UL + 12345UL) & 0x7FFFFFFFUL;
  return g_random_seed;
}

/**
  * @brief  初始化随机数生成器种子
  * @param  seed: 种子值
  */
static void Random_Init(uint32_t seed)
{
  if(seed == 0) {
    seed = 1;  /* 确保种子不为0 */
  }
  g_random_seed = seed;
}

/**
  * @brief  生成指定范围内的随机数
  * @param  min: 最小值（包含）
  * @param  max: 最大值（包含）
  * @retval 范围内的随机数
  */
uint32_t PCap04_Generate_Random(uint32_t min, uint32_t max)
{
  uint32_t range;
  uint32_t random_val;
  
  if(max <= min) {
    return min;
  }
  
  range = max - min + 1;
  random_val = Random_Generate();
  
  /* 将随机数映射到指定范围 */
  return min + (random_val % range);
}

/**
  * @brief  初始化随机数生成器（外部调用）
  * @note   使用系统滴答计数器作为种子
  */
void PCap04_Random_Init(void)
{
  uint32_t seed = HAL_GetTick();  /* 使用系统滴答作为种子 */
  if(seed == 0) {
    seed = 1;  /* 确保种子不为0 */
  }
  Random_Init(seed);
}

/******************************************************************************/
/*                         PCap04 Read Result                                 */
/******************************************************************************/
/**
  * @brief  读取PCap04结果
  * @param  rd_opcode: 读取操作码
  * @param  address: 寄存器地址
  * @retval 电容值
  * 
  * @note   根据 USE_SIMULATION_MODE 宏定义选择：
  *         - 定义 USE_SIMULATION_MODE: 返回随机数据（模拟模式）
  *         - 未定义 USE_SIMULATION_MODE: 从SPI读取真实数据
  *         
  *         使用方法：
  *         - 使用模拟模式（默认）：保持 USE_SIMULATION_MODE 定义
  *         - 使用真实SPI：在编译选项中添加 -DUSE_SIMULATION_MODE=0 或注释掉定义
  */
uint32_t PCap04_Read_Result(uint8_t rd_opcode, uint8_t address)
{
#if (USE_SIMULATION_MODE != 0)
  /* 模拟模式：返回随机数据而不是真实的SPI读取 */
  /* 模拟真实的电容值范围：5000 - 95000 */
  return PCap04_Generate_Random(5000, 95000);
#else
  /* 真实模式：从SPI读取PCap04的实际数据 */
  return Read_Dword(rd_opcode, address);
#endif
}

/******************************************************************************/
/*                         PCap04 Get Status                                 */
/******************************************************************************/
/**
  * @brief  获取PCap04状态信息
  * @retval PCap04状态结构体
  * 
  * @note   此函数会尝试读取PCap04的配置寄存器来判断通信是否正常
  */
PCap04_Status_t PCap04_Get_Status(void)
{
  PCap04_Status_t status = {0};
  
#if (USE_SIMULATION_MODE != 0)
  /* 模拟模式 */
  status.is_simulation_mode = 1;
  status.communication_ok = 1;  /* 模拟模式下认为通信正常 */
  status.is_initialized = 1;     /* 模拟模式下认为已初始化 */
  status.config_reg0 = 0x1D0010;  /* 模拟配置寄存器0的值 */
  status.config_reg1 = 0x10000F;  /* 模拟配置寄存器1的值 */
  status.result_reg0 = PCap04_Generate_Random(5000, 95000);  /* 模拟结果寄存器0 */
  status.result_reg1 = PCap04_Generate_Random(5000, 95000);  /* 模拟结果寄存器1 */
#else
  /* 真实模式：尝试读取配置寄存器来判断通信是否正常 */
  status.is_simulation_mode = 0;
  
  /* 尝试读取配置寄存器0来判断通信是否正常 */
  /* 注意：如果读取失败，Read_Dword可能会返回0xFFFFFFFF或0 */
  status.config_reg0 = Read_Dword(RD_CONFIG, 0x00);
  status.config_reg1 = Read_Dword(RD_CONFIG, 0x01);
  
  /* 尝试读取结果寄存器 */
  status.result_reg0 = Read_Dword(RD_RESULT, 0x00);
  status.result_reg1 = Read_Dword(RD_RESULT, 0x01);
  
  /* 检测通信状态和初始化状态 */
  /* 0xFFFFFFFF 通常表示读取失败（SPI未响应或未初始化） */
  /* 0x00000000 也可能表示未初始化或配置为0 */
  /* 正常情况下，配置寄存器应该有特定的非全0/全1值 */
  if(status.config_reg0 == 0xFFFFFFFF || status.config_reg0 == 0x00000000) {
    /* 读取到全1或全0，可能是未初始化或通信失败 */
    status.communication_ok = 0;
    status.is_initialized = 0;
  } else {
    /* 读取到有效值，认为通信正常 */
    status.communication_ok = 1;
    /* 检查是否为有效的初始化值（根据标准配置，Config Reg0应该类似0x1D0010） */
    /* 如果值在合理范围内（不是0xFFFFFFFF且不是0x00000000），认为已初始化 */
    if(status.config_reg0 != 0xFFFFFFFF && status.config_reg0 != 0x00000000) {
      status.is_initialized = 1;
    } else {
      status.is_initialized = 0;
    }
  }
#endif
  
  return status;
}

/******************************************************************************/
/*                         PCap04 Communication Test                        */
/******************************************************************************/
/**
  * @brief  测试PCap04通信
  * @retval 测试结果结构体
  * 
  * @note   此函数通过发送TEST_READ操作码(0x7E)来测试通信
  *         如果通信正常，PCap04应该返回0x11
  *         如果SPI未初始化或通信失败，返回其他值或超时
  */
PCap04_TestResult_t PCap04_Test_Communication(void)
{
  PCap04_TestResult_t result = {0};
  
#if (USE_SIMULATION_MODE != 0)
  /* 模拟模式：总是返回成功 */
  result.test_result = 1;
  result.received_value = 0x11;  /* 模拟正常返回值 */
  result.error_msg = NULL;
#else
  /* 真实模式：实际测试SPI通信 */
  uint8_t test_data = TEST_READ;  /* 0x7E */
  uint8_t received_data = 0x00;
  HAL_StatusTypeDef hal_status;
  uint8_t timeout = 100;  /* 100ms超时 */
  
  /* SSN默认拉低使能，只需确保为LOW */
  Set_SSN(LOW);
  HAL_Delay(2);  /* 等待稳定 */
  
  /* 发送TEST_READ操作码并同时接收响应 */
  hal_status = HAL_SPI_TransmitReceive(&hspi2, &test_data, &received_data, 1, timeout);
  
  /* SSN默认拉低使能，不需要拉高 */
  
  
  /* 检查结果 */
  if(hal_status == HAL_OK) {
    /* 如果接收到0x11，说明通信正常 */
    if(received_data == 0x11) {
      result.test_result = 1;
      result.received_value = received_data;
      result.error_msg = NULL;
    } else {
      /* 接收到其他值，可能是SPI未初始化或通信异常 */
      result.test_result = 0;
      result.received_value = received_data;
      result.error_msg = "Received unexpected value (expected 0x11)";
    }
  } else {
    /* HAL_SPI调用失败，可能是SPI未初始化或硬件故障 */
    result.test_result = 0;
    result.received_value = received_data;
    switch(hal_status) {
      case HAL_TIMEOUT:
        result.error_msg = "SPI timeout - SPI may not be initialized";
        break;
      case HAL_ERROR:
        result.error_msg = "SPI error - Hardware fault or SPI not initialized";
        break;
      case HAL_BUSY:
        result.error_msg = "SPI busy - Resource conflict";
        break;
      default:
        result.error_msg = "SPI communication failed";
        break;
    }
  }
#endif
  
  return result;
}

