/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_FIFO_H
#define _AVRTOS_FIFO_H

#include <stdbool.h>
#include <stdint.h>

#include <avrtos/dstruct/oqueue.h>
#include <avrtos/multithreading.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Structure representing a fifo.
 *
 * Queue lists all items added to the fifo in order.
 *
 * Waitqueue list contains all threads pending for a fifo item.
 */
struct k_fifo {
	struct oqref queue;	// fifo reference to head item
	struct ditem waitqueue; // waitqueue of thread pending to get an item
};

/*___________________________________________________________________________*/

#define K_FIFO_INIT(fifo)                                                                \
	{                                                                                \
		.queue = INIT_OQREF(), .waitqueue = DLIST_INIT(fifo.waitqueue)           \
	}

#define K_FIFO_DEFINE(fifo_name) struct k_fifo fifo_name = K_FIFO_INIT(fifo_name)

/*___________________________________________________________________________*/

/**
 * @brief Initialize a fifo if it wasn't defined using the macro K_FIFO_DEFINE.
 *
 * @param fifo
 * @return K_NOINLINE
 */
K_NOINLINE void k_fifo_init(struct k_fifo *fifo);

/**
 * @brief Add an item to the fifo.
 *
 * Wake up the first thread pending on a fifo item.
 *
 * Switch thread before returning if a thread is pending on the item.
 *
 * Can be called from an interrupt routine.
 *
 * IMPORTANT : first two bytes of the memory space {item_tie} are reserved
 * for internal usage and cannot be use to store data.
 *
 * Note :
 * Actually it's not the item itself which is added to the fifo but its "tie"
 * member `item_tie` (first 2 bytes). As for workqueue we're using the
 * CONTAINER_OF paragigm. For each item added to the fifo, you must define a
 * structure containing this "tie" and the actual item data. See example
 * "fifo-slab" or "interrupt-yield".
 *
 * Reference design :
 *
 * struct
 * {
 *  struct qitem tie;
 *  uint8_t value;
 * } block1;
 * block1.data = 17u;
 * k_fifo_put(&myfifo, &block1);
 *
 * Note : in Zephyr RTOS project, there are two functions :
 * - k_fifo_put : which needs the first word of the data to be reserved
 *  -
 * https://github.com/zephyrproject-rtos/zephyr/blob/main/include/kernel.h#L2074-L2088
 *
 * - k_fifo_alloc_put : which implicitely allocat this internal "tie" member.
 *  -
 * https://github.com/zephyrproject-rtos/zephyr/blob/main/include/kernel.h#L2095-L2111
 *
 * @param fifo
 * @param item_tie
 * @return K_NOINLINE
 */
K_NOINLINE struct k_thread *k_fifo_put(struct k_fifo *fifo, struct qitem *item_tie);

/**
 * @brief @see k_fifo_put
 *
 * Assume interrupts are disabled
 *
 * @param fifo
 * @param item
 * @return K_NOINLINE
 */
K_NOINLINE struct k_thread *z_fifo_put(struct k_fifo *fifo, struct qitem *item);

/**
 * @brief Get and remove an item from the fifo.
 *
 * If there is no item in the fifo and timeout is different from K_NO_WAIT,
 * the function is blocking until a item is added or timeout.
 *
 * Cannot be called from an interrupt routine
 * if timeout is different from K_NO_WAIT.
 *
 * @param fifo
 * @param timeout
 * @return K_NOINLINE struct* not null if success
 */
K_NOINLINE struct qitem *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout);

/**
 * @brief Cancel pending on a fifo queue.
 * This routine causes first thread pending on fifo, if any,
 * to return from k_fifo_get() call with NULL value (as if timeout expired).
 *
 * @param fifo
 * @return K_NOINLINE
 */
K_NOINLINE uint8_t k_fifo_cancel_wait(struct k_fifo *fifo);

/**
 * @brief Tells if the fifo is empty.
 *
 * @param fifo
 * @return K_NOINLINE
 */
K_NOINLINE bool k_fifo_is_empty(struct k_fifo *fifo);

/**
 * @brief Get without removing the first item in the fifo.
 *
 * @param fifo
 * @return K_NOINLINE struct*
 */
K_NOINLINE struct qitem *k_fifo_peek_head(struct k_fifo *fifo);

/**
 * @brief Get without removing the last item in the fifo.
 *
 * @param fifo
 * @return K_NOINLINE struct*
 */
K_NOINLINE struct qitem *k_fifo_peek_tail(struct k_fifo *fifo);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif