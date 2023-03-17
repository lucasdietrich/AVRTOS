/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"
#include "idle.h"
#include "kernel.h"
#include "kernel_internals.h"

#include <util/atomic.h>
#include <util/delay.h>

/*___________________________________________________________________________*/

#define K_MODULE K_MODULE_KERNEL

/*___________________________________________________________________________*/

void yield(void)
{
	k_yield();
}

/*___________________________________________________________________________*/

static inline void z_set_thread_state(struct k_thread *thread, uint8_t state)
{
	thread->flags =
		(thread->flags & ~Z_THREAD_STATE_MSK) | (state & Z_THREAD_STATE_MSK);
}

static inline uint8_t z_get_thread_state(struct k_thread *thread)
{
	return thread->flags & Z_THREAD_STATE_MSK;
}

#if CONFIG_KERNEL_THREAD_IDLE
#define THREAD_IS_IDLE(_thread) (_thread == &z_thread_idle)
#else
#define THREAD_IS_IDLE(_thread) (0)
#endif

#define THREAD_IS_MAIN(_thread) (_thread == &z_thread_main)

/**
 * @brief number of threads in the runqueue(s)
 * - 0: Only IDLE thread is running
 * - 1: A single thread is running
 * - n > 1: Multiple threads are running
 */
uint8_t z_ready_count = 1u; /* On startup only main thread is running */

uint8_t k_ready_count(void)
{
	return z_ready_count;
}

/**
 * @brief Runqueue containing the queue of all ready threads.
 * Should never be NULL.
 */
#if CONFIG_TREAD_PRIO_MULTIQ
#error "CONFIG_TREAD_PRIO_MULTIQ not supported yet"
dlist_t z_runqs[4u] = {DLIST_INIT(z_runqs[K_PRIO_HIGHEST]),
		       DLIST_INIT(z_runqs[K_PRIO_HIGH]),
		       DLIST_INIT(z_runqs[K_PRIO_LOW]),
		       DLIST_INIT(z_runqs[K_PRIO_LOWEST])};
#define THREAD_GET_RUNQ(thread) (&z_runqs[thread->priority])
#else
// struct dnode z_runq = DLIST_INIT(z_runq);
// #define THREAD_GET_RUNQ(thread) (&z_runq)

struct dnode *z_runq = &z_thread_main.tie.runqueue;
#endif

struct titem *z_events_queue = NULL;

#if CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS
uint8_t z_sched_ticks_remaining = CONFIG_KERNEL_TIME_SLICE_TICKS;
#endif /* CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS */

#if CONFIG_KERNEL_TICKS_COUNTER
// necessary ?
union {
	uint8_t bytes[CONFIG_KERNEL_TICKS_COUNTER_SIZE];
	struct {
		uint32_t u32;

#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
		uint8_t u40_byte;
#endif
	};

} z_ticks = {.bytes = {
		     0,
		     0,
		     0,
		     0,
#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS == 5
		     0,
#endif
	     }};
#endif /* CONFIG_KERNEL_TICKS_COUNTER */

/*___________________________________________________________________________*/

//
// Kernel Private API
//

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

extern struct k_thread z_thread_idle;

/**
 * @brief Schedule the thread to be executed.
 * If the IDLE thread is in the runqueue (it is removed), the scheduled thread
 * become the only thread in the runqueue. Thread is added to the top of the
 * runqueue.
 * - Assume that the thread is Z_READY
 * - Assume that the thread is not in the runqueue
 *
 * @param thread_tie thread.tie.runqueue item
 * @return __attribute__((noinline))
 */
static __kernel void z_schedule(struct k_thread *thread)
{
	__ASSERT_NOINTERRUPT();

#if CONFIG_THREAD_STACK_SENTINEL && CONFIG_KERNEL_ASSERT
	/* check that stack sentinel is still valid before switching to thread
	 */
	if (k_verify_stack_sentinel(thread) == false) {
		__fault(K_FAULT_SENTINEL);
	}
#endif

	/* Mark this thread as READY */
	z_set_thread_state(thread, Z_THREAD_STATE_READY);

	if (z_ready_count == 0) {
		/* Resume from IDLE */
		dlist_init(&thread->tie.runqueue);

		z_runq = &thread->tie.runqueue;
	} else {
		/* Wokek up threads should be executed before any
		 * already running premptive thread, so prepend
		 *
		 * Call k_yield_from_isr_cond() to switch to woke up thread
		 * if called from ISR
		 */
		dlist_prepend(z_runq, &thread->tie.runqueue);
	}

	z_ready_count++;
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
extern void z_thread_switch(struct k_thread *from, struct k_thread *to);

/**
 * @brief Schedule current thread wake up.
 *
 * Assumptions:
 * - thread is suspended (Z_PENDING)
 * - thread is not in the runqueue
 *
 * @param thread
 * @param timeout
 */
static __kernel void z_schedule_wake_up(k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_TRUE((z_current->flags & Z_THREAD_WAKEUP_SCHED_MSK) == 0);

	if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		z_current->flags |= Z_THREAD_WAKEUP_SCHED_MSK;
		z_current->tie.event.timeout = K_TIMEOUT_TICKS(timeout);
		z_current->tie.event.next    = NULL;
		z_tqueue_schedule(&z_events_queue, &z_current->tie.event);
	}
}

/**
 * @brief Cancel the scheduled wake up of a thread (if any).
 *
 * @param thread
 */
static void z_cancel_scheduled_wake_up(struct k_thread *thread)
{
	/* Remove the thread from the events queue */
	if (thread->flags & Z_THREAD_WAKEUP_SCHED_MSK) {
		thread->flags &= ~Z_THREAD_WAKEUP_SCHED_MSK;
		tqueue_remove(&z_events_queue, &thread->tie.event);
	}
}

/**
 * @brief Wake up a thread that is pending for an event.
 *
 * Assumptions:
 *  - thread is in Z_PENDING mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 *  - interrupt flag is cleared when called.
 *
 *
 * @param thread thread to wake up
 */
__kernel void z_wake_up(struct k_thread *thread)
{
	__ASSERT_NOTNULL(thread);
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_STATE(thread, Z_THREAD_STATE_PENDING);

	__K_DBG_WAKEUP(thread); // @

	z_cancel_scheduled_wake_up(thread);

	z_schedule(thread);
}

inline static void swap_endianness(void **addr)
{
	*addr = (void *)HTONS(*addr);
}

static inline void z_thread_finalize_stack_init(struct k_thread *const thread)
{
	/* Swap endianness of addresses in compilation-time built
	 * stacks. We cannot change the endianness of addresses
	 * determined by the linker at compilation time. So we need to
	 * do it here.
	 */
	struct z_callsaved_ctx *ctx = thread->stack.end - Z_CALLSAVED_CTX_SIZE + 1u;
	swap_endianness(&ctx->thread_context);
	swap_endianness((void *)&ctx->thread_entry);
	swap_endianness(&ctx->pc);

#if defined(__AVR_3_BYTE_PC__)
	ctx->pch = 0;
#endif /* __AVR_3_BYTE_PC__ */
}

extern void z_thread_idle_create(void);

/**
 * @brief Initialize the runqueue with all threads ready to be executed.
 * Assume that the interrupt flag is cleared when called.
 */
void z_kernel_init(void)
{
#if CONFIG_KERNEL_THREAD_IDLE
	z_thread_idle_create();

	/* Mark idle thread */
	z_set_thread_state(&z_thread_idle, Z_THREAD_STATE_IDLE);
#endif

#if CONFIG_AVRTOS_LINKER_SCRIPT

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
		if (!THREAD_IS_IDLE(thread) &&
		    (z_get_thread_state(thread) == Z_THREAD_STATE_READY)) {
			z_ready_count++;
			dlist_append(z_runq, &thread->tie.runqueue);
		}

		z_thread_finalize_stack_init(thread);
	}
#endif
}

__STATIC_ASSERT_NOMSG(CONFIG_KERNEL_TIME_SLICE_TICKS != 0);

/* If CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS is enabled and we are in the
 * IDLE thread. The expired thread will be rescheduled only after
 * the current time slice interval finishes. So we lose few ticks in the
 * IDLE thread.
 *
 * TODO Improve this by checking for expired threads more often when
 *  we are in the IDLE thread.
 */
void z_system_shift(void)
{
	__ASSERT_NOINTERRUPT();

	tqueue_shift(&z_events_queue, CONFIG_KERNEL_TIME_SLICE_TICKS);

#if CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS
	z_sched_ticks_remaining = CONFIG_KERNEL_TIME_SLICE_TICKS;
#endif /* CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS */

	struct titem *ready;
	while ((ready = tqueue_pop(&z_events_queue)) != NULL) {
		struct k_thread *const thread = Z_THREAD_FROM_EVENTQUEUE(ready);

		__K_DBG_SCHED_EVENT(thread); // !

		/* set ready thread expired flag */
		thread->flags |= Z_THREAD_TIMER_EXPIRED_MSK;
		thread->flags &= ~Z_THREAD_WAKEUP_SCHED_MSK;

		z_schedule(thread);
	}

#if CONFIG_KERNEL_TIMERS
	z_timers_process();
#endif

#if CONFIG_KERNEL_EVENTS
	z_event_q_process();
#endif /* CONFIG_KERNEL_EVENTS */
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
struct k_thread *z_scheduler(void)
{
	__ASSERT_NOINTERRUPT();

#if CONFIG_THREAD_STACK_SENTINEL && CONFIG_KERNEL_ASSERT
	k_assert_registered_stack_sentinel();
#endif

	struct k_thread *const prev = z_current;

	/* reset flags */
	prev->flags &= ~(Z_THREAD_TIMER_EXPIRED_MSK | Z_THREAD_PEND_CANCELED_MSK);

	/* If previous thread put itself in pending state,
	 * it already removed itself from the runqueue, so we don't need
	 * to do it here
	 */
	if (z_get_thread_state(z_current) == Z_THREAD_STATE_READY) {
		/* Rotate the runqueue, set next thread to first position */
		z_runq = z_runq->next;
	}

	/* Fetch next thread to execute */
	z_current = CONTAINER_OF(z_runq, struct k_thread, tie.runqueue);

	__K_DBG_SCHED_NEXT_THREAD();
	__K_DBG_SCHED_NEXT(z_current);

	return prev;
}

/**
 * @brief Generic function to suspend a thread.
 *
 * - If thread is PENDING, it is removed from the events queue.
 * - If thread is READY, it is removed from the runqueue.
 * 	- If thread is the current thread, the runqueue is prepared so that the
 * 	  normally next thread is executed.
 *
 * @param thread
 */
static void z_suspend(struct k_thread *thread)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_NOT_STATE(thread, Z_THREAD_STATE_STOPPED);
	__ASSERT_THREAD_NOT_STATE(thread, Z_THREAD_STATE_IDLE);

	if (z_get_thread_state(thread) == Z_THREAD_STATE_PENDING) { /* Z_PENDING */
		z_cancel_scheduled_wake_up(thread);
	} else {
		/* Remove thread from runqueue */
		dlist_remove(&thread->tie.runqueue);

		/* Decrement number of threads in the runqueue */
		z_ready_count--;

		/* Check if there is at least one thread running */
		if (z_ready_count == 0) {
#if CONFIG_KERNEL_THREAD_IDLE == 0u
			/* Assert*/
			__ASSERT_LEASTONE_RUNNING();

			/* If IDLE thread is not enabled, then fault */
			__fault(K_FAULT_KERNEL_HALT);
#else
			/* Switch to IDLE thread */
			z_runq = &z_thread_idle.tie.runqueue;
#endif
		} else if (thread == z_current) {
			/* Set runq pointer so that it points to the next thread to be
			 * executed */
			z_runq = thread->tie.runqueue.next->prev;
		}

		__K_DBG_SCHED_SUSPENDED(z_current);
	}
}

__kernel int8_t z_pend_current(struct dnode *waitqueue, k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();

	/* In case of returning without waiting */
	int8_t err = -EBUSY;
	if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {

		/* queue thread to pending queue of the object */
		dlist_append(waitqueue, &z_current->wany);

		/* Suspend the thread */
		z_suspend(z_current);

		/* schedule thread wake up if timeout is set */
		z_schedule_wake_up(timeout);

		/* Mark this thread as pending */
		z_set_thread_state(z_current, Z_THREAD_STATE_PENDING);

		/* Call scheduler */
		z_yield();

		/* if timer expired, we manually remove the thread from
		 * the pending queue
		 */
		if (z_current->flags & Z_THREAD_TIMER_EXPIRED_MSK) {
			dlist_remove(&z_current->wany);
			err = -ETIMEDOUT;
		} else if (z_current->flags & Z_THREAD_PEND_CANCELED_MSK) {
			err = -ECANCELED;
		} else {
			err = 0;
		}
	}
	return err;
}

__kernel struct k_thread *z_unpend_first_thread(struct dnode *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct dnode *tie = dlist_get(waitqueue);
	if (DITEM_VALID(waitqueue, tie)) {
		struct k_thread *pending_thread = Z_THREAD_FROM_WAITQUEUE(tie);

		/* immediate wake up is not more required because
		 * with the swap model, the object is already reserved for the
		 * first pending thread
		 */
		z_wake_up(pending_thread);

		/* we return !NULL if a pending thread got the object*/
		return pending_thread;
	}
	/* if no thread is pending on the object, we simply return */
	return NULL;
}

__kernel struct k_thread *z_unpend_first_and_swap(struct dnode *waitqueue,
						  void *set_swap_data)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = z_unpend_first_thread(waitqueue);
	if ((pending_thread != NULL) && (set_swap_data != NULL)) {
		pending_thread->swap_data = set_swap_data;
	}
	return pending_thread;
}

__kernel void z_cancel_first_pending(struct dnode *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = z_unpend_first_thread(waitqueue);
	if (pending_thread != NULL) {
		/* tells that we canceled the thread pending on the object */
		pending_thread->flags |= Z_THREAD_PEND_CANCELED_MSK;
	}
}

__kernel uint8_t z_cancel_all_pending(struct dnode *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	uint8_t count = 0;
	struct k_thread *pending_thread;

	while ((pending_thread = z_unpend_first_thread(waitqueue)) != NULL) {
		pending_thread->flags |= Z_THREAD_PEND_CANCELED_MSK;
		count++;
	}

	return count;
}

/*___________________________________________________________________________*/

//
// Kernel Public API
//

#if CONFIG_KERNEL_IRQ_LOCK_COUNTER
void irq_disable(void)
{
	cli();

	z_current->irq_lock_cnt++;
}

void irq_enable(void)
{
	if (z_current->irq_lock_cnt == 0) {
		return;
	} else if (z_current->irq_lock_cnt == 1) {
		z_current->irq_lock_cnt = 0;
	} else {
		z_current->irq_lock_cnt--;
		return;
	}

	sei();
}
#endif

void k_sched_lock(void)
{
	const uint8_t key = irq_lock();

	z_current->flags |= Z_THREAD_SCHED_LOCKED_MSK;

	irq_unlock(key);

#if CONFIG_KERNEL_SCHED_LOCK_COUNTER
	z_current->sched_lock_cnt++;
#endif /* CONFIG_KERNEL_SCHED_LOCK_COUNTER */

	__K_DBG_SCHED_LOCK(z_current);
}

void k_sched_unlock(void)
{
#if CONFIG_KERNEL_SCHED_LOCK_COUNTER
	if (z_current->sched_lock_cnt == 0) {
		return;
	} else if (z_current->sched_lock_cnt == 1) {
		z_current->sched_lock_cnt = 0;
	} else {
		z_current->sched_lock_cnt--;
		return;
	}
#endif /* CONFIG_KERNEL_SCHED_LOCK_COUNTER */

	const uint8_t key = irq_lock();
	z_current->flags &= ~Z_THREAD_SCHED_LOCKED_MSK;
	irq_unlock(key);

	__K_DBG_SCHED_UNLOCK();
}

static inline uint8_t z_current_flags_get(void)
{
	return z_current->flags;
}

bool k_sched_locked(void)
{
	const uint8_t flags = z_current_flags_get();

	return (flags & (Z_THREAD_SCHED_LOCKED_MSK | Z_THREAD_PRIO_COOP)) != 0;
}

bool k_cur_is_preempt(void)
{
	return (z_current_flags_get() & Z_THREAD_PRIO_COOP) == 0;
}

bool k_cur_is_coop(void)
{
	return (z_current_flags_get() & Z_THREAD_PRIO_COOP) != 0;
}

void k_sleep(k_timeout_t timeout)
{
	if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		const uint8_t key = irq_lock();

		/* Suspend the thread */
		z_suspend(z_current);

		/* schedule thread wake up if timeout is set */
		z_schedule_wake_up(timeout);

		/* Mark this thread as pending */
		z_set_thread_state(z_current, Z_THREAD_STATE_PENDING);

		/* Call scheduler */
		z_yield();

		irq_unlock(key);
	}
}

#if CONFIG_KERNEL_UPTIME
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
#endif /* CONFIG_KERNEL_UPTIME */

void k_block(k_timeout_t timeout)
{
	const uint8_t key = irq_lock();

	k_ticks_t ticks = K_TIMEOUT_TICKS(timeout);
	while (ticks != 0) {
		_delay_us(K_TICKS_US);
		ticks--;
	}

	irq_unlock(key);
}

int8_t k_thread_start(struct k_thread *thread)
{
	int8_t ret = -EAGAIN;

	if (z_get_thread_state(thread) == Z_THREAD_STATE_STOPPED) {
		const uint8_t key = irq_lock();

		z_schedule(thread);

		irq_unlock(key);

		ret = 0;
	}

	return ret;
}

__kernel int8_t k_thread_stop(struct k_thread *thread)
{
	if (z_get_thread_state(thread) == Z_THREAD_STATE_STOPPED) return -EINVAL;

#if CONFIG_KERNEL_THREAD_IDLE
	if (z_get_thread_state(thread) == Z_THREAD_STATE_IDLE) return -EINVAL;
#endif

	const uint8_t key = irq_lock();

	z_suspend(thread);

	z_set_thread_state(thread, Z_THREAD_STATE_STOPPED);

	if (thread == z_current)
		z_yield();
	else
		irq_unlock(key);

	return 0;
}

void k_stop()
{
	k_thread_stop(z_current);
}

/*___________________________________________________________________________*/

#if CONFIG_KERNEL_UPTIME

uint32_t k_uptime_get(void)
{
#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
	return k_ticks_get_64() / K_TICKS_PER_SECOND;
#else
	return k_ticks_get_32() / K_TICKS_PER_SECOND;
	return 0;
#endif /* CONFIG_KERNEL_UPTIME */
}

uint32_t k_uptime_get_ms32(void)
{
#if CONFIG_KERNEL_TICKS_COUNTER
	return k_ticks_get_32() / K_TICKS_PER_MS;
#else
	return 0;
#endif /* CONFIG_KERNEL_UPTIME */
}

uint64_t k_uptime_get_ms64(void)
{
#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
	return k_ticks_get_64() / K_TICKS_PER_MS;
#elif CONFIG_KERNEL_TICKS_COUNTER
	return k_ticks_get_32() / K_TICKS_PER_MS;
#else
	return 0;
#endif /* CONFIG_KERNEL_UPTIME */
}

#endif /* CONFIG_KERNEL_UPTIME */

/*___________________________________________________________________________*/