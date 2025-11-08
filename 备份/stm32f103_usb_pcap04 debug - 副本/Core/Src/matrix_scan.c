/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    matrix_scan.c
  * @brief   16x16 Capacitance Matrix Scan Implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "matrix_scan.h"
#include "pcap04_spi.h"
#include "mux_control.h"
#include "usbd_cdc_if.h"
#include "usb_command.h"
#include <string.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/

/******************************************************************************/
/*                           Matrix Scan Initialization                      */
/******************************************************************************/
void Matrix_Scan_Init(void)
{
  /* 初始化多路复用器 */
  MUX_Init();
  
  /* 禁用所有多路复用器 */
  MUX_Disable_Column();
  MUX_Disable_Row();
}

/******************************************************************************/
/*                          Scan Single Point                                */
/******************************************************************************/
uint32_t Matrix_Scan_Point(uint8_t row, uint8_t col)
{
  uint32_t result = 0;
  
  /* 限制行列范围 */
  if(row >= MATRIX_SIZE) row = MATRIX_SIZE - 1;
  if(col >= MATRIX_SIZE) col = MATRIX_SIZE - 1;
  
  /* 选择行列 */
  MUX_Select_Row(row);
  MUX_Select_Column(col);

  
  /* 触发PCap04测量 */
  Write_Opcode(CDC_START);  /* 0x8C */
  

  
  /* 读取PCap04电容值 */
  /* RD_RESULT opcode: 0x40, 地址0x00读取RES0 */
  result = PCap04_Read_Result(RD_RESULT, 0x00);
  
  /* 禁用多路复用器 */
  MUX_Disable_Row();
  MUX_Disable_Column();
  
  return result;
}

/******************************************************************************/
/*                          Quantize Value                                     */
/******************************************************************************/
/**
  * @brief  将原始电容值量化到指定范围
  * @param  raw_value: 原始电容值
  * @param  min_val: 量化最小值 (L)
  * @param  max_val: 量化最大值 (H)
  * @param  level: 量化档位 (255 或 1023)
  * @retval 量化后的值 (0 到 level)
  * 
  * 量化公式: quantized = (raw_value - min_val) * level / (max_val - min_val)
  * 如果 raw_value < min_val, 返回 0
  * 如果 raw_value > max_val, 返回 level
  */
uint32_t Quantize_Value(uint32_t raw_value, uint32_t min_val, uint32_t max_val, uint16_t level)
{
  uint32_t quantized = 0;
  uint32_t range;
  
  /* 防止除零 */
  if(max_val <= min_val) {
    return 0;
  }
  
  /* 限制输入值在范围内 */
  if(raw_value < min_val) {
    /* 小于最小值，返回0 */
    return 0;
  }
  if(raw_value > max_val) {
    /* 大于最大值，返回最大值level */
    return level;
  }
  
  /* 计算范围 */
  range = max_val - min_val;
  
  /* 线性量化公式: (value - min) / range * level */
  /* 使用64位避免溢出 */
  uint64_t temp = ((uint64_t)(raw_value - min_val) * (uint64_t)level);
  temp = temp / range;
  quantized = (uint32_t)temp;
  
  /* 确保结果在有效范围内 */
  if(quantized > level) {
    quantized = level;
  }
  
  return quantized;
}

/******************************************************************************/
/*                          Scan Entire Matrix                                */
/******************************************************************************/
void Matrix_Scan_All(MatrixData_t *matrix)
{
  uint8_t row, col;
  
  if(matrix == NULL) {
    return;
  }
  
  /* 扫描所有16x16点 */
  for(row = 0; row < MATRIX_SIZE; row++) {
    for(col = 0; col < MATRIX_SIZE; col++) {
      /* 选择行列 */
      MUX_Select_Row(row);
      MUX_Select_Column(col);
      
  
      
      /* 触发PCap04测量 */
      Write_Opcode(CDC_START);  /* 0x8C */
      
  
      
      /* 读取电容值 */
      matrix->capacitance[row][col] = PCap04_Read_Result(RD_RESULT, 0x00);
      
      /* 禁用多路复用器 */
      MUX_Disable_Row();
      MUX_Disable_Column();
      
 
    }
  }
}

/******************************************************************************/
/*                     Scan and Stream Output via USB                         */
/******************************************************************************/
/**
  * @brief  流式扫描并传输：扫描一个点立即发送一个点
  * @param  matrix: 矩阵数据指针（可选，用于存储数据）
  * @retval None
  * 
  * @note   此函数在扫描每个点的同时立即发送数据，不需要等待全部扫描完成
  *         格式：START -> 数据行... -> END
  */
void Matrix_Scan_And_Stream(MatrixData_t *matrix)
{
  uint8_t tx_buffer[256];  /* USB CDC单次最多64字节 */
  uint16_t len;
  uint8_t row, col;
  uint32_t output_value;
  uint32_t raw_value;
  
  /* 发送开头标记 START */
  len = sprintf((char*)tx_buffer, "START\r\n");
  while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
 
  }
  
  /* 根据输出格式选择不同的输出方式 */
  if(g_output_format == FORMAT_TABLE) {
    /* 表格格式：带行列标记，用逗号分隔 */
    
    /* 发送列标题：X00,X01,X02,...,X15 */
    len = 0;
    len += sprintf((char*)tx_buffer + len, "X00");
    for(col = 1; col < MATRIX_SIZE; col++) {
      len += sprintf((char*)tx_buffer + len, ",X%02d", col);
    }
    len += sprintf((char*)tx_buffer + len, "\r\n");
    
    while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
 
    }
    
    /* 扫描并流式发送每一行数据：Y00,值,值,值,...,值 */
    for(row = 0; row < MATRIX_SIZE; row++) {
      len = 0;
      len += sprintf((char*)tx_buffer + len, "Y%02d", row);
      
      for(col = 0; col < MATRIX_SIZE; col++) {
        /* 扫描当前点 */
        MUX_Select_Row(row);
        MUX_Select_Column(col);
      
        Write_Opcode(CDC_START);
       
        raw_value = PCap04_Read_Result(RD_RESULT, 0x00);
        MUX_Disable_Row();
        MUX_Disable_Column();
     
        
        /* 存储到矩阵（如果提供） */
        if(matrix != NULL) {
          matrix->capacitance[row][col] = raw_value;
        }
        
        /* 根据输出模式选择原始值或量化值 */
        if(g_output_mode == OUTPUT_QUANT) {
          output_value = Quantize_Value(raw_value, 
                                        g_quant_min, 
                                        g_quant_max, 
                                        (uint16_t)g_quant_level);
        } else {
          output_value = raw_value;
        }
        
        /* 用逗号分隔，立即添加到发送缓冲区 */
        len += sprintf((char*)tx_buffer + len, ",%lu", output_value);
      }
      len += sprintf((char*)tx_buffer + len, "\r\n");
      
      /* 立即发送该行数据 */
      while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
      
      }
      
 
    }
  } else {
    /* 简洁格式：X00Y00:值 (每行一个点) */
    for(row = 0; row < MATRIX_SIZE; row++) {
      for(col = 0; col < MATRIX_SIZE; col++) {
        /* 扫描当前点 */
        MUX_Select_Row(row);
        MUX_Select_Column(col);
     
        Write_Opcode(CDC_START);
        raw_value = PCap04_Read_Result(RD_RESULT, 0x00);
        MUX_Disable_Row();
        MUX_Disable_Column();
      
        
        /* 存储到矩阵（如果提供） */
        if(matrix != NULL) {
          matrix->capacitance[row][col] = raw_value;
        }
        
        /* 根据输出模式选择原始值或量化值 */
        if(g_output_mode == OUTPUT_QUANT) {
          output_value = Quantize_Value(raw_value, 
                                        g_quant_min, 
                                        g_quant_max, 
                                        (uint16_t)g_quant_level);
        } else {
          output_value = raw_value;
        }
        
        /* 格式：X00Y00:值 - 立即发送 */
        len = sprintf((char*)tx_buffer, "X%02dY%02d:%lu\r\n", col, row, output_value);
        
        /* 立即发送该点数据 */
        while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
        
        }
      }
   
    }
  }
  
  /* 发送结尾标记 END */
  len = sprintf((char*)tx_buffer, "END\r\n");
  while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
 
  }
}

/******************************************************************************/
/*                          Output Matrix via USB                             */
/******************************************************************************/
void Matrix_Output_USB(MatrixData_t *matrix)
{
  uint8_t tx_buffer[512];  /* USB CDC单次最多64字节，使用512字节缓冲足够 */
  uint16_t len;
  uint8_t row, col;
  uint32_t output_value;
  
  if(matrix == NULL) {
    return;
  }
  
  /* 发送开头标记 */
  len = sprintf((char*)tx_buffer, "START\r\n");
  while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {

  }
  
  /* 根据输出格式选择不同的输出方式 */
  if(g_output_format == FORMAT_TABLE) {
    /* 表格格式：带行列标记，用逗号分隔 */
    
    /* 发送列标题：X00,X01,X02,...,X15 */
    len = 0;
    len += sprintf((char*)tx_buffer + len, "X00");
    for(col = 1; col < MATRIX_SIZE; col++) {
      len += sprintf((char*)tx_buffer + len, ",X%02d", col);
    }
    len += sprintf((char*)tx_buffer + len, "\r\n");
    
    while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
      
    }
    
    /* 发送每一行数据：Y00,值,值,值,...,值 */
    for(row = 0; row < MATRIX_SIZE; row++) {
      len = 0;
      len += sprintf((char*)tx_buffer + len, "Y%02d", row);
      
      for(col = 0; col < MATRIX_SIZE; col++) {
        /* 根据输出模式选择原始值或量化值 */
        if(g_output_mode == OUTPUT_QUANT) {
          output_value = Quantize_Value(matrix->capacitance[row][col], 
                                        g_quant_min, 
                                        g_quant_max, 
                                        (uint16_t)g_quant_level);
        } else {
          output_value = matrix->capacitance[row][col];
        }
        
        /* 用逗号分隔 */
        len += sprintf((char*)tx_buffer + len, ",%lu", output_value);
      }
      len += sprintf((char*)tx_buffer + len, "\r\n");
      
      /* 等待USB发送缓冲区可用并发送 */
      while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
      
      }
      

    }
  } else {
    /* 简洁格式：X00Y00:值 (每行一个点) */
    for(row = 0; row < MATRIX_SIZE; row++) {
      for(col = 0; col < MATRIX_SIZE; col++) {
        /* 根据输出模式选择原始值或量化值 */
        if(g_output_mode == OUTPUT_QUANT) {
          output_value = Quantize_Value(matrix->capacitance[row][col], 
                                        g_quant_min, 
                                        g_quant_max, 
                                        (uint16_t)g_quant_level);
        } else {
          output_value = matrix->capacitance[row][col];
        }
        
        /* 格式：X00Y00:值 */
        len = sprintf((char*)tx_buffer, "X%02dY%02d:%lu\r\n", col, row, output_value);
        
        /* 等待USB发送缓冲区可用并发送 */
        while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {
    
        }
      }
      

    }
  }
  
  /* 发送结尾标记 */
  len = sprintf((char*)tx_buffer, "END\r\n");
  while(CDC_Transmit_FS(tx_buffer, len) == USBD_BUSY) {

  }
}

