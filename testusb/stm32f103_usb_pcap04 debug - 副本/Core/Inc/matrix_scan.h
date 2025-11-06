/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    matrix_scan.h
  * @brief   16x16 Capacitance Matrix Scan Header
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MATRIX_SCAN_H
#define __MATRIX_SCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported constants --------------------------------------------------------*/
#define MATRIX_SIZE 16

/* Exported types ------------------------------------------------------------*/
typedef struct {
  uint32_t capacitance[MATRIX_SIZE][MATRIX_SIZE];  /* 16x16电容值矩阵 */
} MatrixData_t;

/* 量化函数 */
uint32_t Quantize_Value(uint32_t raw_value, uint32_t min_val, uint32_t max_val, uint16_t level);

/* Exported functions prototypes ---------------------------------------------*/
void Matrix_Scan_Init(void);
void Matrix_Scan_All(MatrixData_t *matrix);
uint32_t Matrix_Scan_Point(uint8_t row, uint8_t col);
void Matrix_Output_USB(MatrixData_t *matrix);
void Matrix_Scan_And_Stream(MatrixData_t *matrix);  /* 流式扫描并传输：扫描一个点立即发送一个点 */

#ifdef __cplusplus
}
#endif

#endif /* __MATRIX_SCAN_H */

