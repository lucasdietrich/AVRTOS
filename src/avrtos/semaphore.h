/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Semaphores
 *
 * A semaphore is a synchronization primitive that is used to manage access to a
 * shared resource by multiple threads. It maintains a counter representing the
 * number of available units of the resource, allowing threads to acquire or release
 * units of the resource. When the counter is greater than zero, a thread can
 * decrement the counter and proceed. If the counter is zero, the thread will block
 * until another thread releases a unit by incrementing the counter.
 *
 * Semaphores are useful for controlling access to a finite number of identical
 * resources, such as memory blocks or connection slots.
 *
 * They can also be used to synchronize threads (or thread with irq), as an
 * alternative to signals.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 */

#ifndef _AVRTOS_SEM_H
#define _AVRTOS_SEM_H

#include <stdbool.h>

#include <avr/io.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Kernel Semaphore structure
 */
struct k_sem {
    /**
     * @brief Current count of available semaphores.
     *
     * This value indicates how many semaphores are currently available.
     * If this count is greater than 0, a semaphore can be taken.
     */
    uint8_t count;

    /**
     * @brief Maximum limit of semaphores.
     *
     * This value represents the maximum number of semaphores that can
     * be available at any time.
     */
    uint8_t limit;

    /**
     * @brief Wait queue for threads waiting to acquire the semaphore.
     *
     * Threads waiting for the semaphore are stored in this list, with
     * the first thread to be woken up at the head of the queue.
     */
    struct dnode waitqueue;
};

/**
 * @brief Statically initialize a semaphore.
 *
 * This macro initializes a semaphore at compile time, setting its initial
 * count, maximum limit, and initializing the wait queue.
 *
 * @param sem The semaphore structure to be initialized.
 * @param initial_count Initial count of the semaphore.
 * @param count_limit Maximum count limit of the semaphore.
 */
#define Z_SEM_INIT(sem, initial_count, count_limit)                                      \
    {                                                                                    \
        .count = MIN(initial_count, count_limit), .limit = count_limit,                  \
        .waitqueue = DLIST_INIT(sem.waitqueue)                                           \
    }

/**
 * @brief Statically define and initialize a semaphore.
 *
 * This macro defines a semaphore structure and initializes it at compile time.
 *
 * @param sem_name Name of the semaphore structure.
 * @param initial_count Initial count of the semaphore.
 * @param count_limit Maximum count limit of the semaphore.
 */
#define K_SEM_DEFINE(sem_name, initial_count, count_limit)                               \
    struct k_sem sem_name = Z_SEM_INIT(sem_name, initial_count, count_limit)

/**
 * @brief Initialize a semaphore at runtime.
 *
 * This function initializes a semaphore structure, setting its initial count,
 * maximum limit, and initializing the wait queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param sem Pointer to the semaphore structure to be initialized.
 * @param initial_count Initial count of the semaphore.
 * @param limit Maximum count limit of the semaphore.
 * @return 0 on success, or -EINVAL if the semaphore pointer is NULL.
 */
int8_t k_sem_init(struct k_sem *sem, uint8_t initial_count, uint8_t limit);

/**
 * @brief Take a semaphore, with optional timeout.
 *
 * This function attempts to take a semaphore. If a semaphore is available,
 * it is taken immediately. If no semaphore is available, the calling thread
 * can wait until a semaphore is given or until the specified timeout expires.
 *
 * If timeout is different from K_FOREVER, the thread will be woken up when
 * the timeout expires, and it will check again for semaphore availability.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @param sem Pointer to the semaphore structure.
 * @param timeout Maximum time to wait for a semaphore to become available.
 *                Use K_NO_WAIT for non-blocking operation, or K_FOREVER to wait
 * indefinitely.
 * @return 0 if a semaphore was successfully taken, or an error code otherwise:
 *         - -EINVAL if the semaphore pointer is NULL.
 *         - -ETIMEDOUT if the timeout expired before a semaphore became available.
 */
__kernel int8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout);

/**
 * @brief Give a semaphore.
 *
 * This function releases a semaphore, potentially waking up a thread
 * waiting for the semaphore. If there are threads waiting in the wait queue,
 * the first thread in the queue is woken up.
 *
 * If the semaphore count is at its limit, giving a semaphore has no effect.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param sem Pointer to the semaphore structure.
 * @return Pointer to the thread that was woken up, or NULL if no threads were waiting.
 */
__kernel struct k_thread *k_sem_give(struct k_sem *sem);

/**
 * @brief Cancel waiting threads on a semaphore.
 *
 * This function cancels all threads currently waiting for the semaphore,
 * removing them from the wait queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param sem Pointer to the semaphore structure.
 * @return 0 on success, or an error code otherwise.
 */
__kernel int8_t k_sem_cancel_wait(struct k_sem *sem);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_SEM_H */
