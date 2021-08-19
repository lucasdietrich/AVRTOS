#include "semaphore.h"

#include "kernel.h"

void k_sem_init(struct k_sem *sem, uint8_t initial_count, uint8_t limit)
{
    sem->limit = limit;
    sem->count = MIN(limit, initial_count);
    sem->waitqueue = NULL;
}

uint8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
    int8_t get = _k_sem_take(sem);
    if ((get != 0) && (timeout.value != K_NO_WAIT.value))
    {
        cli();
        queue(&sem->waitqueue, &k_current->wmutex);
        _k_reschedule(timeout);
        k_yield();
        get = _k_sem_take(sem);
        sei();
    }
    return get;
}

void k_sem_give(struct k_sem *sem)
{
    cli();
    struct qitem *const first_waiting_thread = dequeue(&sem->waitqueue);
    if (first_waiting_thread == NULL)
    {
        _k_sem_give(sem);
    }
    else
    {
        struct thread_t *const th = THREAD_OF_QITEM(first_waiting_thread);
        th->wmutex.next = NULL;
        _k_wake_up(th);
        _k_sem_give(sem);
        k_yield();
    }
    sei();
}