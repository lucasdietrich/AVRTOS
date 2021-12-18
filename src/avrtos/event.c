#include "event.h"

#include <util/atomic.h>

#include <avrtos/assert.h>

#define K_MODULE K_MODULE_EVENT

#if KERNEL_EVENTS

struct k_event_q
{
	struct titem *first;
};

#define K_EVENT_Q_INIT()               \
    {                                  \
    	.first = NULL,                 \
    }
#define K_EVENT_Q_DEFINE(name) struct k_event_q name = K_EVENT_Q_INIT()

K_EVENT_Q_DEFINE(_k_event_q);

int k_event_init(struct k_event *event, k_event_handler_t handler)
{
	if (!event) {
		return -EINVAL;
	}

	event->handler = handler;
	
	return 0;
}

int k_event_schedule(struct k_event *event, k_timeout_t timeout)
{
	if (!event || K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		return -EINVAL;
	}

	event->tie.next = NULL;
	event->tie.timeout = K_TIMEOUT_MS(timeout);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_tqueue_schedule(&_k_event_q.first, &event->tie);
	}

	return 0;
}

int k_event_cancel(struct k_event *event)
{
	if (!event) {
		return -EINVAL;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		tqueue_remove(&_k_event_q.first, &event->tie);
	}

	return 0;
}

void _k_event_q_process(void)
{
	struct titem *tie;

	__ASSERT_NOINTERRUPT();

	tqueue_shift(&_k_event_q.first, K_EVENTS_PERIOD_MS);

	while ((tie = tqueue_pop(&_k_event_q.first)) != NULL) {
		struct k_event *event = CONTAINER_OF(tie, struct k_event, tie);

		event->handler(event);
	}
}

#endif /* KERNEL_EVENTS */