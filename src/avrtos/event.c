/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "event.h"

#include <avrtos/assert.h>
#include <util/atomic.h>

#define K_MODULE K_MODULE_EVENT

#if CONFIG_KERNEL_EVENTS

struct k_event_q {
	struct titem *first;
};

#define K_EVENT_Q_INIT()       \
	{                      \
		.first = NULL, \
	}
#define K_EVENT_Q_DEFINE(name) struct k_event_q name = K_EVENT_Q_INIT()

K_EVENT_Q_DEFINE(z_event_q);

int8_t k_event_init(struct k_event *event, k_event_handler_t handler)
{
	if (!event || !handler) {
		return -EINVAL;
	}

	event->handler = handler;

	event->scheduled = 0;

	return 0;
}

int8_t k_event_schedule(struct k_event *event, k_timeout_t timeout)
{
	if (!event || event->scheduled) {
		return -EINVAL;
	}

#if !CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT
	if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		return -EINVAL;
	}
#endif

	const uint8_t lock = irq_lock();

	if (CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT && K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		/* If no wait is requested, execute the handler immediately
		 * without scheduling it */
		event->handler(event);
	} else {
		event->scheduled   = 1u;
		event->tie.next	   = NULL;
		event->tie.timeout = K_TIMEOUT_TICKS(timeout);

		z_tqueue_schedule(&z_event_q.first, &event->tie);
	}

	irq_unlock(lock);

	return 0;
}

int8_t k_event_cancel(struct k_event *event)
{
	if (!event || !event->scheduled) {
		return -EINVAL;
	}

	const uint8_t lock = irq_lock();

	tqueue_remove(&z_event_q.first, &event->tie);
	event->scheduled = 0;

	irq_unlock(lock);

	return 0;
}

bool k_event_pending(struct k_event *event)
{
	if (!event) {
		return false;
	}

	return event->scheduled == 1;
}

void z_event_q_process(void)
{
	struct titem *tie;

	__ASSERT_NOINTERRUPT();

	tqueue_shift(&z_event_q.first, K_EVENTS_PERIOD_TICKS);

	while ((tie = tqueue_pop(&z_event_q.first)) != NULL) {
		struct k_event *event = CONTAINER_OF(tie, struct k_event, tie);

		/* Clear scheduled flag here so that we can schedule the same
		 * event directly from the handler */
		event->scheduled = 0;

		event->handler(event);
	}
}

#endif /* CONFIG_KERNEL_EVENTS */