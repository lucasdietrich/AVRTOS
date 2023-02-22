/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "kernel.h"
#include "workqueue.h"

#include <util/atomic.h>

/*___________________________________________________________________________*/

#define K_MODULE K_MODULE_WORKQUEUE

#define K_WQ_YIELDEACH_POS 0u
#define K_WQ_YIELDEACH_MSK (1u << K_WQ_YIELDEACH_POS)
#define K_WQ_YIELDEACH(_x) (((_x) << K_WQ_YIELDEACH_POS) & K_WQ_YIELDEACH_MSK)

/*___________________________________________________________________________*/

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
		 *the work item
		 **/
		const uint8_t key = irq_lock();
		item->next	  = NULL;
		irq_unlock(key);

		handler(work);

		/* yield if "yieldeach" option is enabled */
		if (workqueue->flags & K_WQ_YIELDEACH_MSK) {
			k_yield();
		}
	}
}

/*___________________________________________________________________________*/

void k_work_init(struct k_work *work, k_work_handler_t handler)
{
	work->handler = handler;
}

inline bool k_work_submittable(struct k_work *work)
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
	if (k_work_submittable(work)) {
		z_fifo_put(&workqueue->q, &work->_tie);
		ret = true;
	}
	irq_unlock(key);
	return ret;
}

void k_workqueue_set_yieldeach(struct k_workqueue *workqueue)
{
	__ASSERT_NOTNULL(workqueue);

	const uint8_t key    = irq_lock();
	workqueue->flags |= K_WQ_YIELDEACH_MSK;
	irq_unlock(key);
}

void k_workqueue_clr_yieldeach(struct k_workqueue *workqueue)
{
	__ASSERT_NOTNULL(workqueue);

	const uint8_t key    = irq_lock();
	workqueue->flags &= ~K_WQ_YIELDEACH_MSK;
	irq_unlock(key);
}

/*___________________________________________________________________________*/

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

/*___________________________________________________________________________*/