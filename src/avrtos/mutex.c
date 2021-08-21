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

uint8_t k_mutex_lock(struct k_mutex *mutex)
{
    return _k_mutex_lock(mutex);
}

NOINLINE uint8_t k_mutex_lock_wait(struct k_mutex *mutex, k_timeout_t timeout)
{
    uint8_t lock = _k_mutex_lock(mutex);
    if ((lock != 0) && (timeout.value != K_NO_WAIT.value))
    {
#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol_runqueue(runqueue);
#endif

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            // must be executed without interruption
            queue(&mutex->waitqueue, &k_current->wmutex);
            _k_reschedule(timeout);
            k_yield();

            lock = _k_mutex_lock(mutex);
        }
    }

#if KERNEL_SCHEDULER_DEBUG
    usart_transmit('#');
#endif

    return lock;
}

NOINLINE void k_mutex_release(struct k_mutex *mutex)
{
    // must be executed without interruption
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol_runqueue(runqueue);
        usart_transmit('*');
#endif

        _k_mutex_release(mutex);

        struct qitem* first_waiting_thread = dequeue(&mutex->waitqueue);
        if (first_waiting_thread != NULL)
        {
            struct thread_t *th = THREAD_OF_QITEM(first_waiting_thread);
            th->wmutex.next = NULL;

            /* immediate: the first thread in the queue must be the first to get the semaphore */
            _k_immediate_wake_up(th);

            k_soft_yield();
        }
    }
}