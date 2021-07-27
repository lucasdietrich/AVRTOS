#include "mutex.h"

#include "kernel.h"

extern struct ditem *runqueue; 

extern struct titem *events_queue;

void mutex_define(mutex_t *mutex)
{
    mutex->lock = 0xFFu;
}

uint8_t mutex_lock(mutex_t *mutex)
{
    return _mutex_lock(mutex);
}

uint8_t mutex_lock_wait(mutex_t *mutex, k_timeout_t timeout)
{
    uint8_t lock = _mutex_lock(mutex);
    if ((lock != 0) && (timeout.value != K_NO_WAIT.value))
    {
        cli();

        k_current->state = WAITING;

        pop_ref(&runqueue);

        queue(&mutex->waitqueue, &k_current->wmutex);

        if(timeout.value != K_FOREVER.value)
        {
            tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
        }

        k_yield();

#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol(runqueue);
#endif
        lock = _mutex_lock(mutex);
        sei();
    }
#if KERNEL_SCHEDULER_DEBUG
    usart_transmit('#');
#endif
    return lock;
}

void mutex_release(mutex_t *mutex)
{
    cli();
    struct qitem* first_waiting_thread = dequeue(&mutex->waitqueue);
    if (first_waiting_thread == NULL)
    {
        _mutex_release(mutex);
    }
    else
    {
        struct thread_t *th = THREAD_OF_QITEM(first_waiting_thread);
        
        // remove from queue
        tqueue_remove(&events_queue, &th->tie.event);

        th->state = READY;

        th->wmutex.next = NULL;

        push_front(runqueue, &th->tie.runqueue);
        
#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol(runqueue);
        usart_transmit('*');
#endif
        _mutex_release(mutex);

        k_yield();
    }
    sei();
}