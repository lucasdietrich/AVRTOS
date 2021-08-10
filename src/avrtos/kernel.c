#include "kernel.h"

#include "idle.h"

/*___________________________________________________________________________*/

struct ditem *runqueue = &_k_thread_main.tie.runqueue; 

struct titem *events_queue = NULL;

/*___________________________________________________________________________*/

//
// Kernel Public API
//

void k_yield(void)
{
    _k_yield();
}

void k_sched_lock(void)
{
    cli();
    SET_BIT(k_current->flags, K_FLAG_COOP);
    sei();
}

void k_sched_unlock(void)
{
    cli();
    CLR_BIT(k_current->flags, K_FLAG_COOP);
    sei();
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

//
// Not tested
//

void k_suspend(void)
{
    cli();
    _k_suspend();
    sei();
}

void k_resume(struct thread_t *th)
{
    k_start(th);
}

void k_start(struct thread_t *th)
{
    if (th->state == STOPPED)
    {
        cli();
        _k_queue(th);
        sei();
    }
    else // thread waiting, ready of running and then already started
    {

    }
}

/*___________________________________________________________________________*/

//
// Kernel Private API
//

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
            _k_queue(&(&__k_threads_start)[i]);
        }
    }
}

void _k_queue(struct thread_t *const th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void _k_catch()
{
    pop_ref(&runqueue);
}

void _k_suspend(void)
{
    pop_ref(&runqueue);
    k_current->flags = STOPPED;
}

void _k_unschedule(struct thread_t *th)
{
    tqueue_remove(&events_queue, &th->tie.event);
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

    // if next thread is idle and there are others threads to be executed, we skip it
    if (KERNEL_THREAD_IDLE && (runqueue == &_k_idle.tie.runqueue) && (runqueue->next != &_k_idle.tie.runqueue))
    {
        ref_requeue(&runqueue);
    }

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

    // if next thread is idle and there are others threads to be executed, we skip it
    if (KERNEL_THREAD_IDLE && (runqueue == &_k_idle.tie.runqueue) && (runqueue->next != &_k_idle.tie.runqueue))
    {
        usart_print("p");
        ref_requeue(&runqueue);
    }

    _thread_symbol_runqueue(runqueue);

    THREAD_OF_DITEM(runqueue)->state = READY;

    return k_current = CONTAINER_OF(runqueue, struct thread_t, tie.runqueue);
}
#endif


void _k_reschedule(k_timeout_t timeout)
{
    _k_catch();

    k_current->state = WAITING;

    if (timeout.value != K_FOREVER.value)
    {
        tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
    }
}

// assumptions : thread not in runqueue and if tqueue
void _k_wake_up(struct thread_t *th)
{
    _k_unschedule(th);

    th->state = READY;
    th->wmutex.next = NULL;

    push_front(runqueue, &th->tie.runqueue);
}

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}


/*___________________________________________________________________________*/

