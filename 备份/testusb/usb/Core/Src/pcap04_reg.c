/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pcap04_reg.c
  * @brief          : PCAP04 Register Control System Implementation
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "pcap04_reg.h"
#include "pcap04.h"
#include "spi.h"
#include "usbd_cdc_if.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define FLASH_SPI_CS_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()    
#define FLASH_SPI_CS_PORT                          SPI_SSN_GPIO_Port
#define FLASH_SPI_CS_PIN                           SPI_SSN_Pin
#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint8_t pcap04_registers[PCAP04_REG_COUNT];
PCAP04_Status_t pcap04_status = {0};
PCAP04_RegMode_t pcap04_reg_mode = PCAP04_REGMODE_SINGLE;

// Default configuration (from pcap04.c)
static const uint8_t pcap04_default_config[64] = {
    0x1D, 0x00, 0x58, 0x10, 0x10, 0x00, 0x0F, 0x20,
    0x00, 0xD0, 0x07, 0x00, 0x00, 0x08, 0xFF, 0x03,
    0x00, 0x24, 0x00, 0x00, 0x00, 0x01, 0x50, 0x30,
    0x73, 0x04, 0x50, 0x08, 0x5A, 0x00, 0x82, 0x08,
    0x08, 0x00, 0x47, 0x40, 0x00, 0x00, 0x00, 0x71,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize PCAP04 register system
  */
void PCAP04_Reg_Init(void)
{
    memset(pcap04_registers, 0, sizeof(pcap04_registers));
    memset(&pcap04_status, 0, sizeof(pcap04_status));
    pcap04_reg_mode = PCAP04_REGMODE_SINGLE;
    
    // Load default configuration
    memcpy(pcap04_registers, pcap04_default_config, sizeof(pcap04_default_config));
}

/**
  * @brief  Read PCAP04 register
  */
uint8_t PCAP04_Reg_Read(uint8_t reg_addr)
{
    if (reg_addr >= PCAP04_REG_COUNT) {
        return 0;
    }
    
    uint8_t opcode = PCAP04_REG_READ_OPCODE;
    uint8_t data = 0;
    
    FLASH_SPI_CS_ENABLE();
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi2, &opcode, 1, 100);
    HAL_SPI_Transmit(&hspi2, &reg_addr, 1, 100);
    HAL_SPI_Receive(&hspi2, &data, 1, 100);
    HAL_Delay(1);
    FLASH_SPI_CS_DISABLE();
    
    pcap04_registers[reg_addr] = data;
    return data;
}

/**
  * @brief  Write PCAP04 register
  */
bool PCAP04_Reg_Write(uint8_t reg_addr, uint8_t value)
{
    if (reg_addr >= PCAP04_REG_COUNT) {
        return false;
    }
    
    uint8_t opcode = PCAP04_REG_WRITE_OPCODE;
    
    FLASH_SPI_CS_ENABLE();
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi2, &opcode, 1, 100);
    HAL_SPI_Transmit(&hspi2, &reg_addr, 1, 100);
    HAL_SPI_Transmit(&hspi2, &value, 1, 100);
    HAL_Delay(1);
    FLASH_SPI_CS_DISABLE();
    
    pcap04_registers[reg_addr] = value;
    return true;
}

/**
  * @brief  Dump all registers
  */
void PCAP04_Reg_Dump(void)
{
    USB_Printf("=== PCAP04 Register Dump ===\r\n");
    USB_Printf("Addr  Value  Addr  Value  Addr  Value  Addr  Value\r\n");
    
    for (uint8_t i = 0; i < PCAP04_REG_COUNT; i += 4) {
        USB_Printf("0x%02X   0x%02X  0x%02X   0x%02X  0x%02X   0x%02X  0x%02X   0x%02X\r\n",
                   i, pcap04_registers[i],
                   i+1, (i+1 < PCAP04_REG_COUNT) ? pcap04_registers[i+1] : 0,
                   i+2, (i+2 < PCAP04_REG_COUNT) ? pcap04_registers[i+2] : 0,
                   i+3, (i+3 < PCAP04_REG_COUNT) ? pcap04_registers[i+3] : 0);
    }
    USB_Printf("============================\r\n");
}

/**
  * @brief  Load default configuration
  */
void PCAP04_Reg_LoadDefault(void)
{
    for (uint8_t i = 0; i < PCAP04_REG_COUNT; i++) {
        PCAP04_Reg_Write(i, pcap04_default_config[i]);
    }
    USB_Printf("PCAP04 default configuration loaded\r\n");
}

/**
  * @brief  Write bulk registers
  */
bool PCAP04_Reg_WriteBulk(uint8_t start_addr, uint8_t *data, uint8_t count)
{
    if (start_addr + count > PCAP04_REG_COUNT) {
        return false;
    }
    
    for (uint8_t i = 0; i < count; i++) {
        if (!PCAP04_Reg_Write(start_addr + i, data[i])) {
            return false;
        }
        HAL_Delay(1);
    }
    
    return true;
}

/**
  * @brief  Read bulk registers
  */
bool PCAP04_Reg_ReadBulk(uint8_t start_addr, uint8_t *data, uint8_t count)
{
    if (start_addr + count > PCAP04_REG_COUNT) {
        return false;
    }
    
    for (uint8_t i = 0; i < count; i++) {
        data[i] = PCAP04_Reg_Read(start_addr + i);
        HAL_Delay(1);
    }
    
    return true;
}

/**
  * @brief  Test PCAP04 SPI communication
  */
bool PCAP04_Reg_Test(void)
{
    return (PCap04_Test() == 1);
}

/**
  * @brief  Get PCAP04 status
  */
PCAP04_Status_t PCAP04_Reg_GetStatus(void)
{
    // Read status registers
    uint8_t status0 = PCAP04_Reg_Read(PCAP04_REG_STATUS0);
    uint8_t status1 = PCAP04_Reg_Read(PCAP04_REG_STATUS1);
    
    pcap04_status.runbit = (status0 >> 0) & 0x01;
    pcap04_status.cdc_active = (status0 >> 1) & 0x01;
    pcap04_status.rdc_ready = (status0 >> 2) & 0x01;
    pcap04_status.error_flags = status1;
    pcap04_status.spi_ok = PCAP04_Reg_Test();
    
    return pcap04_status;
}

/**
  * @brief  Print PCAP04 status
  */
void PCAP04_Reg_PrintStatus(void)
{
    PCAP04_Status_t status = PCAP04_Reg_GetStatus();
    
    USB_Printf("=== PCAP04 Status ===\r\n");
    USB_Printf("SPI OK: %s\r\n", status.spi_ok ? "YES" : "NO");
    USB_Printf("Initialized: %s\r\n", status.initialized ? "YES" : "NO");
    USB_Printf("RUNBIT: %d\r\n", status.runbit);
    USB_Printf("CDC Active: %d\r\n", status.cdc_active);
    USB_Printf("RDC Ready: %d\r\n", status.rdc_ready);
    USB_Printf("Error Flags: 0x%02X\r\n", status.error_flags);
    USB_Printf("====================\r\n");
}

/**
  * @brief  Set differential mode
  */
void PCAP04_Reg_SetDifferential(bool enable)
{
    uint8_t reg4 = PCAP04_Reg_Read(PCAP04_REG_CFG4);
    if (enable) {
        reg4 |= (1 << 1);  // Set C_DIFFERENTIAL bit
    } else {
        reg4 &= ~(1 << 1);  // Clear C_DIFFERENTIAL bit
    }
    PCAP04_Reg_Write(PCAP04_REG_CFG4, reg4);
}

/**
  * @brief  Set floating mode
  */
void PCAP04_Reg_SetFloating(bool enable)
{
    uint8_t reg4 = PCAP04_Reg_Read(PCAP04_REG_CFG4);
    if (enable) {
        reg4 |= (1 << 0);  // Set C_FLOATING bit
    } else {
        reg4 &= ~(1 << 0);  // Clear C_FLOATING bit
    }
    PCAP04_Reg_Write(PCAP04_REG_CFG4, reg4);
}

/**
  * @brief  Set internal reference
  */
void PCAP04_Reg_SetInternalRef(bool enable)
{
    uint8_t reg4 = PCAP04_Reg_Read(PCAP04_REG_CFG4);
    if (enable) {
        reg4 |= (1 << 7);  // Set C_REF_INT bit
    } else {
        reg4 &= ~(1 << 7);  // Clear C_REF_INT bit
    }
    PCAP04_Reg_Write(PCAP04_REG_CFG4, reg4);
}

/**
  * @brief  Set external reference
  */
void PCAP04_Reg_SetExternalRef(bool enable)
{
    PCAP04_Reg_SetInternalRef(!enable);
}

/**
  * @brief  Set port enable mask
  */
void PCAP04_Reg_SetPortEnable(uint8_t port_mask)
{
    PCAP04_Reg_Write(PCAP04_REG_CFG6, port_mask & 0x3F);  // Only bits 0-5
}

/**
  * @brief  Set reference selection
  */
void PCAP04_Reg_SetReferenceSel(uint8_t ref_sel)
{
    if (ref_sel > 31) ref_sel = 31;
    
    uint8_t reg17 = PCAP04_Reg_Read(0x11);  // CFG17
    reg17 &= ~(0x1F << 2);  // Clear C_REF_SEL bits
    reg17 |= (ref_sel << 2);  // Set C_REF_SEL bits
    PCAP04_Reg_Write(0x11, reg17);
}

/**
  * @brief  Set register access mode
  */
void PCAP04_Reg_SetRegMode(PCAP04_RegMode_t mode)
{
    pcap04_reg_mode = mode;
}

/**
  * @brief  Set register bit
  */
void PCAP04_Reg_SetBit(uint8_t reg_addr, uint8_t bit)
{
    if (reg_addr >= PCAP04_REG_COUNT || bit > 7) return;
    
    uint8_t value = PCAP04_Reg_Read(reg_addr);
    value |= (1 << bit);
    PCAP04_Reg_Write(reg_addr, value);
}

/**
  * @brief  Clear register bit
  */
void PCAP04_Reg_ClearBit(uint8_t reg_addr, uint8_t bit)
{
    if (reg_addr >= PCAP04_REG_COUNT || bit > 7) return;
    
    uint8_t value = PCAP04_Reg_Read(reg_addr);
    value &= ~(1 << bit);
    PCAP04_Reg_Write(reg_addr, value);
}

/**
  * @brief  Get register bit
  */
bool PCAP04_Reg_GetBit(uint8_t reg_addr, uint8_t bit)
{
    if (reg_addr >= PCAP04_REG_COUNT || bit > 7) return false;
    
    uint8_t value = PCAP04_Reg_Read(reg_addr);
    return (value >> bit) & 0x01;
}

