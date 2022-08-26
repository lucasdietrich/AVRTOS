/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "timer.h"

#include <util/atomic.h>

#include "kernel.h"

#include "misc/uart.h"

#if KERNEL_TIMERS

#define K_MODULE    K_MODULE_TIMER

extern struct k_timer __k_timers_start;
extern struct k_timer __k_timers_end;

static struct titem *_k_timers_runqueue = NULL;

void _k_timer_init_module(void)
{
	const uint8_t timers_count = &__k_timers_end - &__k_timers_start;
	for (uint8_t i = 0; i < timers_count; i++) {
		struct k_timer *timer = &(&__k_timers_start)[i];
		if (timer->tie.timeout != K_TIMER_STOPPED) {
			_k_timer_start(timer, K_MSEC(timer->tie.timeout));
		}
	}
}

void _k_timer_start(struct k_timer *timer, k_timeout_t starting_delay)
{
	__ASSERT_NOTNULL(timer);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		tqueue_schedule(&_k_timers_runqueue,
				&timer->tie, starting_delay.value);
	}
}

void _k_timers_process(void)
{
	__ASSERT_NOINTERRUPT();

	tqueue_shift(&_k_timers_runqueue, K_TIMERS_PERIOD_TICKS);

	for (;;) {
		struct titem *item = tqueue_pop(&_k_timers_runqueue);
		if (item != NULL) {
			struct k_timer *timer =
				CONTAINER_OF(item, struct k_timer, tie);

			timer->handler(timer);

			timer->tie.next = NULL;
			timer->tie.timeout = timer->timeout.value;
			_tqueue_schedule(&_k_timers_runqueue, &timer->tie);
		} else {
			return;
		}
	}
}

void k_timer_init(struct k_timer *timer, k_timer_handler_t handler,
		  k_timeout_t timeout, k_timeout_t starting_delay)
{
	__ASSERT_NOTNULL(timer);
	__ASSERT_NOTNULL(handler);

	timer->handler = handler;
	timer->timeout = timeout;

	if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		_k_timer_start(timer, starting_delay);
	} else {
		timer->tie.timeout = K_FOREVER.value;
	}
}

bool k_timer_started(struct k_timer *timer)
{
	__ASSERT_NOTNULL(timer);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		return timer->tie.timeout != K_TIMER_STOPPED;
	}

	__builtin_unreachable();
}

int8_t k_timer_stop(struct k_timer *timer)
{
	__ASSERT_NOTNULL(timer);

	if (!k_timer_started(timer)) {
		return -1;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		tqueue_remove(&_k_timers_runqueue, &timer->tie);
	}
	timer->timeout = K_FOREVER;
	return 0;
}

int8_t k_timer_start(struct k_timer *timer, k_timeout_t starting_delay)
{
	__ASSERT_NOTNULL(timer);

	if (k_timer_started(timer)) {
		return -1;
	}

	if (K_TIMEOUT_EQ(starting_delay, K_FOREVER)) {
		return -1;
	}

	_k_timer_start(timer, starting_delay);

	return 0;
}

#endif