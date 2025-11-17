#include "pcap04_register.h"
#include <string.h>
#include <stdio.h>

/*
 * 模块说明：
 * 该文件提供 PCAP04 寄存器配置相关的通用工具，包括寄存器描述查找、
 * 位段读写、寄存器打印以及（示例）Flash 存取占位函数。所有接口均以
 * 寄存器镜像数组为输入，便于上层维护本地缓存并批量写入设备。
 */

static const PCAP04_Register_t* PCAP04_FindRegister(uint8_t reg_addr);
static const PCAP04_RegBit_t* PCAP04_FindBit(const PCAP04_Register_t *reg, const char *bit_name);

uint8_t PCAP04_GetBitMask(uint8_t bit_start, uint8_t bit_end)
{
	uint8_t mask = 0;
	for (uint8_t i = bit_start; i <= bit_end; i++)
	{
		mask |= (1U << i);
	}
	return mask;
}

uint8_t PCAP04_SetBitValue(uint8_t reg_value, uint8_t bit_start, uint8_t bit_end, uint8_t value)
{
	uint8_t mask = PCAP04_GetBitMask(bit_start, bit_end);
	reg_value &= (uint8_t)~mask;
	reg_value |= (uint8_t)((value << bit_start) & mask);
	return reg_value;
}

uint8_t PCAP04_GetBitValue(uint8_t reg_value, uint8_t bit_start, uint8_t bit_end)
{
	uint8_t mask = PCAP04_GetBitMask(bit_start, bit_end);
	return (uint8_t)((reg_value & mask) >> bit_start);
}

const PCAP04_Register_t* PCAP04_GetRegisterDefinition(uint8_t reg_addr)
{
	return PCAP04_FindRegister(reg_addr);
}

static const PCAP04_Register_t* PCAP04_FindRegister(uint8_t reg_addr)
{
	for (uint8_t i = 0; i < PCAP04_REGISTER_TABLE.registers_count; i++)
	{
		if (PCAP04_REGISTER_TABLE.registers[i].address == reg_addr)
		{
			return &PCAP04_REGISTER_TABLE.registers[i];
		}
	}
	return NULL;
}

static const PCAP04_RegBit_t* PCAP04_FindBit(const PCAP04_Register_t *reg, const char *bit_name)
{
	if (reg == NULL || bit_name == NULL)
	{
		return NULL;
	}
	for (uint8_t i = 0; i < reg->bits_count; i++)
	{
		if (strcmp(reg->bits[i].name, bit_name) == 0)
		{
			return &reg->bits[i];
		}
	}
	return NULL;
}

void PCAP04_InitRegisters(uint8_t *reg_array)
{
	if (reg_array == NULL)
	{
		return;
	}
	memset(reg_array, 0x00, PCAP04_REG_COUNT);
	for (uint8_t i = 0; i < PCAP04_REGISTER_TABLE.registers_count; i++)
	{
		uint8_t addr = PCAP04_REGISTER_TABLE.registers[i].address;
		if (addr < PCAP04_REG_COUNT)
		{
			reg_array[addr] = PCAP04_REGISTER_TABLE.registers[i].default_value;
		}
	}
}

uint8_t PCAP04_SetRegisterBit(uint8_t *reg_array, uint8_t reg_addr, const char *bit_name, uint8_t value)
{
	if (reg_array == NULL || bit_name == NULL || reg_addr >= PCAP04_REG_COUNT)
	{
		return 1;
	}

	const PCAP04_Register_t *reg = PCAP04_FindRegister(reg_addr);
	if (reg == NULL)
	{
		return 1;
	}

	const PCAP04_RegBit_t *bit = PCAP04_FindBit(reg, bit_name);
	if (bit == NULL)
	{
		return 1;
	}

	reg_array[reg_addr] = PCAP04_SetBitValue(reg_array[reg_addr], bit->bit_start, bit->bit_end, value);
	return 0;
}

uint8_t PCAP04_GetRegisterBit(const uint8_t *reg_array, uint8_t reg_addr, const char *bit_name)
{
	if (reg_array == NULL || bit_name == NULL || reg_addr >= PCAP04_REG_COUNT)
	{
		return 0;
	}

	const PCAP04_Register_t *reg = PCAP04_FindRegister(reg_addr);
	if (reg == NULL)
	{
		return 0;
	}

	const PCAP04_RegBit_t *bit = PCAP04_FindBit(reg, bit_name);
	if (bit == NULL)
	{
		return 0;
	}

	return PCAP04_GetBitValue(reg_array[reg_addr], bit->bit_start, bit->bit_end);
}

uint8_t PCAP04_ConfigRegisters(uint8_t *reg_array, const PCAP04_RegConfig_t *configs, uint8_t count)
{
	if (reg_array == NULL || configs == NULL)
	{
		return count;
	}

	uint8_t fail_count = 0U;
	for (uint8_t i = 0; i < count; i++)
	{
		if (configs[i].reg_addr >= PCAP04_REG_COUNT)
		{
			fail_count++;
			continue;
		}
		reg_array[configs[i].reg_addr] = PCAP04_SetBitValue(reg_array[configs[i].reg_addr],
		                                                    configs[i].bit_start,
		                                                    configs[i].bit_end,
		                                                    configs[i].value);
	}
	return fail_count;
}

void PCAP04_PrintRegister(const uint8_t *reg_array, uint8_t reg_addr)
{
	if (reg_array == NULL || reg_addr >= PCAP04_REG_COUNT)
	{
		printf("错误: 无效的寄存器地址 0x%02X\n", reg_addr);
		return;
	}

	const PCAP04_Register_t *reg = PCAP04_FindRegister(reg_addr);
	if (reg == NULL)
	{
		printf("寄存器[0x%02X] = 0x%02X (未定义)\n", reg_addr, reg_array[reg_addr]);
		return;
	}

	printf("\n======================================\n");
	printf("寄存器[0x%02X]: %s\n", reg_addr, reg->name);
	printf("描述: %s\n", reg->description);
	printf("当前值: 0x%02X (二进制: ", reg_array[reg_addr]);
	for (int8_t i = 7; i >= 0; i--)
	{
		printf("%d", (reg_array[reg_addr] >> i) & 1);
		if (i == 4)
		{
			printf(" ");
		}
	}
	printf(")\n--------------------------------------\n");
	for (uint8_t i = 0; i < reg->bits_count; i++)
	{
		const PCAP04_RegBit_t *bit = &reg->bits[i];
		uint8_t bit_value = PCAP04_GetBitValue(reg_array[reg_addr], bit->bit_start, bit->bit_end);
		printf("  [%d", bit->bit_start);
		if (bit->bit_start != bit->bit_end)
		{
			printf(":%d", bit->bit_end);
		}
		printf("] %s: %s\n", bit->name, bit->description);
		printf("      当前值: 0x%02X", bit_value);
		for (uint8_t j = 0; j < bit->options_count; j++)
		{
			if (bit->options[j].value == (bit_value << bit->bit_start))
			{
				printf(" (%s - %s)", bit->options[j].name, bit->options[j].description);
				break;
			}
		}
		printf("\n");
	}
	printf("======================================\n");
}

void PCAP04_PrintAllRegisters(const uint8_t *reg_array)
{
	if (reg_array == NULL)
	{
		return;
	}
	printf("\n========== PCAP04 寄存器配置 ==========\n");
	for (uint8_t i = 0; i < PCAP04_REGISTER_TABLE.registers_count; i++)
	{
		PCAP04_PrintRegister(reg_array, PCAP04_REGISTER_TABLE.registers[i].address);
	}
	printf("\n========== 完整寄存器数组 (HEX) ==========\n");
	for (uint8_t i = 0; i < PCAP04_REG_COUNT; i++)
	{
		if ((i % 16U) == 0U)
		{
			printf("\n[0x%02X-%02X]: ", i, (uint8_t)(i + 15U < PCAP04_REG_COUNT ? i + 15U : (PCAP04_REG_COUNT - 1U)));
		}
		printf("%02X ", reg_array[i]);
	}
	printf("\n==========================================\n");
}

uint8_t PCAP04_SaveToFlash(const uint8_t *reg_array, uint32_t flash_addr)
{
	/* TODO: integrate with actual flash routines; currently only trace. */
	if (reg_array == NULL)
	{
		return 1U;
	}
	printf("保存寄存器配置到Flash地址: 0x%08lX\n", (unsigned long)flash_addr);
	printf("数据大小: %d 字节\n数据内容:", PCAP04_REG_COUNT);
	for (uint8_t i = 0; i < PCAP04_REG_COUNT; i++)
	{
		if ((i % 16U) == 0U)
		{
			printf("\n  ");
		}
		printf("%02X ", reg_array[i]);
	}
	printf("\n");
	return 0U;
}

uint8_t PCAP04_LoadFromFlash(uint8_t *reg_array, uint32_t flash_addr)
{
	/* TODO: integrate with actual flash routines; currently only trace. */
	if (reg_array == NULL)
	{
		return 1U;
	}
	printf("从Flash地址 0x%08lX 读取寄存器配置 (模拟)\n", (unsigned long)flash_addr);
	return 0U;
}


