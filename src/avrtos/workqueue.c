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
	int8_t ret;

#if CONFIG_KERNEL_ARGS_CHECKS
	if (!workqueue || !thread || !stack || !stack_size) {
		return -EINVAL;
	}
#endif

	k_fifo_init(&workqueue->q);
	workqueue->flags = 0u;

	ret = k_thread_create(thread, (thread_entry_t)z_workqueue_entry, stack,
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

		/* we cannot cancel a workqueue pending on a work item */
		__ASSERT_NOTNULL(item);

		work = CONTAINER_OF(item, struct k_work, _tie);

		const k_work_handler_t handler = work->handler;

		/*
		 * Set the work item as "submittable" again.
		 * This only means that the work item can be submitted again
		 * (even during the being processed handler)
		 *
		 * However, we can't do any assumption regarding the context of
		 * the work item. Proper synchronization must be done by the
		 * user.
		 **/
		const uint8_t key = irq_lock();
		item->next	  = NULL;
		irq_unlock(key);

		handler(work);

		/* yield if "yieldeach" option is enabled */
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
 * @brief Tells whether the work item is submittable or not.
 *
 * Requires interrupts to be disabled.
 *
 * @param work
 * @return true if submittable
 * @return false if in already in queue
 */
static inline bool z_work_submittable(struct k_work *work)
{
	return work->_tie.next == NULL;
}

bool k_work_submit(struct k_workqueue *workqueue, struct k_work *work)
{
	__ASSERT_NOTNULL(workqueue);
	__ASSERT_NOTNULL(work);
	__ASSERT_NOTNULL(work->handler);

	bool ret = false;

	/* if item not already in queue */
	const uint8_t key = irq_lock();
	if (z_work_submittable(work)) {
		z_fifo_put(&workqueue->q, &work->_tie);
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

	/* work and _workqueue should remain valid until here */
	k_work_submit(dwork->_workqueue, &dwork->work);
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

int k_work_delayable_schedule(struct k_workqueue *workqueue,
			      struct k_work_delayable *dwork,
			      k_timeout_t timeout)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!workqueue || !dwork) {
		return -EINVAL;
	}
#endif

	int ret		   = 0;
	const uint8_t lock = irq_lock();

	/* Ensure the work item is not already pending for submission or already in queue.
	 */
	if (dwork->_event.scheduled || !z_work_submittable(&dwork->work)) {
		ret = -EBUSY;
		goto exit;
	}

	/* By the use of the lock We make ensure _workqueue is not written while the work
	 * item is being sent from delayable_work_trigger.
	 */
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

int k_work_delayable_cancel(struct k_work_delayable *dwork)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!workqueue || !dwork) {
		return -EINVAL;
	}
#endif

	int ret = k_event_cancel(&dwork->_event);
	if (ret == -EAGAIN) {
		const uint8_t lock = irq_lock();

		/* In case the work was not pending for submission, we need to check if
		 * the work item is pending for processing. This is purely informative.
		 */
		if (!z_work_submittable(&dwork->work)) {
			/* If yes, we consider the work item as busy */
			ret = -EBUSY;
		}

		irq_unlock(lock);
	}

	return ret;
}

#if CONFIG_SYSTEM_WORKQUEUE_ENABLE
int k_system_work_delayable_schedule(struct k_work_delayable *dwork, k_timeout_t timeout)
{
	return k_work_delayable_schedule(&z_system_workqueue, dwork, timeout);
}
#endif
#endif