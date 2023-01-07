/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fifo.h"

#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>
#include <avrtos/dstruct/oqueue.h>

#define K_MODULE    K_MODULE_FIFO

void k_fifo_init(struct k_fifo *fifo)
{
        oqref_init(&fifo->queue);
        dlist_init(&fifo->waitqueue);
}

struct k_thread *z_fifo_put(struct k_fifo *fifo, struct qitem *item)
{
	__ASSERT_NOINTERRUPT();

	/* If there is a thread pending on a fifo item,
	* we to give the item directly to the thread
	* (using thread->swap_data)
	*/
	struct k_thread *const thread =
		z_unpend_first_and_swap(&fifo->waitqueue, (void *)item);

	if (thread == NULL) {
			/* otherwise we queue the item to the fifo */
		oqueue(&fifo->queue, item);
	}

	return thread;
}

struct k_thread *k_fifo_put(struct k_fifo *fifo, struct qitem *item)
{
	__ASSERT_NOTNULL(fifo);
	__ASSERT_NOTNULL(item);

	const uint8_t lock = irq_lock();

	struct k_thread *const thread = z_fifo_put(fifo, item);

	irq_unlock(lock);

	return thread;
}

struct qitem *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout)
{
	__ASSERT_NOTNULL(fifo);

	const uint8_t lock = irq_lock();
	struct qitem *item = odequeue(&fifo->queue);

	if (item == NULL) {
		if (z_pend_current(&fifo->waitqueue, timeout) == 0) {
			item = (struct qitem *)z_current->swap_data;
		}
	}

	irq_unlock(lock);

	return item;
}

uint8_t k_fifo_cancel_wait(struct k_fifo *fifo)
{
	__ASSERT_NOTNULL(fifo);

	const uint8_t lock = irq_lock();

	const uint8_t ret = z_cancel_all_pending(&fifo->waitqueue);

	irq_unlock(lock);

	return ret;
}

bool k_fifo_is_empty(struct k_fifo *fifo)
{
        return k_fifo_peek_head(fifo) == NULL;
}

struct qitem *k_fifo_peek_head(struct k_fifo *fifo)
{
	const uint8_t lock = irq_lock();

	struct qitem *const item = opeek_head(&fifo->queue);

	irq_unlock(lock);

	return item;
}

struct qitem *k_fifo_peek_tail(struct k_fifo *fifo)
{
	const uint8_t lock = irq_lock();

	struct qitem *const item = opeek_tail(&fifo->queue);

	irq_unlock(lock);

	return item;
}