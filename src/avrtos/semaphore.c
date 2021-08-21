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

NOINLINE uint8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
    int8_t get = _k_sem_take(sem);
    if ((get != 0) && (timeout.value != K_NO_WAIT.value))
    {
#if KERNEL_SCHEDULER_DEBUG
        usart_print("($");
        _thread_symbol_runqueue(runqueue);
#endif

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            queue(&sem->waitqueue, &k_current->wmutex);
            _k_reschedule(timeout);
            k_yield();

            get = _k_sem_take(sem);
        }
    }

#if KERNEL_SCHEDULER_DEBUG
    if (get == 0)
    {
        usart_print("-$");
        _thread_symbol_runqueue(runqueue);
    }
#endif

    return get;
}

NOINLINE void k_sem_give(struct k_sem *sem)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#if KERNEL_SCHEDULER_DEBUG
        usart_print("+$");
        _thread_symbol_runqueue(runqueue);
        usart_transmit('*');
#endif
        _k_sem_give(sem);

        struct qitem *const first_waiting_thread = dequeue(&sem->waitqueue);
        if (first_waiting_thread != NULL)
        {
            struct thread_t *const th = THREAD_OF_QITEM(first_waiting_thread);
            th->wmutex.next = NULL;

            /* immediate: the first thread in the queue must be the first to get the semaphore */
            _k_immediate_wake_up(th);

            k_yield();
        }
    }
}