/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Mutexes
 *
 * Related configuration options:
 * - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 * - CONFIG_KERNEL_REENTRANCY: Enable reentrant mutexes
 */

#ifndef _AVRTOS_MUTEX_H_
#define _AVRTOS_MUTEX_H_

#include <stdbool.h>

#include <avr/io.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Kernel Mutex structure
 *
 * This structure represents a mutex, which is a synchronization
 * primitive used to protect shared resources. The mutex can be locked
 * or unlocked, with a wait queue for threads that need to wait until
 * the mutex becomes available.
 */
struct k_mutex {
	/**
	 * @brief Lock status of the mutex.
	 *
	 * 0 indicates the mutex is unlocked, any other value indicates it is locked.
	 * If the CONFIG_KERNEL_REENTRANCY feature is enabled, this value represents
	 * the number of times the mutex has been locked by the owning thread.
	 */
	uint8_t lock;

	/**
	 * @brief Wait queue for threads waiting to acquire the mutex.
	 *
	 * Threads waiting for the mutex are stored in this list, with the first
	 * thread to be woken up at the head of the queue.
	 */
	struct dnode waitqueue;

	/**
	 * @brief Pointer to the thread that currently owns the mutex.
	 *
	 * This field is NULL if the mutex is not currently owned by any thread.
	 */
	struct k_thread *owner;
};

/**
 * @brief Statically initialize a mutex.
 *
 * This macro initializes a mutex at compile time, setting its lock status
 * to unlocked, initializing the wait queue, and setting the owner to NULL.
 *
 * @param mutex The mutex structure to be initialized.
 */
#define Z_MUTEX_INIT(mutex)                                                              \
	{                                                                                    \
		.lock = 0u, .waitqueue = DLIST_INIT(mutex.waitqueue), .owner = NULL              \
	}

/**
 * @brief Statically define and initialize a mutex.
 *
 * This macro defines a mutex structure and initializes it at compile time.
 *
 * @param mutex_name Name of the mutex structure.
 */
#define K_MUTEX_DEFINE(mutex_name) struct k_mutex mutex_name = Z_MUTEX_INIT(mutex_name)

/**
 * @brief Initialize a mutex at runtime.
 *
 * This function initializes a mutex structure, setting its lock status
 * to unlocked, initializing the wait queue, and setting the owner to NULL.
 *
 * @param mutex Pointer to the mutex structure to be initialized.
 * @return 0 on success, or -EINVAL if the mutex pointer is NULL.
 */
int8_t k_mutex_init(struct k_mutex *mutex);

/**
 * @brief Lock a mutex, with optional timeout.
 *
 * This function attempts to lock a mutex. If the mutex is already locked,
 * the calling thread can wait until the mutex becomes available or until
 * the specified timeout expires.
 *
 * If the CONFIG_KERNEL_REENTRANCY feature is enabled, a thread can lock
 * the mutex multiple times, and must unlock it the same number of times.
 * Otherwise, a thread SHALL NOT lock a mutex it already owns.
 *
 * Safety: This function is generally not safe to call from an ISR context.
 * 		   It becomes safe when the given timeout is K_NO_WAIT.
 *
 * @param mutex Pointer to the mutex structure.
 * @param timeout Maximum time to wait for the mutex to become available.
 *                Use K_NO_WAIT for non-blocking operation, or K_FOREVER to wait
 * indefinitely.
 * @return 0 if the mutex was successfully locked, or an error code otherwise:
 *         - -EINVAL if the mutex pointer is NULL.
 *         - -ETIMEDOUT if the timeout expired before the mutex became available.
 */
__kernel int8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout);

/**
 * @brief Unlock a mutex.
 *
 * This function releases the mutex, allowing other threads waiting on the mutex
 * to acquire it. If there are threads waiting in the wait queue, the first thread
 * in the queue is woken up.
 *
 * This function should only be called by the thread that currently owns the mutex.
 * Otherwise, no action is taken and the function returns NULL.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param mutex Pointer to the mutex structure.
 * @return Pointer to the thread that was woken up, or NULL if no threads were waiting
 * 		   , argument checks failed or the current thread does not own the mutex.
 */
__kernel struct k_thread *k_mutex_unlock(struct k_mutex *mutex);

/**
 * @brief Cancel waiting threads on a mutex.
 *
 * This function cancels all threads currently waiting for the mutex,
 * removing them from the wait queue.
 *
 * @param mutex Pointer to the mutex structure.
 * @return 0 on success, or an error code otherwise.
 */
__kernel int8_t k_mutex_cancel_wait(struct k_mutex *mutex);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_MUTEX_H_ */