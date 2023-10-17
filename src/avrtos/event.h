/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_EVENT_H_
#define _AVRTOS_EVENT_H_

#include <stdbool.h>
#include <stdint.h>

#include "dstruct/tqueue.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct k_event;

/**
 * @brief Event handler function.
 *
 * @warning Never call any k_yield(), k_yield_from_isr(),
 * k_yield_from_isr_cond() from a timer handler.
 *
 * @param event Event that triggered the handler.
 */
typedef void (*k_event_handler_t)(struct k_event *);

struct k_event {
	struct titem tie;

	k_event_handler_t handler;

	uint8_t scheduled : 1;
};

#define K_EVENT_INIT(hdlr)                                                   \
	{                                                                    \
		.tie = INIT_TITEM_DEFAULT(), .handler = hdlr, .scheduled = 0 \
	}
#define K_EVENT_DEFINE(name, hdlr) struct k_event name = K_EVENT_INIT(hdlr)

/**
 * @brief Initialize an event.
 *
 * @param event
 * @param handler
 * @return int
 */
__kernel int8_t k_event_init(struct k_event *event, k_event_handler_t handler);

/**
 * @brief Schedule the event to be triggered after timeout (in ms).
 *
 * Can be called from an interrupt.
 *
 * @param event
 * @param timeout
 * @return int
 */
__kernel int8_t k_event_schedule(struct k_event *event, k_timeout_t timeout);

/**
 * @brief Unschedule the event.
 *
 * Can be called from an interrupt.
 *
 * @param event
 * @return int Return 0 if the event was pending, -EAGAIN otherwise.
 */
__kernel int8_t k_event_cancel(struct k_event *event);

/**
 * @brief Unschedule the event.
 *
 * Can be called from an interrupt.
 *
 * @param event
 * @return int
 */
__kernel bool k_event_pending(struct k_event *event);

/**
 * @brief Internal function shifting the event queue and executing the handlers
 * for the events that have expired.
 *
 * The call periodicity is constant and equals CONFIG_KERNEL_TIME_SLICE_US.
 */
__kernel void z_event_q_process(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_IO_H_ */