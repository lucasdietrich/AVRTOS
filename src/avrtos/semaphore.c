#include "semaphore.h"

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <util/atomic.h>

extern struct ditem *runqueue; 
extern struct titem *events_queue;

void k_sem_init(struct k_sem *sem, uint8_t initial_count, uint8_t limit)
{
    sem->limit = limit;
    sem->count = MIN(limit, initial_count);
    sem->waitqueue = NULL;
}

uint8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
    int8_t get;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        get = _k_sem_take(sem);
        if ((get != 0) && (timeout.value != K_NO_WAIT.value))
        {
            __K_DBG_SEM_WAIT(k_current);

            dlist_queue(&sem->waitqueue, &k_current->wsem);

            _k_reschedule(timeout);

            k_yield();

            if (TEST_BIT(k_current->flags, K_FLAG_TIMER_EXPIRED))
            {
                dlist_remove(&sem->waitqueue, &k_current->wsem);
            }

            get = _k_sem_take(sem);
        }
    }

    if (get == 0)
    {
        __K_DBG_SEM_TAKE(k_current);
    }

    return get;
}

void k_sem_give(struct k_sem *sem)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _k_sem_give(sem);

        __K_DBG_SEM_GIVE(k_current);

        struct ditem *const first_thread = dlist_dequeue(&sem->waitqueue);
        if (first_thread != NULL)
        {
            struct k_thread *const th = THREAD_OF_QITEM(first_thread);

            /* immediate: the first thread in the queue 
             * must be the first to get the semaphore */
            _k_immediate_wake_up(th);

            k_yield();
        }
    }
}