#include "workqueue.h"

#include <util/atomic.h>

#include "kernel.h"

/*___________________________________________________________________________*/

#define K_MODULE    K_MODULE_WORKQUEUE

/*___________________________________________________________________________*/

void _k_workqueue_entry(struct k_workqueue *const workqueue)
{
        struct qitem *item;
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
		 * However, we can't do any assumption regarding the context of the work item
		 **/
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                        item->next = NULL;
                }

                handler(work);
		
                /* yield if "yieldeach" option is enabled */
                if (workqueue->yieldeach) {
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

        /* if item not already in queue */
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                if (k_work_submittable(work)) {
                        _k_fifo_put(&workqueue->q, &work->_tie);

			return true;
                }
        }
	return false;
}

void k_workqueue_set_yieldeach(struct k_workqueue *workqueue)
{
        __ASSERT_NOTNULL(workqueue);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                workqueue->yieldeach = 1u;
        }
}

void k_workqueue_clr_yieldeach(struct k_workqueue *workqueue)
{
        __ASSERT_NOTNULL(workqueue);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                workqueue->yieldeach = 0u;
        }
}

/*___________________________________________________________________________*/

#if SYSTEM_WORKQUEUE_ENABLE

K_WORKQUEUE_DEFINE(_k_system_workqueue, SYSTEM_WORKQUEUE_STACK_SIZE, SYSTEM_WORKQUEUE_PRIORITY, 'W');

bool k_system_workqueue_submit(struct k_work *work)
{
	return k_work_submit(&_k_system_workqueue, work);
}

#endif

/*___________________________________________________________________________*/