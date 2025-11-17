#ifndef __PCAP04_REGISTER_H
#define __PCAP04_REGISTER_H

#include <stdint.h>

/* 寄存器总数 */
#define PCAP04_REG_COUNT 52

typedef struct
{
	uint8_t value;
	const char *name;
	const char *description;
} PCAP04_BitOption_t;

typedef struct
{
	uint8_t bit_start;
	uint8_t bit_end;
	const char *name;
	const char *description;
	uint8_t options_count;
	const PCAP04_BitOption_t *options;
} PCAP04_RegBit_t;

typedef struct
{
	uint8_t address;
	const char *name;
	const char *description;
	uint8_t bits_count;
	const PCAP04_RegBit_t *bits;
	uint8_t default_value;
} PCAP04_Register_t;

typedef struct
{
	uint8_t registers_count;
	const PCAP04_Register_t *registers;
} PCAP04_RegisterTable_t;

typedef struct
{
	uint8_t reg_addr;
	uint8_t bit_start;
	uint8_t bit_end;
	uint8_t value;
} PCAP04_RegConfig_t;

extern const PCAP04_RegisterTable_t PCAP04_REGISTER_TABLE;

void PCAP04_InitRegisters(uint8_t *reg_array);
uint8_t PCAP04_SetRegisterBit(uint8_t *reg_array, uint8_t reg_addr, const char *bit_name, uint8_t value);
uint8_t PCAP04_GetRegisterBit(const uint8_t *reg_array, uint8_t reg_addr, const char *bit_name);
void PCAP04_PrintRegister(const uint8_t *reg_array, uint8_t reg_addr);
void PCAP04_PrintAllRegisters(const uint8_t *reg_array);
uint8_t PCAP04_SaveToFlash(const uint8_t *reg_array, uint32_t flash_addr);
uint8_t PCAP04_LoadFromFlash(uint8_t *reg_array, uint32_t flash_addr);

uint8_t PCAP04_ConfigRegisters(uint8_t *reg_array, const PCAP04_RegConfig_t *configs, uint8_t count);

uint8_t PCAP04_GetBitMask(uint8_t bit_start, uint8_t bit_end);
uint8_t PCAP04_SetBitValue(uint8_t reg_value, uint8_t bit_start, uint8_t bit_end, uint8_t value);
uint8_t PCAP04_GetBitValue(uint8_t reg_value, uint8_t bit_start, uint8_t bit_end);

const PCAP04_Register_t* PCAP04_GetRegisterDefinition(uint8_t reg_addr);

#endif /* __PCAP04_REGISTER_H */

#ifndef __PCAP04_REGISTER_H
#define __PCAP04_REGISTER_H

#include <stdint.h>

// 寄存器总数
#define PCAP04_REG_COUNT 52

// 位选项定义
typedef struct {
    uint8_t value;          // 位值
    const char *name;       // 选项名称
    const char *description;// 中文描述
} PCAP04_BitOption_t;

// 寄存器位定义
typedef struct {
    uint8_t bit_start;      // 起始位
    uint8_t bit_end;        // 结束位
    const char *name;       // 位字段名称
    const char *description;// 中文描述
    uint8_t options_count;  // 可选项数量
    const PCAP04_BitOption_t *options; // 可选项数组
} PCAP04_RegBit_t;

// 寄存器定义
typedef struct {
    uint8_t address;        // 寄存器地址
    const char *name;       // 寄存器名称
    const char *description;// 中文描述
    uint8_t bits_count;     // 位字段数量
    const PCAP04_RegBit_t *bits; // 位字段数组
    uint8_t default_value;  // 默认值
} PCAP04_Register_t;

// 寄存器表定义
typedef struct {
    uint8_t registers_count;// 寄存器总数
    const PCAP04_Register_t *registers; // 寄存器数组
} PCAP04_RegisterTable_t;

// 寄存器配置结构体
typedef struct {
    uint8_t reg_addr;       // 寄存器地址
    uint8_t bit_start;      // 位起始
    uint8_t bit_end;        // 位结束
    uint8_t value;          // 配置值
} PCAP04_RegConfig_t;

// 全局寄存器表声明
extern const PCAP04_RegisterTable_t PCAP04_REGISTER_TABLE;

// 寄存器操作函数
void PCAP04_InitRegisters(uint8_t *reg_array);
uint8_t PCAP04_SetRegisterBit(uint8_t *reg_array, uint8_t reg_addr, const char *bit_name, uint8_t value);
uint8_t PCAP04_GetRegisterBit(const uint8_t *reg_array, uint8_t reg_addr, const char *bit_name);
void PCAP04_PrintRegister(const uint8_t *reg_array, uint8_t reg_addr);
void PCAP04_PrintAllRegisters(const uint8_t *reg_array);
uint8_t PCAP04_SaveToFlash(const uint8_t *reg_array, uint32_t flash_addr);
uint8_t PCAP04_LoadFromFlash(uint8_t *reg_array, uint32_t flash_addr);

// 批量配置函数
uint8_t PCAP04_ConfigRegisters(uint8_t *reg_array, const PCAP04_RegConfig_t *configs, uint8_t count);

// 位操作辅助函数
uint8_t PCAP04_GetBitMask(uint8_t bit_start, uint8_t bit_end);
uint8_t PCAP04_SetBitValue(uint8_t reg_value, uint8_t bit_start, uint8_t bit_end, uint8_t value);
uint8_t PCAP04_GetBitValue(uint8_t reg_value, uint8_t bit_start, uint8_t bit_end);

const PCAP04_Register_t* PCAP04_GetRegisterDefinition(uint8_t reg_addr);

#endif // __PCAP04_REGISTER_H
