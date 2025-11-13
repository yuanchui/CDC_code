#include "cmd_queue.h"
#include <string.h>

/* Two-level queues; keep small to bound ISR work */
#define CMD_QUEUE_CAPACITY 8

typedef struct
{
	uint8_t        items_data[CMD_QUEUE_CAPACITY][CMD_QUEUE_ITEM_MAX_LEN];
	uint16_t       items_len[CMD_QUEUE_CAPACITY];
	uint16_t       head;
	uint16_t       tail;
	uint16_t       count;
} ring_t;

static ring_t q_high;
static ring_t q_low;

static inline void ring_init(ring_t* r)
{
	r->head = r->tail = r->count = 0;
}

static inline uint8_t ring_push(ring_t* r, const uint8_t* data, uint16_t len)
{
	if (r->count >= CMD_QUEUE_CAPACITY || data == NULL)
	{
		return 0;
	}
	uint16_t copy_len = (len >= CMD_QUEUE_ITEM_MAX_LEN) ? (CMD_QUEUE_ITEM_MAX_LEN - 1U) : len;
	memcpy(r->items_data[r->tail], data, copy_len);
	r->items_data[r->tail][copy_len] = 0U;
	r->items_len[r->tail] = copy_len;
	r->tail = (uint16_t)((r->tail + 1U) % CMD_QUEUE_CAPACITY);
	r->count++;
	return 1;
}

static inline uint8_t ring_pop(ring_t* r, cmd_item_t* out, cmd_priority_t prio)
{
	if (r->count == 0U) return 0;
	out->data = r->items_data[r->head];
	out->length = r->items_len[r->head];
	out->priority = prio;
	r->head = (uint16_t)((r->head + 1U) % CMD_QUEUE_CAPACITY);
	r->count--;
	return 1;
}

void cmd_queue_init(void)
{
	ring_init(&q_high);
	ring_init(&q_low);
}

uint8_t cmd_queue_push(const uint8_t* data, uint16_t length, cmd_priority_t prio)
{
	if (prio == CMD_PRIO_HIGH)
	{
		return ring_push(&q_high, data, length);
	}
	else
	{
		return ring_push(&q_low, data, length);
	}
}

uint8_t cmd_queue_pop(cmd_item_t* out)
{
	if (ring_pop(&q_high, out, CMD_PRIO_HIGH)) return 1;
	if (ring_pop(&q_low, out, CMD_PRIO_LOW)) return 1;
	return 0;
}

uint16_t cmd_queue_count(void)
{
	return (uint16_t)(q_high.count + q_low.count);
}


