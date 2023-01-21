/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_KERNEL
#define _AVRTOS_KERNEL

#include <avrtos/dstruct/tqueue.h>
#include <avrtos/multithreading.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Requires additionnal stack (IDLE thread for example)
 */

/*___________________________________________________________________________*/

struct k_timer;

typedef void (*k_timer_handler_t)(struct k_timer *);

struct k_timer {
	struct titem tie;
	k_timeout_t timeout;
	k_timer_handler_t handler;
};

#define K_TIMER_INIT(timer_handler, timeout_ms, starting_delay)                          \
	{                                                                                \
		.tie = INIT_TITEM(starting_delay), .timeout = timeout_ms,                \
		.handler = timer_handler,                                                \
	}

#if CONFIG_AVRTOS_KERNEL_SECTIONS
#define K_TIMER_DEFINE(timer_name, handler, timeout_ms, starting_delay)                  \
	Z_LINK_KERNEL_SECTION(.k_timers)                                                 \
	static struct k_timer timer_name =                                               \
		K_TIMER_INIT(handler, timeout_ms, starting_delay)
#endif

#define K_TIMER_STOPPED ((k_delta_t)-1)

/*___________________________________________________________________________*/

K_NOINLINE void z_timer_init_module(void);

K_NOINLINE void z_timers_process(void);

K_NOINLINE void z_timer_start(struct k_timer *timer, k_timeout_t starting_delay);

K_NOINLINE void k_timer_init(struct k_timer *timer,
			     k_timer_handler_t handler,
			     k_timeout_t timeout,
			     k_timeout_t starting_delay);

K_NOINLINE bool k_timer_started(struct k_timer *timer);

K_NOINLINE int8_t k_timer_stop(struct k_timer *timer);

K_NOINLINE int8_t k_timer_start(struct k_timer *timer, k_timeout_t starting_delay);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif