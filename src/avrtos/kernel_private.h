/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Functions for which the usage is restricted to the kernel code */

#ifndef _AVRTOS_KERNEL_INTERNALS_H
#define _AVRTOS_KERNEL_INTERNALS_H

#include "assert.h"
#include "defines.h"
#include "sys.h"
#include "types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Swap the endianness of a memory address.
 *
 * This function converts the endianness of a memory address from big-endian to
 * little-endian, or vice versa. This is useful for data format conversion between
 * different endianness representations.
 *
 * @param addr Pointer to the memory address whose endianness is to be swapped.
 */
__always_inline void swap_endianness(void **addr)
{
    *addr = (void *)HTONS(*addr);
}

/**
 * @brief Set the state of a thread.
 *
 * This function updates the state of a thread by modifying the relevant flags in
 * the thread's structure.
 *
 * @param thread Pointer to the thread structure to be updated.
 * @param state The new state to set for the thread.
 */
__always_inline void z_set_thread_state(struct k_thread *thread, uint8_t state)
{
    thread->flags = (thread->flags & ~Z_THREAD_STATE_MSK) | (state & Z_THREAD_STATE_MSK);
}

/**
 * @brief Get the state of a thread.
 *
 * This function retrieves the current state of a thread by extracting the relevant
 * bits from the thread's flags.
 *
 * @param thread Pointer to the thread structure from which to get the state.
 * @return The current state of the thread.
 */
__always_inline uint8_t z_get_thread_state(struct k_thread *thread)
{
    return thread->flags & Z_THREAD_STATE_MSK;
}

/**
 * @def Z_THREAD_IS_IDLE(_thread)
 * @brief Check if a thread is the idle thread.
 *
 * This macro checks if the given thread pointer corresponds to the idle thread,
 * which is a special system thread that runs when no other threads are ready to execute.
 *
 * @param _thread Pointer to the thread structure to check.
 * @return 1 if the thread is the idle thread, 0 otherwise.
 */
#if CONFIG_KERNEL_THREAD_IDLE
#define Z_THREAD_IS_IDLE(_thread) (_thread == &z_thread_idle)
#else
#define Z_THREAD_IS_IDLE(_thread) (0)
#endif

/**
 * @def Z_THREAD_IS_MAIN(_thread)
 * @brief Check if a thread is the main thread.
 *
 * This macro checks if the given thread pointer corresponds to the main thread,
 * which is a primary thread in the system.
 *
 * @param _thread Pointer to the thread structure to check.
 * @return 1 if the thread is the main thread, 0 otherwise.
 */
#define Z_THREAD_IS_MAIN(_thread) (_thread == &z_thread_main)

/**
 * @brief Perform a context switch between two threads (assembly function).
 *
 * This function performs a context switch from one thread to another. The context
 * of the first thread is saved, and then the context of the second thread is restored.
 *
 * @param from Pointer to the thread structure of the current thread.
 * @param to Pointer to the thread structure of the thread to switch to.
 */
extern void z_thread_switch(struct k_thread *from, struct k_thread *to);

/**
 * @brief Suspend the current thread and wait for an object to become available.
 *
 * This function suspends the current thread and adds it to the wait queue. The thread
 * will resume execution when the object becomes available or when the specified timeout
 * expires.
 *
 * If the timeout is `K_FOREVER`, the thread will be awakened when the object becomes
 * available.
 *
 * If the timeout is `K_NO_WAIT`, the function returns immediately without waiting
 * with -EAGAIN as the return value.
 *
 * If waitqueue is NULL, the thread is suspended but not added to any wait queue.
 * Consequently, the thread must be woken up manually using `z_wake_up()`.
 *
 * Assumptions:
 * - The interrupt flag is cleared when this function is called.
 *
 * @param waitqueue Pointer to the wait queue to add the thread to.
 * @param timeout The timeout period to wait for the object.
 * @return 0 if the object became available
 * @return -EAGAIN if the timeout is K_NO_WAIT
 * @return -ETIMEDOUT if the timeout expired
 * @return -ECANCELED if the wait was canceled
 */
__kernel int8_t z_pend_current_on(struct dnode *waitqueue, k_timeout_t timeout);

/**
 * @brief Wake up the first thread waiting on an object and switch to it.
 *
 * This function wakes up the first thread that is waiting on the specified object and
 * switches to that thread. The `swap_data` parameter can be set with information about
 * the available object.
 *
 * Assumptions:
 * - The interrupt flag is cleared when this function is called.
 * - The wait queue is not null.
 * - The thread in the run queue is suspended.
 *
 * @param waitqueue Pointer to the wait queue to wake a thread from.
 * @param swap_data Pointer to the object information to set (if any).
 * @return Pointer to the thread that received the object, or NULL if no thread was woken.
 */
__kernel struct k_thread *z_unpend_first_and_swap(struct dnode *waitqueue,
                                                  void *swap_data);

/**
 * @brief Wake up the first thread waiting on an object.
 *
 * This function wakes up the first thread waiting on the specified object. The thread
 * will be added to the run queue, and a context switch will occur if necessary.
 *
 * Assumptions:
 * - The interrupt flag is cleared when this function is called.
 * - The wait queue is not null.
 * - The thread in the run queue is suspended.
 *
 * @param waitqueue Pointer to the wait queue to wake a thread from.
 * @return Pointer to the thread that received the object, or NULL if no thread was woken.
 */
__kernel struct k_thread *z_unpend_first_thread(struct dnode *waitqueue);

/**
 * @brief Cancel the first pending thread on a wait queue.
 *
 * This function sets the "pend_canceled" flag on the first thread pending on the
 * specified wait queue. Any wait function for that thread will return with -ECANCELED.
 *
 * @param waitqueue Pointer to the wait queue from which to cancel the pending thread.
 */
__kernel void z_cancel_first_pending(struct dnode *waitqueue);

/**
 * @brief Cancel all pending threads on a wait queue.
 *
 * This function sets the "pend_canceled" flag on all threads pending on the
 * specified wait queue. Any wait function for these threads will return with -ECANCELED.
 *
 * @param waitqueue Pointer to the wait queue from which to cancel all pending threads.
 * @return The number of threads that were canceled.
 */
__kernel uint8_t z_cancel_all_pending(struct dnode *waitqueue);

/**
 * @brief Wake up a thread.
 *
 * This function cancels any scheduled wake-up for the specified thread and adds it to
 * the run queue. A context switch may occur if necessary.
 *
 * @param thread Pointer to the thread to wake up.
 */
__kernel void z_wake_up(struct k_thread *thread);

/**
 * @brief Check if interrupts are enabled.
 *
 * This helper function checks whether interrupts are currently enabled.
 *
 * @return true if interrupts are enabled, false otherwise.
 */
bool z_interrupts(void);

/**
 * @brief Initialize stack sentinel bytes for all threads.
 *
 * This function sets up stack sentinel bytes for all threads in the system. Stack
 * sentinel bytes are used to detect stack overflows.
 */
__kernel void z_init_stacks_sentinel(void);

/**
 * @brief Initialize stack sentinel bytes for a specific thread.
 *
 * This function sets up stack sentinel bytes for the specified thread. Stack sentinel
 * bytes are used to detect stack overflows.
 *
 * @param thread Pointer to the thread whose stack sentinel bytes are to be initialized.
 */
__kernel void z_init_thread_stack_sentinel(struct k_thread *thread);

/**
 * @brief Verify if the stack sentinel bytes are intact for a given thread.
 *
 * This function checks if the stack sentinel bytes are still intact for the specified
 * thread. Stack sentinel bytes are used to detect stack overflows.
 *
 * @param thread Pointer to the thread whose stack sentinel bytes are to be verified.
 * @return true if the stack sentinel bytes are intact, false otherwise.
 */
__kernel bool z_thread_verify_sent(struct k_thread *thread);

#if defined(__cplusplus)
}
#endif

#endif /* _AVRTOS_KERNEL_INTERNALS_H */
