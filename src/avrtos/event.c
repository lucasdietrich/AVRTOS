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

#define Z_EVENTS_PERIOD_TICKS Z_KERNEL_TIME_SLICE_TICKS

/**
 * @brief Event queue structure.
 *
 * This structure holds the first event in the queue, which is used to manage
 * scheduled events in a time-ordered manner.
 */
struct k_event_q {
	struct titem *first; ///< Pointer to the first event in the queue.
};

/**
 * @brief Macro to initialize the event queue.
 */
#define Z_EVENT_Q_INIT()                                                                 \
	{                                                                                    \
		.first = NULL,                                                                   \
	}

/**
 * @brief Macro to define and initialize an event queue.
 *
 * This macro defines a global event queue named `name` and initializes it using
 * `Z_EVENT_Q_INIT()`.
 */
#define K_EVENT_Q_DEFINE(name) struct k_event_q name = Z_EVENT_Q_INIT()

/**
 * @brief Global event queue instance.
 *
 * This is the main event queue used for scheduling and processing events.
 */
K_EVENT_Q_DEFINE(z_event_q);

int8_t k_event_init(struct k_event *event, k_event_handler_t handler)
{
	Z_ARGS_CHECK(event && handler) return -EINVAL;

	event->handler	 = handler;
	event->scheduled = 0;

	return 0;
}

/**
 * @brief Internal function to schedule an event.
 *
 * This function schedules an event to be triggered after a specified timeout.
 * It requires interrupts to be disabled.
 *
 * @param event Pointer to the event structure.
 * @param timeout Timeout in ticks after which the event will be triggered.
 */
void z_event_schedule(struct k_event *event, k_timeout_t timeout)
{
	event->scheduled   = 1u;
	event->tie.next	   = NULL;
	event->tie.timeout = K_TIMEOUT_TICKS(timeout);

	z_tqueue_schedule(&z_event_q.first, &event->tie);
}

int8_t k_event_schedule(struct k_event *event, k_timeout_t timeout)
{
	Z_ARGS_CHECK(event) return -EINVAL;

#if !CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT
	Z_ARGS_CHECK(!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) return -EINVAL;
#endif

	int8_t ret		   = 0;
	const uint8_t lock = irq_lock();

	if (event->scheduled) {
		ret = -EAGAIN;
		goto exit;
	}

	if (CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT && K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		/* If no wait is requested, execute the handler immediately
		 * without scheduling it. */
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
	Z_ARGS_CHECK(event) return -EINVAL;

	int8_t ret		   = 0;
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
	Z_ARGS_CHECK(event) return false;

	return event->scheduled == 1;
}

void z_event_q_process(void)
{
	struct titem *tie;

	__ASSERT_NOINTERRUPT();

	/* Shift the event queue forward by the configured time slice */
	tqueue_shift(&z_event_q.first, Z_EVENTS_PERIOD_TICKS);

	/* Process all expired events in the queue */
	while ((tie = tqueue_pop(&z_event_q.first)) != NULL) {
		struct k_event *event = CONTAINER_OF(tie, struct k_event, tie);

		/* Clear the scheduled flag to allow rescheduling from the handler */
		event->scheduled = 0;

		/* Execute the event handler */
		event->handler(event);
	}
}

#endif /* CONFIG_KERNEL_EVENTS */
