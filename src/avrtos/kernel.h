/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_KERNEL_H
#define _AVRTOS_KERNEL_H

/*
 * Kernel API
 *
 * This header file gathers the main kernel functions and macros for creating and managing
 * threads, scheduling, and timing.
 *
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "assert.h"
#include "defines.h"
#include "deprecated.h"
#include "sys.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Kernel internal structure.
 *
 * This external symbol represents the kernel's internal structure, which contains
 * information about the current state of the kernel, including the current thread,
 * the run queue, the timeouts queue, and other kernel-related data.
 */
extern struct z_kernel z_ker;

/**
 * @brief Main thread
 *
 * This external symbol represents the main thread in the system.
 */
extern struct k_thread z_thread_main;

/**
 * @brief Define a new thread at runtime and initialize its stack.
 *
 * @param thread Pointer to the thread structure.
 * @param entry Entry function for the thread.
 * @param stack Start location of the thread's stack.
 * @param stack_size Size of the thread's stack.
 * @param priority Priority of the thread.
 * @param context_p Pointer to the context data passed to the entry function.
 * @param symbol Symbol representing the thread.
 * @return int8_t Returns 0 on success.
 */
int8_t k_thread_create(struct k_thread *thread,
                       k_thread_entry_t entry,
                       void *stack,
                       size_t stack_size,
                       uint8_t priority,
                       void *context_p,
                       char symbol);

/**
 * @brief Start the execution of the specified thread.
 *
 * @param thread Pointer to the thread to be started.
 * @return int8_t Returns 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_thread_start(struct k_thread *thread);

/**
 * @brief Stop the execution of the specified thread.
 *
 * @param thread Pointer to the thread to be stopped.
 * @return int8_t Returns 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_thread_abort(struct k_thread *thread);

/**
 * @brief Wait for the specified thread to finish execution.
 *
 * This function waits for the specified thread to finish execution.
 *
 * @param thread Pointer to the thread to wait for.
 * @param timeout The maximum time to wait for the thread to finish.
 * @return int8_t Returns 0 on success, or a negative error code on failure.
 *  - -EINVAL if the thread pointer is NULL.
 *  - -ETIMEDOUT if the timeout expired before the thread finished.
 *  - -ECANCELED if the thread waiting was aborted.
 *  - -EAGAIN if the thread is not finished and the timeout is K_NO_WAIT.
 */
__kernel int8_t k_thread_join(struct k_thread *thread, k_timeout_t timeout);

/**
 * @brief Stop the execution of the current thread.
 *
 * This function stops the currently executing thread.
 */
__kernel void k_abort(void);

/**
 * @brief Set the priority of the specified thread.
 *
 * This function changes the priority of a given thread. The `prio` parameter can be set
 * to either `K_COOPERATIVE` or `K_PREEMPTIVE`.
 *
 * @param thread Pointer to the thread whose priority is to be changed.
 * @param prio The desired priority (`K_COOPERATIVE` or `K_PREEMPTIVE`).
 */
__kernel void k_thread_set_priority(struct k_thread *thread, uint8_t prio);

/**
 * @brief Thread entry point function.
 *
 * This function serves as the entry point for a new thread. It is responsible for
 * executing the code within the thread and receives a context pointer as an argument.
 *
 * @param context Pointer to the context data for the thread.
 */
void z_thread_entry(void *context);

/**
 * @brief Get the currently executing thread.
 *
 * @return struct k_thread* Pointer to the current thread.
 */
__always_inline struct k_thread *k_thread_get_current(void)
{
    return z_ker.current;
}

/**
 * @brief Get the main thread.
 *
 * @return struct k_thread* Pointer to the main thread.
 */
__always_inline struct k_thread *k_thread_get_main(void)
{
    return &z_thread_main;
}

/**
 * @brief Disable interrupts in the current thread.
 */
#define irq_disable cli

/**
 * @brief Enable interrupts in the current thread.
 */
#define irq_enable sei

#define CRITICAL_SECTION_BEGIN() irq_disable()
#define CRITICAL_SECTION_END()   irq_enable()

/**
 * @brief Lock interrupts and return the previous state.
 *
 * @return uint8_t The previous state of the interrupts.
 */
__always_inline uint8_t irq_lock(void)
{
    const uint8_t key = SREG;
    cli();
    return key;
}

/**
 * @brief Unlock interrupts and restore the previous state.
 *
 * @param key The state to restore the interrupts to.
 */
__always_inline void irq_unlock(uint8_t key)
{
    SREG = key;
}

/**
 * @brief Lock the scheduler for the current thread.
 *
 * This function sets the current thread as cooperative until `k_sched_unlock` is called.
 * The scheduler continues to execute system tasks and handle timeouts. However, for
 * cooperative threads, it is recommended to keep the execution time short to avoid
 * delaying other threads and potentially impacting time-sensitive operations.
 *
 * Locking and unlocking the scheduler has no effect on cooperative threads.
 *
 * This function cannot be called from an interrupt routine.
 *
 * If `CONFIG_KERNEL_REENTRANCY` is defined, `k_sched_unlock()` can be called recursively.
 * The maximum number of calls to `k_sched_lock` without calling `k_sched_unlock()` is
 * 255.
 *
 * Note: Locking the scheduler does not affect functions that explicitly yield the CPU,
 * such as `k_yield()`, `k_sleep()`, or any kernel function waiting for an event to be
 * signaled (e.g., `k_sem_take()`, `k_mutex_lock()` with delay).
 */
__kernel void k_sched_lock(void);

/**
 * @brief Unlock the scheduler for the current thread and set it as preemptive.
 *
 * This function unlocks the scheduler for the current thread, allowing it to be preempted
 * by other threads.
 *
 * Locking and unlocking the scheduler has no effect on cooperative threads.
 *
 * This function cannot be called from an interrupt routine.
 *
 * @see k_sched_lock()
 */
__kernel void k_sched_unlock(void);

/**
 * @brief Check if the scheduler is locked by the current thread.
 *
 * This function determines whether the scheduler is locked by the current thread or if
 * the thread is cooperative.
 *
 * @return bool True if the scheduler is locked for the current thread or if the thread is
 * cooperative, false otherwise.
 */
__kernel bool k_sched_locked(void);

/**
 * @brief Check if the current thread is preemptive.
 *
 * @return bool True if the current thread is preemptive.
 */
__kernel bool k_cur_is_preempt(void);

/**
 * @brief Check if the current thread is cooperative.
 *
 * @return bool True if the current thread is cooperative.
 */
__kernel bool k_cur_is_coop(void);

/**
 * @brief Restore the scheduler state.
 *
 * This function is used in the `K_SCHED_LOCK_CONTEXT` macro to restore the scheduler
 * state.
 *
 * @param __s Pointer to the scheduler state.
 */
static __inline__ void z_sched_restore(const uint8_t *__s)
{
    k_sched_unlock();
    memory_barrier();
    ARG_UNUSED(__s);
}

/**
 * @brief Lock the scheduler and return a value.
 *
 * This function is used in the `K_SCHED_LOCK_CONTEXT` macro to lock the scheduler and
 * return a value.
 *
 * @return uint8_t Always returns 1.
 */
static __inline__ uint8_t z_sched_lock_ret(void)
{
    k_sched_lock();
    return 1;
}

/**
 * @brief Macro to lock the scheduler within a specific scope.
 *
 * This macro locks the scheduler for the duration of the inner scope. The code within the
 * block is equivalent to the code between `k_sched_lock()` and `k_sched_unlock()`.
 *
 * This macro is inspired by the `ATOMIC_BLOCK(ATOMIC_FORCEON)` macro in `<avr/atomic.h>`.
 */
#define K_SCHED_LOCK_CONTEXT                                                             \
    for (uint8_t __k_schedl_x __attribute__((__cleanup__(z_sched_restore))) = 0u,        \
                              __k_todo = z_sched_lock_ret();                             \
         __k_todo; __k_todo            = 0)

/**
 * @brief Suspend the execution of the current thread for a specified amount of time.
 *
 * This function schedules the current thread to be woken up after the specified amount of
 * time.
 *
 * If the timeout is set to `K_FOREVER`, the thread will not be executed again until
 * explicitly resumed with `z_wake_up()` or `z_schedule()`.
 *
 * If the timeout is set to `K_NO_WAIT`, the function has no effect.
 *
 * @param timeout The duration of the sleep in milliseconds.
 */
__kernel void k_sleep(k_timeout_t timeout);

/**
 * @brief Sleep for a specified number of milliseconds.
 *
 * This function is a convenience wrapper around `k_sleep()` to simplify sleeping for
 * milliseconds.
 *
 * @param ms The duration of the sleep in milliseconds.
 */
__kernel void k_msleep(uint32_t milliseconds);

/**
 * @brief This mode keeps the thread ready but yields the CPU to another thread
 * if available. If no other thread is available, it puts the CPU into a sleep
 * state, making the system responsive to interrupts.
 */
#define K_WAIT_MODE_IDLE 0u

/**
 * @brief This mode keeps the thread ready and actively uses the CPU until the
 * timeout.
 * - If the thread is preemptive, the CPU will be shared with other threads.
 * - If the thread is cooperative, it will keep the CPU until the timeout,
 * blocking others. For cooperative threads, this mode is equivalent to
 * K_WAIT_BLOCK.
 */
#define K_WAIT_MODE_ACTIVE 1u

/**
 * @brief This mode actively waits for the specified number of milliseconds
 * by locking the CPU for the current thread.
 */
#define K_WAIT_MODE_BLOCK 2u

/**
 * @brief Make the thread wait for a specified amount of time in milliseconds while
 * keeping it ready.
 *
 * Unlike `k_sleep`, this function keeps the thread in the ready state while waiting. The
 * function polls the elapsed time and resumes execution when the wait time has passed.
 *
 * This function does not disable interrupts or break system time, but the chosen mode can
 * affect the execution of other threads.
 *
 * Note: This function can be used without an IDLE thread (with
 * `CONFIG_KERNEL_THREAD_IDLE` disabled).
 *
 * Note: Requires `KERNEL_UPTIME` to be enabled.
 *
 * @param timeout The duration to wait in milliseconds.
 * @param mode The wait mode (`K_WAIT_MODE_IDLE`, `K_WAIT_MODE_ACTIVE`, or
 * `K_WAIT_MODE_BLOCK`).
 */
__kernel void k_wait(k_timeout_t timeout, uint8_t mode);

/**
 * @brief Block the RTOS (scheduler + SYSCLOCK) for a specified amount of time in
 * microseconds.
 *
 * This function blocks the RTOS for a short delay. It should not be used for delays
 * longer than `CONFIG_KERNEL_SYSCLOCK_PERIOD_US` (converted to milliseconds), as it may
 * delay system uptime.
 *
 * This function relies on `_delay_us()` from `<util/delay.h>`, which can only be used in
 * release mode (not in debug mode).
 *
 * @param delay_us The duration to block in microseconds.
 */
__kernel void z_cpu_block_us(uint32_t delay_us);

/**
 * @brief Block the RTOS (scheduler + SYSCLOCK) for a specified amount of time in
 * milliseconds.
 *
 * This function blocks the RTOS for a short delay. It should not be used for delays
 * longer than `CONFIG_KERNEL_SYSCLOCK_PERIOD_US` (converted to milliseconds), as it may
 * delay system uptime.
 *
 * This function relies on `_delay_ms()` from `<util/delay.h>`, which can only be used in
 * release mode (not in debug mode).
 *
 * @param delay_ms The duration to block in milliseconds.
 */
__kernel void z_cpu_block_ms(uint32_t delay_ms);

/**
 * @brief Get the number of currently ready threads.
 *
 * This function returns the count of threads that are currently in the ready state,
 * meaning they are eligible for execution. If no threads are ready and the CPU is in the
 * IDLE state, it returns 0.
 *
 * @return uint8_t The number of currently ready threads.
 */
uint8_t k_ready_count(void);

/**
 * @brief Yield the CPU to another ready thread, assuming interrupts are disabled.
 *
 * This function yields the CPU to another thread while assuming that interrupts are
 * already disabled. It is similar to `k_yield()` but with this additional assumption.
 *
 * @see k_yield
 */
void z_yield(void);

/**
 * @brief Yield the CPU to the next thread in the scheduler's runqueue.
 *
 * This function releases the CPU by stopping the execution of the current thread. If the
 * thread is still in the ready state, it is placed at the end of the runqueue, allowing
 * other threads to execute before it.
 *
 * This function restores the context of the current thread when returning and can be
 * called from both cooperative and preemptive threads. It can also be called from ISRs.
 */
__always_inline void k_yield(void)
{
    const uint8_t key = irq_lock();
    z_yield();
    irq_unlock(key);
}

/**
 * @brief Yield function for Arduino compatibility.
 *
 * This function provides compatibility with Arduino and serves as a wrapper for the
 * `k_yield()` function.
 */
void yield(void);

/**
 * @brief Assertion helper to check if the code is running in user context.
 *
 * This function checks if the code is executing in user context. It is used to verify
 * that certain operations or code paths are accessed only in user context.
 */
extern void z_assert_user_context(void);

/**
 * @brief Assertion helper to check if a thread is in the ready state.
 *
 * This function checks if a given thread is in the ready state, meaning it is eligible
 * for execution. If the thread is not in the ready state, an assertion failure or error
 * is triggered.
 *
 * @param thread The thread to check.
 */
extern void z_assert_thread_ready(struct k_thread *thread);

/**
 * @brief Yield the interrupted thread to the next thread in the runqueue from an
 * interrupt context.
 *
 * This function should only be called from a USER interrupt routine. It should not be
 * called from a `k_timer` or `k_event` callback handler, as those handlers are called in
 * "kernel context" and scheduling threads from those handlers will cause an immediate
 * switch to the last unpended thread.
 *
 * Note: This function should be the last instruction in the interrupt routine, as any
 * code after it will be delayed. It is typically used in an ISR after waking up a thread
 * (e.g., `k_sem_give`) to immediately give the CPU to the woken-up thread.
 *
 * Inlining this function reduces the required stack size by 2 (or 3) bytes when the
 * interrupt is called.
 *
 * @example
 * ISR(USART0_RX_vect)
 * {
 *     const char flags = USART0_DEVICE->UDRn;
 *     ll_usart_sync_putc(USART0_DEVICE, flags);
 *
 *     int8_t ret = k_flags_notify(&flags, flags, K_FLAGS_SET);
 *     if (ret > 0) {
 *         k_yield_from_isr();
 *     }
 * }
 */
__always_inline void k_yield_from_isr(void)
{
    // ASSERT ISR CONTEXT
    // ASSERT IRQ LOCKED
    // ASSERT NOT kernel context

#if CONFIG_KERNEL_ASSERT
    z_assert_user_context();
#endif

    /* Check whether the current thread can be preempted */
    if ((z_ker.current->flags & (Z_THREAD_SCHED_LOCKED_MSK | Z_THREAD_PRIO_COOP)) == 0u) {
        z_yield();
    }
}

/**
 * @brief Yield the interrupted thread if a thread is provided as an argument.
 *
 * This function should only be called from a USER interrupt routine. It should not be
 * called from a `k_timer` or `k_event` callback handler, as those handlers are called in
 * "kernel context" and scheduling threads from those handlers will cause an immediate
 * switch to the last unpended thread.
 *
 * This function should be the last instruction in the interrupt routine, as any code
 * after it will be delayed. It is typically used in an ISR after waking up a thread
 * (e.g., `k_sem_give`) to immediately give the CPU to the woken-up thread.
 *
 * Inlining this function reduces the required stack size by 2 (or 3) bytes when the
 * interrupt is called.
 *
 * @param thread Pointer to the thread that is ready to be scheduled. If the thread is
 * `NULL`, this function does nothing.
 *
 * @example
 * ISR(INT0_vect)
 * {
 *     led_toggle();
 *     const struct k_thread *thread = k_sem_give(&button_sem);
 *     k_yield_from_isr_cond(thread);
 * }
 */
__always_inline void k_yield_from_isr_cond(struct k_thread *thread)
{
    if (thread != NULL) {
#if CONFIG_KERNEL_ASSERT
        z_assert_thread_ready(thread);
#endif

        k_yield_from_isr();
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_KERNEL_H */
