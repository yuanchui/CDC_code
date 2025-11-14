#include "pcap04_format.h"
#include <string.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

typedef struct
{
	uint8_t reg_start;
	uint8_t reg_end;
	const char *name;
	const char *description;
	uint8_t bit_high;
	uint8_t bit_low;
	const char *range_hint;
} PCAP04_CombinedFieldDesc_t;

static const PCAP04_CombinedFieldDesc_t s_combined_fields[] =
{
	{0x07, 0x08, "C_AVRG", "Average sample count (13-bit)", 12U, 0U, "0-8191"},
	{0x09, 0x0B, "CONV_TIME", "Conversion time (23-bit)", 22U, 0U, "0-8388607"},
	{0x0C, 0x0D, "DISCHARGE_TIME", "Discharge time (10-bit)", 9U, 0U, "0-1023"},
	{0x0E, 0x0F, "PRECHARGE_TIME", "Precharge time (10-bit)", 9U, 0U, "0-1023"},
	{0x10, 0x11, "FULLCHARGE_TIME", "Full charge time (10-bit)", 9U, 0U, "0-1023"}
};

static const PCAP04_Register_t* pcap04_find_register(uint8_t addr)
{
	for (uint8_t i = 0U; i < PCAP04_REGISTER_TABLE.registers_count; ++i)
	{
		if (PCAP04_REGISTER_TABLE.registers[i].address == addr)
		{
			return &PCAP04_REGISTER_TABLE.registers[i];
		}
	}
	return NULL;
}

static uint8_t pcap04_read_reg_value(const uint8_t *regs, const PCAP04_Register_t *def)
{
	if (regs != NULL)
	{
		return regs[def->address];
	}
	return def->default_value;
}

static uint32_t pcap04_read_range_value(const uint8_t *regs, const PCAP04_CombinedFieldDesc_t *desc)
{
	uint32_t value = 0U;
	uint8_t shift = 0U;
	for (uint8_t addr = desc->reg_start; addr <= desc->reg_end; ++addr, shift += 8U)
	{
		const PCAP04_Register_t *def = pcap04_find_register(addr);
		if (def == NULL)
		{
			continue;
		}
		uint8_t reg_value = pcap04_read_reg_value(regs, def);
		value |= ((uint32_t)reg_value) << shift;
	}
	uint8_t width = (uint8_t)(desc->bit_high - desc->bit_low + 1U);
	uint32_t mask = (width >= 32U) ? 0xFFFFFFFFUL : ((1UL << width) - 1UL);
	return (value >> desc->bit_low) & mask;
}

static void pcap04_fill_field(const PCAP04_Register_t *reg_def,
                              const PCAP04_RegBit_t *bit,
                              const uint8_t *regs,
                              PCAP04_FieldFormat_t *field)
{
	memset(field, 0, sizeof(*field));
	field->reg_start = reg_def->address;
	field->reg_end = reg_def->address;
	field->name = bit->name;
	field->description = bit->description;

	uint8_t high = bit->bit_start > bit->bit_end ? bit->bit_start : bit->bit_end;
	uint8_t low = bit->bit_start > bit->bit_end ? bit->bit_end : bit->bit_start;
	field->bit_high = high;
	field->bit_low = low;
	field->options = bit->options;
	field->options_count = bit->options_count;
	field->range_hint = NULL;

	uint8_t reg_value = pcap04_read_reg_value(regs, reg_def);
	uint8_t width = (uint8_t)(high - low + 1U);
	uint32_t mask = (width >= 32U) ? 0xFFFFFFFFUL : ((1UL << width) - 1UL);
	field->value = (reg_value >> low) & mask;
}

uint8_t PCAP04_BuildRegisterFormat(uint8_t reg_addr, const uint8_t *regs, PCAP04_RegisterFormat_t *out)
{
	if (out == NULL)
	{
		return 0U;
	}
	memset(out, 0, sizeof(*out));

	const PCAP04_Register_t *reg_def = pcap04_find_register(reg_addr);
	if (reg_def == NULL)
	{
		return 0U;
	}

	out->reg_start = reg_addr;
	out->reg_end = reg_addr;
	out->name = reg_def->name;
	out->description = reg_def->description;

	for (uint8_t i = 0U; i < reg_def->bits_count && i < PCAP04_MAX_FIELDS_PER_REG; ++i)
	{
		pcap04_fill_field(reg_def, &reg_def->bits[i], regs, &out->fields[out->field_count]);
		out->field_count++;
	}
	return 1U;
}

uint8_t PCAP04_BuildRegisterRangeFormat(uint8_t reg_start, uint8_t reg_end, const uint8_t *regs, PCAP04_RegisterFormat_t *out)
{
	if (out == NULL || reg_start > reg_end)
	{
		return 0U;
	}
	memset(out, 0, sizeof(*out));

	for (uint8_t idx = 0U; idx < ARRAY_SIZE(s_combined_fields); ++idx)
	{
		const PCAP04_CombinedFieldDesc_t *desc = &s_combined_fields[idx];
		if (desc->reg_start == reg_start && desc->reg_end == reg_end)
		{
			out->reg_start = desc->reg_start;
			out->reg_end = desc->reg_end;
			out->name = desc->name;
			out->description = desc->description;
			out->field_count = 1U;
			PCAP04_FieldFormat_t *field = &out->fields[0];
			memset(field, 0, sizeof(*field));
			field->reg_start = desc->reg_start;
			field->reg_end = desc->reg_end;
			field->name = desc->name;
			field->description = desc->description;
			field->bit_high = desc->bit_high;
			field->bit_low = desc->bit_low;
			field->range_hint = desc->range_hint;
			field->options = NULL;
			field->options_count = 0U;
			field->value = pcap04_read_range_value(regs, desc);
			return 1U;
		}
	}
	return 0U;
}

size_t PCAP04_FormatToString(const PCAP04_RegisterFormat_t *fmt, char *buffer, size_t buffer_len)
{
	if (buffer_len == 0U)
	{
		return 0U;
	}
	buffer[0] = '\0';
	if (fmt == NULL)
	{
		return 0U;
	}

	int written = 0;
	if (fmt->reg_start == fmt->reg_end)
	{
		written = snprintf(buffer, buffer_len, "REG%02X{%s", fmt->reg_start, fmt->name != NULL ? fmt->name : "");
	}
	else
	{
		written = snprintf(buffer, buffer_len, "REG%02X-%02X{%s", fmt->reg_start, fmt->reg_end, fmt->name != NULL ? fmt->name : "");
	}
	if (written < 0 || (size_t)written >= buffer_len)
	{
		buffer[buffer_len - 1U] = '\0';
		return (size_t)(buffer_len - 1U);
	}
	size_t total = (size_t)written;

	for (uint8_t i = 0U; i < fmt->field_count; ++i)
	{
		const PCAP04_FieldFormat_t *field = &fmt->fields[i];
		int ret = snprintf(buffer + total, buffer_len - total, ",{%s,%u-%u,%s{%u,%u},",
		                   field->name != NULL ? field->name : "",
		                   field->bit_high,
		                   field->bit_low,
		                   field->description != NULL ? field->description : "",
		                   field->bit_high,
		                   field->bit_low);
		if (ret < 0)
		{
			break;
		}
		if ((size_t)ret >= buffer_len - total)
		{
			total = buffer_len - 1U;
			buffer[total] = '\0';
			return total;
		}
		total += (size_t)ret;

		if (field->options_count > 0U && field->options != NULL)
		{
			for (uint8_t opt = 0U; opt < field->options_count; ++opt)
			{
				const PCAP04_BitOption_t *option = &field->options[opt];
				ret = snprintf(buffer + total, buffer_len - total, "%s%s", opt == 0U ? "" : "|", option->name != NULL ? option->name : "");
				if (ret < 0)
				{
					break;
				}
				if ((size_t)ret >= buffer_len - total)
				{
					total = buffer_len - 1U;
					buffer[total] = '\0';
					return total;
				}
				total += (size_t)ret;
			}
		}
		else if (field->range_hint != NULL)
		{
			ret = snprintf(buffer + total, buffer_len - total, "%s", field->range_hint);
			if (ret < 0)
			{
				break;
			}
			if ((size_t)ret >= buffer_len - total)
			{
				total = buffer_len - 1U;
				buffer[total] = '\0';
				return total;
			}
			total += (size_t)ret;
		}
		ret = snprintf(buffer + total, buffer_len - total, ",%lu}", (unsigned long)field->value);
		if (ret < 0)
		{
			break;
		}
		if ((size_t)ret >= buffer_len - total)
		{
			total = buffer_len - 1U;
			buffer[total] = '\0';
			return total;
		}
		total += (size_t)ret;
	}

	if (total < buffer_len - 1U)
	{
		buffer[total++] = '}';
		buffer[total] = '\0';
	}
	else
	{
		buffer[buffer_len - 1U] = '\0';
	}
	return total;
}


