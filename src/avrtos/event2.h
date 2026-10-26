/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Event Handling
 *
 * The event handling system provides a mechanism for scheduling
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
 *   context.
 *
 * Limitations:
 * - Like software timers (timer.h), the main limitation is that the events are processed
 *   within the tick interrupt handler (kernel code), the code executed in the handler
 * must then be compliant with the constraints of the interrupt context.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_EVENTS2: Enables the event (v2) handling system.
 */

#ifndef _AVRTOS_EVENT2_H_
#define _AVRTOS_EVENT2_H_

#include <stdbool.h>
#include <stdint.h>

#include "defines.h"
#include "dstruct/tqueue.h"
#include "kernel.h"
#include "evq.h"

#ifdef __cplusplus
extern "C" {
#endif

struct k_event2;

/**
 * @brief Event structure.
 */
struct k_event2 {
    struct titem tie;          ///< Timer queue item for scheduling the event.
    uint8_t scheduled : 1;     ///< Flag indicating if the event is currently scheduled.
};

/**
 * @brief Statically initialize an event.
 *
 * This macro initializes a `k_event2` structure.
 */
#define K_EVENT2_INIT()                                                               \
    {                                                                                    \
        .tie = INIT_TITEM_DEFAULT(),                                                      \
        .scheduled = 0,                                                                 \
    }

/**
 * @brief Statically define and initialize an event.
 *
 * This macro defines and initializes a `k_event2` structure.
 *
 * @param name Name of the event.
 * @param hdlr Function to handle the event.
 */
#define K_EVENT2_DEFINE(name) struct k_event2 name = K_EVENT2_INIT()

/**
 * @brief Initialize an event at runtime.
 *
 * This function initializes a `k_event2` structure.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param event Pointer to the event structure.
 * @param handler Function to handle the event when triggered.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_event2_init(struct k_event2 *event);

/**
 * @brief Schedule an event to be triggered after a timeout.
 *
 * This function schedules the specified event to occur after the given timeout.
 * It can be called from an interrupt context.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param evq Pointer to the event queue in which the event will be scheduled.
 * @param event Pointer to the event structure.
 * @param timeout Timeout in milliseconds after which the event will be triggered.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_event2_schedule(struct k_event_q *evq, struct k_event2 *event, k_ticks_t timeout);

inline int8_t k_event2_schedule_sys(struct k_event2 *event, k_ticks_t timeout)
{
    return k_event2_schedule(NULL, event, timeout);
}

/**
 * @brief Cancel a scheduled event.
 *
 * This function cancels a previously scheduled event. It can be called from
 * an interrupt context.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param evq Pointer to the event queue in which the event was scheduled.
 * @param event Pointer to the event structure.
 * @return 0 if the event was successfully canceled, or -EAGAIN if the event was not
 * pending.
 */
__kernel int8_t k_event2_cancel(struct k_event_q *evq, struct k_event2 *event);

inline int8_t k_event2_cancel_sys(struct k_event2 *event)
{
    return k_event2_cancel(NULL, event);
}

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
__kernel bool k_event2_pending(struct k_event2 *event);

/**
 * @brief Advance the event queue by a specified timeout.
 *
 * This function advances the event queue, potentially triggering events that are
 * ready. It can be called from an interrupt context.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param evq Pointer to the event queue.
 * @param timeout Timeout in ticks to advance the event queue.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_event2_q_advance(struct k_event_q *evq, k_ticks_t timeout);

// TODO document
__kernel int8_t k_event2_q_advance(struct k_event_q *evq, k_ticks_t delta);

__kernel int8_t k_event2_pend(struct k_event_q *evq, k_timeout_t timeout, struct k_event2 **evp);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_EVENT2_H_ */
