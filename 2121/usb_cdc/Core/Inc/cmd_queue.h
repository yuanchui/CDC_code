/* Simple ISR-safe command queue with two priority levels and FIFO ordering within level */
#ifndef CMD_QUEUE_H
#define CMD_QUEUE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	CMD_PRIO_HIGH = 0,
	CMD_PRIO_LOW  = 1,
} cmd_priority_t;

enum { CMD_QUEUE_ITEM_MAX_LEN = 96 };

typedef struct
{
	const uint8_t* data;
	uint16_t       length;
	cmd_priority_t priority;
} cmd_item_t;

void cmd_queue_init(void);
/* Returns 1 if enqueued, 0 if full */
uint8_t cmd_queue_push(const uint8_t* data, uint16_t length, cmd_priority_t prio);
/* Returns 1 if an item is popped into out, 0 if empty */
uint8_t cmd_queue_pop(cmd_item_t* out);
/* Returns total pending items */
uint16_t cmd_queue_count(void);

#ifdef __cplusplus
}
#endif

#endif /* CMD_QUEUE_H */


