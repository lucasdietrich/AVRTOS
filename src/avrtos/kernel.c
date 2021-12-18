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

/*___________________________________________________________________________*/

//
// Kernel Public API
//

#if KERNEL_IRQ_LOCK_COUNTER
void irq_disable(void)
{
        cli();

        _current->irq_lock_cnt++;
}

void irq_enable(void)
{
        if (_current->irq_lock_cnt == 0) {
                return;
        } else if (_current->irq_lock_cnt == 1) {
                _current->irq_lock_cnt = 0;
        } else {
                _current->irq_lock_cnt--;
                return;
        }

        sei();
}
#endif

void k_sched_lock(void)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                _current->sched_lock = 1;
        }

#if KERNEL_SCHED_LOCK_COUNTER
        _current->sched_lock_cnt++;
#endif /* KERNEL_SCHED_LOCK_COUNTER */

        __K_DBG_SCHED_LOCK(_current);
}

void k_sched_unlock(void)
{
#if KERNEL_SCHED_LOCK_COUNTER
        if (_current->sched_lock_cnt == 0) {
                return;
        } else if (_current->sched_lock_cnt == 1) {
                _current->sched_lock_cnt = 0;
        } else {
                _current->sched_lock_cnt--;
                return;
        }
#endif /* KERNEL_SCHED_LOCK_COUNTER */

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                _current->sched_lock = 0;
        }

        __K_DBG_SCHED_UNLOCK();
}

bool k_sched_locked(void)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                return _current->sched_lock == 1;
        }

        __builtin_unreachable();
}

void k_sleep(k_timeout_t timeout)
{
        if (timeout.value != K_NO_WAIT.value) {
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
        if (th->state == PENDING) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                        th->state = READY;
                        _k_schedule(th);
                }
        } else {
                /* thread pending, ready of running and then already started */
        }
}

void k_start(struct k_thread *th)
{
	if (th->state == STOPPED) {
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

extern struct k_thread _k_idle;

uint8_t _k_thread_count = 0;

inline static void swap_endianness(uint16_t *const addr)
{
        *addr = HTONS(*addr);
}

void _k_kernel_init(void)
{
	_k_thread_count = &__k_threads_end - &__k_threads_start;

	/* main thread is the first running (ready or not),
	 * and it is already in queue */
	for (uint8_t i = 0; i < _k_thread_count; i++) {
		struct k_thread *const thread = &(&__k_threads_start)[i];

#if KERNEL_THREAD_IDLE
		const bool is_thread_idle = thread == &_k_idle;
#else
		const bool is_thread_idle = false;
#endif

		/* idle thread must not be added to the
		 * runqueue as the main thread is running */
		if (!is_thread_idle &&
		    (thread->state == READY) &&
		    (thread != _current)) {
			push_back(runqueue, &thread->tie.runqueue);
		}

		/* Swap endianness of addresses in compilation-time built stacks.
		 * We cannot change the endianness of addresses determined by the
		 * linker at compilation time. So we need to do it on system start up
		 */
		if (_current != thread) {
		    /* thread kernel entry function address */
			swap_endianness(thread->stack.end - 1u);

#if THREAD_ALLOW_RETURN == 1
			/* thread kernel entry function address */
			swap_endianness(thread->stack.end - 1u -
					(6u + _K_ARCH_STACK_SIZE_FIXUP + 2u));
#endif

			/* thread context address */
			swap_endianness(thread->stack.end - 1u -
					(8u + _K_ARCH_STACK_SIZE_FIXUP + 2u));
		}
	}
}

void _k_queue(struct k_thread *const th)
{
        __ASSERT_NOINTERRUPT();

        push_back(runqueue, &th->tie.runqueue);
}

void _k_schedule(struct k_thread *thread)
{
        __ASSERT_NOINTERRUPT();

#if THREAD_STACK_SENTINEL
	/* check that stack sentinel is still valid before switching to thread */
	if (k_verify_stack_sentinel(thread) == false) {
		__fault(K_FAULT_SENTINEL);
	}
#endif

#if KERNEL_THREAD_IDLE
        if (k_is_cpu_idle()) {
                dlist_ref(&thread->tie.runqueue);
                runqueue = &thread->tie.runqueue;
                return;
        }
#endif

        push_front(runqueue, &thread->tie.runqueue);
}

void _k_schedule_wake_up(k_timeout_t timeout)
{
        __ASSERT_NOINTERRUPT();
	__ASSERT_TRUE(_current->wakeup_schd == 0);
	
        if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		_current->wakeup_schd = 1;
                _current->tie.event.timeout = K_TIMEOUT_MS(timeout);
                _current->tie.event.next = NULL;
                _tqueue_schedule(&events_queue, &_current->tie.event);
        }
}

void _k_suspend(void)
{
        __ASSERT_NOINTERRUPT();

        _current->state = PENDING;

#if KERNEL_THREAD_IDLE
        if (_k_runqueue_single()) {
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

struct k_thread *_k_scheduler(void)
{
        __ASSERT_NOINTERRUPT();

        /* reset flags */
        _current->pend_canceled = 0;
        _current->timer_expired = 0;

        if (_current->state == PENDING) {
                /* runqueue is positionned to the 
                 * normally next thread to be executed */
                __K_DBG_SCHED_PENDING();        // ~
        } else {
                /* next thread is positionned at the top of the runqueue */
                ref_requeue(&runqueue);

                __K_DBG_SCHED_REQUEUE();        // >
        }

        _current = CONTAINER_OF(runqueue, struct k_thread, tie.runqueue);

        __K_DBG_SCHED_NEXT(_current);

        return _current;
}

void _k_wake_up(struct k_thread *th)
{
	__ASSERT_NOTNULL(th);
        __ASSERT_NOINTERRUPT();
        __ASSERT_THREAD_STATE(th, PENDING);

        __K_DBG_WAKEUP(th); // @

	/* Remove the thread from the events queue */
	if (th->wakeup_schd) {
		tqueue_remove(&events_queue, &th->tie.event);
	}

	th->state = READY;
	th->wakeup_schd = 0;

        _k_schedule(th);
}

void _k_reschedule(k_timeout_t timeout)
{
        __ASSERT_NOINTERRUPT();

        _k_suspend();

        _k_schedule_wake_up(timeout);
}

/*___________________________________________________________________________*/


#if KERNEL_UPTIME
union {
	uint32_t u32;
#if KERNEL_UPTIME_40BITS
	uint8_t bytes[5]; /* 35 years overflow */
#else
	uint8_t bytes[4]; /* 49 days overflow */
#endif /* KERNEL_UPTIME_40BITS */

} _k_uptime_ms = { .u32 = 0LU };
#endif /* KERNEL_UPTIME */

#if KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS
uint8_t _k_remaining_sysclock_hits = KERNEL_TIME_SLICE / SYSCLOCK_PERIOD_MS;
#endif /* KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS */

void _k_system_shift(void)
{
	__ASSERT_NOINTERRUPT();

        tqueue_shift(&events_queue, KERNEL_TIME_SLICE);

        struct ditem *ready = (struct ditem *)tqueue_pop(&events_queue);
        if (ready != NULL) {
		struct k_thread *thread = THREAD_FROM_EVENTQUEUE(ready);

                __K_DBG_SCHED_EVENT(thread);  // !

                /* set ready thread expired flag */
                thread->timer_expired = 1u;

		/* mark as "not in events queue" */
		thread->wakeup_schd = 0;

                _k_schedule(thread);
        }

#if KERNEL_TIMERS
	_k_timers_process();
#endif

#if KERNEL_EVENTS
	_k_event_q_process();
#endif /* KERNEL_EVENTS */
}

uint32_t k_uptime_get_ms32(void)
{
#if KERNEL_UPTIME
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		return _k_uptime_ms.u32;
	}

	__builtin_unreachable();
#else
	return 0;
#endif /* KERNEL_UPTIME */
}

uint64_t k_uptime_get_ms64(void)
{
	uint64_t ms64 = 0x0ULL;
#if KERNEL_UPTIME_40BITS
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// ms64 = (uint64_t) _k_uptime_ms.u32;
		// ms64 <<= 8;
		// ms64 |= _k_uptime_ms.bytes[4];
		memcpy(&ms64, &_k_uptime_ms.bytes, sizeof(_k_uptime_ms.bytes));
	}
#elif KERNEL_UPTIME
	ms64 |= (uint32_t) k_uptime_get_ms32();
#endif /* KERNEL_UPTIME */

return ms64;
}

uint32_t k_uptime_get(void)
{
	return k_uptime_get_ms32() / 1000;
}

void k_timespec_get(struct timespec *ts)
{
	if (ts == NULL) {
		return;
	}

#if KERNEL_UPTIME_40BITS
	uint64_t ms = k_uptime_get_ms64();
#else 
	uint32_t ms = k_uptime_get_ms32();
#endif

	ts->tv_sec = ms / 1000;
	ts->tv_msec = ms % 1000;
}

/*___________________________________________________________________________*/

int8_t _k_pend_current(struct ditem *waitqueue, k_timeout_t timeout)
{
        __ASSERT_NOINTERRUPT();

        int8_t err = -1;
        if (timeout.value != 0) {
                /* queue thread to pending queue of the object */
                dlist_queue(waitqueue, &_current->wany);

                /* schedule thread wake up if timeout is set */
                _k_reschedule(timeout);

                k_yield();

                /* if timer expired, we manually remove the thread from
                 * the pending queue
                 */
                if (_current->timer_expired) {
                        dlist_remove(&_current->wany);
                        err = -ETIMEOUT;
                } else if (_current->pend_canceled) {
                        err =  -ECANCEL;
                } else {
                        err = 0;
                }
        }
        return err;
}

struct k_thread *_k_unpend_first_thread(struct ditem *waitqueue)
{
        __ASSERT_NOINTERRUPT();
        __ASSERT_NOTNULL(waitqueue);

        struct ditem *tie = dlist_dequeue(waitqueue);
        if (DITEM_VALID(waitqueue, tie)) {
                struct k_thread *pending_thread = THREAD_FROM_WAITQUEUE(tie);

                /* immediate wake up is not more required because
                 * with the swap model, the object is already reserved for the
                 * first pending thread
                 */
                _k_wake_up(pending_thread);
                
                /* we return !NULL if a pending thread got the object*/
                return pending_thread;
        }
        /* if no thread is pending on the object, we simply return */
        return NULL;
}

K_NOINLINE struct k_thread *_k_unpend_first_and_swap(struct ditem *waitqueue,
                                                     void *set_swap_data)
{
        __ASSERT_NOINTERRUPT();
        __ASSERT_NOTNULL(waitqueue);

        struct k_thread *pending_thread = _k_unpend_first_thread(waitqueue);
        if ((pending_thread != NULL) && (set_swap_data != NULL)) {
                pending_thread->swap_data = set_swap_data;
        }
        return pending_thread;
}

void _k_cancel_first_pending(struct ditem *waitqueue)
{
        __ASSERT_NOINTERRUPT();
        __ASSERT_NOTNULL(waitqueue);

        struct k_thread *pending_thread = _k_unpend_first_thread(waitqueue);
        if (pending_thread != NULL) {
                /* tells that we canceled the thread pending on the object */
                pending_thread->pend_canceled = 1;
        }
}

uint8_t _k_cancel_pending(struct ditem *waitqueue)
{
        __ASSERT_NOINTERRUPT();
        __ASSERT_NOTNULL(waitqueue);

        uint8_t count = 0;
        struct k_thread *pending_thread;
        for (;;) {
                pending_thread = _k_unpend_first_thread(waitqueue);
                if (pending_thread != NULL) {
                        pending_thread->pend_canceled = 1;

                        count++;
                } else {
                        return count;
                }
        }
}

/*___________________________________________________________________________*/