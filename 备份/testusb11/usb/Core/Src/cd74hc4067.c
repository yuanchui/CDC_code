/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : cd74hc4067.c
  * @brief          : CD74HC4067 16-channel analog multiplexer driver
  ******************************************************************************
  * @attention
  *
  * CD74HC4067 is a 16-channel analog multiplexer/demultiplexer
  * 
  * Pin Configuration:
  * - S0, S1, S2, S3: 4-bit binary channel selection (0-15)
  * - ENX: Enable for X multiplexer (active low)
  * - ENY: Enable for Y multiplexer (active low)
  *
  * Usage Examples:
  * 1. Scan specific row: MUX_ScanRow(5) - Y=5, X scans 0-15
  * 2. Scan specific column: MUX_ScanColumn(3) - X=3, Y scans 0-15
  * 3. Select specific point: MUX_SelectXY(2, 7) - X=2, Y=7
  * 4. Single channel: MUX_SelectX(10) - Only X=10 enabled
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "cd74hc4067.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static MUX_Status_t mux_status = {
    .mode = MUX_MODE_DISABLED,
    .x_channel = 0,
    .y_channel = 0,
    .enx_state = 1,  // Disabled (high)
    .eny_state = 1   // Disabled (high)
};

/* Private function prototypes -----------------------------------------------*/
static void MUX_SetXChannel(uint8_t channel);
static void MUX_SetYChannel(uint8_t channel);
static void MUX_SetChannelBits(uint8_t channel, GPIO_TypeDef* s0_port, uint16_t s0_pin,
                               GPIO_TypeDef* s1_port, uint16_t s1_pin,
                               GPIO_TypeDef* s2_port, uint16_t s2_pin,
                               GPIO_TypeDef* s3_port, uint16_t s3_pin);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Set channel selection bits for X or Y multiplexer
  * @param  channel: Channel number (0-15)
  * @param  s0_port, s0_pin: S0 pin configuration
  * @param  s1_port, s1_pin: S1 pin configuration
  * @param  s2_port, s2_pin: S2 pin configuration
  * @param  s3_port, s3_pin: S3 pin configuration
  * @retval None
  */
static void MUX_SetChannelBits(uint8_t channel, GPIO_TypeDef* s0_port, uint16_t s0_pin,
                               GPIO_TypeDef* s1_port, uint16_t s1_pin,
                               GPIO_TypeDef* s2_port, uint16_t s2_pin,
                               GPIO_TypeDef* s3_port, uint16_t s3_pin)
{
    // Limit channel to 0-15
    channel &= 0x0F;
    
    // Set S0 (bit 0)
    HAL_GPIO_WritePin(s0_port, s0_pin, (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // Set S1 (bit 1)
    HAL_GPIO_WritePin(s1_port, s1_pin, (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // Set S2 (bit 2)
    HAL_GPIO_WritePin(s2_port, s2_pin, (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    // Set S3 (bit 3)
    HAL_GPIO_WritePin(s3_port, s3_pin, (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  Set X multiplexer channel
  * @param  channel: Channel number (0-15)
  * @retval None
  */
static void MUX_SetXChannel(uint8_t channel)
{
    MUX_SetChannelBits(channel, SX0_GPIO_Port, SX0_Pin,
                               SX1_GPIO_Port, SX1_Pin,
                               SX2_GPIO_Port, SX2_Pin,
                               SX3_GPIO_Port, SX3_Pin);
    mux_status.x_channel = channel & 0x0F;
}

/**
  * @brief  Set Y multiplexer channel
  * @param  channel: Channel number (0-15)
  * @retval None
  */
static void MUX_SetYChannel(uint8_t channel)
{
    MUX_SetChannelBits(channel, SY0_GPIO_Port, SY0_Pin,
                               SY1_GPIO_Port, SY1_Pin,
                               SY2_GPIO_Port, SY2_Pin,
                               SY3_GPIO_Port, SY3_Pin);
    mux_status.y_channel = channel & 0x0F;
}

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize CD74HC4067 multiplexer
  * @retval None
  */
void MUX_Init(void)
{
    // Initialize all channels to 0
    MUX_SetXChannel(0);
    MUX_SetYChannel(0);
    
    // Disable both multiplexers initially
    MUX_DisableBoth();
    
    // Update status
    mux_status.mode = MUX_MODE_DISABLED;
    mux_status.x_channel = 0;
    mux_status.y_channel = 0;
    
    printf("CD74HC4067 Multiplexer initialized\r\n");
}

/**
  * @brief  Select multiplexer mode and channels
  * @param  mode: Operation mode
  * @param  x_channel: X channel (0-15)
  * @param  y_channel: Y channel (0-15)
  * @retval None
  */
void MUX_SelectMode(MUX_Mode_t mode, uint8_t x_channel, uint8_t y_channel)
{
    // Set channels first
    MUX_SetXChannel(x_channel);
    MUX_SetYChannel(y_channel);
    
    // Apply mode
    switch(mode) {
        case MUX_MODE_DISABLED:
            MUX_DisableBoth();
            break;
            
        case MUX_MODE_X_ONLY:
            MUX_EnableX();
            MUX_DisableY();
            break;
            
        case MUX_MODE_Y_ONLY:
            MUX_DisableX();
            MUX_EnableY();
            break;
            
        case MUX_MODE_XY_BOTH:
            MUX_EnableBoth();
            break;
            
        default:
            MUX_DisableBoth();
            break;
    }
    
    mux_status.mode = mode;
    
    printf("MUX Mode: %d, X: %d, Y: %d\r\n", mode, x_channel, y_channel);
}

/**
  * @brief  Select only X channel
  * @param  channel: Channel number (0-15)
  * @retval None
  */
void MUX_SelectX(uint8_t channel)
{
    MUX_SelectMode(MUX_MODE_X_ONLY, channel, mux_status.y_channel);
}

/**
  * @brief  Select only Y channel
  * @param  channel: Channel number (0-15)
  * @retval None
  */
void MUX_SelectY(uint8_t channel)
{
    MUX_SelectMode(MUX_MODE_Y_ONLY, mux_status.x_channel, channel);
}

/**
  * @brief  Select both X and Y channels
  * @param  x_channel: X channel (0-15)
  * @param  y_channel: Y channel (0-15)
  * @retval None
  */
void MUX_SelectXY(uint8_t x_channel, uint8_t y_channel)
{
    MUX_SelectMode(MUX_MODE_XY_BOTH, x_channel, y_channel);
}

/**
  * @brief  Enable X multiplexer
  * @retval None
  */
void MUX_EnableX(void)
{
    HAL_GPIO_WritePin(ENX_GPIO_Port, ENX_Pin, GPIO_PIN_RESET);  // Active low
    mux_status.enx_state = 0;
}

/**
  * @brief  Disable X multiplexer
  * @retval None
  */
void MUX_DisableX(void)
{
    HAL_GPIO_WritePin(ENX_GPIO_Port, ENX_Pin, GPIO_PIN_SET);    // Active low
    mux_status.enx_state = 1;
}

/**
  * @brief  Enable Y multiplexer
  * @retval None
  */
void MUX_EnableY(void)
{
    HAL_GPIO_WritePin(ENY_GPIO_Port, ENY_Pin, GPIO_PIN_RESET);  // Active low
    mux_status.eny_state = 0;
}

/**
  * @brief  Disable Y multiplexer
  * @retval None
  */
void MUX_DisableY(void)
{
    HAL_GPIO_WritePin(ENY_GPIO_Port, ENY_Pin, GPIO_PIN_SET);    // Active low
    mux_status.eny_state = 1;
}

/**
  * @brief  Enable both X and Y multiplexers
  * @retval None
  */
void MUX_EnableBoth(void)
{
    MUX_EnableX();
    MUX_EnableY();
}

/**
  * @brief  Disable both X and Y multiplexers
  * @retval None
  */
void MUX_DisableBoth(void)
{
    MUX_DisableX();
    MUX_DisableY();
}

/**
  * @brief  Scan specific row (Y fixed, X scans 0-15)
  * @param  row: Row number (Y channel, 0-15)
  * @retval None
  */
void MUX_ScanRow(uint8_t row)
{
    printf("=== Scanning Row %d ===\r\n", row);
    
    // Set Y channel to the specified row
    MUX_SetYChannel(row);
    MUX_EnableY();
    MUX_EnableX();
    
    // Scan all X channels (columns) in this row
    for(uint8_t x = 0; x <= MUX_CHANNEL_MAX; x++) {
        MUX_SetXChannel(x);
        printf("Position [%d,%d] - ", x, row);
        
        // Add your measurement code here
        // Example: Read PCAP04 data
        HAL_Delay(50);  // Settling time
        
        printf("measured\r\n");
    }
    
    mux_status.mode = MUX_MODE_XY_BOTH;
    printf("Row %d scan completed\r\n", row);
}

/**
  * @brief  Scan specific column (X fixed, Y scans 0-15)
  * @param  column: Column number (X channel, 0-15)
  * @retval None
  */
void MUX_ScanColumn(uint8_t column)
{
    printf("=== Scanning Column %d ===\r\n", column);
    
    // Set X channel to the specified column
    MUX_SetXChannel(column);
    MUX_EnableX();
    MUX_EnableY();
    
    // Scan all Y channels (rows) in this column
    for(uint8_t y = 0; y <= MUX_CHANNEL_MAX; y++) {
        MUX_SetYChannel(y);
        printf("Position [%d,%d] - ", column, y);
        
        // Add your measurement code here
        // Example: Read PCAP04 data
        HAL_Delay(50);  // Settling time
        
        printf("measured\r\n");
    }
    
    mux_status.mode = MUX_MODE_XY_BOTH;
    printf("Column %d scan completed\r\n", column);
}

/**
  * @brief  Scan all positions (16x16 matrix)
  * @retval None
  */
void MUX_ScanAll(void)
{
    printf("=== Scanning All Positions (16x16 Matrix) ===\r\n");
    
    MUX_EnableBoth();
    
    for(uint8_t y = 0; y <= MUX_CHANNEL_MAX; y++) {
        MUX_SetYChannel(y);
        
        for(uint8_t x = 0; x <= MUX_CHANNEL_MAX; x++) {
            MUX_SetXChannel(x);
            printf("[%2d,%2d] ", x, y);
            
            // Add your measurement code here
            // Example: Read PCAP04 data
            HAL_Delay(10);  // Settling time
            
            if((x + 1) % 8 == 0) printf("\r\n");  // New line every 8 positions
        }
        printf("\r\n");
    }
    
    mux_status.mode = MUX_MODE_XY_BOTH;
    printf("Full matrix scan completed (256 positions)\r\n");
}

/**
  * @brief  Get current multiplexer status
  * @retval MUX_Status_t: Current status structure
  */
MUX_Status_t MUX_GetStatus(void)
{
    return mux_status;
}

/**
  * @brief  Print current multiplexer status
  * @retval None
  */
void MUX_PrintStatus(void)
{
    const char* mode_names[] = {"DISABLED", "X_ONLY", "Y_ONLY", "XY_BOTH"};
    
    printf("=== CD74HC4067 Multiplexer Status ===\r\n");
    printf("Mode: %s\r\n", mode_names[mux_status.mode]);
    printf("X Channel: %d (0x%X)\r\n", mux_status.x_channel, mux_status.x_channel);
    printf("Y Channel: %d (0x%X)\r\n", mux_status.y_channel, mux_status.y_channel);
    printf("ENX State: %s\r\n", mux_status.enx_state ? "Disabled (High)" : "Enabled (Low)");
    printf("ENY State: %s\r\n", mux_status.eny_state ? "Disabled (High)" : "Enabled (Low)");
    
    // Show binary representation
    printf("X Binary: %d%d%d%d\r\n", 
           (mux_status.x_channel >> 3) & 1,
           (mux_status.x_channel >> 2) & 1,
           (mux_status.x_channel >> 1) & 1,
           (mux_status.x_channel >> 0) & 1);
    printf("Y Binary: %d%d%d%d\r\n", 
           (mux_status.y_channel >> 3) & 1,
           (mux_status.y_channel >> 2) & 1,
           (mux_status.y_channel >> 1) & 1,
           (mux_status.y_channel >> 0) & 1);
    printf("=====================================\r\n");
}

/**
  * @brief  Reset multiplexer to initial state
  * @retval None
  */
void MUX_Reset(void)
{
    MUX_Init();
    printf("CD74HC4067 Multiplexer reset to initial state\r\n");
}
