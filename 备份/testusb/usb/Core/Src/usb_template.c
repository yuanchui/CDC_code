/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_template.c
  * @brief          : USB Data Output Template System Implementation
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usb_template.h"
#include "usb_common.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

Template_Config_t g_template_config = {
    .format = TEMPLATE_FORMAT_TABLE,
    .mode = TEMPLATE_MODE_RAW,
    .simple_style = TEMPLATE_SIMPLE_COLON,
    .table_delim = TEMPLATE_DEFAULT_DELIM,
    .precision = TEMPLATE_DEFAULT_PRECISION,
    .show_header = true,
    .use_hex = true,
    .matrix_rows = 16,
    .matrix_cols = 16
};

/* Private function prototypes -----------------------------------------------*/

static void Template_FormatValue(uint32_t raw_data, double float_data, char *buffer, int buffer_size);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize template system
  */
void Template_Init(void)
{
    strcpy(g_template_config.start_tag, TEMPLATE_DEFAULT_START_TAG);
    strcpy(g_template_config.end_tag, TEMPLATE_DEFAULT_END_TAG);
    g_template_config.matrix_rows = 16;
    g_template_config.matrix_cols = 16;
}

/**
  * @brief  Set output format
  */
void Template_SetFormat(Template_Format_t format)
{
    g_template_config.format = format;
}

/**
  * @brief  Set data mode
  */
void Template_SetMode(Template_Mode_t mode)
{
    g_template_config.mode = mode;
}

/**
  * @brief  Set table delimiter
  */
void Template_SetTableDelim(char delim)
{
    g_template_config.table_delim = delim;
}

/**
  * @brief  Set simple style
  */
void Template_SetSimpleStyle(Template_SimpleStyle_t style)
{
    g_template_config.simple_style = style;
}

/**
  * @brief  Set precision
  */
void Template_SetPrecision(uint8_t precision)
{
    if (precision > 9) precision = 9;
    g_template_config.precision = precision;
}

/**
  * @brief  Set header display
  */
void Template_SetHeader(bool enable)
{
    g_template_config.show_header = enable;
}

/**
  * @brief  Set hex output
  */
void Template_SetHex(bool enable)
{
    g_template_config.use_hex = enable;
}

/**
  * @brief  Set boundary tags
  */
void Template_SetBoundaryTags(const char *start, const char *end)
{
    if (start) {
        strncpy(g_template_config.start_tag, start, 15);
        g_template_config.start_tag[15] = '\0';
    }
    if (end) {
        strncpy(g_template_config.end_tag, end, 15);
        g_template_config.end_tag[15] = '\0';
    }
}

/**
  * @brief  Set matrix size
  */
void Template_SetMatrixSize(uint8_t rows, uint8_t cols)
{
    if (rows > USB_MATRIX_MAX_SIZE) rows = USB_MATRIX_MAX_SIZE;
    if (cols > USB_MATRIX_MAX_SIZE) cols = USB_MATRIX_MAX_SIZE;
    g_template_config.matrix_rows = rows;
    g_template_config.matrix_cols = cols;
}

/**
  * @brief  Output start tag
  */
void Template_OutputStart(void)
{
    USB_Printf("%s\r\n", g_template_config.start_tag);
    
    if (g_template_config.show_header && g_template_config.format == TEMPLATE_FORMAT_TABLE) {
        Template_OutputHeader();
    }
}

/**
  * @brief  Output end tag
  */
void Template_OutputEnd(void)
{
    USB_Printf("%s\r\n", g_template_config.end_tag);
}

/**
  * @brief  Output header row
  */
void Template_OutputHeader(void)
{
    if (g_template_config.format == TEMPLATE_FORMAT_TABLE) {
        // Output column headers: X00, X01, X02, ...
        USB_Printf("X00");
        for (uint8_t col = 1; col < g_template_config.matrix_cols; col++) {
            USB_Printf("%cX%02d", g_template_config.table_delim, col);
        }
        USB_Printf("\r\n");
    }
}

/**
  * @brief  Format value based on mode
  */
static void Template_FormatValue(uint32_t raw_data, double float_data, char *buffer, int buffer_size)
{
    if (g_template_config.mode == TEMPLATE_MODE_RAW) {
        if (g_template_config.use_hex) {
            snprintf(buffer, buffer_size, "0x%08X", raw_data);
        } else {
            snprintf(buffer, buffer_size, "%lu", (unsigned long)raw_data);
        }
    } else {  // QUANT mode
        char format_str[16];
        snprintf(format_str, sizeof(format_str), "%%.%df", g_template_config.precision);
        snprintf(buffer, buffer_size, format_str, float_data);
    }
}

/**
  * @brief  Output single data point
  */
void Template_OutputData(uint8_t row, uint8_t col, uint32_t raw_data, double float_data)
{
    char value_str[32];
    Template_FormatValue(raw_data, float_data, value_str, sizeof(value_str));
    
    if (g_template_config.format == TEMPLATE_FORMAT_TABLE) {
        // Table format: value with delimiter
        if (col == 0) {
            USB_Printf("Y%02d%c%s", row, g_template_config.table_delim, value_str);
        } else {
            USB_Printf("%c%s", g_template_config.table_delim, value_str);
        }
    } else {  // SIMPLE format
        // Simple format: X00Y00:value
        const char *separator = "";
        switch (g_template_config.simple_style) {
            case TEMPLATE_SIMPLE_COLON:
                separator = ":";
                break;
            case TEMPLATE_SIMPLE_SPACE:
                separator = " ";
                break;
            case TEMPLATE_SIMPLE_NONE:
                separator = "";
                break;
        }
        USB_Printf("X%02dY%02d%s%s\r\n", col, row, separator, value_str);
    }
}

/**
  * @brief  Output row header
  */
void Template_OutputRowHeader(uint8_t row)
{
    if (g_template_config.format == TEMPLATE_FORMAT_TABLE) {
        USB_Printf("Y%02d", row);
    }
}

/**
  * @brief  Output entire row
  */
void Template_OutputRowData(uint8_t row, uint32_t *data_array, double *float_array)
{
    if (g_template_config.format == TEMPLATE_FORMAT_TABLE) {
        Template_OutputRowHeader(row);
        
        for (uint8_t col = 0; col < g_template_config.matrix_cols; col++) {
            char value_str[32];
            uint32_t raw_data = (data_array) ? data_array[col] : 0;
            double float_data = (float_array) ? float_array[col] : 0.0;
            
            Template_FormatValue(raw_data, float_data, value_str, sizeof(value_str));
            USB_Printf("%c%s", g_template_config.table_delim, value_str);
        }
        USB_Printf("\r\n");
    } else {  // SIMPLE format
        for (uint8_t col = 0; col < g_template_config.matrix_cols; col++) {
            uint32_t raw_data = (data_array) ? data_array[col] : 0;
            double float_data = (float_array) ? float_array[col] : 0.0;
            Template_OutputData(row, col, raw_data, float_data);
        }
    }
}

/**
  * @brief  Output entire matrix
  */
void Template_OutputMatrix(uint32_t *data_matrix, double *float_matrix)
{
    Template_OutputStart();
    
    for (uint8_t row = 0; row < g_template_config.matrix_rows; row++) {
        uint32_t *row_data = (data_matrix) ? &data_matrix[row * g_template_config.matrix_cols] : NULL;
        double *row_float = (float_matrix) ? &float_matrix[row * g_template_config.matrix_cols] : NULL;
        Template_OutputRowData(row, row_data, row_float);
    }
    
    Template_OutputEnd();
}

/**
  * @brief  Get format name string
  */
const char* Template_GetFormatName(Template_Format_t format)
{
    switch (format) {
        case TEMPLATE_FORMAT_TABLE: return "TABLE";
        case TEMPLATE_FORMAT_SIMPLE: return "SIMPLE";
        default: return "UNKNOWN";
    }
}

/**
  * @brief  Get mode name string
  */
const char* Template_GetModeName(Template_Mode_t mode)
{
    switch (mode) {
        case TEMPLATE_MODE_RAW: return "RAW";
        case TEMPLATE_MODE_QUANT: return "QUANT";
        default: return "UNKNOWN";
    }
}

/**
  * @brief  Print template configuration
  */
void Template_PrintConfig(void)
{
    USB_Printf("Template Config:\r\n");
    USB_Printf("  Format: %s\r\n", Template_GetFormatName(g_template_config.format));
    USB_Printf("  Mode: %s\r\n", Template_GetModeName(g_template_config.mode));
    USB_Printf("  Delimiter: '%c'\r\n", g_template_config.table_delim);
    USB_Printf("  Precision: %d\r\n", g_template_config.precision);
    USB_Printf("  Header: %s\r\n", g_template_config.show_header ? "ON" : "OFF");
    USB_Printf("  Hex: %s\r\n", g_template_config.use_hex ? "ON" : "OFF");
    USB_Printf("  Matrix: %dx%d\r\n", g_template_config.matrix_rows, g_template_config.matrix_cols);
    USB_Printf("  Tags: %s / %s\r\n", g_template_config.start_tag, g_template_config.end_tag);
}

