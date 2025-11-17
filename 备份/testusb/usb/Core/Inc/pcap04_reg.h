/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pcap04_reg.h
  * @brief          : PCAP04 Register Control System
  *                   Register access and configuration based on datasheet
  ******************************************************************************
  * @attention
  *
  * PCAP04 Register Control System
  * Based on PCAP04 Datasheet DS000574-2-00
  * 
  * Features:
  * - Register read/write operations
  * - Register dump functionality
  * - Configuration management
  * - Status monitoring
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __PCAP04_REG_H
#define __PCAP04_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pcap04.h"
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  PCAP04 register access mode
  */
typedef enum {
    PCAP04_REGMODE_SINGLE = 0,   // Single register access
    PCAP04_REGMODE_BULK           // Bulk register access
} PCAP04_RegMode_t;

/**
  * @brief  PCAP04 status structure
  */
typedef struct {
    bool spi_ok;                  // SPI communication OK
    bool initialized;             // PCAP04 initialized
    uint8_t runbit;               // RUNBIT status
    uint8_t cdc_active;           // CDC active flag
    uint8_t rdc_ready;            // RDC ready flag
    uint8_t error_flags;          // Error flags
} PCAP04_Status_t;

/* Exported constants --------------------------------------------------------*/

#define PCAP04_REG_COUNT          64      // Total register count (0-63)
#define PCAP04_REG_WRITE_OPCODE   0xA0    // Write register opcode
#define PCAP04_REG_READ_OPCODE    0x44    // Read register opcode

// Register addresses (Configuration Registers 0-47)
#define PCAP04_REG_CFG0           0x00
#define PCAP04_REG_CFG1           0x01
#define PCAP04_REG_CFG4           0x04    // C_REF_INT, C_COMP_EXT, C_COMP_INT, C_DIFFERENTIAL, C_FLOATING
#define PCAP04_REG_CFG6           0x06    // C_PORT_EN
#define PCAP04_REG_CFG47          0x2F    // RUNBIT

// Status register addresses
#define PCAP04_REG_STATUS0        0x20    // STATUS_0 (Address 32)
#define PCAP04_REG_STATUS1        0x21    // STATUS_1 (Address 33)
#define PCAP04_REG_STATUS2        0x22    // STATUS_2 (Address 34)

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern uint8_t pcap04_registers[PCAP04_REG_COUNT];
extern PCAP04_Status_t pcap04_status;
extern PCAP04_RegMode_t pcap04_reg_mode;

/* Exported functions prototypes ---------------------------------------------*/

// Initialization
void PCAP04_Reg_Init(void);

// Register access
uint8_t PCAP04_Reg_Read(uint8_t reg_addr);
bool PCAP04_Reg_Write(uint8_t reg_addr, uint8_t value);
void PCAP04_Reg_Dump(void);
void PCAP04_Reg_LoadDefault(void);

// Bulk operations
bool PCAP04_Reg_WriteBulk(uint8_t start_addr, uint8_t *data, uint8_t count);
bool PCAP04_Reg_ReadBulk(uint8_t start_addr, uint8_t *data, uint8_t count);

// Status and test
bool PCAP04_Reg_Test(void);
PCAP04_Status_t PCAP04_Reg_GetStatus(void);
void PCAP04_Reg_PrintStatus(void);

// Configuration helpers
void PCAP04_Reg_SetDifferential(bool enable);
void PCAP04_Reg_SetFloating(bool enable);
void PCAP04_Reg_SetInternalRef(bool enable);
void PCAP04_Reg_SetExternalRef(bool enable);
void PCAP04_Reg_SetPortEnable(uint8_t port_mask);
void PCAP04_Reg_SetReferenceSel(uint8_t ref_sel);
void PCAP04_Reg_SetRegMode(PCAP04_RegMode_t mode);

// Register bit manipulation
void PCAP04_Reg_SetBit(uint8_t reg_addr, uint8_t bit);
void PCAP04_Reg_ClearBit(uint8_t reg_addr, uint8_t bit);
bool PCAP04_Reg_GetBit(uint8_t reg_addr, uint8_t bit);

#ifdef __cplusplus
}
#endif

#endif /* __PCAP04_REG_H */

