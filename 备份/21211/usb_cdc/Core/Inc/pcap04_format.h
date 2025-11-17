#ifndef PCAP04_FORMAT_H
#define PCAP04_FORMAT_H

#include <stdint.h>
#include <stddef.h>
#include "pcap04_register.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCAP04_MAX_FIELDS_PER_REG 8U

typedef struct
{
	uint8_t reg_start;
	uint8_t reg_end;
	const char *name;
	const char *description;
	uint8_t bit_high;
	uint8_t bit_low;
	const char *range_hint;
	const PCAP04_BitOption_t *options;
	uint8_t options_count;
	uint32_t value;
} PCAP04_FieldFormat_t;

typedef struct
{
	uint8_t reg_start;
	uint8_t reg_end;
	const char *name;
	const char *description;
	uint8_t field_count;
	PCAP04_FieldFormat_t fields[PCAP04_MAX_FIELDS_PER_REG];
} PCAP04_RegisterFormat_t;

uint8_t PCAP04_BuildRegisterFormat(uint8_t reg_addr, const uint8_t *regs, PCAP04_RegisterFormat_t *out);
uint8_t PCAP04_BuildRegisterRangeFormat(uint8_t reg_start, uint8_t reg_end, const uint8_t *regs, PCAP04_RegisterFormat_t *out);
size_t PCAP04_FormatToString(const PCAP04_RegisterFormat_t *fmt, char *buffer, size_t buffer_len);

#ifdef __cplusplus
}
#endif

#endif /* PCAP04_FORMAT_H */


