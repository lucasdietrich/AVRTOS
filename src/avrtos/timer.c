/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "timer.h"

#include <util/atomic.h>

#include "kernel.h"
#include "misc/serial.h"

#if CONFIG_KERNEL_TIMERS

#define K_MODULE K_MODULE_TIMER

static struct titem *z_timers_runqueue = NULL;

#if CONFIG_AVRTOS_LINKER_SCRIPT
extern struct k_timer __k_timers_start;
extern struct k_timer __k_timers_end;

void z_timer_init_module(void)
{
	const uint8_t timers_count = &__k_timers_end - &__k_timers_start;
	for (uint8_t i = 0; i < timers_count; i++) {
		struct k_timer *timer = &(&__k_timers_start)[i];
		if (timer->tie.timeout != K_TIMER_STOPPED) {
			z_timer_start(timer, K_MSEC(timer->tie.timeout));
		}
	}
}
#endif

void z_timer_start(struct k_timer *timer, k_timeout_t starting_delay)
{
	__ASSERT_NOTNULL(timer);

	const uint8_t key = irq_lock();
	tqueue_schedule(&z_timers_runqueue, &timer->tie, starting_delay.value);
	irq_unlock(key);
}

void z_timers_process(void)
{
	struct titem *item;
	struct k_timer *timer;

	__ASSERT_NOINTERRUPT();

	tqueue_shift(&z_timers_runqueue, K_TIMERS_PERIOD_TICKS);

	while (!!(item = tqueue_pop(&z_timers_runqueue))) {
		timer = CONTAINER_OF(item, struct k_timer, tie);

		int ret = timer->handler(timer);

		/* stop timer if handler returns non-zero */
		if (ret != 0) {
			timer->tie.timeout = K_TIMER_STOPPED;
		}

		/* reschedule if not stopped */
		if (timer->tie.timeout != K_TIMER_STOPPED) {
			timer->tie.next	   = NULL;
			timer->tie.timeout = timer->timeout.value;
			z_tqueue_schedule(&z_timers_runqueue, &timer->tie);
		}
	}
}

int8_t k_timer_init(struct k_timer *timer,
					k_timer_handler_t handler,
					k_timeout_t timeout,
					k_timeout_t starting_delay)
{
	__ASSERT_NOTNULL(timer);
	__ASSERT_NOTNULL(handler);

	if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) return -EINVAL;

	timer->handler = handler;
	timer->timeout = timeout;

	if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		z_timer_start(timer, starting_delay);
	} else {
		timer->tie.timeout = K_FOREVER.value;
	}

	return 0;
}

bool k_timer_started(struct k_timer *timer)
{
	__ASSERT_NOTNULL(timer);

	bool ret;

	const uint8_t key = irq_lock();
	ret				  = timer->tie.timeout != K_TIMER_STOPPED;
	irq_unlock(key);

	return ret;
}

int8_t k_timer_stop(struct k_timer *timer)
{
	__ASSERT_NOTNULL(timer);

	int ret;

	if (timer->tie.timeout != K_TIMER_STOPPED) {
		const uint8_t key = irq_lock();
		tqueue_remove(&z_timers_runqueue, &timer->tie);
		timer->tie.timeout = K_TIMER_STOPPED;
		irq_unlock(key);
		ret = 0;
	} else {
		ret = -1;
	}

	return ret;
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

	z_timer_start(timer, starting_delay);

	return 0;
}

#endif