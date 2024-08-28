/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Event Handling
 *
 * The event handling system in AVRTOS provides a mechanism for scheduling
 * and managing events that occur after a specified delay or at regular intervals.
 * Events are typically used to trigger specific actions at a later time, such as
 * handling timeouts, periodic tasks, or deferred processing.
 *
 * Events are managed through an event queue that ensures they are executed in a
 * timely manner based on their scheduled time. Each event is associated with a
 * handler function that is called when the event is triggered. Events can be
 * scheduled, canceled, and checked for pending status.
 *
 * Key Features:
 * - **Time-based Event Scheduling**: Events can be scheduled to trigger after a
 *   specified timeout, making it easy to implement timeouts and delays in your
 *   system.
 * - **Immediate Execution Option**: Events can be executed immediately if no delay
 *   is needed, bypassing the scheduling mechanism.
 * - **Interrupt-safe Operations**: Events can be scheduled and canceled from interrupt
 *   context, ensuring that time-sensitive operations can be managed reliably.
 * - **Efficient Event Queue Management**: The event queue processes events in a
 *   time-ordered manner, ensuring that events are handled as soon as their timeouts
 *   expire.
 *
 * Limitations:
 * - Like software timers (timer.h), the main limitation is that the events are processed 
 *   within the tick interrupt handler (kernel code), the code executed in the handler must then 
 *   be compliant with the constraints of the interrupt context.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_EVENTS: Enables the event handling system.
 *  - CONFIG_KERNEL_TIME_SLICE_US: Defines the time slice used for event processing.
 *  - CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT: Allows events to be executed immediately without
 * delay.
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
 * @brief Event handler function type.
 *
 * This type defines the function signature for event handlers. Event handlers
 * are functions that are called when an event is triggered.
 *
 * @warning Event handlers should never call `k_yield()`, `k_yield_from_isr()`,
 * or `k_yield_from_isr_cond()` from a timer handler.
 *
 * @param event Pointer to the event that triggered the handler.
 */
typedef void (*k_event_handler_t)(struct k_event *);

/**
 * @brief Event structure.
 *
 * The `k_event` structure represents an event that can be scheduled to occur after
 * a specified timeout. It includes a handler function that is called when the event
 * is triggered.
 */
struct k_event {
	struct titem tie;		   ///< Timer queue item for scheduling the event.
	k_event_handler_t handler; ///< Function to handle the event when triggered.
	uint8_t scheduled : 1;	   ///< Flag indicating if the event is currently scheduled.
};

/**
 * @brief Statically initialize an event.
 *
 * This macro initializes a `k_event` structure with the specified event handler.
 *
 * @param hdlr The function that will handle the event.
 */
#define Z_EVENT_INIT(hdlr)                                                               \
	{                                                                                    \
		.tie = INIT_TITEM_DEFAULT(), .handler = hdlr, .scheduled = 0                     \
	}

/**
 * @brief Statically define and initialize an event.
 *
 * This macro defines and initializes a `k_event` structure with the specified handler.
 *
 * @param name Name of the event.
 * @param hdlr Function to handle the event.
 */
#define K_EVENT_DEFINE(name, hdlr) struct k_event name = Z_EVENT_INIT(hdlr)

/**
 * @brief Initialize an event at runtime.
 *
 * This function initializes a `k_event` structure with the specified handler function.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param event Pointer to the event structure.
 * @param handler Function to handle the event when triggered.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_event_init(struct k_event *event, k_event_handler_t handler);

/**
 * @brief Schedule an event to be triggered after a timeout.
 *
 * This function schedules the specified event to occur after the given timeout.
 * It can be called from an interrupt context.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param event Pointer to the event structure.
 * @param timeout Timeout in milliseconds after which the event will be triggered.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_event_schedule(struct k_event *event, k_timeout_t timeout);

/**
 * @brief Cancel a scheduled event.
 *
 * This function cancels a previously scheduled event. It can be called from
 * an interrupt context.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param event Pointer to the event structure.
 * @return 0 if the event was successfully canceled, or -EAGAIN if the event was not
 * pending.
 */
__kernel int8_t k_event_cancel(struct k_event *event);

/**
 * @brief Check if an event is pending.
 *
 * This function checks if the specified event is currently scheduled.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param event Pointer to the event structure.
 * @return true if the event is pending, false otherwise.
 */
__kernel bool k_event_pending(struct k_event *event);

/**
 * @brief Process the event queue and execute handlers for expired events.
 *
 * This internal function processes the event queue, executing the handlers for
 * any events whose timeouts have expired. It is called periodically with a frequency
 * defined by `CONFIG_KERNEL_TIME_SLICE_US`.
 */
__kernel void z_event_q_process(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_EVENT_H_ */
