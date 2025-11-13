#ifndef PCAP04_IF_H
#define PCAP04_IF_H

#include <stdint.h>
#include "pcap04_register.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	PCAP04_EVENT_NONE = 0,
	PCAP04_EVENT_REG_SYNC_DONE,
	PCAP04_EVENT_FIRMWARE_DONE
} pcap04_event_type_t;

typedef struct
{
	pcap04_event_type_t type;
	uint8_t             status;
	uint8_t             reg_addr;
	uint8_t             reg_value;
} pcap04_event_t;

void pcap04_if_init(void);
void pcap04_if_tick_5ms(void);

uint8_t pcap04_if_reg_set(uint8_t addr, const char* bitname, uint8_t value, uint8_t* old_value, uint8_t* new_value);
uint8_t pcap04_if_reg_get(uint8_t addr, const char* bitname, uint8_t* out_value);
uint8_t pcap04_if_reg_byte(uint8_t addr);
const uint8_t* pcap04_if_reg_snapshot(void);
uint8_t pcap04_if_sync_all(void);

uint8_t pcap04_if_save_to_flash(void);
uint8_t pcap04_if_load_from_flash(void);

uint8_t pcap04_if_fetch_event(pcap04_event_t* out);

/* Test utility mentioned by user */
int PCap04_Test(void);

#ifdef __cplusplus
}
#endif

#endif /* PCAP04_IF_H */


