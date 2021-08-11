#include "mutex.h"

#include "kernel.h"
#include "debug.h"

#include "debug.h"

extern struct ditem *runqueue; 
extern struct titem *events_queue;

void k_mutex_init(mutex_t *mutex)
{
    mutex->lock = 0xFFu;
}

uint8_t k_mutex_lock(mutex_t *mutex)
{
    return _k_mutex_lock(mutex);
}

uint8_t k_mutex_lock_wait(mutex_t *mutex, k_timeout_t timeout)
{
    uint8_t lock = _k_mutex_lock(mutex);
    if ((lock != 0) && (timeout.value != K_NO_WAIT.value))
    {
        cli();
        queue(&mutex->waitqueue, &k_current->wmutex);

        _k_reschedule(timeout);

        k_yield();

#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol_runqueue(runqueue);
#endif

        lock = _k_mutex_lock(mutex);
        sei();
    }
#if KERNEL_SCHEDULER_DEBUG
    usart_transmit('#');
#endif

    return lock;
}

void k_mutex_release(mutex_t *mutex)
{
    cli();
    struct qitem* first_waiting_thread = dequeue(&mutex->waitqueue);
    if (first_waiting_thread == NULL)
    {
        _k_mutex_release(mutex);
    }
    else
    {
        struct thread_t *th = THREAD_OF_QITEM(first_waiting_thread);

        th->wmutex.next = NULL;

        _k_wake_up(th);
        
#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol_runqueue(runqueue);
        usart_transmit('*');
#endif
        _k_mutex_release(mutex);

        k_yield();
    }
    sei(); // todo only enable interrupt if interrupt flag was set previously
}