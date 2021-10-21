#include "workqueue.h"

#include <util/atomic.h>

#include "kernel.h"

/*___________________________________________________________________________*/

#define K_MODULE    K_MODULE_WORKQUEUE

/*___________________________________________________________________________*/

void _k_workqueue_entry(struct k_workqueue *const workqueue)
{
        struct ditem *item;

        for (;;) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                        item = dlist_dequeue(&workqueue->queue);
                }

                if (DITEM_VALID(&workqueue->queue, item)) {
                        /* set the work as "submittable" */
                        item->next = NULL;

                        struct k_work *work =
                                CONTAINER_OF(item, struct k_work, tie);

                        work->handler(work);

                        /* yield if "yieldeach" option is enabled */
                        if (TEST_BIT(workqueue->flags, K_WORKQUEUE_YIELDEACH)) {
                                k_yield();
                        }
                } else {
                        /* K_WORKQUEUE_IDLE help to differentiate a work sleep
                         * from the workqueue sleeping */
                        SET_BIT(workqueue->flags, K_WORKQUEUE_IDLE);

                        /* sleep until a new work item is added to the queue */
                        k_sleep(K_UNTIL_WAKEUP);

                        CLR_BIT(workqueue->flags, K_WORKQUEUE_IDLE);
                }
        }
}

/*___________________________________________________________________________*/

void k_work_init(struct k_work *work, k_work_handler_t handler)
{
        work->handler = handler;
}

void k_work_submit(struct k_workqueue *workqueue, struct k_work *work)
{
        __ASSERT_NOTNULL(workqueue);
        __ASSERT_NOTNULL(work);
        __ASSERT_NOTNULL(work->handler);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                /* if item not already in queue */
                if (work->tie.next == NULL)
                {
                        /* we need to check if the workqueue is processing 
                        * an item, because we shouldn't wake it up if a work item
                        * being processed is waiting for an event while
                        * beiing processed.
                        *
                        * The workqueue should be idle to wake it up.
                        *
                        * If a work item is rescheduling another work item while
                        * beiing processed, there is no need to wake up the 
                        * workqueue thread.
                        */
                        if (TEST_BIT(workqueue->flags, K_WORKQUEUE_IDLE) &&
                                workqueue->thread->state == WAITING) {
                                _k_wake_up(workqueue->thread);
                        }

                        dlist_queue(&workqueue->queue, &work->tie);
                }
        }
}

void k_workqueue_set_yieldeach(struct k_workqueue *workqueue)
{
        __ASSERT_NOTNULL(workqueue);

        k_sched_lock();
        SET_BIT(workqueue->flags, K_WORKQUEUE_YIELDEACH);
        k_sched_unlock();
}

void k_workqueue_clr_yieldeach(struct k_workqueue *workqueue)
{
        __ASSERT_NOTNULL(workqueue);

        k_sched_lock();
        CLR_BIT(workqueue->flags, K_WORKQUEUE_YIELDEACH);
        k_sched_unlock();
}

/*___________________________________________________________________________*/

#if SYSTEM_WORKQUEUE_ENABLE

K_WORKQUEUE_DEFINE(_k_system_workqueue, SYSTEM_WORKQUEUE_STACK_SIZE, SYSTEM_WORKQUEUE_PRIORITY, 'W');

void k_system_workqueue_submit(struct k_work *work)
{
        k_work_submit(&_k_system_workqueue, work);
}

#endif

/*___________________________________________________________________________*/