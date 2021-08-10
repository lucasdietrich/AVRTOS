#include "workqueue.h"

#include "kernel.h"

/*___________________________________________________________________________*/

void _k_workqueue_entry(struct k_workqueue * context)
{
    struct k_workqueue * const workqueue = (struct k_workqueue *) context;
    for(;;)
    {
        k_sched_lock();
        struct ditem * const item = dlist_dequeue(&workqueue->queue);
        k_sched_unlock();

        if (item != nullptr)
        {
            item->prev = NULL; // reset item prev in order to know that the work item can be pushed again
            
            struct k_work * work = CONTAINER_OF(item, struct k_work, tie);

            work->handler(work);    // call the task
        }
        else
        {
            SET_BIT(workqueue->flags, K_WORKQUEUE_IDLE);

            k_sleep(K_FOREVER); // sleep until a new work item is added to the queue

            CLR_BIT(workqueue->flags, K_WORKQUEUE_IDLE);
        }
    }
}

/*___________________________________________________________________________*/

void k_work_init(struct k_work * work, k_work_handler_t handler)
{
    work->handler = handler;
}

// TODO : don't submit if it's already queued
void k_work_submit(struct k_workqueue *workqueue, struct k_work *work)
{
    cli(); // maybe optimize, can be only k_sched_lock arround dlist_queue

    if (work->tie.prev == NULL) // if item is not already in queue
    {

        // we need to check if the workqueue is processing an item (workqueue should be idle),
        // because we shouldn't wake up it if there if the item is waiting for an event while beiing processed
        if (TEST_BIT(workqueue->flags, K_WORKQUEUE_IDLE) &&
            workqueue->thread->state == WAITING)
        {
            _k_wake_up(workqueue->thread);
        }
        dlist_queue(&workqueue->queue, &work->tie);
    }

    sei();
}

/*___________________________________________________________________________*/

#if SYSTEM_WORKQUEUE_ENABLE

K_WORKQUEUE_DEFINE(_k_system_workqueue, SYSTEM_WORKQUEUE_STACK_SIZE, DEFAULT_SYSTEM_WORKQUEUE_PRIORITY);

void k_system_workqueue_submit(struct k_work * work)
{
    k_work_submit(&_k_system_workqueue, work);
}

#endif

/*___________________________________________________________________________*/