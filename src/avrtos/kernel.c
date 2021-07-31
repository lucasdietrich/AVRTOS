#include "kernel.h"

/*___________________________________________________________________________*/

struct ditem *runqueue = &_k_thread_main.tie.runqueue; 

struct titem *events_queue = NULL;

/*___________________________________________________________________________*/

void k_queue(struct thread_t *th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void k_yield(void)
{
    _k_yield();
}

void _k_unschedule()
{
    pop_ref(&runqueue);
}

void _k_reschedule(k_timeout_t timeout)
{
    k_current->state = WAITING;

    pop_ref(&runqueue);

    if (timeout.value != K_FOREVER.value)
    {
        tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
    }
}

// assumptions : thread not in runqueue and if tqueue
void _k_wake_up(struct thread_t *th)
{
    tqueue_remove(&events_queue, &th->tie.event);

    th->state = READY;
    th->wmutex.next = NULL;

    push_front(runqueue, &th->tie.runqueue);
}

extern struct thread_t __k_threads_start;
extern struct thread_t __k_threads_end;

uint8_t _k_thread_count = 1;

void _k_kernel_init(void)
{
    _k_thread_count = &__k_threads_end - &__k_threads_start;

    // main thread is the first running (ready or not), already in queue
    for (uint8_t i = 0; i < _k_thread_count; i++)
    {
        if ((&__k_threads_start)[i].state == READY) // only queue ready threads
        {
            k_queue(&(&__k_threads_start)[i]);
        }
    }
}



#if KERNEL_SCHEDULER_DEBUG == 0

struct thread_t *_k_scheduler(void)
{
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL)
    {
        push_ref(&runqueue, next);
    }
    else if(k_current->state != WAITING)
    {
        ref_requeue(&runqueue);
    }
    // if next thread is idle, we skip it
    // if (KERNEL_THREAD_IDLE && (runqueue == &_k_idle.tie.runqueue) && (runqueue->next != &_k_idle.tie.runqueue))
    // {
    //     ref_requeue(&runqueue);
    // }

    THREAD_OF_DITEM(runqueue)->state = READY;

    return k_current = CONTAINER_OF(runqueue, struct thread_t, tie.runqueue);
}

#else

#include "misc/uart.h"
#include "debug.h"

// todo write this function in assembly
// ! mean tqueue item
// ~ thread set to waiting
// > default next thread
struct thread_t *_k_scheduler(void)
{
    // print_tqueue(events_queue, _thread_symbol);
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL)
    {
        push_ref(&runqueue, next);
        usart_transmit('!');
    }
    else if (k_current->state == WAITING)
    {
        usart_transmit('~');
    }
    else
    {
        ref_requeue(&runqueue);
        usart_transmit('>');
    }

    _thread_symbol_runqueue(runqueue);

    THREAD_OF_DITEM(runqueue)->state = READY;

    return k_current = CONTAINER_OF(runqueue, struct thread_t, tie.runqueue);
}
#endif


/*___________________________________________________________________________*/

#include "misc/uart.h"

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.value != K_NO_WAIT.value)
    {
        cli();

        _k_reschedule(timeout);
     
        _k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/

