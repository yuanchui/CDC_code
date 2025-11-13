#ifndef USB_CMD_H
#define USB_CMD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void usb_cmd_init(void);

/* Called from USB RX ISR context, copies data into internal double buffer and sets flag */
void usb_cmd_rx_push(const uint8_t* data, uint16_t length);

/* Called from TIM1 5ms handler, processes RX, parses commands and schedules work */
void usb_cmd_tick_5ms(void);

/* Pause/resume command processing and CDC transfers when critical operations (PCAP04 FW/register write) */
void usb_cmd_pause_cdc(void);
void usb_cmd_resume_cdc(void);
uint8_t usb_cmd_is_cdc_paused(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_CMD_H */


