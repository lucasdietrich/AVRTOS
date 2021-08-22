#include "mutex.h"

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <util/atomic.h>

extern struct ditem *runqueue; 
extern struct titem *events_queue;

void k_mutex_init(struct k_mutex *mutex)
{
    mutex->lock = 0xFFu;
}

NOINLINE uint8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
    uint8_t lock = _k_mutex_lock(mutex);
    if ((lock != 0) && (timeout.value != K_NO_WAIT.value))
    {
        __K_DBG_MUTEX_WAIT(k_current);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            // must be executed without interruption
            queue(&mutex->waitqueue, &k_current->wmutex);
            _k_reschedule(timeout);
            k_yield();

            lock = _k_mutex_lock(mutex);
        }
    }

    if (lock == 0)
    {
        __K_DBG_MUTEX_LOCKED(k_current);
    }

    return lock;
}

NOINLINE void k_mutex_unlock(struct k_mutex *mutex)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _k_mutex_unlock(mutex);

        __K_DBG_MUTEX_UNLOCKED(k_current);

        struct qitem* first_waiting_thread = dequeue(&mutex->waitqueue);
        if (first_waiting_thread != NULL)
        {
            struct k_thread *th = THREAD_OF_QITEM(first_waiting_thread);
            th->wmutex.next = NULL;

            /* immediate: the first thread in the queue must be the first to get the semaphore */
            _k_immediate_wake_up(th);

            k_soft_yield();
        }
    }
}