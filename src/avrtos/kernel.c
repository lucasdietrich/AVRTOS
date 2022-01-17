#include "kernel.h"

#include "idle.h"

#include <util/delay.h>
#include <util/atomic.h>

#include "debug.h"

/*___________________________________________________________________________*/

#define K_MODULE    K_MODULE_KERNEL

/*___________________________________________________________________________*/

/**
 * @brief Runqueue containing the queue of all ready threads.
 * Should never be NULL.
 */
struct ditem *_k_runqueue = &_k_thread_main.tie.runqueue;

struct titem *_k_events_queue = NULL;

static inline bool _k_runqueue_single(void)
{
        return _k_runqueue->next == _k_runqueue;
}

#if KERNEL_SCHEDULER_VARIABLE_FREQUENCY
uint8_t _k_sched_ticks_remaining = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_SCHEDULER_VARIABLE_FREQUENCY */

#if KERNEL_THREAD_MONITORING
uint8_t _k_sched_ticks_last = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_THREAD_MONITORING */

#if KERNEL_TICKS
// necessary ?
volatile union {
	uint8_t bytes[KERNEL_TICKS_SIZE];
	struct {
		uint32_t u32;

#if KERNEL_TICKS_40BITS
		uint8_t u40_byte;
#endif
	};

} _k_ticks = {
	.bytes = {
		0,
		0,
		0,
		0,
#if KERNEL_TICKS_40BITS == 5
		0,
#endif
	}
};
#endif /* KERNEL_TICKS */

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
                return (_current->sched_lock) == 1 || (_current->coop == 1);
        }

        __builtin_unreachable();
}


bool _k_preemptive(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return _current->coop == 0;
	}

	__builtin_unreachable();
}

bool _k_cooperative(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return _current->coop == 1;
	}

	__builtin_unreachable();
}

void k_sleep(k_timeout_t timeout)
{
	
        if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                        _k_reschedule(timeout);

                        _k_yield();
                }
        }
}

#if KERNEL_UPTIME
void k_wait(k_timeout_t timeout)
{
	__ASSERT_INTERRUPT();

	uint64_t ticks = k_ticks_get_64();
	uint64_t now;
	
	do {
		k_idle(); /* idle the thread */

		now = k_ticks_get_64();
	} while (now - ticks < K_TIMEOUT_TICKS(timeout));
}
#endif /* KERNEL_UPTIME */

void k_block(k_timeout_t timeout)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		_delay_ms(K_TIMEOUT_TICKS(timeout));
	}
}

/*___________________________________________________________________________*/

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

void _k_stop(void)
{
	__ASSERT_NOINTERRUPT();

	_k_suspend();

	_current->state = STOPPED;
}

void k_stop()
{
	cli();

	_k_stop();

	_k_yield();
}

/*___________________________________________________________________________*/

//
// Kernel Private API
//

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

extern struct k_thread _k_idle;

inline static void swap_endianness(void **addr)
{
	*addr = (void*) HTONS(*addr);
}

void _k_kernel_init(void)
{
	/* main thread is the first running (ready or not),
	 * and it is already in queue */
	for (uint8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
		struct k_thread *const thread = &(&__k_threads_start)[i];

		if (thread == &_k_thread_main) {
			continue;
		}

#if KERNEL_THREAD_IDLE
		const bool is_thread_idle = thread == &_k_idle;
#else
		const bool is_thread_idle = false;
#endif

		/* idle thread must not be added to the
		 * runqueue as the main thread is running */
		if (!is_thread_idle &&
		    (thread->state == READY)) {
			push_back(_k_runqueue, &thread->tie.runqueue);
		}

		/* Swap endianness of addresses in compilation-time built stacks.
		* We cannot change the endianness of addresses determined by the
		* linker at compilation time. So we need to do it on system start up
		*/
		struct _k_callsaved_ctx *ctx = thread->stack.end -
			sizeof(struct _k_callsaved_ctx) - _K_ARCH_PC_SIZE + 1u;
		swap_endianness(&ctx->thread_context);
		swap_endianness(&ctx->thread_entry);
		swap_endianness(thread->stack.end - _K_ARCH_PC_SIZE + 1u);
	}
}

void _k_queue(struct k_thread *const th)
{
        __ASSERT_NOINTERRUPT();

        push_back(_k_runqueue, &th->tie.runqueue);
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

	/* mark as "not in events queue" */
	thread->wakeup_schd = 0;

	/* EXPLAIN WHY THIS DISAPPEARED IN THE ORIGINAL CODE */
	thread->state = READY;
	
#if KERNEL_THREAD_IDLE
        if (k_is_cpu_idle()) {
                dlist_ref(&thread->tie.runqueue);
                _k_runqueue = &thread->tie.runqueue;
                return;
        }
#endif

        push_front(_k_runqueue, &thread->tie.runqueue);
}

void _k_schedule_wake_up(k_timeout_t timeout)
{
        __ASSERT_NOINTERRUPT();
	__ASSERT_TRUE(_current->wakeup_schd == 0);
	
        if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		_current->wakeup_schd = 1;
                _current->tie.event.timeout = K_TIMEOUT_TICKS(timeout);
                _current->tie.event.next = NULL;
                _tqueue_schedule(&_k_events_queue, &_current->tie.event);
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
                _k_runqueue = tie;
                return;
        }
#else
        __ASSERT_LEASTONE_RUNNING();
#endif

        pop_ref(&_k_runqueue);
}

void _k_system_shift(void)
{
	__ASSERT_NOINTERRUPT();
	__STATIC_ASSERT(KERNEL_TIME_SLICE_TICKS != 0);

	tqueue_shift(&_k_events_queue, KERNEL_TIME_SLICE_TICKS);

#if KERNEL_SCHEDULER_VARIABLE_FREQUENCY
	_k_sched_ticks_remaining = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_SCHEDULER_VARIABLE_FREQUENCY */

#if KERNEL_THREAD_MONITORING
#if KERNEL_SCHEDULER_VARIABLE_FREQUENCY
	_current->ticks += KERNEL_TIME_SLICE_TICKS - _k_sched_ticks_remaining;
	_k_sched_ticks_last = KERNEL_TIME_SLICE_TICKS;
#else
	_current->ticks += KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_SCHEDULER_VARIABLE_FREQUENCY */
#endif /* KERNEL_THREAD_MONITORING */

        struct ditem *const ready = (struct ditem *)tqueue_pop(&_k_events_queue);
        if (ready != NULL) {
		struct k_thread *const thread = THREAD_FROM_EVENTQUEUE(ready);

                __K_DBG_SCHED_EVENT(thread);  // !

                /* set ready thread expired flag */
                thread->timer_expired = 1u;

                _k_schedule(thread);
        }

#if KERNEL_TIMERS
	_k_timers_process();
#endif

#if KERNEL_EVENTS
	_k_event_q_process();
#endif /* KERNEL_EVENTS */
}

/* __attribute((naked)) */ struct k_thread *_k_scheduler(void)
{
        __ASSERT_NOINTERRUPT();

	struct k_thread *const prev = _current;

#if KERNEL_THREAD_MONITORING && KERNEL_SCHEDULER_VARIABLE_FREQUENCY
	/* as this function can be called from either 
	 * the sysclock handler or the yield function
	 * only add time that passed since last scheduling
	 */
	prev->ticks += _k_sched_ticks_last - _k_sched_ticks_remaining;
	_k_sched_ticks_last = _k_sched_ticks_remaining;
#endif /* KERNEL_THREAD_MONITORING */

        /* reset flags */
        prev->pend_canceled = 0;
        prev->timer_expired = 0;

        if (prev->state == PENDING) {
                /* runqueue is positionned to the 
                 * normally next thread to be executed */
                __K_DBG_SCHED_PENDING();        // ~
        } else {
                /* next thread is positionned at the top of the runqueue */
                ref_requeue(&_k_runqueue);

                __K_DBG_SCHED_REQUEUE();        // >
        }

        _current = CONTAINER_OF(_k_runqueue, struct k_thread, tie.runqueue);
	
	__ASSERT_THREAD_STATE(_current, READY);

        __K_DBG_SCHED_NEXT(_current);

	return prev;
}

void _k_wake_up(struct k_thread *th)
{
	__ASSERT_NOTNULL(th);
        __ASSERT_NOINTERRUPT();
        __ASSERT_THREAD_STATE(th, PENDING);

        __K_DBG_WAKEUP(th); // @

	/* Remove the thread from the events queue */
	if (th->wakeup_schd) {
		tqueue_remove(&_k_events_queue, &th->tie.event);
	}

        _k_schedule(th);
}

void _k_reschedule(k_timeout_t timeout)
{
        __ASSERT_NOINTERRUPT();

        _k_suspend();

        _k_schedule_wake_up(timeout);
}

/*___________________________________________________________________________*/

uint32_t k_uptime_get(void)
{
#if KERNEL_TICKS_40BITS
	return k_ticks_get_64() / K_TICKS_PER_SECOND;
#elif KERNEL_TICKS
	return k_ticks_get_32() / K_TICKS_PER_SECOND;
#else
	return 0;
#endif /* KERNEL_UPTIME */
}

uint32_t k_uptime_get_ms32(void)
{
#if KERNEL_TICKS
	return k_ticks_get_32() / K_TICKS_PER_MS;
#else
	return 0;
#endif /* KERNEL_UPTIME */
}

uint64_t k_uptime_get_ms64(void)
{
#if KERNEL_TICKS_40BITS
	return k_ticks_get_64() / K_TICKS_PER_MS;
#elif KERNEL_TICKS
	return k_ticks_get_32() / K_TICKS_PER_MS;
#else
	return 0;
#endif /* KERNEL_UPTIME */
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

                _k_yield();

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