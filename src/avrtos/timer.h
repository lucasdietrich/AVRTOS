/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_TIMER_H_
#define _AVRTOS_TIMER_H_

/*
 * Software Timers (timer.h)
 *
 * Software timers are used for scheduling tasks period tasks.
 *
 * Key Concepts:
 * - **Timer Initialization**: Define and initialize timers with specific timeouts
 *   and handler functions.
 * - **Timer Operations**: Start and stop timers, and check their status.
 * - **Timer Handlers**: Define functions that are called when timers expire.
 *
 * Limitations:
 * - Like events (event.h), the main limitation is that the timers are processed
 * within the tick interrupt handler (kernel code), the code executed in the handler must
 * then be compliant with the constraints of the interrupt context.
 */

#include "dstruct/tqueue.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct k_timer;

/**
 * @brief Timer handler function prototype.
 *
 * This function is called when a timer expires. The handler should
 * not call any of the following functions: `k_yield()`, `k_yield_from_isr()`,
 * or `k_yield_from_isr_cond()`.
 *
 * @param timer Pointer to the `k_timer` structure associated with the timer.
 * @return Return value is implementation-dependent.
 */
typedef int (*k_timer_handler_t)(struct k_timer *);

/**
 * @brief Timer structure definition.
 */
struct k_timer {
    struct titem tie;          /**< Queue item for scheduling. */
    k_timeout_t timeout;       /**< Timer timeout duration. */
    k_timer_handler_t handler; /**< Function to call when timer expires. */
};

/**
 * @brief Macro to initialize a timer.
 *
 * This macro initializes a `k_timer` structure with the specified handler,
 * timeout, and starting delay.
 *
 * @param timer_handler The function to call when the timer expires.
 * @param timeout_ms Timer timeout duration in milliseconds.
 * @param starting_delay Initial delay before the timer starts.
 */
#define Z_TIMER_INIT(timer_handler, timeout_ms, starting_delay)                          \
    {                                                                                    \
        .tie = INIT_TITEM(starting_delay), .timeout = timeout_ms,                        \
        .handler = timer_handler,                                                        \
    }

/**
 * @brief Macro to define and initialize a timer.
 *
 * This macro defines a `k_timer` structure with the specified name, handler,
 * timeout, and starting delay, and places it in the kernel's timer section.
 *
 * @param timer_name Name of the timer variable.
 * @param handler The function to call when the timer expires.
 * @param timeout_ms Timer timeout duration in milliseconds.
 * @param starting_delay Initial delay before the timer starts.
 */
#define K_TIMER_DEFINE(timer_name, handler, timeout_ms, starting_delay)                  \
    Z_LINK_KERNEL_SECTION(.k_timers)                                                     \
    static struct k_timer timer_name = Z_TIMER_INIT(handler, timeout_ms, starting_delay)

/**
 * @brief Value indicating a stopped timer.
 *
 * This constant represents a timer that has been stopped.
 */
#define K_TIMER_STOPPED ((k_delta_t)-1)

/**
 * @brief Initialize the timer module.
 *
 * This function must be called to initialize the timer subsystem.
 */
__kernel void z_timer_init_module(void);

/**
 * @brief Process scheduled timers.
 *
 * This function processes all timers that are due to expire. It should be called
 * periodically, typically from the main loop or a periodic task.
 */
__kernel void z_timers_process(void);

/**
 * @brief Start a timer.
 *
 * This function starts a timer with a specified initial delay.
 *
 * @param timer Pointer to the `k_timer` structure.
 * @param starting_delay Initial delay before the timer starts.
 */
__kernel void z_timer_start(struct k_timer *timer, k_timeout_t starting_delay);

/**
 * @brief Initialize a timer.
 *
 * This function initializes a timer with the specified handler, timeout, and
 * starting delay.
 *
 * @param timer Pointer to the `k_timer` structure.
 * @param handler The function to call when the timer expires.
 * @param timeout Timer timeout duration.
 * @param starting_delay Initial delay before the timer starts.
 * @return Status code indicating success or failure.
 */
__kernel int8_t k_timer_init(struct k_timer *timer,
                             k_timer_handler_t handler,
                             k_timeout_t timeout,
                             k_timeout_t starting_delay);

/**
 * @brief Check if a timer is started.
 *
 * This function checks whether the specified timer is currently active.
 *
 * @param timer Pointer to the `k_timer` structure.
 * @return `true` if the timer is started, `false` otherwise.
 */
__kernel bool k_timer_started(struct k_timer *timer);

/**
 * @brief Stop a timer.
 *
 * This function stops a running timer. Note that stopping a timer from within
 * a timer handler is discouraged. Instead, return a non-zero value from the
 * handler to stop the timer.
 *
 * @param timer Pointer to the `k_timer` structure.
 * @return Status code indicating success or failure.
 */
__kernel int8_t k_timer_stop(struct k_timer *timer);

/**
 * @brief Start a timer with a specified initial delay.
 *
 * This function starts a timer with a given initial delay.
 *
 * @param timer Pointer to the `k_timer` structure.
 * @param starting_delay Initial delay before the timer starts.
 * @return Status code indicating success or failure.
 */
__kernel int8_t k_timer_start(struct k_timer *timer, k_timeout_t starting_delay);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_TIMER_H_ */
