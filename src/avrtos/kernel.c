/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"
#include "idle.h"
#include "kernel.h"
#include "kernel_internals.h"

#include <avr/sleep.h>
#include <util/atomic.h>
#include <util/delay.h>

#define K_MODULE K_MODULE_KERNEL

void yield(void)
{
	k_yield();
}

/**
 * @brief Number of threads in the runqueue(s).
 *
 * This variable represents the count of threads currently in the runqueue(s),
 * indicating the number of threads that are ready to run. It is initialized to 1
 * during startup, indicating that only the main thread is running initially.
 *
 * The possible values and their meanings are as follows:
 * - 0: Indicates that only the IDLE thread is running.
 * - 1: Indicates that a single thread, typically the main thread, is running.
 * - n > 1: Indicates that multiple threads are running.
 */
uint8_t z_ready_count = 1u;

/**
 * @brief Get the count of threads in the runqueue(s).
 *
 * This function returns the value of z_ready_count, which represents the count
 * of threads currently in the runqueue(s). The returned value provides information
 * about the number of threads that are ready to run.
 *
 * @return The count of threads in the runqueue(s).
 */
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
		       DLIST_INIT(z_runqs[K_PRIO_HIGH]), DLIST_INIT(z_runqs[K_PRIO_LOW]),
		       DLIST_INIT(z_runqs[K_PRIO_LOWEST])};
#define THREAD_GET_RUNQ(thread) (&z_runqs[thread->priority])
#else
extern struct k_thread z_thread_main;
/**
 * @brief Pointer to the currently running thread in the runqueue.
 *
 * This variable represents a pointer to the thread (node) in the runqueue, which is
 * currently running. The runqueue is implemented as a doubly-linked list, where each
 * node represents a thread. During startup, this variable is initialized with the
 * pointer to the node corresponding to the main thread.
 *
 * The runqueue is structured as a circular doubly-linked list, where each node contains
 * a pointer to the next and previous nodes. The illustration below demonstrates the
 * relationship between the nodes in the runqueue:
 *
 *         A     B     C
 *     -> n --> n --> n --   (n for next, p for previous)
 *     -- p <-- p <-- p <-
 *             /\
 *  ___________|
 *  z_runq points to the thread currently running, here B
 *
 * By maintaining a pointer to the currently running thread (node) in z_runq, it becomes
 * convenient to execute the next thread by simply setting z_runq to z_runq->next.
 */
struct dnode *z_runq = &z_thread_main.tie.runqueue;
#endif

/**
 * @brief Pointer to the head of the timeouts queue.
 *
 * This variable represents a pointer to the head of the timeouts queue. The timeouts
 * queue is a linked list that holds the pending threads in the system. Each thread in the
 * list corresponds to a timeout event and is represented by a `struct titem`.
 */
struct titem *z_timeouts_queue = NULL;

#if CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS
/**
 * @brief Number of ticks remaining in the current time slice.
 *
 * This variable represents the number of ticks remaining in the current time slice.
 * A time slice is a predetermined time duration allocated to each thread for execution
 * before being preempted and giving control to another thread.
 *
 * The variable `z_sched_ticks_remaining` is initialized with the value specified in
 * `CONFIG_KERNEL_TIME_SLICE_TICKS`, which indicates the length of the time slice in
 * ticks. As the thread runs and consumes ticks, the value of this variable decreases.
 * When it reaches zero, the thread may be preempted, and control is given to another
 * thread.
 */
uint8_t z_sched_ticks_remaining = CONFIG_KERNEL_TIME_SLICE_TICKS;
#endif /* CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS */

#if CONFIG_KERNEL_ASSERT
/**
 * @brief Flag indicating execution context.
 *
 * This flag is used to indicate whether the kernel code is currently being executed.
 * Its purpose is to detect if a user handler is being executed from a kernel context,
 * such as within kernel timer or kernel event handlers.
 *
 * The variable `z_kernel_mode` is initially set to 0, indicating that the kernel code
 * is not being executed. When the kernel code is entered, this flag is typically set
 * to a non-zero value, indicating that the code is now running in the kernel context.
 */
uint8_t z_kernel_mode = 0u;
#endif

#if CONFIG_KERNEL_TICKS_COUNTER
/**
 * @brief Ticks counter for the kernel.
 *
 * This array represents the ticks counter used by the kernel to keep track of time.
 * The counter is implemented as an array of bytes with a specific size determined by
 * `CONFIG_KERNEL_TICKS_COUNTER_SIZE`.
 *
 * The `z_ticks` array is initialized with all elements set to 0 during startup.
 */
uint8_t z_ticks[CONFIG_KERNEL_TICKS_COUNTER_SIZE] = {0u};
#endif /* CONFIG_KERNEL_TICKS_COUNTER */

//
// Kernel Private API
//

/**
 * @brief Start and end pointers for the kernel thread array.
 *
 * These symbols are defined in linker script if CONFIG_AVRTOS_LINKER_SCRIPT is set.
 *
 * These external symbols represent the start and end pointers of the kernel thread array.
 * The kernel thread array is a collection of `struct k_thread` instances that represent
 * the threads managed by the kernel.
 *
 * The symbol `__k_threads_start` points to the beginning of the kernel thread array, and
 * the symbol `__k_threads_end` points to the end of the kernel thread array. The range
 * between these two pointers inclusively represents all the threads in the kernel.
 */
extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

/**
 * @brief Idle thread.
 *
 * This external symbol represents the idle thread in the system. The idle thread is a
 * special system thread that runs when no other threads are eligible for execution.
 * It performs idle-time processing and waits for other threads to become ready.
 */
extern struct k_thread z_thread_idle;

/**
 * @brief Schedule the thread to be executed.
 *
 * This function is responsible for scheduling a thread to be executed. If the IDLE thread
 * is present in the runqueue, it is removed, and the scheduled thread becomes the only
 * thread in the runqueue. The scheduled thread is added to the top of the runqueue.
 *
 * Preconditions:
 * - The thread is in the Z_READY state.
 * - The thread is not already in the runqueue.
 *
 * @param thread Pointer to the `thread.tie.runqueue` item of the thread.
 * @return None.
 */
__kernel static void z_schedule(struct k_thread *thread)
{
	__ASSERT_NOINTERRUPT();

#if CONFIG_THREAD_STACK_SENTINEL && CONFIG_KERNEL_ASSERT
	/* check that stack sentinel is still valid before switching to thread */
	if (k_verify_stack_sentinel(thread) == false) {
		__fault(K_FAULT_SENTINEL);
	}
#endif

	/* Mark this thread as READY */
	z_set_thread_state(thread, Z_THREAD_STATE_READY);

	if (z_ready_count == 0u) {
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
 * @brief Schedule wake-up of the current thread.
 *
 * This function is responsible for scheduling the wake-up of the current thread. The
 * function assumes that the thread is currently suspended (Z_PENDING) and not in the
 * runqueue.
 *
 * The function takes two parameters:
 * - @p thread: Pointer to the current thread.
 * - @p timeout: Timeout value indicating the time at which the wake-up should occur.
 *
 * Preconditions:
 * - The thread is suspended (Z_PENDING).
 * - The thread is not in the runqueue.
 *
 * Postconditions:
 * - If the @p timeout value is not K_FOREVER, the current thread is marked for wake-up
 *   by setting the Z_THREAD_WAKEUP_SCHED_MSK flag.
 * - The current thread is added to the timeouts queue for wake-up event scheduling using
 *   the provided @p timeout value.
 *
 * Note: This is an assembly function implemented in assembly language, and it performs
 * low-level operations required for scheduling the wake-up of a suspended thread.
 */

/**
 * @brief Schedules a thread wakeup with a timeout.
 *
 * This function is responsible for scheduling the wake-up of the current thread. The
 * function assumes that the thread is currently suspended (Z_PENDING) and not in the
 * runqueue.
 *
 * @param timeout The timeout value for the wakeup.
 */
__kernel static void z_schedule_wake_up(k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_TRUE((z_current->flags & Z_THREAD_WAKEUP_SCHED_MSK) == 0);

	if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		z_current->flags |= Z_THREAD_WAKEUP_SCHED_MSK;
		z_current->tie.event.timeout = K_TIMEOUT_TICKS(timeout);
		z_current->tie.event.next    = NULL;
		z_tqueue_schedule(&z_timeouts_queue, &z_current->tie.event);
	}
}

/**
 * @brief Cancels a scheduled thread wakeup.
 *
 * This function cancels a previously scheduled wakeup for the specified thread.
 *
 * @param thread Pointer to the thread
 */
static void z_cancel_scheduled_wake_up(struct k_thread *thread)
{
	/* Remove the thread from the events queue */
	if (thread->flags & Z_THREAD_WAKEUP_SCHED_MSK) {
		thread->flags &= ~Z_THREAD_WAKEUP_SCHED_MSK;
		tqueue_remove(&z_timeouts_queue, &thread->tie.event);
	}
}

/**
 * @brief Wake up a thread that is pending for an event.
 *
 * This function wakes up a thread that is pending for an event. It assumes that
 * the thread is in the Z_PENDING mode, not in the runqueue, and may be in the
 * events queue. It also assumes that the interrupt flag is cleared when called.
 *
 * @param thread Pointer to the thread to wake up.
 */
__kernel void z_wake_up(struct k_thread *thread)
{
	__ASSERT_NOTNULL(thread);
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_STATE(thread, Z_THREAD_STATE_PENDING);

	__K_DBG_WAKEUP(thread);	 // @

	z_cancel_scheduled_wake_up(thread);

	z_schedule(thread);
}

/**
 * @brief Swaps the endianness of a memory address.
 *
 * This function swaps the endianness of a memory address. It is used to convert
 * data between big-endian and little-endian formats.
 *
 * @param addr Pointer to the memory address to swap endianness.
 */
inline static void swap_endianness(void **addr)
{
	*addr = (void *)HTONS(*addr);
}

/**
 * @brief Finalize the thread stack.
 *
 * @param thread Pointer to the thread
 */
static inline void z_thread_finalize_stack_init(struct k_thread *const thread)
{
	/* Swap endianness of addresses in compilation-time built
	 * stacks. We cannot change the endianness of addresses
	 * determined by the linker at compilation time. So we need to
	 * do it here.
	 */
	struct z_callsaved_ctx *const ctx = thread->stack.end - Z_CALLSAVED_CTX_SIZE + 1u;
	swap_endianness(&ctx->thread_context);
	swap_endianness((void *)&ctx->thread_entry);
	swap_endianness(&ctx->pc);

#if defined(__AVR_3_BYTE_PC__)
	ctx->pch = 0;
#endif /* __AVR_3_BYTE_PC__ */
}

/**
 * @brief Create the idle thread.
 */
extern void z_thread_idle_create(void);

/**
 * @brief Initialize the kernel scheduler.
 *
 * This function initializes the kernel scheduler by performing various initialization
 * tasks such as creating the idle thread and initializing other threads based
 * on the configuration. It also sets the state of the idle thread and adds
 * ready threads to the runqueue if necessary.
 */
void z_kernel_init(void)
{
#if CONFIG_KERNEL_THREAD_IDLE
	z_thread_idle_create();

	/* Mark idle thread */
	z_set_thread_state(&z_thread_idle, Z_THREAD_STATE_IDLE);
#endif

#if CONFIG_AVRTOS_LINKER_SCRIPT
	/* The main thread is the first running */
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

/*
 * Evaluate timeouts for threads/timers and events.
 * Schedule threads consequently.
 *
 * If CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS is enabled and we are in the
 * IDLE thread, the expired thread will be rescheduled only after the current
 * time slice interval finishes. As a result, we may lose a few ticks in the IDLE thread.
 * TODO: Improve this by checking for expired threads more often when we are in the IDLE
 * thread.
 *
 * Assumptions: The interrupt flag is cleared when called.
 */
void z_handle_timeouts(void)
{
	__Z_DBG_SYSTICK_ENTER();

#if CONFIG_KERNEL_ASSERT
	__ASSERT(!z_kernel_mode, K_ASSERT_USER_MODE);
	z_kernel_mode = 1u;
#endif

	__ASSERT_NOINTERRUPT();

	tqueue_shift(&z_timeouts_queue, CONFIG_KERNEL_TIME_SLICE_TICKS);

#if CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS
	z_sched_ticks_remaining = CONFIG_KERNEL_TIME_SLICE_TICKS;
#endif /* CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS */

	struct titem *ready;
	while ((ready = tqueue_pop(&z_timeouts_queue)) != NULL) {
		struct k_thread *const thread = Z_THREAD_FROM_EVENTQUEUE(ready);

		__K_DBG_SCHED_EVENT(thread);  // !

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

#if CONFIG_KERNEL_ASSERT
	z_kernel_mode = 0u;
#endif

	__Z_DBG_SYSTICK_EXIT();
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

/**
 * @brief Suspend a thread.
 *
 * This function suspends a thread by removing it from the events queue or the
 * runqueue, depending on its current state.
 * - If the thread is in the PENDING state, it is removed from the events queue.
 * - If the thread is in the READY state, it is removed from the runqueue.
 * - If the thread is the current thread, the runqueue is prepared so that the
 * normally next thread is executed.
 *
 * @param thread Pointer to the thread to suspend.
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
			/* Set runq pointer so that it points to the next thread
			 * to be executed */
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
void k_wait(k_timeout_t delay)
{
	__ASSERT_INTERRUPT();

	uint32_t now;
	uint32_t ticks = k_ticks_get_32();

	do {
		k_idle();

		now = k_ticks_get_32();
	} while (now - ticks < K_TIMEOUT_TICKS(delay));
}

void k_busy_wait(k_timeout_t delay)
{
	__ASSERT_INTERRUPT();

	uint32_t now;
	uint32_t ticks;

	k_sched_lock();

	ticks = k_ticks_get_32();

	do {
#ifndef __QEMU__
		sleep_cpu(); /* idle the thread until next interrupt */
#endif			     /* __QEMU__ */

		now = k_ticks_get_32();
	} while (now - ticks < K_TIMEOUT_TICKS(delay));

	k_sched_unlock();
}
#endif /* CONFIG_KERNEL_UPTIME */

void k_block_us(uint32_t delay_us)
{
	const uint8_t key = irq_lock();

	_delay_us(delay_us);

	irq_unlock(key);
}

void k_block_ms(uint32_t delay_ms)
{
	const uint8_t key = irq_lock();

	_delay_ms(delay_ms);

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

void k_thread_set_priority(struct k_thread *thread, uint8_t prio)
{
	const uint8_t key = irq_lock();

	thread->flags = (thread->flags & ~Z_THREAD_PRIO_MSK) | (prio & Z_THREAD_PRIO_MSK);

	irq_unlock(key);
}

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