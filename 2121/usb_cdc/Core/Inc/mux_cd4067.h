#ifndef MUX_CD4067_H
#define MUX_CD4067_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	MUX_SCAN_ROW,
	MUX_SCAN_COL,
	MUX_SCAN_POINT,
	MUX_SCAN_FULL
} mux_scan_mode_t;

/* Configure scan parameters; non-blocking operations scheduled in tick */
void mux_init(void);
void mux_set_mode(mux_scan_mode_t mode, uint8_t row, uint8_t col);
void mux_enable(uint8_t en); /* controls both EN pins combined policy */
void mux_set_period_ms(uint16_t period_ms);

/* Called from 5ms tick to progress scanning without blocking */
void mux_tick_5ms(void);

#ifdef __cplusplus
}
#endif

#endif /* MUX_CD4067_H */


