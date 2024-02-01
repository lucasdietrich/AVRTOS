/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
 * @brief Swaps the endianness of a memory address.
 *
 * This function swaps the endianness of a memory address. It is used to convert
 * data between big-endian and little-endian formats.
 *
 * @param addr Pointer to the memory address to swap endianness.
 */
__always_inline void swap_endianness(void **addr)
{
	*addr = (void *)HTONS(*addr);
}

/**
 * @brief Set the state of a thread.
 *
 * This function sets the state of a thread by updating the corresponding flags.
 *
 * @param thread Pointer to the thread structure.
 * @param state The new state of the thread.
 */
__always_inline void z_set_thread_state(struct k_thread *thread, uint8_t state)
{
	thread->flags = (thread->flags & ~Z_THREAD_STATE_MSK) | (state & Z_THREAD_STATE_MSK);
}

/**
 * @brief Get the state of a thread.
 *
 * This function retrieves the state of a thread by extracting the relevant bits
 * from the flags.
 *
 * @param thread Pointer to the thread structure.
 * @return The state of the thread.
 */
__always_inline uint8_t z_get_thread_state(struct k_thread *thread)
{
	return thread->flags & Z_THREAD_STATE_MSK;
}

/**
 * @def THREAD_IS_IDLE(_thread)
 * @brief Checks if a thread is the idle thread.
 *
 * This macro checks whether a given thread pointer corresponds to the idle
 * thread. The idle thread is a special system thread that runs when no other
 * threads are eligible for execution.
 *
 * @param _thread Pointer to the thread structure to be checked.
 * @return 1 if the thread is the idle thread, 0 otherwise.
 */
#if CONFIG_KERNEL_THREAD_IDLE
#define THREAD_IS_IDLE(_thread) (_thread == &z_thread_idle)
#else
#define THREAD_IS_IDLE(_thread) (0)
#endif

/**
 * @def THREAD_IS_MAIN(_thread)
 * @brief Checks if a thread is the main thread.
 *
 * This macro checks whether a given thread pointer corresponds to the main
 * thread.
 *
 * @param _thread Pointer to the thread structure to be checked.
 * @return 1 if the thread is the main thread, 0 otherwise.
 */
#define THREAD_IS_MAIN(_thread) (_thread == &z_thread_main)

/**
 * @brief Perform a thread switch (assembly function).
 *
 * This function performs a context switch between two threads. The steps
 * involved in the thread switch are as follows:
 *
 * 1. Save the context of the first thread.
 * 2. Store the stack pointer (SP) of the first thread to its structure.
 * 3. Restore the stack pointer (SP) of the second thread from its structure.
 * 4. Restore the context of the second thread.
 *
 * @param from Pointer to the thread structure of the first thread.
 * @param to Pointer to the thread structure of the second thread.
 */
extern void z_thread_switch(struct k_thread *from, struct k_thread *to);

/**
 * @brief Make the current thread waiting/pending for an object being available.
 *
 * Suspend the thread and add it to the waitqueue.
 * The function will return if the thread is awakaned or on timeout.
 *
 * If timeout is K_FOREVER, the thread should we awakaned.
 * If timeout is K_NO_WAIT, the thread returns immediately
 *
 * Assumptions :
 *  - interrupt flag is cleared when called.
 *
 * @param waitqueue
 * @param timeout
 * @return 0 on success (object available), ETIMEDOUT on timeout, negative error
 *  in other cases.
 */
__kernel int8_t z_pend_current(struct dnode *waitqueue, k_timeout_t timeout);

/**
 * @brief Wake up the first thread pending on an object.
 * Switch thread before returning.
 *
 * Assumptions :
 * - interrupt flag is cleared when called
 * - waitqueue is not null
 * - Thread in the runqueue is suspended
 *
 * @param waitqueue
 * @param swap_data : available object information
 * @return uint8_t return 0 if a thread got the object, any other value
 * otherwise
 */
__kernel struct k_thread *z_unpend_first_thread(struct dnode *waitqueue);

/**
 * @brief Wake up the first thread pending on an object.
 * Set the first pending thread swap_data parameter if set.
 * Switch thread before returning.
 *
 * @see z_unpend_first_thread
 *
 * Assumptions :
 * - interrupt flag is cleared when called
 * - waitqueue is not null
 * - Thread in the runqueue is suspended
 *
 * @param waitqueue
 * @param set_swap_data
 * @return thread pointer if a thread got the object, NULL otherwise
 */
__kernel struct k_thread *z_unpend_first_and_swap(struct dnode *waitqueue,
												  void *set_swap_data);

/**
 * @brief Cancel the first pending thread on a waitqueue.
 *
 * Raise "pend_canceled" flag on the thread, any wait function will return
 * with -ECANCELED.
 *
 * @param waitqueue
 */
__kernel void z_cancel_first_pending(struct dnode *waitqueue);

/**
 * @brief Cancel all pending threads on a waitqueue.
 *
 * Raise "pend_canceled" flag on the thread, any wait function will return
 * with -ECANCELED.
 *
 * @param waitqueue
 * @return Number of canceled threads
 */
__kernel uint8_t z_cancel_all_pending(struct dnode *waitqueue);

/**
 * @brief Wake up a thread.
 *
 * Cancel scheduled wake up if any and add the thread to the runqueue.
 *
 * @param thread
 */
__kernel void z_wake_up(struct k_thread *thread);

/**
 * @brief Helper function
 *
 * @return true, interrupt flag is set
 * @return false, interrupt flag is cleared
 */
bool z_interrupts(void);

#if defined(__cplusplus)
}
#endif

#endif /* _AVRTOS_KERNEL_INTERNALS_H */