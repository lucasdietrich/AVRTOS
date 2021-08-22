#include "kernel.h"

#include "idle.h"

#include <util/atomic.h>

// debug
#include "debug.h"

/*___________________________________________________________________________*/

struct ditem *runqueue = &_k_thread_main.tie.runqueue; 

struct titem *events_queue = NULL;

/*___________________________________________________________________________*/

//
// Kernel Public API
//

void k_sched_lock(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SET_BIT(k_current->flags, K_FLAG_COOP);
    }

    __K_DBG_SCHED_LOCK(k_current);
}

void k_sched_unlock(void)
{
    __K_DBG_SCHED_UNLOCK();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        CLR_BIT(k_current->flags, K_FLAG_COOP);
    }
}

bool k_sched_locked(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        return (bool) TEST_BIT(k_current->flags, K_FLAG_COOP);
    }

    __builtin_unreachable();
}

void k_soft_yield(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (!TEST_BIT(k_current->flags, K_FLAG_COOP))
        {   
            k_yield();
        }
    }
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.value != K_NO_WAIT.value)
    {
        cli();

        _k_reschedule(timeout);
     
        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/

//
// Not tested
//

void k_suspend(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _k_suspend();
    }
}

void k_resume(struct k_thread *th)
{
    k_start(th);
}

void k_start(struct k_thread *th)
{
    if (th->state == STOPPED)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            _k_queue(th);
        }
    }
    else // thread waiting, ready of running and then already started
    {

    }
}

/*___________________________________________________________________________*/

//
// Kernel Private API
//

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

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

void _k_queue(struct k_thread *const th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void _k_catch()
{
    pop_ref(&runqueue);
}

void _k_suspend(void)
{
    _k_catch();

    k_current->flags = STOPPED;
}

// TODO : use double linked list in order to remove the event with 1 call (without loop)
void _k_unschedule(struct k_thread *th)
{
    tqueue_remove(&events_queue, &th->tie.event);
}

struct k_thread *_k_scheduler(void)
{
    void *ready = (struct titem*) tqueue_pop(&events_queue);
    if (ready != NULL)
    {
        __K_DBG_SCHED_EVENT();
        
        // next thread is in immediate mode and should be executed first (no reorder)
        struct k_thread * const default_next_th = THREAD_OF_DITEM(runqueue->next);
        if(TEST_BIT(default_next_th->flags, K_FLAG_IMMEDIATE))
        {
            CLR_BIT(default_next_th->flags, K_FLAG_IMMEDIATE);
            push_front(runqueue->next, ready);
            
            __K_DBG_SCHED_EVENT_ON_IMMEDIATE(THREAD_OF_DITEM(ready));
        }
        else
        {
            push_front(runqueue, ready);
        }

        ref_requeue(&runqueue);
    }
    else if (k_current->state == WAITING)
    {
        __K_DBG_SCHED_WAITING();
    }
    else
    {
        ref_requeue(&runqueue);
        
        __K_DBG_SCHED_REQUEUE();
    }

    // if next thread is idle and there are others threads to be executed, we skip it
    if (KERNEL_THREAD_IDLE && (runqueue == &_k_idle.tie.runqueue) && (runqueue->next != &_k_idle.tie.runqueue))
    {
        ref_requeue(&runqueue);

        __K_DBG_SCHED_SKIP_IDLE();
    }

    k_current = CONTAINER_OF(runqueue, struct k_thread, tie.runqueue);
    k_current->state = READY;

    __K_DBG_SCHED_NEXT(k_current);

    return k_current;
}


void _k_reschedule(k_timeout_t timeout)
{
    _k_catch();

    k_current->state = WAITING;

    if (timeout.value != K_FOREVER.value)
    {
        tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
    }
}

// assumptions : thread not in runqueue and (potentially) in tqueue
void _k_wake_up(struct k_thread *th)
{
    __K_DBG_WAKEUP(th);

    _k_unschedule(th);
    th->state = READY;
    push_front(runqueue, &th->tie.runqueue);
}

void _k_immediate_wake_up(struct k_thread *th)
{
    SET_BIT(th->flags, K_FLAG_IMMEDIATE);

    _k_wake_up(th);
}

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}


/*___________________________________________________________________________*/

