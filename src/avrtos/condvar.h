/*
 * Copyright (c) 2026 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Condition Variables
 *
 * A condition variable is a synchronization primitive that allows threads to
 * wait until a particular condition becomes true. It is always used in
 * conjunction with a mutex: the mutex protects the shared state that the
 * condition depends on, while the condition variable provides a way for
 * threads to sleep until that state changes.
 *
 * The canonical usage pattern for a waiting thread is:
 *
 *   k_mutex_lock(&mutex, K_FOREVER);
 *   while (!condition) {
 *       k_condvar_wait(&condvar, &mutex, K_FOREVER);
 *   }
 *   // ... act on condition ...
 *   k_mutex_unlock(&mutex);
 *
 * The predicate must always be re-checked in a loop after k_condvar_wait()
 * returns, because k_condvar_broadcast() wakes all waiting threads but only
 * one can satisfy the condition at a time.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 */

#ifndef _AVRTOS_CONDVAR_H_
#define _AVRTOS_CONDVAR_H_

#include <stdbool.h>

#include <avrtos/kernel.h>
#include <avrtos/mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Statically initialize a condition variable.
 *
 * This macro initializes a condition variable at compile time, setting up
 * its internal wait queue.
 *
 * @param _name The condition variable structure to be initialized.
 */
#define Z_CONDVAR_INIT(_name)                                                             \
    {                                                                                    \
        .waitqueue = DLIST_INIT(_name.waitqueue),                                         \
    }

/**
 * @brief Kernel Condition Variable structure
 */
struct k_condvar {
    /**
     * @brief Wait queue for threads pending on the condition variable.
     *
     * Threads waiting on the condition variable are stored in this list, with
     * the first thread to be woken up at the head of the queue.
     */
    struct dnode waitqueue;
};

/**
 * @brief Statically define and initialize a condition variable.
 *
 * This macro defines a condition variable structure and initializes it at
 * compile time.
 *
 * @param _name Name of the condition variable structure.
 */
#define K_CONDVAR_DEFINE(_name) struct k_condvar _name = Z_CONDVAR_INIT(_name);

/**
 * @brief Initialize a condition variable at runtime.
 *
 * This function initializes a condition variable structure, setting up its
 * internal wait queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param condvar Pointer to the condition variable structure to be initialized.
 * @return 0 on success, or -EINVAL if the condvar pointer is NULL.
 */
__kernel int k_condvar_init(struct k_condvar *condvar);

/**
 * @brief Wake all threads waiting on a condition variable.
 *
 * This function unblocks every thread currently suspended in the condition
 * variable's wait queue. Each woken thread will attempt to re-acquire the
 * mutex it released when it called k_condvar_wait(), and must re-check its
 * predicate before proceeding.
 *
 * If no threads are waiting, this function has no effect.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param condvar Pointer to the condition variable structure.
 * @return The number of threads that were woken up, or -EINVAL if the condvar
 *         pointer is NULL.
 */
__kernel int k_condvar_broadcast(struct k_condvar *condvar);

/**
 * @brief Wait on a condition variable.
 *
 * This function atomically releases the given mutex and suspends the calling
 * thread on the condition variable's wait queue. When the thread is woken up
 * (by k_condvar_broadcast()), it re-acquires the mutex before returning.
 *
 * The caller must hold the mutex before calling this function. Because
 * k_condvar_broadcast() wakes all waiting threads, the predicate should
 * always be re-checked in a loop after this function returns:
 *
 *   while (!condition) {
 *       k_condvar_wait(&condvar, &mutex, K_FOREVER);
 *   }
 *
 * Safety: This function is not safe to call from an ISR context if the
 *         timeout is different from K_NO_WAIT.
 *
 * @param condvar Pointer to the condition variable structure.
 * @param mutex   Pointer to the mutex currently held by the calling thread.
 *                It is released atomically before the thread is suspended and
 *                re-acquired before the function returns.
 * @param timeout Maximum time to wait for the condition variable to be
 *                signalled. Use K_NO_WAIT for non-blocking operation, or
 *                K_FOREVER to wait indefinitely.
 * @return 0 if the thread was woken by k_condvar_broadcast(), or an error
 *         code otherwise:
 *         - -EINVAL if condvar or mutex is NULL.
 *         - -ETIMEDOUT if the timeout expired before the condition variable
 *           was signalled.
 */
__kernel int
k_condvar_wait(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_CONDVAR_H_ */