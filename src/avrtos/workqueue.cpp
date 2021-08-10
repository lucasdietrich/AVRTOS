#include "workqueue.h"

#include "kernel.h"

/*___________________________________________________________________________*/

void k_work_init(struct k_work * work, k_work_handler_t handler)
{
    work->handler = handler;
    work->tie.timeout = K_NO_WAIT.value; // TODO timeout not supported for now

    // K_FOREVER is illegal here
}

void _k_workqueue_entry(struct k_workqueue * context)
{
    struct k_workqueue * const workqueue = (struct k_workqueue *) context;
    for(;;)
    {
        k_sched_lock();
        struct titem * const item = tqueue_pop(&workqueue->root);
        k_sched_unlock();
        if (item != nullptr)
        {
            CLR_BIT(workqueue->flags, K_WORKQUEUE_IDLE);

            struct k_work * work = CONTAINER_OF(item, struct k_work, tie);

            work->handler(work);    // call the task
        }
        else
        {
            SET_BIT(workqueue->flags, K_WORKQUEUE_IDLE);

            k_sleep(K_FOREVER); // sleep until a new work item is added to the queue
        }
    }
}

// TODO : don't submit if it's already queued
void k_work_submit(struct k_workqueue * workqueue, struct k_work * work)
{
    cli();
    
    // we need to check if the workqueue is processing an item (workqueue should be idle), 
    // because we shouldn't wake up it if there if the item is waiting for an event while beiing processed
    if (TEST_BIT(workqueue->flags, K_WORKQUEUE_IDLE) &&
        workqueue->thread->state == WAITING)
    {
        _k_wake_up(workqueue->thread);
    }

    tqueue_schedule(&workqueue->root, &work->tie, K_NO_WAIT.value);
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