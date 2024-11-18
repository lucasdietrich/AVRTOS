/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "kernel.h"

#include <avr/sleep.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "assert.h"
#include "canaries.h"
#include "debug.h"
#include "event.h"
#include "fault.h"
#include "idle.h"
#include "kernel_private.h"
#include "stack_sentinel.h"
#include "systime.h"
#include "timer.h"

#define K_MODULE K_MODULE_KERNEL

/* Make sure the members are at the expected offsets for k_thread and k_kernel
 * these members are accessed directly in assembly code
 */
__STATIC_ASSERT_NOMSG(offsetof(struct k_thread, sp) == 0);
__STATIC_ASSERT_NOMSG(offsetof(struct k_thread, flags) == sizeof(void *));
__STATIC_ASSERT_NOMSG(offsetof(z_kernel_t, current) == 0);
#if CONFIG_KERNEL_TICKS_COUNTER
__STATIC_ASSERT_NOMSG(offsetof(z_kernel_t, ticks) == sizeof(struct k_thread *));
#endif
#if Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS
__STATIC_ASSERT_NOMSG(offsetof(z_kernel_t, sched_ticks_remaining) ==
					  sizeof(struct k_thread *) + sizeof(z_ticks_t));
#endif

#if CONFIG_KERNEL_ASSERT
#warning "Assertions are globally enabled"
#endif

#if CONFIG_THREAD_EXPLICIT_MAIN_STACK == 1
__noinit char z_main_stack[CONFIG_THREAD_MAIN_STACK_SIZE];
Z_STACK_SENTINEL_REGISTER(z_main_stack);
#endif

/* Set the main thread priority */
#if CONFIG_THREAD_MAIN_COOPERATIVE == 1
#define Z_THREAD_MAIN_PRIORITY Z_THREAD_PRIO_COOP
#else
#define Z_THREAD_MAIN_PRIORITY Z_THREAD_PRIO_PREEMPT
#endif

/* Calculate the main thread stack end address */
#if CONFIG_THREAD_EXPLICIT_MAIN_STACK == 1
/* Explicit stack defined, the stack is at the end of the defined buffer */
#define Z_THREAD_MAIN_STACK_START_ADDR (void *)z_main_stack
#define Z_THREAD_MAIN_STACK_END_ADDR                                                     \
	(void *)Z_STACK_END(Z_THREAD_MAIN_STACK_START_ADDR, CONFIG_THREAD_MAIN_STACK_SIZE)
#else
/* Implicit stack, we set the main thread stack end at the end of the RAM */
#define Z_THREAD_MAIN_STACK_END_ADDR (void *)RAMEND

#if CONFIG_THREAD_MAIN_MONITOR == 1
#if CONFIG_THREAD_MAIN_STACK_SIZE == 0
#error                                                                                   \
	"CONFIG_THREAD_MAIN_STACK_SIZE must be set when CONFIG_THREAD_MAIN_MONITOR is enabled"
#endif
#define Z_THREAD_MAIN_STACK_START_ADDR                                                   \
	(void *)Z_STACK_START(Z_THREAD_MAIN_STACK_END_ADDR, CONFIG_THREAD_MAIN_STACK_SIZE)
Z_LINK_KERNEL_SECTION(.k_sentinels)
void *z_sent_z_main_stack = Z_THREAD_MAIN_STACK_START_ADDR;
#endif
#endif

/**
 * @brief Main thread instance.
 *
 * This variable represents the main thread of the application.
 *
 * The field initializations are as follows:
 * - `.sp`: Set to 0 to indicate that the main thread is running and no context
 *   is currently saved.
 * - `.flags`: Combination of flags indicating the state and priority of the
 *   thread. The specific combination depends on the configuration options.
 * - `.tie`: Contains the reference element for the thread in the runqueue.
 * - `.wany`: Initialized to indicate that the thread is not pending on any
 *   events.
 * - `.swap_data`: Set to NULL as there is no explicit swapping data associated
 *   with the main thread.
 * - `.stack`: Contains information about the thread's stack. The specific
 *   values depend on the configuration options, such as whether an explicit stack
 *   is defined or an implicit stack is used.
 * - `.symbol`: Default symbol for the main thread, here set to 'M'.
 */
K_THREAD struct k_thread z_thread_main = {
	.sp	   = 0, // Main thread is running, context already "restored"
	.flags = Z_THREAD_STATE_READY | Z_THREAD_MAIN_PRIORITY | Z_THREAD_PRIO_LOW,
	.tie =
		{
			.runqueue =
				{
					// Thread before initialization at
					// the top of the runqueue (is actually
					// the reference element)
					.prev = &z_thread_main.tie.runqueue,
					.next = &z_thread_main.tie.runqueue,
				},
		},
	.wany	   = DITEM_INIT_NULL(), // The thread isn't pending on any events
	.swap_data = NULL,
	.stack =
		{
			.end = Z_THREAD_MAIN_STACK_END_ADDR,
			/* If CONFIG_THREAD_EXPLICIT_MAIN_STACK is disabled
			 * the size is used by CONFIG_THREAD_MAIN_MONITOR
			 */
			.size = CONFIG_THREAD_MAIN_STACK_SIZE,
		},
	.symbol = 'M' // Default main thread symbol
};

struct z_kernel z_ker = {
	.current		= &z_thread_main,
	.ready_count	= 1u,
	.run_queue		= &z_thread_main.tie.runqueue,
	.timeouts_queue = NULL,
#if CONFIG_KERNEL_TICKS_COUNTER
	.ticks = {0u},
#endif /* CONFIG_KERNEL_TICKS_COUNTER */
#if Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS
	.sched_ticks_remaining = Z_KERNEL_TIME_SLICE_TICKS,
#endif /* Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS */
#if CONFIG_KERNEL_ASSERT
	.kernel_mode = 0u,
#endif
};

/**
 * @brief Start and end pointers for the kernel thread array.
 *
 * These symbols are defined in the linker script if CONFIG_AVRTOS_LINKER_SCRIPT is
 * set.
 *
 * These external symbols represent the start and end pointers of the kernel
 * thread array. The kernel thread array is a collection of `struct k_thread`
 * instances that represent the threads managed by the kernel.
 *
 * The symbol `__k_threads_start` points to the beginning of the kernel thread
 * array, and the symbol `__k_threads_end` points to the end of the kernel
 * thread array. The range between these two pointers inclusively represents all
 * the threads in the kernel.
 */
extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

/**
 * @brief Idle thread.
 *
 * This external symbol represents the idle thread in the system. The idle
 * thread is a special system thread that runs when no other threads are
 * eligible for execution. It performs idle-time processing and waits for other
 * threads to become ready.
 */
extern struct k_thread z_thread_idle;

#if CONFIG_THREAD_MONITOR
/**
 * @brief Monitor and verify the stack of a thread.
 *
 * Triggers a fault if the stack is invalid.
 *
 * @param thread Pointer to the thread
 */
static void z_thread_monitor(struct k_thread *thread)
{
	if (!Z_THREAD_IS_MONITORED(thread)) {
		return;
	}

	/* Verify if a stack overflow occurs at the moment of executing the current code.
	 */
	const uint16_t stack_offset = sys_ptr_diff(thread->stack.end, SP);
	if (stack_offset > thread->stack.size) {
		__fault(K_FAULT_STACK_OVERFLOW);
	}

#if CONFIG_THREAD_STACK_SENTINEL
	/* Check for the presence of the stack sentinel */
	if (z_thread_verify_sent(thread) == false) {
		__fault(K_FAULT_STACK_SENTINEL);
	}
#endif /* CONFIG_THREAD_STACK_SENTINEL */
}
#endif /* CONFIG_THREAD_MONITOR */

//
// Kernel Private API
//

/**
 * @brief Schedule a thread to be executed.
 *
 * This function is responsible for scheduling a thread to be executed. If the
 * IDLE thread is present in the runqueue, it is removed, and the scheduled
 * thread becomes the only thread in the runqueue. The scheduled thread is added
 * to the top of the runqueue.
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

#if CONFIG_THREAD_MONITOR
	/* Check that the stack is valid before scheduling the thread */
	z_thread_monitor(thread);
#endif

	/* Mark this thread as READY */
	z_set_thread_state(thread, Z_THREAD_STATE_READY);

	if (z_ker.ready_count == 0u) {
		/* Resume from IDLE */
		dlist_init(&thread->tie.runqueue);
		z_ker.run_queue = &thread->tie.runqueue;
	} else {
		/* Woken up threads should be executed before any
		 * already running preemptive thread, so prepend
		 *
		 * Call k_yield_from_isr_cond() to switch to the woken-up thread
		 * if called from ISR
		 */
		dlist_prepend(z_ker.run_queue, &thread->tie.runqueue);
	}

	z_ker.ready_count++;
}

/**
 * @brief Schedule wake-up of the current thread.
 *
 * This function schedules the wake-up of the current
 * thread. The function assumes that the thread is currently suspended
 * (Z_PENDING) and not in the runqueue.
 *
 * The function takes two parameters:
 * - @p thread: Pointer to the current thread.
 * - @p timeout: Timeout value indicating the time at which the wake-up should
 * occur.
 *
 * Preconditions:
 * - The thread is suspended (Z_PENDING).
 * - The thread is not in the runqueue.
 *
 * Postconditions:
 * - If the @p timeout value is not K_FOREVER, the current thread is marked for
 * wake-up by setting the Z_THREAD_WAKEUP_SCHED_MSK flag.
 * - The current thread is added to the timeouts queue for wake-up event
 * scheduling using the provided @p timeout value.
 *
 * Note: This is an assembly function implemented in assembly language, and it
 * performs low-level operations required for scheduling the wake-up of a
 * suspended thread.
 */

/**
 * @brief Cancel a scheduled thread wake-up.
 *
 * This function cancels a previously scheduled wake-up for the specified thread.
 *
 * @param thread Pointer to the thread
 */
static void z_cancel_scheduled_wake_up(struct k_thread *thread)
{
	/* Remove the thread from the events queue */
	if (thread->flags & Z_THREAD_WAKEUP_SCHED_MSK) {
		thread->flags &= ~Z_THREAD_WAKEUP_SCHED_MSK;
		tqueue_remove(&z_ker.timeouts_queue, &thread->tie.event);
	}
}

/**
 * @brief Wake up a thread that is pending an event.
 *
 * This function wakes up a thread that is pending an event. It assumes that
 * the thread is in Z_PENDING mode, not in the runqueue, and may be in the
 * events queue. It also assumes that the interrupt flag is cleared when called.
 *
 * @param thread Pointer to the thread to wake up.
 */
__kernel void z_wake_up(struct k_thread *thread)
{
	__ASSERT_NOTNULL(thread);
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_STATE(thread, Z_THREAD_STATE_PENDING);

	__Z_DBG_WAKEUP(thread); // @

	z_cancel_scheduled_wake_up(thread);

	z_schedule(thread);
}

/**
 * @brief Finalize the thread stack.
 *
 * @param thread Pointer to the thread
 */
__always_inline void z_thread_finalize_stack_init(struct k_thread *const thread)
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
 * This function initializes the kernel scheduler by performing various
 * initialization tasks such as creating the idle thread and initializing other
 * threads based on the configuration. It also sets the state of the idle thread
 * and adds ready threads to the runqueue if necessary.
 */
void z_init_threads(void)
{
#if CONFIG_KERNEL_THREAD_IDLE
#if CONFIG_AVRTOS_LINKER_SCRIPT == 0
	z_thread_idle_create();
#endif

	/* Mark the idle thread */
	z_set_thread_state(&z_thread_idle, Z_THREAD_STATE_IDLE);
#endif

#if CONFIG_AVRTOS_LINKER_SCRIPT
	/* The main thread is the first running */
	for (uint8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
		struct k_thread *const thread = &(&__k_threads_start)[i];

		/* Main thread already in queue */
		if (Z_THREAD_IS_MAIN(thread)) {
			continue;
		}

		/* Only auto-start threads must be added to the runqueue */
		if (z_get_thread_state(thread) == Z_THREAD_STATE_READY) {
			z_ker.ready_count++;
			dlist_append(z_ker.run_queue, &thread->tie.runqueue);
		}

		z_thread_finalize_stack_init(thread);
	}
#endif
}

__STATIC_ASSERT_NOMSG(Z_KERNEL_TIME_SLICE_TICKS != 0);

/*
 * Evaluate timeouts for threads/timers and events.
 * Schedule threads accordingly.
 *
 * If Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS is enabled and we are in the
 * IDLE thread, the expired thread will be rescheduled only after the current
 * time slice interval finishes. As a result, we may lose a few ticks in the
 * IDLE thread.
 * TODO: Improve this by checking for expired threads more often when we are in
 * the IDLE thread.
 *
 * Assumptions: The interrupt flag is cleared when called.
 */
void z_sched_enter(void)
{
	__Z_DBG_SYSTICK_ENTER();

	__ASSERT_NOINTERRUPT();

#if CONFIG_KERNEL_ASSERT
	__ASSERT(!z_ker.kernel_mode, K_ASSERT_USER_MODE);
	z_ker.kernel_mode = 1u;
#endif

	tqueue_shift(&z_ker.timeouts_queue, Z_KERNEL_TIME_SLICE_TICKS);

#if Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS
	z_ker.sched_ticks_remaining = Z_KERNEL_TIME_SLICE_TICKS;
#endif /* Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS */

	struct titem *ready;
	while ((ready = tqueue_pop(&z_ker.timeouts_queue)) != NULL) {
		struct k_thread *const thread = Z_THREAD_FROM_EVENTQUEUE(ready);

		__Z_DBG_SCHED_EVENT(thread); // !

		/* Set the ready thread expired flag */
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
	z_ker.kernel_mode = 0u;
#endif

	__Z_DBG_SYSTICK_EXIT();
}

/**
 * @brief Choose the next thread to be executed.
 * This function is called during any thread switch to determine which
 * one is the next thread to be executed.
 *
 * This function is called in the k_yield function.
 *
 * @return struct k_thread* : Next thread to be executed
 */
struct k_thread *z_scheduler(void)
{
	__ASSERT_NOINTERRUPT();

	struct k_thread *const prev = z_ker.current;

#if CONFIG_THREAD_MONITOR
	z_thread_monitor(prev);
#endif

	/* Reset flags */
	prev->flags &= ~(Z_THREAD_TIMER_EXPIRED_MSK | Z_THREAD_PEND_CANCELED_MSK);

	/* If the previous thread put itself in a pending state,
	 * it already removed itself from the runqueue, so we don't need
	 * to do it here
	 */
	if (z_get_thread_state(z_ker.current) == Z_THREAD_STATE_READY) {
		/* Rotate the runqueue, set the next thread to the first position */
		z_ker.run_queue = z_ker.run_queue->next;
	}

	/* Fetch the next thread to execute */
	z_ker.current = CONTAINER_OF(z_ker.run_queue, struct k_thread, tie.runqueue);

	__Z_DBG_SCHED_NEXT_THREAD();
	__Z_DBG_SCHED_NEXT(z_ker.current);

#if CONFIG_THREAD_MONITOR
	z_thread_monitor(z_ker.current);
#endif

	return prev;
}

/**
 * @brief Suspend a thread.
 *
 * This function suspends a thread by removing it from the events queue or the
 * runqueue, depending on its current state.
 * - If the thread is in the PENDING state, it is removed from the events queue.
 * - If the thread is in the READY state, it is removed from the runqueue.
 * - If the thread is the current thread, the runqueue is prepared so that the
 * next thread is executed.
 *
 * @param thread Pointer to the thread to suspend.
 */
static void z_suspend(struct k_thread *thread)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_NOT_STATE(thread, Z_THREAD_STATE_STOPPED);
	__ASSERT_THREAD_NOT_STATE(thread, Z_THREAD_STATE_IDLE);

#if CONFIG_THREAD_MONITOR
	z_thread_monitor(thread);
#endif

	if (z_get_thread_state(thread) == Z_THREAD_STATE_PENDING) { /* Z_PENDING */
		z_cancel_scheduled_wake_up(thread);
	} else {
		/* Remove thread from the runqueue */
		dlist_remove(&thread->tie.runqueue);

		/* Decrement the number of threads in the runqueue */
		z_ker.ready_count--;

		/* Check if there is at least one thread running */
		if (z_ker.ready_count == 0) {
#if CONFIG_KERNEL_THREAD_IDLE == 0u
			/* Assert */
			__ASSERT_LEASTONE_RUNNING();

			/* If the IDLE thread is not enabled, then fault */
			__fault(K_FAULT_KERNEL_HALT);
#else
			/* Switch to the IDLE thread */
			z_ker.run_queue = &z_thread_idle.tie.runqueue;
#endif
		} else if (thread == z_ker.current) {
			/* Set the runqueue pointer so that it points to the next thread
			 * to be executed */
			z_ker.run_queue = thread->tie.runqueue.next->prev;
		}

		__Z_DBG_SCHED_SUSPENDED(z_ker.current);
	}
}

/**
 * @brief Schedule a thread wake-up with a timeout.
 *
 * This function schedules the wake-up of the current
 * thread. The function assumes that the thread is currently suspended
 * (Z_PENDING) and not in the runqueue.
 *
 * @param timeout The timeout value for the wake-up.
 */
static inline void z_schedule_wake_up(k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_TRUE((z_ker.current->flags & Z_THREAD_WAKEUP_SCHED_MSK) == 0);

	if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		z_ker.current->flags |= Z_THREAD_WAKEUP_SCHED_MSK;
		z_ker.current->tie.event.timeout = K_TIMEOUT_TICKS(timeout);
		z_ker.current->tie.event.next	 = NULL;
		z_tqueue_schedule(&z_ker.timeouts_queue, &z_ker.current->tie.event);
	}
}

/**
 * @brief Suspend the current thread and wait until the timeout expires or the
 * thread is woken up.
 *
 * @param timeout The timeout value for the sleep.
 */
__kernel void z_pend_current(k_timeout_t timeout)
{
	/* Suspend the thread */
	z_suspend(z_ker.current);

	/* Schedule thread wake-up if timeout is set */
	z_schedule_wake_up(timeout);

	/* Mark this thread as pending */
	z_set_thread_state(z_ker.current, Z_THREAD_STATE_PENDING);

	/* Call scheduler */
	z_yield();
}

__kernel int8_t z_pend_current_on(struct dnode *waitqueue, k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		return -EAGAIN;
	}

	int err;

	/* Queue the thread to the pending queue of the object */
	dlist_append(waitqueue, &z_ker.current->wany);

	/* Make the thread until wake-up */
	z_pend_current(timeout);

	/* If the timer expired, we manually remove the thread from
	 * the pending queue
	 */
	if (z_ker.current->flags & Z_THREAD_TIMER_EXPIRED_MSK) {
		dlist_remove(&z_ker.current->wany);
		err = -ETIMEDOUT;
	} else if (z_ker.current->flags & Z_THREAD_PEND_CANCELED_MSK) {
		err = -ECANCELED;
	} else {
		err = 0;
	}

	return err;
}

__kernel struct k_thread *z_unpend_first_thread(struct dnode *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = NULL;
	struct dnode *tie				= dlist_get(waitqueue);

	if (DITEM_VALID(waitqueue, tie)) {
		pending_thread = Z_THREAD_FROM_WAITQUEUE(tie);

		/* Immediate wake-up is no longer required because
		 * with the swap model, the object is already reserved for the
		 * first pending thread
		 */
		z_wake_up(pending_thread);
	}
	/* If no thread is pending on the object, we simply return */
	return pending_thread;
}

__kernel struct k_thread *z_unpend_first_and_swap(struct dnode *waitqueue,
												  void *set_swap_data)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = z_unpend_first_thread(waitqueue);
	if (pending_thread != NULL) {
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
		/* Indicates that we canceled the thread pending on the object */
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

static void z_thread_stack_create(struct k_thread *const thread,
								  k_thread_entry_t entry,
								  void *const context_p)
{
	struct z_callsaved_ctx *const ctx = Z_THREAD_CTX_START(thread->stack.end);

	/* Initialize unused registers with default value */
	for (uint8_t *reg = ctx->regs; reg < ctx->regs + sizeof(ctx->regs); reg++) {
		*reg = 0x00u;
	}

	ctx->sreg			= 0U;
	ctx->init_sreg		= CONFIG_THREAD_DEFAULT_SREG;
	ctx->thread_context = (void *)K_SWAP_ENDIANNESS(context_p);
	ctx->thread_entry	= (void *)K_SWAP_ENDIANNESS(entry);
	ctx->pc				= (void *)K_SWAP_ENDIANNESS(z_thread_entry);

#if defined(__AVR_3_BYTE_PC__)
	ctx->pch = 0;
#endif

	/* Save SP in the thread structure */
	thread->sp = ctx;

	/* Adjust the pointer to the top of the stack */
	thread->sp--;
}

//
// Kernel Public API
//

int8_t k_thread_create(struct k_thread *thread,
					   k_thread_entry_t entry,
					   void *stack,
					   size_t stack_size,
					   uint8_t prio,
					   void *context_p,
					   char symbol)
{
	Z_ARGS_CHECK(thread && entry && stack && stack_size >= Z_THREAD_STACK_MIN_SIZE)
	{
		return -EINVAL;
	}

	thread->stack.end  = (void *)Z_STACK_END(stack, stack_size);
	thread->stack.size = stack_size;

#if CONFIG_THREAD_CANARIES
	z_init_thread_stack_canaries(thread);
#endif /* CONFIG_THREAD_CANARIES */

#if CONFIG_THREAD_STACK_SENTINEL
	z_init_thread_stack_sentinel(thread);
#endif /* CONFIG_THREAD_STACK_SENTINEL */

	z_thread_stack_create(thread, entry, context_p);

	/* Initialize internal data */
	thread->flags	  = Z_THREAD_STATE_STOPPED | (prio & Z_THREAD_PRIO_MSK);
	thread->symbol	  = symbol;
	thread->swap_data = NULL;

	return 0;
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
#if CONFIG_KERNEL_THREAD_IDLE
	__ASSERT_THREAD_NOT_STATE(thread, Z_THREAD_STATE_IDLE);
#endif

	if (z_get_thread_state(thread) == Z_THREAD_STATE_STOPPED) return -EINVAL;

	const uint8_t key = irq_lock();

	z_suspend(thread);

	z_set_thread_state(thread, Z_THREAD_STATE_STOPPED);

	if (thread == z_ker.current) z_yield();

	/* Unlock in all cases:
	 * - if we stopped ourselves, in which case we need to unlock in any case
	 *   another thread started/resumed us again.
	 * - if we stopped another thread, in which case we need to unlock
	 */
	irq_unlock(key);

	return 0;
}

void k_stop()
{
	k_thread_stop(z_ker.current);
}

void k_thread_set_priority(struct k_thread *thread, uint8_t prio)
{
	const uint8_t key = irq_lock();

	thread->flags = (thread->flags & ~Z_THREAD_PRIO_MSK) | (prio & Z_THREAD_PRIO_MSK);

	irq_unlock(key);
}

uint8_t k_ready_count(void)
{
	return z_ker.ready_count;
}

void k_sched_lock(void)
{
	const uint8_t key = irq_lock();
	z_ker.current->flags |= Z_THREAD_SCHED_LOCKED_MSK;
	irq_unlock(key);

#if CONFIG_KERNEL_REENTRANCY
	z_ker.current->sched_lock_cnt++;
#endif /* CONFIG_KERNEL_REENTRANCY */

	__Z_DBG_SCHED_LOCK(z_ker.current);
}

void k_sched_unlock(void)
{
#if CONFIG_KERNEL_REENTRANCY
	if (z_ker.current->sched_lock_cnt == 0) {
		return;
	} else if (z_ker.current->sched_lock_cnt == 1) {
		z_ker.current->sched_lock_cnt = 0;
	} else {
		z_ker.current->sched_lock_cnt--;
		return;
	}
#endif /* CONFIG_KERNEL_REENTRANCY */

	const uint8_t key = irq_lock();
	z_ker.current->flags &= ~Z_THREAD_SCHED_LOCKED_MSK;
	irq_unlock(key);

	__Z_DBG_SCHED_UNLOCK();
}

__always_inline uint8_t z_current_flags_get(void)
{
	return z_ker.current->flags;
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
	const uint8_t key = irq_lock();

	/* Make the thread until wake-up */
	z_pend_current(timeout);

	irq_unlock(key);
}

#if CONFIG_KERNEL_UPTIME
void k_wait(k_timeout_t delay, uint8_t mode)
{
	__ASSERT_INTERRUPT();

	uint32_t now;
	uint32_t ticks	  = k_ticks_get_32();
	bool unlock_sched = false;

	if (mode == K_WAIT_MODE_BLOCK && (CONFIG_KERNEL_REENTRANCY || !k_sched_locked())) {
		unlock_sched = true;
		k_sched_lock();
	}

	do {
		switch (mode) {
		case K_WAIT_MODE_ACTIVE:
			break;
		case K_WAIT_MODE_BLOCK:
#ifndef __QEMU__
			sleep_cpu(); /* Idle the thread until the next interrupt */
#endif					 /* __QEMU__ */
			break;
		case K_WAIT_MODE_IDLE:
		default:
			k_idle();
			break;
		}
		now = k_ticks_get_32();
	} while (now - ticks < K_TIMEOUT_TICKS(delay));

	if (unlock_sched) k_sched_unlock();
}
#endif /* CONFIG_KERNEL_UPTIME */

void z_cpu_block_us(uint32_t delay_us)
{
	const uint8_t key = irq_lock();

	_delay_us(delay_us);

	irq_unlock(key);
}

void z_cpu_block_ms(uint32_t delay_ms)
{
	const uint8_t key = irq_lock();

	_delay_ms(delay_ms);

	irq_unlock(key);
}

#if CONFIG_ARDUINO_FRAMEWORK
/**
 * @brief Arduino definition of the yield function.
 */
void yield(void)
{
	k_yield();
}
#endif
