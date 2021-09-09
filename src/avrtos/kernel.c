#include "kernel.h"

#include "idle.h"

#include <util/atomic.h>

#include "debug.h"

/*___________________________________________________________________________*/

#define K_MODULE    K_MODULE_KERNEL

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

extern bool __k_interrupts(void);

/*___________________________________________________________________________*/

//
// Kernel Public API
//

void k_sched_lock(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SET_BIT(k_current->flags, K_FLAG_SCHED_LOCKED);
    }

    __K_DBG_SCHED_LOCK(k_current);
}

void k_sched_unlock(void)
{
    __K_DBG_SCHED_UNLOCK();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        CLR_BIT(k_current->flags, K_FLAG_SCHED_LOCKED);
    }
}

bool k_sched_locked(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        return (bool) TEST_BIT(k_current->flags, K_FLAG_SCHED_LOCKED);
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

    /* main thread is the first running (ready or not), 
     * and it is already in queue */
    for (uint8_t i = 0; i < _k_thread_count; i++)
    {
        struct k_thread *const thread = &(&__k_threads_start)[i];
        if (!IS_THREAD_IDLE(thread) && (thread->state == READY))
        {
            push_front(runqueue, &thread->tie.runqueue);
        }
    }
}

void _k_queue(struct k_thread *const th)
{
    __ASSERT_NOINTERRUPT();

    push_back(runqueue, &th->tie.runqueue);
}

void _k_schedule(struct ditem *const thread_tie)
{
    __ASSERT_NOINTERRUPT();

#if KERNEL_THREAD_IDLE
    if (_k_runqueue_idle())
    {
        dlist_ref(thread_tie);
        runqueue = thread_tie;
        return;
    }
#endif

    push_front(runqueue, thread_tie);
}

void _k_schedule_wake_up(struct k_thread *thread, k_timeout_t timeout)
{
    __ASSERT_NOINTERRUPT();

    if (timeout.value != K_FOREVER.value)
    {
        tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
    }
}

void _k_suspend(void)
{
    __ASSERT_NOINTERRUPT();

    k_current->state = WAITING;

#if KERNEL_THREAD_IDLE
    if (_k_runqueue_single())
    {
        struct ditem *const tie = &_k_idle.tie.runqueue;
        dlist_ref(tie);
        runqueue = tie;
        return;
    }
#else
    __ASSERT_LEASTONE_RUNNING();
#endif

    pop_ref(&runqueue);
}

void _k_unschedule(struct k_thread *th)
{
    __ASSERT_NOINTERRUPT();

    tqueue_remove(&events_queue, &th->tie.event);
}

struct k_thread* _k_scheduler(void)
{
    __ASSERT_NOINTERRUPT();

    k_current->timer_expired = 0;
    k_current->immediate = 0;

    if (k_current->state == WAITING) {
        /* runqueue is positionned to the normally next thread to be executed */
        __K_DBG_SCHED_WAITING();        // ~
    }
    else {
        /* next thread is positionned at the top of the runqueue */
        ref_requeue(&runqueue);

        __K_DBG_SCHED_REQUEUE();        // >
    }

    struct ditem* ready = (struct ditem*)tqueue_pop(&events_queue);
    if (ready != NULL) {
        __K_DBG_SCHED_EVENT();  // !

        /* set ready thread expired flag */
        THREAD_FROM_EVENTQUEUE(ready)->timer_expired = 1u;

        /* if a thread has been woken up with the immediate flag set,
         * the expired thread will be push just after it */
        /* as idle thread cannot be immediate, it will never be prioritized */
        if (THREAD_FROM_EVENTQUEUE(runqueue)->immediate) {
            __K_DBG_SCHED_EVENT_ON_IMMEDIATE(THREAD_FROM_EVENTQUEUE(ready));   // '

            push_front(runqueue->next, ready);
        }
        else {
            _k_schedule(ready);
        }
    }

    k_current = CONTAINER_OF(runqueue, struct k_thread, tie.runqueue);
    k_current->state = READY;

    __K_DBG_SCHED_NEXT(k_current);  // thread symbol

    return k_current;
}

void _k_wake_up(struct k_thread *th)
{
    __ASSERT_NOINTERRUPT();
    __ASSERT_THREAD_STATE(th, WAITING);

    __K_DBG_WAKEUP(th); // @

    th->state = READY;

    _k_unschedule(th);

    _k_schedule(&th->tie.runqueue);     // schedule in runqueue
}

void _k_immediate_wake_up(struct k_thread *th)
{
    __ASSERT_NOINTERRUPT();

    th->immediate = 1u;

    _k_wake_up(th);
}

void _k_reschedule(k_timeout_t timeout)
{
    __ASSERT_NOINTERRUPT();

    _k_suspend();
    
    _k_schedule_wake_up(k_current, timeout);
}

/*___________________________________________________________________________*/

void _k_system_shift(void)
{
    __ASSERT_NOINTERRUPT();
    
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}

/*___________________________________________________________________________*/

int8_t _k_pend_current(struct ditem *waitqueue, k_timeout_t timeout)
{
    int8_t err = -1;
    if (timeout.value != 0) {
        dlist_queue(waitqueue, &k_current->wany);

        _k_reschedule(timeout);
        k_yield();

        if (k_current->timer_expired) {
            dlist_remove(&k_current->wany);
            err = -ETIMEOUT;
        } else {
            err = 0;
        }
    }
    return err;
}

void _k_unpend_first_thread(struct ditem* waitqueue)
{
    struct ditem* pending_thread = dlist_dequeue(waitqueue);
    if (DITEM_VALID(waitqueue, pending_thread)) {
        struct k_thread* th = THREAD_FROM_WAITQUEUE(pending_thread);

        /* immediate: the first thread in the queue
         * must be the first to get the object */
        _k_immediate_wake_up(th);

        k_yield();
    }
}

/*___________________________________________________________________________*/