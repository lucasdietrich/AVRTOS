#include "kernel.h"

#include "idle.h"

#include <util/atomic.h>

// debug
#include "debug.h"

/*___________________________________________________________________________*/

/**
 * @brief Runqueue containing the queue of all ready threads.
 * Should never be NULL.
 */
struct ditem *runqueue = &_k_thread_main.tie.runqueue; 

struct titem *events_queue = NULL;

static inline bool _k_runqueue_single(void)
{
    return runqueue->next == runqueue;
}

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

void k_sleep(k_timeout_t timeout)
{
    if (timeout.value != K_NO_WAIT.value)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            _k_reschedule(timeout);

            k_yield();
        }
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
    if (th->state == WAITING)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            th->state = READY;
            _k_schedule(&th->tie.runqueue);
        }
    }
    else // thread waiting, ready of running and then already started
    {

    }
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
        struct k_thread *const thread = &(&__k_threads_start)[i];
        if ((thread != &_k_idle) && (thread->state == READY)) // only queue ready threads
        {
            push_front(runqueue, &thread->tie.runqueue);
        }
    }
}

void _k_queue(struct k_thread *const th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void _k_schedule(struct ditem * const thread_tie)
{
    if (KERNEL_THREAD_IDLE && _k_runqueue_idle())
    {
        dlist_ref(thread_tie);
        runqueue = thread_tie;
    }
    else
    {
        push_front(runqueue, thread_tie);
    }
}

void _k_schedule_wake_up(struct k_thread *thread, k_timeout_t timeout)
{
    if (timeout.value != K_FOREVER.value)
    {
        tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
    }
}

void _k_suspend(void)
{
    if (KERNEL_THREAD_IDLE && _k_runqueue_single())
    {
        struct ditem *const tie = &_k_idle.tie.runqueue;
        dlist_ref(tie);
        runqueue = tie;
    }
    else
    {
        pop_ref(&runqueue);
    }

    k_current->state = WAITING;
}

void _k_unschedule(struct k_thread *th)
{
    tqueue_remove(&events_queue, &th->tie.event);
}

struct k_thread *_k_scheduler(void)
{
    k_current->timer_expired = 0;
    k_current->immediate = 0;

    if (k_current->state == WAITING)
    {
        // runqueue is positionned to the normally next thread to be executed
        __K_DBG_SCHED_WAITING();
    }
    else
    {
        // next thread is positionned at the top of the runqueue
        ref_requeue(&runqueue);

        __K_DBG_SCHED_REQUEUE();
    }

    struct ditem* ready = (struct ditem*) (struct titem*) tqueue_pop(&events_queue);
    if (ready != NULL)
    {
        __K_DBG_SCHED_EVENT();

        // idle thread cannot be immediate
        if (THREAD_OF_DITEM(runqueue)->immediate)
        {
            __K_DBG_SCHED_EVENT_ON_IMMEDIATE(THREAD_OF_DITEM(ready));

            push_front(runqueue->next, ready);
        }
        else
        {
            _k_schedule(ready);
        }
    }

    k_current = CONTAINER_OF(runqueue, struct k_thread, tie.runqueue);
    k_current->state = READY;

    __K_DBG_SCHED_NEXT(k_current);

    return k_current;
}

void _k_wake_up(struct k_thread *th)
{
    __K_DBG_WAKEUP(th);

    th->state = READY;

    _k_unschedule(th);

    _k_schedule(&th->tie.runqueue);     // schedule in runqueue
}

void _k_immediate_wake_up(struct k_thread *th)
{
    SET_BIT(th->flags, K_FLAG_IMMEDIATE);

    _k_wake_up(th);
}

void _k_reschedule(k_timeout_t timeout)
{
    _k_suspend();
    _k_schedule_wake_up(k_current, timeout);
}

/*___________________________________________________________________________*/

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}

/*___________________________________________________________________________*/

