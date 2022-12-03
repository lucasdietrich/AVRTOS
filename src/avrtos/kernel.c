/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "kernel_internals.h"
#include "kernel.h"

#include "idle.h"

#include <util/delay.h>
#include <util/atomic.h>

#include "debug.h"

/*___________________________________________________________________________*/

#define K_MODULE    K_MODULE_KERNEL

/*___________________________________________________________________________*/

void yield(void)
{
	k_yield();
}

/*___________________________________________________________________________*/

#if KERNEL_THREAD_IDLE
#	define THREAD_IS_IDLE(_thread) (_thread == &_k_idle)
#else
#	define THREAD_IS_IDLE(_thread) (0)
#endif

#define THREAD_IS_MAIN(_thread) (_thread == &_k_thread_main)

/**
 * @brief number of threads in the runqueue(s)
 * - 0: Only IDLE thread is running
 * - 1: A single thread is running
 * - n > 1: Multiple threads are running
 */
uint8_t _k_ready_count = 1u; /* On startup only main thread is running */

uint8_t k_ready_count(void)
{
	return _k_ready_count;
}

/**
 * @brief Runqueue containing the queue of all ready threads.
 * Should never be NULL.
 */
#if TREAD_PRIO_MULTIQ
dlist_t _k_runqs[4u] = {
	DLIST_INIT(_k_runqs[K_PRIO_HIGHEST]),
	DLIST_INIT(_k_runqs[K_PRIO_HIGH]),
	DLIST_INIT(_k_runqs[K_PRIO_LOW]),
	DLIST_INIT(_k_runqs[K_PRIO_LOWEST])
};
#define THREAD_GET_RUNQ(thread) (&_k_runqs[thread->priority])
#else
// struct ditem _k_runq = DLIST_INIT(_k_runq);
// #define THREAD_GET_RUNQ(thread) (&_k_runq)

struct ditem *_k_runq = &_k_thread_main.tie.runqueue;
#endif

struct titem *_k_events_queue = NULL;

#if KERNEL_TIME_SLICE_MULTIPLE_TICKS
uint8_t _k_sched_ticks_remaining = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_TIME_SLICE_MULTIPLE_TICKS */

#if KERNEL_TICKS
// necessary ?
union {
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
// Kernel Private API
//

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

extern struct k_thread _k_idle;

/**
 * @brief Schedule the thread to be executed.
 * If the IDLE thread is in the runqueue (it is removed), the scheduled thread become the only thread in the runqueue.
 * Thread is added to the top of the runqueue.
 * - Assume that the thread is K_READY
 * - Assume that the thread is not in the runqueue
 *
 * @param thread_tie thread.tie.runqueue item
 * @return __attribute__((noinline))
 */
static K_NOINLINE void _k_schedule(struct k_thread *thread)
{
	__ASSERT_NOINTERRUPT();

#if THREAD_STACK_SENTINEL && KERNEL_ASSERT
	/* check that stack sentinel is still valid before switching to thread */
	if (k_verify_stack_sentinel(thread) == false) {
		__fault(K_FAULT_SENTINEL);
	}
#endif

	/* Mark this thread as READY */
	thread->state = K_READY;

	if (_k_ready_count == 0) {
		/* Resume from IDLE */
		dlist_init(&thread->tie.runqueue);

		_k_runq = &thread->tie.runqueue;
	} else {
		/* Wokek up threads should be executed before any 
		 * already running premptive thread, so prepend
		 *
		 * Call k_yield_from_isr_cond() to switch to woke up thread
		 * if called from ISR
		 */
		dlist_prepend(_k_runq, &thread->tie.runqueue);
	}
	
	_k_ready_count++;
}

/**
 * @brief Do a thread switch (ASM function)
 * 1. save context of the first thread
 * 2. store the SP of the first thread to its structure
 * 3. restore the SP of the second thread from its structure
 * 4. restore context of the second thread
 *
 * @param from
 * @param to
 */
extern void _k_thread_switch(struct k_thread *from,
			     struct k_thread *to);

/**
 * @brief Schedule current thread wake up.
 *
 * Assumptions:
 * - thread is suspended (K_PENDING)
 * - thread is not in the runqueue
 *
 * @param thread
 * @param timeout
 * @return K_NOINLINE
 */
static K_NOINLINE void _k_schedule_wake_up(k_timeout_t timeout)
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

/**
 * @brief Remove the current thread from the runqueue.
 * Stop the execution of the current thread (until it is scheduled again with function _k_schedule or _k_schedule_wake_up)
 * State flag is changed to K_PENDING.
 *
 * Assumptions:
 * - interrupt flag is cleared when called.
 * - thread is in the runqueue
 */
static K_NOINLINE void _k_suspend(void)
{
	__ASSERT_NOINTERRUPT();

	/* Illegal to suspend the IDLE thread, handled by kernel */
	__ASSERT_TRUE(&_current->tie.runqueue == _k_runq);

	/* Mark this thread as pending */
	_current->state = K_PENDING;

	/* Remove thread from runqueue */
	dlist_remove(_k_runq);

	/* Reference next thread to be executed */
	_k_runq = _current->tie.runqueue.next;

	/* Decrement number of threads in the runqueue */
	_k_ready_count--;

	if (_k_ready_count == 0) {
#if KERNEL_THREAD_IDLE == 0u
		/* Assert*/
		__ASSERT_LEASTONE_RUNNING();

		/* If IDLE thread is not enabled, then fault */
		__fault(K_FAULT_KERNEL_HALT);
#else
		/* Switch to IDLE thread */
		_k_runq = &_k_idle.tie.runqueue;
#endif
	}

	__K_DBG_SCHED_SUSPENDED(_current);
}

/**
 * @brief Wake up a thread that is pending for an event.
 *
 * Assumptions:
 *  - thread is in K_PENDING mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 *  - interrupt flag is cleared when called.
 *
 *
 * @param th thread to wake up
 */
K_NOINLINE void _k_wake_up(struct k_thread *th)
{
	__ASSERT_NOTNULL(th);
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_STATE(th, K_PENDING);

	__K_DBG_WAKEUP(th); // @

	/* Remove the thread from the events queue */
	if (th->wakeup_schd) {
		tqueue_remove(&_k_events_queue, &th->tie.event);
		th->wakeup_schd = 0;
	}

	_k_schedule(th);
}

inline static void swap_endianness(void **addr)
{
	*addr = (void *)HTONS(*addr);
}

/**
 * @brief Initialize the runqueue with all threads ready to be executed.
 * Assume that the interrupt flag is cleared when called.
 */
void _k_kernel_init(void)
{
#if KERNEL_THREAD_IDLE
	/* Mark idle thread */
	_k_idle.state = K_IDLE;
#endif

	/* main thread is the first running (ready or not),
	 * and it is already in queue */
	for (uint8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
		struct k_thread *const thread = &(&__k_threads_start)[i];

		/* Main thread already in queue */
		if (THREAD_IS_MAIN(thread)) {
			continue;
		}

		/* idle thread must not be added to the
		 * runqueue as the main thread is running */
		if (!THREAD_IS_IDLE(thread) && (thread->state == K_READY)) {
			_k_ready_count++;
			dlist_append(_k_runq, &thread->tie.runqueue);
		}

		/* Swap endianness of addresses in compilation-time built stacks.
		* We cannot change the endianness of addresses determined by the
		* linker at compilation time. So we need to do it here.
		*/
		struct _k_callsaved_ctx *ctx = thread->stack.end -
			_K_CALLSAVED_CTX_SIZE + 1u;
		swap_endianness(&ctx->thread_context);
		swap_endianness((void *)&ctx->thread_entry);
		swap_endianness(&ctx->pc);

#if defined(__AVR_3_BYTE_PC__)
		ctx->pch = 0;
#endif /* __AVR_3_BYTE_PC__ */
	}
}

/* If KERNEL_TIME_SLICE_MULTIPLE_TICKS is enabled and we are in the 
 * IDLE thread. The expired thread will be rescheduled only after
 * the current time slice interval finishes. So we lose few ticks in the 
 * IDLE thread.
 * 
 * TODO Improve this by checking for expired threads more often when
 *  we are in the IDLE thread.
 */
void _k_system_shift(void)
{

	__ASSERT_NOINTERRUPT();
	__STATIC_ASSERT_AUTOMSG(KERNEL_TIME_SLICE_TICKS != 0);

	tqueue_shift(&_k_events_queue, KERNEL_TIME_SLICE_TICKS);

#if KERNEL_TIME_SLICE_MULTIPLE_TICKS
	_k_sched_ticks_remaining = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_TIME_SLICE_MULTIPLE_TICKS */

	struct titem *ready;
	while ((ready = tqueue_pop(&_k_events_queue)) != NULL) {
		struct k_thread *const thread = THREAD_FROM_EVENTQUEUE(ready);

		__K_DBG_SCHED_EVENT(thread);  // !

		/* set ready thread expired flag */
		thread->timer_expired = 1u;
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


/**
 * @brief Choose the next thread to be executed.
 * This function is called during any thread switch in order to determine which
 * one is the following thread to be executed.
 *
 * This function is called in k_yield function
 *
 * @return struct k_thread* : next thread to be executed
 */
struct k_thread *_k_scheduler(void)
{
	__ASSERT_NOINTERRUPT();

#if THREAD_STACK_SENTINEL && KERNEL_ASSERT
	k_assert_registered_stack_sentinel();
#endif

	struct k_thread *const prev = _current;

	/* reset flags */
	prev->pend_canceled = 0;
	prev->timer_expired = 0;

	/* If previous thread put itself in pending state,
	 * it already removed itself from the runqueue, so we don't need
	 * to do it here 
	 */
	if (_current->state == K_READY) {
		/* Rotate the runqueue, set next thread to first position */
		_k_runq = _k_runq->next;
	}

	/* Fetch next thread to execute */
	_current = CONTAINER_OF(_k_runq, struct k_thread, tie.runqueue);

	__K_DBG_SCHED_NEXT_THREAD();
	__K_DBG_SCHED_NEXT(_current);

	return prev;
}

K_NOINLINE int8_t _k_pend_current(struct ditem *waitqueue,
				  k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();

	/* In case of returning without waiting */
	int8_t err = -EBUSY;
	if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {

		/* queue thread to pending queue of the object */
		dlist_append(waitqueue, &_current->wany);

		/* Suspend the thread */
		_k_suspend();

		/* schedule thread wake up if timeout is set */
		_k_schedule_wake_up(timeout);

		/* Call scheduler */
		_k_yield();

		/* if timer expired, we manually remove the thread from
		 * the pending queue
		 */
		if (_current->timer_expired) {
			dlist_remove(&_current->wany);
			err = -ETIMEDOUT;
		} else if (_current->pend_canceled) {
			err = -ECANCELED;
		} else {
			err = 0;
		}
	}
	return err;
}

K_NOINLINE struct k_thread *_k_unpend_first_thread(struct ditem *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct ditem *tie = dlist_get(waitqueue);
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

K_NOINLINE void _k_cancel_first_pending(struct ditem *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = _k_unpend_first_thread(waitqueue);
	if (pending_thread != NULL) {
		/* tells that we canceled the thread pending on the object */
		pending_thread->pend_canceled = 1;
	}
}

K_NOINLINE uint8_t _k_cancel_all_pending(struct ditem *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	uint8_t count = 0;
	struct k_thread *pending_thread;

	while ((pending_thread = _k_unpend_first_thread(waitqueue)) != NULL) {
		pending_thread->pend_canceled = 1;
		count++;
	}

	return count;
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

static uint8_t _cur_get_flags(void)
{
	return _current->flags;
}

bool k_sched_locked(void)
{
	const uint8_t flags = _cur_get_flags();

	return (flags & (K_FLAG_SCHED_LOCKED | K_FLAG_COOP)) != 0;
}


bool k_cur_is_preempt(void)
{
	return (_cur_get_flags() & K_FLAG_COOP) == 0;
}

bool k_cur_is_coop(void)
{
	return (_cur_get_flags() & K_FLAG_COOP) != 0;
}

void k_sleep(k_timeout_t timeout)
{
	if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			/* Suspend the thread */
			_k_suspend();

			/* schedule thread wake up if timeout is set */
			_k_schedule_wake_up(timeout);

			/* Call scheduler */
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
		k_idle(); /* idle the thread until next interrupt */

		now = k_ticks_get_64();
	} while (now - ticks < K_TIMEOUT_TICKS(timeout));
}
#endif /* KERNEL_UPTIME */

void k_block(k_timeout_t timeout)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		k_ticks_t ticks = K_TIMEOUT_TICKS(timeout);
		while (ticks != 0) {
			_delay_us(K_TICKS_US);
			ticks--;
		}
	}
}

void k_suspend(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		_k_suspend();
	}
}

void k_resume(struct k_thread *th)
{
	if (th->state == K_PENDING) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			_k_schedule(th);
		}
	} else {
		/* thread pending, ready of running and then already started */
	}
}

void k_thread_start(struct k_thread *th)
{
	if (th->state == K_STOPPED) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			_k_schedule(th);
		}
	}
}

/**
 * @brief Stop the thread
 *
 * @param th : ready/pending thread to start.
 */
static K_NOINLINE void _k_stop(void)
{
	__ASSERT_NOINTERRUPT();

	_k_suspend();

	_current->state = K_STOPPED;
}

void k_stop()
{
	cli();

	_k_stop();

	_k_yield();
}

/*___________________________________________________________________________*/

#if KERNEL_UPTIME

uint32_t k_uptime_get(void)
{
#if KERNEL_TICKS_40BITS
	return k_ticks_get_64() / K_TICKS_PER_SECOND;
#else
	return k_ticks_get_32() / K_TICKS_PER_SECOND;
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

#endif /* KERNEL_UPTIME */

/*___________________________________________________________________________*/