/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * FIFOs (First-In, First-Out Queues)
 *
 * A FIFO (First-In, First-Out) queue is a data structure used to manage a sequence of
 * elements where the first element added is the first one to be removed. This is
 * particularly useful in scenarios where data needs to be processed in the order it
 * was received, such as message passing between threads or buffering of input/output
 * data.
 *
 * The FIFO in this implementation supports operations for adding items to the queue,
 * retrieving items from the queue, and inspecting the queue's state (e.g., checking
 * if it is empty). Additionally, it manages a wait queue for threads that are waiting
 * to retrieve items, allowing threads to block until an item becomes available.
 *
 * Example usage of a FIFO:
 *
 * The FIFO structure requires that items have a special "tie" member for internal
 * management, meaning that only the "tie" portion of the item is directly managed
 * by the FIFO. Users must define items with the following layout:
 *
 * ```c
 * struct my_item {
 *    struct snode tie;
 *   // Other item data
 *    int data1;
 *    int data2;
 * };
 *
 * K_FIFO_DEFINE(my_fifo);
 *
 * void thread1(void) {
 *   struct my_item item;
 *   item.data1 = 42;
 *   item.data2 = 43;
 *   k_fifo_put(&my_fifo, &item.tie);
 * }
 *
 * void thread2(void) {
 *  struct snode *tie = k_fifo_get(&my_fifo, K_FOREVER);
 *  struct my_item *item = CONTAINER_OF(tie, struct my_item, tie);
 *  // Process item
 * }
 * ```
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 */

#ifndef _AVRTOS_FIFO_H
#define _AVRTOS_FIFO_H

#include <stdbool.h>
#include <stdint.h>

#include "dstruct/slist.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Kernel FIFO structure
 *
 * A FIFO (First-In, First-Out) queue is a data structure used to manage
 * ordered sequences of elements. Items are added to the end of the queue
 * and removed from the front. The FIFO structure also contains a wait queue
 * for threads that are waiting for an item to become available.
 */
struct k_fifo {
    struct slist queue;     ///< FIFO reference to the head item
    struct dnode waitqueue; ///< Wait queue for threads waiting for a FIFO item
};

/**
 * @brief Statically initialize a FIFO.
 *
 * This macro initializes a FIFO at compile time, setting up its queue and wait queue.
 *
 * @param fifo The FIFO structure to be initialized.
 */
#define Z_FIFO_INIT(fifo)                                                                \
    {                                                                                    \
        .queue = SLIST_INIT(), .waitqueue = DLIST_INIT(fifo.waitqueue)                   \
    }

/**
 * @brief Statically define and initialize a FIFO.
 *
 * This macro defines a FIFO structure and initializes it at compile time.
 *
 * @param fifo_name Name of the FIFO structure.
 */
#define K_FIFO_DEFINE(fifo_name) struct k_fifo fifo_name = Z_FIFO_INIT(fifo_name)

/**
 * @brief Initialize a FIFO at runtime.
 *
 * This function initializes a FIFO structure, setting up its queue and wait queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure to be initialized.
 * @return 0 on success, or -EINVAL if the FIFO pointer is NULL.
 */
__kernel int8_t k_fifo_init(struct k_fifo *fifo);

/**
 * @brief Add an item to the FIFO.
 *
 * This function adds an item to the end of the FIFO queue. If there are threads
 * waiting for an item, the first thread in the wait queue is woken up.
 *
 * @note The first two bytes of the item's memory space (item_tie) are reserved
 * for internal use and cannot store data.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure.
 * @param item_tie Pointer to the "tie" member of the item to add.
 * @return Pointer to the thread that was woken up, or NULL if no thread was pending.
 */
__kernel struct k_thread *k_fifo_put(struct k_fifo *fifo, struct snode *item_tie);

/**
 * @brief Add an item to the FIFO, assuming interrupts are disabled.
 *
 * This is a variant of k_fifo_put() that should be used when interrupts are disabled.
 *
 * Safety: This function is not safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure.
 * @param item Pointer to the item to add.
 * @return Pointer to the thread that was woken up, or NULL if no thread was pending.
 */
__kernel struct k_thread *z_fifo_put(struct k_fifo *fifo, struct snode *item);

/**
 * @brief Get and remove an item from the FIFO.
 *
 * This function removes an item from the front of the FIFO queue. If the FIFO
 * is empty and the timeout is different from K_NO_WAIT, the calling thread
 * will block until an item is added or the timeout expires.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @param fifo Pointer to the FIFO structure.
 * @param timeout Maximum time to wait for an item to become available.
 * @return Pointer to the item if successful, or NULL on timeout or error.
 */
__kernel struct snode *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout);

/**
 * @brief Cancel pending threads waiting on the FIFO.
 *
 * This function causes the first thread pending on the FIFO, if any, to return
 * from k_fifo_get() with a NULL value, as if the timeout expired.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure.
 * @return The number of threads that were woken up.
 */
__kernel int8_t k_fifo_cancel_wait(struct k_fifo *fifo);

/**
 * @brief Check if the FIFO is empty.
 *
 * This function returns true if the FIFO is empty, and false otherwise.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure.
 * @return True if the FIFO is empty, false otherwise.
 */
__kernel bool k_fifo_is_empty(struct k_fifo *fifo);

/**
 * @brief Peek at the head of the FIFO without removing the item.
 *
 * This function returns a pointer to the first item in the FIFO without removing it.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure.
 * @return Pointer to the head item, or NULL if the FIFO is empty.
 */
__kernel struct snode *k_fifo_peek_head(struct k_fifo *fifo);

/**
 * @brief Peek at the tail of the FIFO without removing the item.
 *
 * This function returns a pointer to the last item in the FIFO without removing it.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param fifo Pointer to the FIFO structure.
 * @return Pointer to the tail item, or NULL if the FIFO is empty.
 */
__kernel struct snode *k_fifo_peek_tail(struct k_fifo *fifo);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_FIFO_H */