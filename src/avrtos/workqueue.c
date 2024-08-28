/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "workqueue.h"
#include <util/atomic.h>
#include "kernel.h"

#define K_MODULE K_MODULE_WORKQUEUE

#define Z_WQ_YIELDEACH_POS 0u
#define Z_WQ_YIELDEACH_MSK (1u << Z_WQ_YIELDEACH_POS)
#define Z_WQ_YIELDEACH(_x) (((_x) << Z_WQ_YIELDEACH_POS) & Z_WQ_YIELDEACH_MSK)

int8_t k_workqueue_create(struct k_workqueue *workqueue,
						  struct k_thread *thread,
						  uint8_t *stack,
						  size_t stack_size,
						  uint8_t prio_flags,
						  char symbol)
{
	Z_ARGS_CHECK(workqueue && thread && stack && stack_size) return -EINVAL;

	k_fifo_init(&workqueue->q);
	workqueue->flags = 0u;

	int8_t ret = k_thread_create(thread, (k_thread_entry_t)z_workqueue_entry, stack,
								 stack_size, prio_flags, (void *)workqueue, symbol);
	if (ret == 0) {
		k_thread_start(thread);
	}

	return ret;
}

void z_workqueue_entry(struct k_workqueue *const workqueue)
{
	struct snode *item;
	struct k_work *work;

	for (;;) {
		item = k_fifo_get(&workqueue->q, K_FOREVER);

		/* Ensure that the work item is valid */
		__ASSERT_NOTNULL(item);

		work = CONTAINER_OF(item, struct k_work, _tie);

		const k_work_handler_t handler = work->handler;

		/*
		 * Mark the work item as submittable again.
		 * This allows the work item to be resubmitted even while it is being processed.
		 * 
		 * However, we can't do any assumption regarding the context of
		 * the work item, proper synchronization is the user's responsibility.
		 */
		const uint8_t key = irq_lock();
		item->next = NULL;
		irq_unlock(key);

		handler(work);

		/* Yield if the "yieldeach" option is enabled */
		if (workqueue->flags & Z_WQ_YIELDEACH_MSK) {
			k_yield();
		}
	}
}

void k_work_init(struct k_work *work, k_work_handler_t handler)
{
	work->handler = handler;
}

/**
 * @brief Check if the work item is ready to be submitted.
 *
 * This function requires interrupts to be disabled.
 *
 * @param work Pointer to the work item.
 * @return true if the work item can be submitted, false if it is already in the queue.
 */
__always_inline bool z_work_submittable(struct k_work *work)
{
	return work->_tie.next == NULL;
}

__always_inline void z_work_submit(struct k_workqueue *workqueue, struct k_work *work)
{
	z_fifo_put(&workqueue->q, &work->_tie);
}

bool k_work_submit(struct k_workqueue *workqueue, struct k_work *work)
{
	__ASSERT_NOTNULL(workqueue);
	__ASSERT_NOTNULL(work);
	__ASSERT_NOTNULL(work->handler);

	bool ret = false;

	/* Check if the work item is not already in the queue */
	const uint8_t key = irq_lock();
	if (z_work_submittable(work)) {
		z_work_submit(workqueue, work);
		ret = true;
	}
	irq_unlock(key);
	return ret;
}

void k_workqueue_enable_yieldeach(struct k_workqueue *workqueue)
{
	__ASSERT_NOTNULL(workqueue);

	const uint8_t key = irq_lock();
	workqueue->flags |= Z_WQ_YIELDEACH_MSK;
	irq_unlock(key);
}

void k_workqueue_disable_yieldeach(struct k_workqueue *workqueue)
{
	__ASSERT_NOTNULL(workqueue);

	const uint8_t key = irq_lock();
	workqueue->flags &= ~Z_WQ_YIELDEACH_MSK;
	irq_unlock(key);
}

#if CONFIG_SYSTEM_WORKQUEUE_ENABLE
K_WORKQUEUE_DEFINE(z_system_workqueue,
				   CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE,
				   CONFIG_SYSTEM_WORKQUEUE_PRIORITY,
				   'W');

bool k_system_workqueue_submit(struct k_work *work)
{
	return k_work_submit(&z_system_workqueue, work);
}
#endif

#if CONFIG_WORKQUEUE_DELAYABLE

#if !CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT
#error "CONFIG_WORKQUEUE_DELAYABLE requires CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT"
#endif

#if !CONFIG_KERNEL_EVENTS
#error "CONFIG_WORKQUEUE_DELAYABLE requires CONFIG_KERNEL_EVENTS"
#endif

static void delayable_work_trigger(struct k_event *event)
{
	struct k_work_delayable *const dwork =
		CONTAINER_OF(event, struct k_work_delayable, _event);

	/* Submit the work item to the associated workqueue */
	z_work_submit(dwork->_workqueue, &dwork->work);
}

void k_work_delayable_init(struct k_work_delayable *dwork, k_work_handler_t handler)
{
	__ASSERT_NOTNULL(dwork);
	__ASSERT_NOTNULL(handler);

	k_work_init(&dwork->work, handler);
	k_event_init(&dwork->_event, delayable_work_trigger);
	dwork->_workqueue = NULL;
}

extern void z_event_schedule(struct k_event *event, k_timeout_t timeout);

int8_t k_work_delayable_schedule(struct k_workqueue *workqueue,
								 struct k_work_delayable *dwork,
								 k_timeout_t timeout)
{
	Z_ARGS_CHECK(workqueue && dwork) return -EINVAL;

	int8_t ret = 0;
	const uint8_t lock = irq_lock();

	/* Ensure the work item is not already pending or in the queue */
	if (dwork->_event.scheduled || !z_work_submittable(&dwork->work)) {
		ret = -EBUSY;
		goto exit;
	}

	/* Safely assign the workqueue, ensuring it is not modified during submission */
	dwork->_workqueue = workqueue;

	if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		z_fifo_put(&workqueue->q, &dwork->work._tie);
	} else {
		z_event_schedule(&dwork->_event, timeout);
	}

exit:
	irq_unlock(lock);
	return ret;
}

int8_t k_work_delayable_cancel(struct k_work_delayable *dwork)
{
	Z_ARGS_CHECK(dwork) return -EINVAL;

	int8_t ret = k_event_cancel(&dwork->_event);
	if (ret == -EAGAIN) {
		const uint8_t lock = irq_lock();

		/* If the work item is not pending, check if it is already in the queue */
		if (!z_work_submittable(&dwork->work)) {
			ret = -EBUSY;
		}

		irq_unlock(lock);
	}

	return ret;
}

#if CONFIG_SYSTEM_WORKQUEUE_ENABLE
int8_t k_system_work_delayable_schedule(struct k_work_delayable *dwork,
										k_timeout_t timeout)
{
	return k_work_delayable_schedule(&z_system_workqueue, dwork, timeout);
}
#endif
#endif
