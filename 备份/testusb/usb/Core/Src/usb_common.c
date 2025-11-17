/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_common.c
  * @brief          : USB Command/Control Protocol System Implementation
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usb_common.h"
#include "usb_template.h"
#include "pcap04_reg.h"
#include "cd74hc4067.h"
#include "pcap04.h"
#include "usbd_cdc_if.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/

typedef struct {
    char cmd[USB_CMD_MAX_LEN];
    bool valid;
} CMD_QueueItem_t;

/* Private define ------------------------------------------------------------*/

#define CMD_TOKEN_MAX     16
#define CMD_PARAM_MAX     8

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

USB_SystemStatus_t g_usb_status = {
    .mode = USB_MODE_STOPPED,
    .scan_rate_ms = 1000,  // Default: 1 second
    .current_row = 0,
    .current_col = 0,
    .matrix_rows = 16,
    .matrix_cols = 16,
    .is_running = false,
    .timer_counter = 0,
    .timer_threshold = 200  // Default: 200 * 5ms = 1000ms (1 second)
};

static CMD_QueueItem_t cmd_queue[USB_CMD_QUEUE_SIZE];
// Reserved for future FIFO queue implementation
// static uint8_t cmd_queue_head = 0;
// static uint8_t cmd_queue_tail = 0;
static bool cmd_queue_active = false;
static char cmd_buffer[USB_CMD_MAX_LEN];

// Scan trigger flag (set by timer interrupt, checked by main loop)
volatile bool scan_trigger_flag = false;

/* Private function prototypes -----------------------------------------------*/

static CMD_Result_t CMD_ParseAndExecute(const char *cmd);
static void CMD_SplitCommand(const char *cmd_line, char tokens[][CMD_TOKEN_MAX], int *token_count);
static CMD_Result_t CMD_ExecuteSingle(const char *cmd, char params[][CMD_TOKEN_MAX], int param_count);
static uint32_t CMD_ParseUint32(const char *str);
static uint8_t CMD_ParseUint8(const char *str);
static bool CMD_ParseBool(const char *str);
static void USB_OutputScanData(uint8_t row, uint8_t col, 
                                uint32_t raw_data_ca, double float_data_ca,
                                uint32_t raw_data_cb, double float_data_cb);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize USB common system
  */
void USB_Common_Init(void)
{
    memset(&g_usb_status, 0, sizeof(g_usb_status));
    g_usb_status.mode = USB_MODE_STOPPED;
    g_usb_status.scan_rate_ms = 1000;  // Default: 1 second
    g_usb_status.matrix_rows = 16;
    g_usb_status.matrix_cols = 16;
    g_usb_status.timer_threshold = 200;  // 200 * 5ms = 1000ms (1 second)
    
    memset(cmd_queue, 0, sizeof(cmd_queue));
    // cmd_queue_head = 0;  // Reserved for future FIFO queue implementation
    // cmd_queue_tail = 0;  // Reserved for future FIFO queue implementation
    cmd_queue_active = false;
}

/**
  * @brief  Process command line (supports && separator)
  */
CMD_Result_t USB_ProcessCommand(const char *cmd_line)
{
    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return CMD_RESULT_ERROR;
    }
    
    // Copy command line
    strncpy(cmd_buffer, cmd_line, USB_CMD_MAX_LEN - 1);
    cmd_buffer[USB_CMD_MAX_LEN - 1] = '\0';
    
    // Split by &&
    char *saveptr;
    char *token = strtok_r(cmd_buffer, "&", &saveptr);
    CMD_Result_t result = CMD_RESULT_OK;
    
    while (token != NULL) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
            *end = '\0';
            end--;
        }
        
        if (strlen(token) > 0) {
            CMD_Result_t cmd_result = CMD_ParseAndExecute(token);
            if (cmd_result != CMD_RESULT_OK) {
                result = cmd_result;
            }
        }
        
        token = strtok_r(NULL, "&", &saveptr);
    }
    
    return result;
}

/**
  * @brief  Parse and execute single command
  */
static CMD_Result_t CMD_ParseAndExecute(const char *cmd)
{
    char tokens[CMD_PARAM_MAX][CMD_TOKEN_MAX];
    int token_count = 0;
    
    CMD_SplitCommand(cmd, tokens, &token_count);
    
    if (token_count == 0) {
        return CMD_RESULT_ERROR;
    }
    
    return CMD_ExecuteSingle(tokens[0], &tokens[1], token_count - 1);
}

/**
  * @brief  Split command into tokens
  */
static void CMD_SplitCommand(const char *cmd, char tokens[][CMD_TOKEN_MAX], int *token_count)
{
    *token_count = 0;
    const char *p = cmd;
    
    while (*p != '\0' && *token_count < CMD_PARAM_MAX) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;
        
        // Find token end
        const char *start = p;
        while (*p != '\0' && *p != ':' && *p != ' ' && *p != '\t') {
            p++;
        }
        
        int len = p - start;
        if (len > 0 && len < CMD_TOKEN_MAX) {
            strncpy(tokens[*token_count], start, len);
            tokens[*token_count][len] = '\0';
            (*token_count)++;
        }
        
        if (*p == ':') p++;  // Skip colon
    }
}

/**
  * @brief  Execute single command
  */
static CMD_Result_t CMD_ExecuteSingle(const char *cmd, char params[][CMD_TOKEN_MAX], int param_count)
{
    // Convert to uppercase for comparison
    char cmd_upper[CMD_TOKEN_MAX];
    strncpy(cmd_upper, cmd, CMD_TOKEN_MAX - 1);
    cmd_upper[CMD_TOKEN_MAX - 1] = '\0';
    for (int i = 0; cmd_upper[i]; i++) {
        if (cmd_upper[i] >= 'a' && cmd_upper[i] <= 'z') {
            cmd_upper[i] = cmd_upper[i] - 'a' + 'A';
        }
    }
    
    // === General Control Commands ===
    if (strcmp(cmd_upper, "START") == 0) {
        USB_Start();
        USB_SendOK("START");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "STOP") == 0) {
        USB_Stop();
        USB_SendOK("STOP");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "STATUS") == 0) {
        USB_SendStatus();
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "HELP") == 0 || strcmp(cmd_upper, "?") == 0) {
        USB_PrintHelp();
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "SINGLE_SCAN") == 0) {
        USB_SingleScan();
        USB_SendOK("SINGLE_SCAN");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "FAST_MODE") == 0) {
        USB_SetMode(USB_MODE_FAST);
        USB_SendOK("FAST_MODE");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "NORMAL_MODE") == 0) {
        USB_SetMode(USB_MODE_NORMAL);
        USB_SendOK("NORMAL_MODE");
        return CMD_RESULT_OK;
    }
    // === SET_RATE:<ms> ===
    else if (strncmp(cmd_upper, "SET_RATE", 8) == 0) {
        if (param_count >= 1) {
            uint32_t rate = CMD_ParseUint32(params[0]);
            if (rate > 0 && rate <= 10000) {
                USB_SetScanRate(rate);
                USB_SendOK("SET_RATE");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(1, "Invalid SET_RATE parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_ROW:<n> ===
    else if (strncmp(cmd_upper, "SET_ROW", 7) == 0) {
        if (param_count >= 1) {
            uint8_t row = CMD_ParseUint8(params[0]);
            if (row <= 15) {
                USB_SetRow(row);
                USB_SendOK("SET_ROW");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(2, "Invalid SET_ROW parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_COL:<n> ===
    else if (strncmp(cmd_upper, "SET_COL", 7) == 0) {
        if (param_count >= 1) {
            uint8_t col = CMD_ParseUint8(params[0]);
            if (col <= 15) {
                USB_SetCol(col);
                USB_SendOK("SET_COL");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(3, "Invalid SET_COL parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    else if (strcmp(cmd_upper, "GET_ROW") == 0) {
        USB_Printf("ROW:%d\r\n", USB_GetRow());
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "GET_COL") == 0) {
        USB_Printf("COL:%d\r\n", USB_GetCol());
        return CMD_RESULT_OK;
    }
    // === SCAN_POINT:<r>:<c> ===
    else if (strncmp(cmd_upper, "SCAN_POINT", 10) == 0) {
        if (param_count >= 2) {
            uint8_t row = CMD_ParseUint8(params[0]);
            uint8_t col = CMD_ParseUint8(params[1]);
            if (row <= 15 && col <= 15) {
                USB_ScanPoint(row, col);
                USB_SendOK("SCAN_POINT");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(4, "Invalid SCAN_POINT parameters");
        return CMD_RESULT_INVALID_PARAM;
    }
    else if (strcmp(cmd_upper, "MATRIX_INFO") == 0) {
        USB_PrintMatrixInfo();
        return CMD_RESULT_OK;
    }
    // === Queue Commands ===
    else if (strcmp(cmd_upper, "QUEUE_START") == 0) {
        USB_QueueStart();
        USB_SendOK("QUEUE_START");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "QUEUE_END") == 0) {
        USB_QueueEnd();
        USB_SendOK("QUEUE_END");
        return CMD_RESULT_OK;
    }
    // === WAIT:<ms> ===
    else if (strncmp(cmd_upper, "WAIT", 4) == 0) {
        if (param_count >= 1) {
            uint32_t ms = CMD_ParseUint32(params[0]);
            USB_QueueWait(ms);
            return CMD_RESULT_OK;
        }
        USB_SendError(5, "Invalid WAIT parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === PCAP04 Commands ===
    else if (strcmp(cmd_upper, "PCAP04_STATUS") == 0) {
        PCAP04_Reg_PrintStatus();
        USB_SendOK("PCAP04_STATUS");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "PCAP04_TEST") == 0) {
        bool result = PCAP04_Reg_Test();
        USB_Printf("PCAP04_TEST:%s\r\n", result ? "OK" : "FAIL");
        USB_SendOK("PCAP04_TEST");
        return CMD_RESULT_OK;
    }
    // === PCAP04_READ:<reg> ===
    else if (strncmp(cmd_upper, "PCAP04_READ", 11) == 0) {
        if (param_count >= 1) {
            uint8_t reg = CMD_ParseUint8(params[0]);
            if (reg < PCAP04_REG_COUNT) {
                uint8_t value = PCAP04_Reg_Read(reg);
                USB_Printf("PCAP04_REG[0x%02X]=0x%02X\r\n", reg, value);
                USB_SendOK("PCAP04_READ");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(10, "Invalid PCAP04_READ parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === PCAP04_WRITE:<reg>:<val> ===
    else if (strncmp(cmd_upper, "PCAP04_WRITE", 12) == 0) {
        if (param_count >= 2) {
            uint8_t reg = CMD_ParseUint8(params[0]);
            uint8_t val = CMD_ParseUint8(params[1]);
            if (reg < PCAP04_REG_COUNT) {
                PCAP04_Reg_Write(reg, val);
                USB_SendOK("PCAP04_WRITE");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(11, "Invalid PCAP04_WRITE parameters");
        return CMD_RESULT_INVALID_PARAM;
    }
    else if (strcmp(cmd_upper, "PCAP04_DUMP") == 0) {
        PCAP04_Reg_Dump();
        USB_SendOK("PCAP04_DUMP");
        return CMD_RESULT_OK;
    }
    else if (strcmp(cmd_upper, "PCAP04_LOAD_DEFAULT") == 0) {
        PCAP04_Reg_LoadDefault();
        USB_SendOK("PCAP04_LOAD_DEFAULT");
        return CMD_RESULT_OK;
    }
    // === SET_CDIFF:<0/1> ===
    else if (strncmp(cmd_upper, "SET_CDIFF", 9) == 0) {
        if (param_count >= 1) {
            bool enable = CMD_ParseBool(params[0]);
            PCAP04_Reg_SetDifferential(enable);
            USB_SendOK("SET_CDIFF");
            return CMD_RESULT_OK;
        }
        USB_SendError(12, "Invalid SET_CDIFF parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_INTREF:<0/1> ===
    else if (strncmp(cmd_upper, "SET_INTREF", 10) == 0) {
        if (param_count >= 1) {
            bool enable = CMD_ParseBool(params[0]);
            PCAP04_Reg_SetInternalRef(enable);
            USB_SendOK("SET_INTREF");
            return CMD_RESULT_OK;
        }
        USB_SendError(13, "Invalid SET_INTREF parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_EXTREF:<0/1> ===
    else if (strncmp(cmd_upper, "SET_EXTREF", 10) == 0) {
        if (param_count >= 1) {
            bool enable = CMD_ParseBool(params[0]);
            PCAP04_Reg_SetExternalRef(enable);
            USB_SendOK("SET_EXTREF");
            return CMD_RESULT_OK;
        }
        USB_SendError(14, "Invalid SET_EXTREF parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_HEX:<0/1> ===
    else if (strncmp(cmd_upper, "SET_HEX", 7) == 0) {
        if (param_count >= 1) {
            bool enable = CMD_ParseBool(params[0]);
            Template_SetHex(enable);
            USB_SendOK("SET_HEX");
            return CMD_RESULT_OK;
        }
        USB_SendError(15, "Invalid SET_HEX parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_PRECISION:<n> ===
    else if (strncmp(cmd_upper, "SET_PRECISION", 13) == 0) {
        if (param_count >= 1) {
            uint8_t prec = CMD_ParseUint8(params[0]);
            Template_SetPrecision(prec);
            USB_SendOK("SET_PRECISION");
            return CMD_RESULT_OK;
        }
        USB_SendError(16, "Invalid SET_PRECISION parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_MODE:<raw/quant> ===
    else if (strncmp(cmd_upper, "SET_MODE", 8) == 0) {
        if (param_count >= 1) {
            if (strcmp(params[0], "raw") == 0 || strcmp(params[0], "RAW") == 0) {
                Template_SetMode(TEMPLATE_MODE_RAW);
            } else if (strcmp(params[0], "quant") == 0 || strcmp(params[0], "QUANT") == 0) {
                Template_SetMode(TEMPLATE_MODE_QUANT);
            } else {
                USB_SendError(17, "Invalid SET_MODE parameter (raw/quant)");
                return CMD_RESULT_INVALID_PARAM;
            }
            USB_SendOK("SET_MODE");
            return CMD_RESULT_OK;
        }
        USB_SendError(17, "Invalid SET_MODE parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_FORMAT:<table/simple> ===
    else if (strncmp(cmd_upper, "SET_FORMAT", 10) == 0) {
        if (param_count >= 1) {
            if (strcmp(params[0], "table") == 0 || strcmp(params[0], "TABLE") == 0) {
                Template_SetFormat(TEMPLATE_FORMAT_TABLE);
            } else if (strcmp(params[0], "simple") == 0 || strcmp(params[0], "SIMPLE") == 0) {
                Template_SetFormat(TEMPLATE_FORMAT_SIMPLE);
            } else {
                USB_SendError(18, "Invalid SET_FORMAT parameter (table/simple)");
                return CMD_RESULT_INVALID_PARAM;
            }
            USB_SendOK("SET_FORMAT");
            return CMD_RESULT_OK;
        }
        USB_SendError(18, "Invalid SET_FORMAT parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_TABLE_DELIM:<char> ===
    else if (strncmp(cmd_upper, "SET_TABLE_DELIM", 15) == 0) {
        if (param_count >= 1 && strlen(params[0]) > 0) {
            Template_SetTableDelim(params[0][0]);
            USB_SendOK("SET_TABLE_DELIM");
            return CMD_RESULT_OK;
        }
        USB_SendError(19, "Invalid SET_TABLE_DELIM parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_HEADER:<0/1> ===
    else if (strncmp(cmd_upper, "SET_HEADER", 10) == 0) {
        if (param_count >= 1) {
            bool enable = CMD_ParseBool(params[0]);
            Template_SetHeader(enable);
            USB_SendOK("SET_HEADER");
            return CMD_RESULT_OK;
        }
        USB_SendError(20, "Invalid SET_HEADER parameter");
        return CMD_RESULT_INVALID_PARAM;
    }
    // === SET_MATRIX_SIZE:<row>:<col> ===
    else if (strncmp(cmd_upper, "SET_MATRIX_SIZE", 15) == 0) {
        if (param_count >= 2) {
            uint8_t rows = CMD_ParseUint8(params[0]);
            uint8_t cols = CMD_ParseUint8(params[1]);
            if (rows > 0 && rows <= 16 && cols > 0 && cols <= 16) {
                Template_SetMatrixSize(rows, cols);
                g_usb_status.matrix_rows = rows;
                g_usb_status.matrix_cols = cols;
                USB_SendOK("SET_MATRIX_SIZE");
                return CMD_RESULT_OK;
            }
        }
        USB_SendError(21, "Invalid SET_MATRIX_SIZE parameters");
        return CMD_RESULT_INVALID_PARAM;
    }
    
    // Unknown command
    USB_SendError(255, "Unknown command");
    return CMD_RESULT_UNKNOWN_CMD;
}

/**
  * @brief  Parse uint32 from string
  */
static uint32_t CMD_ParseUint32(const char *str)
{
    if (str == NULL) return 0;
    
    // Handle hex (0x prefix)
    if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0) {
        return (uint32_t)strtoul(str, NULL, 16);
    }
    
    return (uint32_t)strtoul(str, NULL, 10);
}

/**
  * @brief  Parse uint8 from string
  */
static uint8_t CMD_ParseUint8(const char *str)
{
    return (uint8_t)CMD_ParseUint32(str);
}

/**
  * @brief  Parse bool from string
  */
static bool CMD_ParseBool(const char *str)
{
    if (str == NULL) return false;
    if (strcmp(str, "1") == 0 || strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0) {
        return true;
    }
    return false;
}

/**
  * @brief  Set system mode
  */
void USB_SetMode(USB_SystemMode_t mode)
{
    g_usb_status.mode = mode;
    if (mode == USB_MODE_STOPPED) {
        g_usb_status.is_running = false;
    }
}

/**
  * @brief  Set scan rate
  */
void USB_SetScanRate(uint32_t rate_ms)
{
    if (rate_ms < 1) rate_ms = 1;
    if (rate_ms > 10000) rate_ms = 10000;
    
    g_usb_status.scan_rate_ms = rate_ms;
    // Calculate timer threshold: rate_ms / 5ms per interrupt
    g_usb_status.timer_threshold = (rate_ms + 4) / 5;  // Round up
    if (g_usb_status.timer_threshold < 1) g_usb_status.timer_threshold = 1;
}

/**
  * @brief  Start scanning
  */
void USB_Start(void)
{
    g_usb_status.is_running = true;
    g_usb_status.timer_counter = 0;
    if (g_usb_status.mode == USB_MODE_STOPPED) {
        g_usb_status.mode = USB_MODE_NORMAL;
    }
    // Start timer for periodic scanning
    extern TIM_HandleTypeDef htim1;
    extern void TIM1_Start(void);
    TIM1_Start();
    
    // Trigger immediate first scan (don't wait for timer)
    scan_trigger_flag = true;
}

/**
  * @brief  Stop scanning
  */
void USB_Stop(void)
{
    g_usb_status.is_running = false;
    g_usb_status.mode = USB_MODE_STOPPED;
    // Stop timer
    extern void TIM1_Stop(void);
    TIM1_Stop();
}

/**
  * @brief  Execute single scan at current position
  */
void USB_SingleScan(void)
{
    // Scan current position
    uint8_t row = g_usb_status.current_row;
    uint8_t col = g_usb_status.current_col;
    
    // Select multiplexer channels
    MUX_SelectXY(col, row);
    HAL_Delay(10);  // Settling time
    
    // Start PCAP04 CDC measurement
    extern void PCap04_CDCStart(void);
    PCap04_CDCStart();
    HAL_Delay(20);  // Wait for measurement to complete
    
    // Read PCAP04 data
    // PC0, PC1: Reference capacitance (CA)
    // PC2, PC3: Measurement capacitance (CB)
    extern uint32_t PCAP04_Read_CDC_Result_data(int Nun);
    extern double integrated_data(uint32_t data);
    
    uint32_t raw_data_pc0 = PCAP04_Read_CDC_Result_data(0);
    uint32_t raw_data_pc1 = PCAP04_Read_CDC_Result_data(1);
    uint32_t raw_data_pc2 = PCAP04_Read_CDC_Result_data(2);
    uint32_t raw_data_pc3 = PCAP04_Read_CDC_Result_data(3);
    
    double float_data_pc0 = integrated_data(raw_data_pc0);
    double float_data_pc1 = integrated_data(raw_data_pc1);
    double float_data_pc2 = integrated_data(raw_data_pc2);
    double float_data_pc3 = integrated_data(raw_data_pc3);
    
    // Calculate CA (reference) - average of PC0 and PC1
    uint32_t raw_data_ca = (raw_data_pc0 + raw_data_pc1) / 2;
    double float_data_ca = (float_data_pc0 + float_data_pc1) / 2.0;
    
    // Calculate CB (measurement) - average of PC2 and PC3
    uint32_t raw_data_cb = (raw_data_pc2 + raw_data_pc3) / 2;
    double float_data_cb = (float_data_pc2 + float_data_pc3) / 2.0;
    
    // Output using new CA/CB format
    USB_OutputScanData(row, col, raw_data_ca, float_data_ca, raw_data_cb, float_data_cb);
    
    // Move to next position
    col++;
    if (col >= g_usb_status.matrix_cols) {
        col = 0;
        row++;
        if (row >= g_usb_status.matrix_rows) {
            row = 0;
            // Matrix scan complete, restart from beginning
        }
    }
    g_usb_status.current_row = row;
    g_usb_status.current_col = col;
}

/**
  * @brief  Check if scan should be triggered (called from main loop)
  * @retval true if scan should be executed
  */
bool USB_CheckScanTrigger(void)
{
    if (scan_trigger_flag) {
        scan_trigger_flag = false;
        return true;
    }
    return false;
}

/**
  * @brief  Set current row
  */
void USB_SetRow(uint8_t row)
{
    if (row > 15) row = 15;
    g_usb_status.current_row = row;
    MUX_SetYChannel(row);
}

/**
  * @brief  Set current column
  */
void USB_SetCol(uint8_t col)
{
    if (col > 15) col = 15;
    g_usb_status.current_col = col;
    MUX_SetXChannel(col);
}

/**
  * @brief  Get current row
  */
uint8_t USB_GetRow(void)
{
    return g_usb_status.current_row;
}

/**
  * @brief  Get current column
  */
uint8_t USB_GetCol(void)
{
    return g_usb_status.current_col;
}

/**
  * @brief  Format and output scan data with CA/CB format
  */
static void USB_OutputScanData(uint8_t row, uint8_t col, 
                                uint32_t raw_data_ca, double float_data_ca,
                                uint32_t raw_data_cb, double float_data_cb)
{
    extern Template_Config_t g_template_config;
    char ca_value_str[32];
    char cb_value_str[32];
    char value_str[32];
    
    // Format CA value
    if (g_template_config.mode == TEMPLATE_MODE_RAW) {
        if (g_template_config.use_hex) {
            snprintf(ca_value_str, sizeof(ca_value_str), "0x%08X", raw_data_ca);
        } else {
            snprintf(ca_value_str, sizeof(ca_value_str), "%lu", (unsigned long)raw_data_ca);
        }
    } else {  // QUANT mode
        char format_str[16];
        snprintf(format_str, sizeof(format_str), "%%.%df", g_template_config.precision);
        snprintf(ca_value_str, sizeof(ca_value_str), format_str, float_data_ca);
    }
    
    // Format CB value
    if (g_template_config.mode == TEMPLATE_MODE_RAW) {
        if (g_template_config.use_hex) {
            snprintf(cb_value_str, sizeof(cb_value_str), "0x%08X", raw_data_cb);
        } else {
            snprintf(cb_value_str, sizeof(cb_value_str), "%lu", (unsigned long)raw_data_cb);
        }
    } else {  // QUANT mode
        char format_str[16];
        snprintf(format_str, sizeof(format_str), "%%.%df", g_template_config.precision);
        snprintf(cb_value_str, sizeof(cb_value_str), format_str, float_data_cb);
    }
    
    // Format measurement value (use CB)
    if (g_template_config.mode == TEMPLATE_MODE_RAW) {
        if (g_template_config.use_hex) {
            snprintf(value_str, sizeof(value_str), "0x%08X", raw_data_cb);
        } else {
            snprintf(value_str, sizeof(value_str), "%lu", (unsigned long)raw_data_cb);
        }
    } else {  // QUANT mode
        char format_str[16];
        snprintf(format_str, sizeof(format_str), "%%.%df", g_template_config.precision);
        snprintf(value_str, sizeof(value_str), format_str, float_data_cb);
    }
    
    // Output: CA:value,CB:value MODE:X,X:col,Y:row,value\r\n
    USB_Printf("X:%d,Y:%d CA:%s,CB:%s\r\n", col, row,ca_value_str, cb_value_str);
}

/**
  * @brief  Scan specific point
  */
void USB_ScanPoint(uint8_t row, uint8_t col)
{
    USB_SetRow(row);
    USB_SetCol(col);
    MUX_SelectXY(col, row);
    
    // Wait for settling
    HAL_Delay(10);
    
    // Start PCAP04 CDC measurement
    extern void PCap04_CDCStart(void);
    PCap04_CDCStart();
    HAL_Delay(20);  // Wait for measurement to complete
    
    // Read PCAP04 data
    // PC0, PC1: Reference capacitance (CA)
    // PC2, PC3: Measurement capacitance (CB)
    extern uint32_t PCAP04_Read_CDC_Result_data(int Nun);
    extern double integrated_data(uint32_t data);
    
    uint32_t raw_data_pc0 = PCAP04_Read_CDC_Result_data(0);
    uint32_t raw_data_pc1 = PCAP04_Read_CDC_Result_data(1);
    uint32_t raw_data_pc2 = PCAP04_Read_CDC_Result_data(2);
    uint32_t raw_data_pc3 = PCAP04_Read_CDC_Result_data(3);
    
    double float_data_pc0 = integrated_data(raw_data_pc0);
    double float_data_pc1 = integrated_data(raw_data_pc1);
    double float_data_pc2 = integrated_data(raw_data_pc2);
    double float_data_pc3 = integrated_data(raw_data_pc3);
    
    // Calculate CA (reference) - average of PC0 and PC1
    uint32_t raw_data_ca = (raw_data_pc0 + raw_data_pc1) / 2;
    double float_data_ca = (float_data_pc0 + float_data_pc1) / 2.0;
    
    // Calculate CB (measurement) - average of PC2 and PC3
    uint32_t raw_data_cb = (raw_data_pc2 + raw_data_pc3) / 2;
    double float_data_cb = (float_data_pc2 + float_data_pc3) / 2.0;
    
    // Output using new format
    USB_OutputScanData(row, col, raw_data_ca, float_data_ca, raw_data_cb, float_data_cb);
}

/**
  * @brief  Get system status
  */
USB_SystemStatus_t USB_GetStatus(void)
{
    return g_usb_status;
}

/**
  * @brief  Print status
  */
void USB_PrintStatus(void)
{
    const char *mode_names[] = {"STOPPED", "SINGLE_SCAN", "NORMAL", "FAST"};
    USB_Printf("STAT:%s:%dms:%dx%d\r\n", 
               mode_names[g_usb_status.mode],
               g_usb_status.scan_rate_ms,
               g_usb_status.matrix_rows,
               g_usb_status.matrix_cols);
}

/**
  * @brief  Print help
  */
void USB_PrintHelp(void)
{
    // Send all help text using USB_SendLongString to ensure complete transmission
    const char *help_text = 
        "=== USB Command Help ===\r\n"
        "General: START, STOP, STATUS, HELP, ?\r\n"
        "Scan: SET_RATE:<ms>, FAST_MODE, NORMAL_MODE, SINGLE_SCAN\r\n"
        "Matrix: SET_ROW:<n>, SET_COL:<n>, GET_ROW, GET_COL, SCAN_POINT:<r>:<c>, MATRIX_INFO\r\n"
        "PCAP04: PCAP04_STATUS, PCAP04_TEST, PCAP04_READ:<reg>, PCAP04_WRITE:<reg>:<val>\r\n"
        "PCAP04: PCAP04_DUMP, PCAP04_LOAD_DEFAULT, SET_CDIFF:<0/1>, SET_INTREF:<0/1>, SET_EXTREF:<0/1>\r\n"
        "Template: SET_MODE:<raw/quant>, SET_FORMAT:<table/simple>, SET_TABLE_DELIM:<char>\r\n"
        "Template: SET_HEX:<0/1>, SET_PRECISION:<n>, SET_HEADER:<0/1>, SET_MATRIX_SIZE:<r>:<c>\r\n"
        "Queue: QUEUE_START, QUEUE_END, WAIT:<ms>\r\n"
        "Combined: Use && to chain commands\r\n"
        "========================\r\n";
    
    // Use USB_SendLongString to ensure all data is sent properly
    USB_SendLongString(help_text);
}

/**
  * @brief  Print matrix info
  */
void USB_PrintMatrixInfo(void)
{
    USB_Printf("MATRIX_INFO:%dx%d:ROW:%d:COL:%d\r\n",
               g_usb_status.matrix_rows,
               g_usb_status.matrix_cols,
               g_usb_status.current_row,
               g_usb_status.current_col);
}

/**
  * @brief  Send OK response
  */
void USB_SendOK(const char *cmd)
{
    USB_Printf("OK:%s\r\n", cmd);
}

/**
  * @brief  Send error response
  */
void USB_SendError(uint8_t code, const char *info)
{
    USB_Printf("ERR:%d:%s\r\n", code, info);
}

/**
  * @brief  Send status response
  */
void USB_SendStatus(void)
{
    USB_PrintStatus();
}

/**
  * @brief  Start command queue
  */
void USB_QueueStart(void)
{
    cmd_queue_active = true;
    // cmd_queue_head = 0;  // Reserved for future FIFO queue implementation
    // cmd_queue_tail = 0;  // Reserved for future FIFO queue implementation
    memset(cmd_queue, 0, sizeof(cmd_queue));
}

/**
  * @brief  End command queue
  */
void USB_QueueEnd(void)
{
    cmd_queue_active = false;
    // Execute all queued commands
    USB_ProcessCommandQueue();
}

/**
  * @brief  Wait in queue
  */
void USB_QueueWait(uint32_t ms)
{
    HAL_Delay(ms);
}

/**
  * @brief  Process command queue
  */
CMD_Result_t USB_ProcessCommandQueue(void)
{
    CMD_Result_t result = CMD_RESULT_OK;
    
    for (uint8_t i = 0; i < USB_CMD_QUEUE_SIZE; i++) {
        if (cmd_queue[i].valid) {
            CMD_Result_t cmd_result = USB_ProcessCommand(cmd_queue[i].cmd);
            if (cmd_result != CMD_RESULT_OK) {
                result = cmd_result;
            }
            cmd_queue[i].valid = false;
        }
    }
    
    return result;
}

/**
  * @brief  Check if queue is active
  */
bool USB_IsQueueActive(void)
{
    return cmd_queue_active;
}

