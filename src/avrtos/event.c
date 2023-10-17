/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "event.h"

#include <util/atomic.h>

#include "assert.h"

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
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!event || !handler) {
		return -EINVAL;
	}
#endif

	event->handler = handler;

	event->scheduled = 0;

	return 0;
}

void z_event_schedule(struct k_event *event, k_timeout_t timeout)
{
	event->scheduled   = 1u;
	event->tie.next	   = NULL;
	event->tie.timeout = K_TIMEOUT_TICKS(timeout);

	z_tqueue_schedule(&z_event_q.first, &event->tie);
}

int8_t k_event_schedule(struct k_event *event, k_timeout_t timeout)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!event) return -EINVAL;
#if !CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT
	if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) return -EINVAL;
#endif
#endif

	int ret		   = 0;
	const uint8_t lock = irq_lock();

	if (event->scheduled) {
		ret = -EAGAIN;
		goto exit;
	}

	if (CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT && K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		/* If no wait is requested, execute the handler immediately
		 * without scheduling it */
		event->handler(event);
	} else {
		z_event_schedule(event, timeout);
	}

exit:
	irq_unlock(lock);
	return ret;
}

int8_t k_event_cancel(struct k_event *event)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!event) {
		return -EINVAL;
	}
#endif

	int ret		   = 0;
	const uint8_t lock = irq_lock();

	if (!event->scheduled) {
		ret = -EAGAIN;
		goto exit;
	}

	tqueue_remove(&z_event_q.first, &event->tie);
	event->scheduled = 0;

exit:
	irq_unlock(lock);
	return ret;
}

bool k_event_pending(struct k_event *event)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!event) {
		return false;
	}
#endif

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