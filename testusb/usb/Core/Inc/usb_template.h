/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_template.h
  * @brief          : USB Data Output Template System
  *                   Flexible data formatting and output templates
  ******************************************************************************
  * @attention
  *
  * Data Output Template System for capacitance matrix data
  * 
  * Features:
  * - Multiple output formats (TABLE, SIMPLE)
  * - Multiple data modes (RAW, QUANT)
  * - Customizable delimiters and styles
  * - Header control
  * - Custom boundary tags
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USB_TEMPLATE_H
#define __USB_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  Output format type
  */
typedef enum {
    TEMPLATE_FORMAT_TABLE = 0,   // Table format (CSV-like)
    TEMPLATE_FORMAT_SIMPLE       // Simple format (X00Y00:value)
} Template_Format_t;

/**
  * @brief  Data mode type
  */
typedef enum {
    TEMPLATE_MODE_RAW = 0,       // Raw hex data
    TEMPLATE_MODE_QUANT          // Quantized data
} Template_Mode_t;

/**
  * @brief  Simple style type
  */
typedef enum {
    TEMPLATE_SIMPLE_COLON = 0,   // X00Y00:value
    TEMPLATE_SIMPLE_SPACE,        // X00Y00 value
    TEMPLATE_SIMPLE_NONE          // X00Y00value
} Template_SimpleStyle_t;

/**
  * @brief  Template configuration structure
  */
typedef struct {
    Template_Format_t format;         // Output format
    Template_Mode_t mode;             // Data mode
    Template_SimpleStyle_t simple_style; // Simple format style
    char table_delim;                // Table delimiter (default ',')
    uint8_t precision;                // Float precision (0-9)
    bool show_header;                 // Show header
    bool use_hex;                     // Use hex output (for RAW mode)
    char start_tag[16];               // Start boundary tag
    char end_tag[16];                 // End boundary tag
    uint8_t matrix_rows;              // Matrix rows
    uint8_t matrix_cols;              // Matrix columns
} Template_Config_t;

/* Exported constants --------------------------------------------------------*/

#define TEMPLATE_DEFAULT_DELIM      ','
#define TEMPLATE_DEFAULT_PRECISION  3
#define TEMPLATE_DEFAULT_START_TAG  "START"
#define TEMPLATE_DEFAULT_END_TAG    "END"

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern Template_Config_t g_template_config;

/* Exported functions prototypes ---------------------------------------------*/

// Initialization
void Template_Init(void);

// Configuration
void Template_SetFormat(Template_Format_t format);
void Template_SetMode(Template_Mode_t mode);
void Template_SetTableDelim(char delim);
void Template_SetSimpleStyle(Template_SimpleStyle_t style);
void Template_SetPrecision(uint8_t precision);
void Template_SetHeader(bool enable);
void Template_SetHex(bool enable);
void Template_SetBoundaryTags(const char *start, const char *end);
void Template_SetMatrixSize(uint8_t rows, uint8_t cols);

// Output functions
void Template_OutputStart(void);
void Template_OutputEnd(void);
void Template_OutputHeader(void);
void Template_OutputData(uint8_t row, uint8_t col, uint32_t raw_data, double float_data);
void Template_OutputRowHeader(uint8_t row);
void Template_OutputRowData(uint8_t row, uint32_t *data_array, double *float_array);
void Template_OutputMatrix(uint32_t *data_matrix, double *float_matrix);

// Utility
const char* Template_GetFormatName(Template_Format_t format);
const char* Template_GetModeName(Template_Mode_t mode);
void Template_PrintConfig(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_TEMPLATE_H */

