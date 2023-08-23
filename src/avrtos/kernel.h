/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_KERNEL_H
#define _AVRTOS_KERNEL_H

#include <avr/interrupt.h>
#include <avr/io.h>

#include "assert.h"
#include "defines.h"
#include "sys.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define a new thread at runtime and initialize its stack
 *
 * @param thread hread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param stack_size thread stack size
 * @param priority thread priority
 * @param context_p thread context passed to entry function
 * @param symbol thread symbol letter
 * @return int 0 on success
 */
int8_t k_thread_create(struct k_thread *thread,
		       thread_entry_t entry,
		       void *stack,
		       size_t stack_size,
		       uint8_t priority,
		       void *context_p,
		       char symbol);

/**
 * @brief Start the execution of the given thread.
 *
 * @param thread The thread to start execution.
 * @return 0 if successful, otherwise a negative error code.
 */
__kernel int8_t k_thread_start(struct k_thread *thread);

/**
 * @brief Stop the execution of the specified thread.
 *
 * @param thread The thread to stop execution.
 * @return 0 if successful, otherwise a negative error code.
 */
__kernel int8_t k_thread_stop(struct k_thread *thread);

/**
 * @brief Stop the execution of the current thread.
 *
 * @see k_thread_stop
 *
 * @param thread The ready/pending thread to start.
 */
__kernel void k_stop(void);

/**
 * @brief Set the priority of the specified thread.
 *
 * This function allows you to change the priority of a given thread. The prio parameter
 * can be set to either K_COOPERATIVE or K_PREEMPTIVE.
 *
 * @param thread The thread for which to change the priority.
 * @param prio The desired priority (K_COOPERATIVE or K_PREEMPTIVE).
 * @return The status code indicating the success or failure of the operation.
 */
__kernel void k_thread_set_priority(struct k_thread *thread, uint8_t prio);

/**
 * @brief Thread entry point function.
 *
 * This function serves as the entry point for a new thread. It is responsible for
 * executing the code within the thread and receives a context pointer as an argument.
 *
 * @param context A pointer to the context data for the thread.
 */
void z_thread_entry(void *context);

extern struct k_thread *z_current;

/**
 * @brief Get current thread
 *
 * @see struct k_thread * z_current
 *
 * @return thread_t*
 */
static inline struct k_thread *k_thread_get_current(void)
{
	return z_current;
}

extern struct k_thread z_thread_main;

/**
 * @brief Get main thread
 *
 * @see struct k_thread z_thread_main
 *
 * @return struct k_thread*
 */
static inline struct k_thread *k_thread_get_main(void)
{
	return &z_thread_main;
}

#if CONFIG_KERNEL_IRQ_LOCK_COUNTER == 0

/**
 * @brief Disable interrupts in the current thread
 */
#define irq_disable cli

/**
 * @brief Enable interrupts in the current thread
 */
#define irq_enable sei

#else

/**
 * @brief Disable interrupts in the current thread.
 *
 * Can be called recursively.
 */
__kernel void irq_disable(void);

/**
 * @brief Enable interrupts in the current thread
 *
 * Can be called recursively.
 */
__kernel void irq_enable(void);

#endif /* CONFIG_KERNEL_IRQ_LOCK_COUNTER */

#define CRITICAL_SECTION_BEGIN() irq_disable()
#define CRITICAL_SECTION_END()	 irq_enable()

/**
 * @brief Lock interrupts and return the previous state
 *
 * @return uint8_t
 */
static inline uint8_t irq_lock(void)
{
	const uint8_t key = SREG;
	irq_disable();
	return key;
}

/**
 * @brief Unlock interrupts (restore the previous state)
 *
 * @param key
 */
static inline void irq_unlock(uint8_t key)
{
	SREG = key;
}

/**
 * @brief Lock the scheduler for the current thread.
 *
 * This function sets the current thread as a cooperative thread until the function
 * k_sched_unlock is called. The scheduler continues to execute system tasks and
 * handle timeout. However, for cooperative threads, it is
 * recommended that the execution time is kept short to avoid delaying other threads
 * and potentially impacting time-sensitivethreads.
 *
 * Locking and unlocking the scheduler has no effect on cooperative threads.
 *
 * This function cannot be called from an interrupt routine.
 *
 * If CONFIG_KERNEL_SCHED_LOCK_COUNTER is defined, k_sched_unlock() can be called
 * recursively. In this case, the maximum number of calls to k_sched_lock without
 * calling k_sched_unlock() is 255.
 *
 * Note: Locking the scheduler doesn't have an impact on functions that explicitely
 * yields the CPU, such as k_yield(), k_sleep(), or any kernel function waiting
 * for an event to be signaled (e.g., k_sem_take(), k_mutex_lock() with delay),
 * is called.
 */
__kernel void k_sched_lock(void);

/**
 * @brief Unlock the scheduler for the current thread and set it as preemptive.
 *
 * This function unlocks the scheduler for the current thread, allowing it to be
 * preempted by other threads.
 *
 * For cooperative threads, locking and unlocking the scheduler has no effect.
 *
 * This function cannot be called from an interrupt routine.
 *
 * @see k_sched_lock()
 */
__kernel void k_sched_unlock(void);

/**
 * @brief Check if the scheduler is locked by the current thread.
 *
 * This function determines whether the scheduler is locked by the current thread
 * or if the thread is a cooperative thread.
 *
 * @see k_sched_lock()
 * @see k_sched_unlock()
 *
 * @return true if the scheduler is locked for the current thread or if the thread is
 * cooperative.
 * @return false otherwise.
 */
__kernel bool k_sched_locked(void);

/**
 * @brief Check if the current thread is preemptive.
 *
 * This function determines whether the current thread is preemptive.
 *
 * @return true if the current thread is preemptive.
 */
__kernel bool k_cur_is_preempt(void);

/**
 * @brief Check if the current thread is cooperative.
 *
 * This function determines whether the current thread is cooperative.
 *
 * @return true if the current thread is cooperative.
 */
__kernel bool k_cur_is_coop(void);

/**
 * @brief used in K_SCHED_LOCK_CONTEXT macro
 *
 * @see k_sched_unlock
 *
 * @param __s
 */
static __inline__ void z_sched_restore(const uint8_t *__s)
{
	k_sched_unlock();
	__asm__ volatile("" ::: "memory");
	ARG_UNUSED(__s);
}

/**
 * @brief used in K_SCHED_LOCK_CONTEXT macro
 *
 * @return 1
 */
static __inline__ uint8_t z_sched_lock_ret(void)
{
	k_sched_lock();

	return 1;
}

/**
 * @brief Macro to lock the scheduler within a specific scope.
 *
 * This macro locks the scheduler for the duration of the inner scope.
 * The code within the block is equivalent to the code between k_sched_lock()
 * and k_sched_unlock().
 *
 * This macro draws inspiration from the ATOMIC_BLOCK(ATOMIC_FORCEON) macro
 * in <avr/atomic.h>.
 */
#define K_SCHED_LOCK_CONTEXT                                                          \
	for (uint8_t __k_schedl_x __attribute__((__cleanup__(z_sched_restore))) = 0u, \
				  __k_todo = z_sched_lock_ret();                      \
	     __k_todo; __k_todo		   = 0)

/**
 * @brief Suspend the execution of the current thread for the specified amount of time.
 *
 * This function schedules the current thread to be woken up after the specified amount of
 * time.
 *
 * If the timeout is set to K_FOREVER, the thread will not be executed again until
 * explicitly resumed with z_wake_up() or z_schedule().
 *
 * If the timeout is set to K_NO_WAIT, the function has no effect.
 *
 * @see k_yield
 *
 * @param timeout The duration of the sleep in milliseconds.
 */
__kernel void k_sleep(k_timeout_t timeout);

/**
 * @brief Sleep for a specified number of milliseconds.
 *
 * This function is a convenience wrapper around k_sleep() to simplify sleeping for
 * milliseconds.
 *
 * @see k_sleep
 *
 * @param ms The duration of the sleep in milliseconds.
 */
static inline void k_msleep(uint32_t ms)
{
	k_sleep(K_MSEC(ms));
}

/**
 * @brief This mode keeps the thread ready but yields the CPU to another thread if
 * available. If no other thread is available, it puts the CPU into a sleep state making
 * the system responsive to interrupts.
 */
#define K_WAIT_MODE_IDLE 0u

/**
 * @brief  This mode keeps the thread ready and actively using the CPU until the timeout.
 * - If the thread is preemptive, the CPU will be shared with other threads.
 * - If the thread is cooperative, it will keep the CPU until the timeout, blocking other.
 *   For cooperative threads this mode is equivalent to K_WAIT_BLOCK
 */
#define K_WAIT_MODE_ACTIVE 1u

/**
 * @brief  This function actively waits for the specified number of milliseconds by
 * locking the CPU for the current thread.
 */
#define K_WAIT_MODE_BLOCK 2u

/**
 * @brief Make the thread wait for the specified amount of time in milliseconds,
 * but keep it ready (in opposite to k_sleep which suspends the thread).
 *
 * The function must poll the elapsed time and is in charge to exit to continu
 * the execution of the current thread.
 *
 * This function doesn't disable interrupts and doesn't break system time.
 * However the choosen mode can affect the execution of other threads.
 *
 * Note: This function can always be used without an IDLE thread (with
 * CONFIG_KERNEL_THREAD_IDLE disabled).
 *
 * Note: Requires KERNEL_UPTIME to be enabled.
 *
 * @param timeout The duration to wait in milliseconds.
 * @param mode The wait mode (K_WAIT_MODE_IDLE, K_WAIT_MODE_ACTIVE, K_WAIT_MODE_BLOCK)
 */
__kernel void k_wait(k_timeout_t delay, uint8_t mode);

/**
 * @brief Block the RTOS (scheduler + SYSCLOCK) for the specified amount of time in
 * microseconds.
 *
 * Note: Do not exceed CONFIG_KERNEL_SYSCLOCK_PERIOD_US, otherwise the system uptime may
 * be delayed.
 *
 * This function relies on _delay_us() from <util/delay.h> which can only be used
 * in release mode(not in debug mode).
 *
 * @param delay_us The duration to block in microseconds.
 */
__kernel void z_cpu_block_us(uint32_t delay_us);

/**
 * @brief Block the RTOS (scheduler + SYSCLOCK) for the specified amount of time in
 * milliseconds.
 *
 * Note: Do not exceed CONFIG_KERNEL_SYSCLOCK_PERIOD_US (converted to milliseconds),
 * otherwise the system uptime may be delayed.
 *
 * This function relies on _delay_ms() from <util/delay.h> which can only be used
 * in release mode(not in debug mode).
 *
 * @param delay_ms The duration to block in milliseconds.
 */
__kernel void z_cpu_block_ms(uint32_t delay_ms);

/**
 * @brief Get the number of currently ready threads.
 *
 * This function returns the count of threads that are currently in the ready state,
 * meaning they are eligible for execution. If the CPU is in the IDLE state with no
 * threads ready for execution, the function will return 0.
 *
 * @return The number of currently ready threads.
 */
uint8_t k_ready_count(void);

/**
 * @brief Yield the CPU to another ready thread, assuming interrupts are disabled.
 *
 * The behavior is similar to k_yield(), but with the additional assumption that
 * interrupts are already disabled.
 *
 * @see k_yield
 */
void z_yield(void);

/**
 * @brief Yield the CPU to the next thread in the scheduler's runqueue.
 *
 * This function releases the CPU by stopping the execution of the current thread.
 * If the thread is still in the ready state, it is placed at the end of the runqueue,
 * allowing other threads to execute before it.
 *
 * The function restores the context of the current thread when returning.
 *
 * It can be called from both cooperative and preemptive threads.
 *
 * Can be called from ISRs.
 */
static inline void k_yield(void)
{
	const uint8_t key = irq_lock();
	z_yield();
	irq_unlock(key);
}

/**
 * @brief Yield function for Arduino compatibility.
 *
 * This function is provided for compatibility with Arduino and serves as a wrapper
 * for the k_yield() function.
 *
 * @see k_yield
 */
void yield(void);

/**
 * @brief Assertion helper to check if the code is running in user context.
 *
 * This function is an assertion helper that checks if the code is executing in user
 * context. It can be used to verify that certain operations or code paths are only
 * accessed in user context. If the code is not running in user context, an assertion
 * failure or error can be triggered.
 */
extern void z_assert_user_context(void);

/**
 * @brief Assertion helper to check if a thread is in the ready state.
 *
 * This function is an assertion helper that checks if a given thread is in the ready
 * state, indicating that it is eligible for execution.
 *
 * If the thread is not in the ready state, an assertion failure or error can be
 * triggered.
 *
 * @param thread The thread to check.
 */
extern void z_assert_thread_ready(struct k_thread *thread);

/**
 * @brief Contain the address of the thread currently running.
 */
extern struct k_thread *z_current;

/**
 * @brief Yield the interrupted thread to the next thread in the runqueue from an
 * interrupt context.
 *
 * It should only be called from a USER interrupt routine. It should not be called from a
 * k_timer or k_event callback handler, as those handlers are called in "kernel context"
 * and scheduling threads from those handlers will cause an immediate switch to the last
 * unpended thread.
 *
 * Note: This function should be called only from an interrupt routine.
 *
 * This function should be the last instruction in the interrupt routine because any code
 * after it will be delayed. It is typically used in an ISR after waking up a thread
 * (e.g., k_sem_give) to immediately give the CPU to the woken-up thread.
 *
 * Inlining this function reduces the required stack size by 2 (or 3) bytes when the
 * interrupt is called.
 *
 * @example
 * ISR(USART0_RX_vect)
 * {
 * 	const char flags = USART0_DEVICE->UDRn;
 * 	ll_usart_sync_putc(USART0_DEVICE, flags);
 *
 * 	int ret = k_flags_notify(&flags, flags, K_FLAGS_SET);
 * 	if (ret > 0) {
 * 		k_yield_from_isr();
 * 	}
 * }
 */
static inline void k_yield_from_isr(void)
{
	// ASSERT ISR CONTEXT
	// ASSERT IRQ LOCKED
	// ASSERT NOT kernel context

#if CONFIG_KERNEL_ASSERT
	z_assert_user_context();
#endif

	/* Check whether current thread can be preempted */
	if ((z_current->flags & (Z_THREAD_SCHED_LOCKED_MSK | Z_THREAD_PRIO_COOP)) == 0u) {
		z_yield();
	}
}

/**
 * @brief Yield the interrupted thread if a thread is given as an argument.
 *
 * It should only be called from a USER interrupt routine. It should not be called from a
 * k_timer or k_event callback handler, as those handlers are called in "kernel context"
 * and scheduling threads from those handlers will cause an immediate switch to the last
 * unpended thread.
 *
 * This function should be the last instruction in the interrupt routine because any code
 * after it will be delayed. It is typically used in an ISR after waking up a thread
 * (e.g., k_sem_give) to immediately give the CPU to the woken-up thread.
 *
 * Inlining this function reduces the required stack size by 2 (or 3) bytes when the
 * interrupt is called.
 *
 * @see k_yield_from_isr
 * @param thread The thread that is ready to be scheduled. If the thread is NULL, this
 * function does nothing.
 *
 * @example:
 * ISR(INT0_vect)
 * {
 * 	led_toggle();
 * 	const struct k_thread *thread = k_sem_give(&button_sem);
 * 	k_yield_from_isr_cond(thread);
 * }
 */
static inline void k_yield_from_isr_cond(struct k_thread *thread)
{
	if (thread != NULL) {
#if CONFIG_KERNEL_ASSERT
		z_assert_thread_ready(thread);
#endif

		k_yield_from_isr();
	}
}

/**
 * @brief Get uptime in ticks (32 bit), if KERNEL_TICKS is enabled
 *
 * @return Kernel ticks value (32 bits)
 */
__kernel uint32_t k_ticks_get_32(void);

/**
 * @brief Get uptime in ticks (32 bit), if KERNEL_TICKS is enabled
 *
 * @see k_ticks_get_32
 *
 * @return uint32_t
 */
static inline uint32_t k_ticks_get(void)
{
	return k_ticks_get_32();
}

/**
 * @brief Get uptime in ticks (64 bits), if KERNEL_TICKS is enabled.
 * Meaningful only if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS is enabled,
 * otherwise, this function is equivalent to k_ticks_get_32.
 *
 * @return Kernel ticks value (64 bits)
 */
__kernel uint64_t k_ticks_get_64(void);

/**
 * @brief Get uptime in milliseconds, if KERNEL_UPTIME is enabled
 *
 * @return Uptime in milliseconds
 */
__kernel uint32_t k_uptime_get_ms32(void);

/**
 * @brief Get uptime in milliseconds, if KERNEL_UPTIME is enabled.
 *
 * Should be used if KERNEL_UPTIME_40BITS is enabled.
 *
 * @return Uptime in milliseconds (64 bits)
 */
__kernel uint64_t k_uptime_get_ms64(void);

/**
 * @brief Get uptime in seconds, if KERNEL_UPTIME is enabled
 *
 * @return Uptime in seconds
 */
__kernel uint32_t k_uptime_get(void);

#ifdef __cplusplus
}
#endif

#endif
