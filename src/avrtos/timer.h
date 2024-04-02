/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_KERNEL
#define _AVRTOS_KERNEL

#include "dstruct/tqueue.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Requires additionnal stack (IDLE thread for example)
 */

struct k_timer;

/**
 * @brief Timer handler function prototype
 *
 * @warning Never call any k_yield(), k_yield_from_isr(),
 * k_yield_from_isr_cond() from a timer handler.
 *
 */
typedef int (*k_timer_handler_t)(struct k_timer *);

struct k_timer {
	struct titem tie;
	k_timeout_t timeout;
	k_timer_handler_t handler;
};

#define K_TIMER_INIT(timer_handler, timeout_ms, starting_delay)                          \
	{                                                                                    \
		.tie = INIT_TITEM(starting_delay), .timeout = timeout_ms,                        \
		.handler = timer_handler,                                                        \
	}

#define K_TIMER_DEFINE(timer_name, handler, timeout_ms, starting_delay)                  \
	Z_LINK_KERNEL_SECTION(.k_timers)                                                     \
	static struct k_timer timer_name = K_TIMER_INIT(handler, timeout_ms, starting_delay)

#define K_TIMER_STOPPED ((k_delta_t)-1)

__kernel void z_timer_init_module(void);

__kernel void z_timers_process(void);

__kernel void z_timer_start(struct k_timer *timer, k_timeout_t starting_delay);

__kernel int8_t k_timer_init(struct k_timer *timer,
							 k_timer_handler_t handler,
							 k_timeout_t timeout,
							 k_timeout_t starting_delay);

__kernel bool k_timer_started(struct k_timer *timer);

/**
 * @brief Stop a timer
 *
 * Note: Usage of this function is discouraged in timer handlers.
 * Prefer returning any non-zero value from the handler to stop the timer.
 *
 * @param timer
 * @return __kernel
 */
__kernel int8_t k_timer_stop(struct k_timer *timer);

__kernel int8_t k_timer_start(struct k_timer *timer, k_timeout_t starting_delay);

#ifdef __cplusplus
}
#endif

#endif