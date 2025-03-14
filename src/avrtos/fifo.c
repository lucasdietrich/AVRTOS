/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fifo.h"

#include "dstruct/slist.h"
#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_FIFO

int8_t k_fifo_init(struct k_fifo *fifo)
{
    Z_ARGS_CHECK(fifo) return -EINVAL;

    slist_init(&fifo->queue);
    dlist_init(&fifo->waitqueue);

    return 0;
}

struct k_thread *z_fifo_put(struct k_fifo *fifo, struct snode *item)
{
    __ASSERT_NOINTERRUPT();
    __ASSERT_NOTNULL(fifo);
    __ASSERT_NOTNULL(item);

    /* If there is a thread pending on a FIFO item,
     * give the item directly to the thread (using thread->swap_data)
     */
    struct k_thread *const thread =
        z_unpend_first_and_swap(&fifo->waitqueue, (void *)item);

    if (thread == NULL) {
        /* Otherwise, queue the item to the FIFO */
        slist_append(&fifo->queue, item);
    }

    return thread;
}

struct k_thread *k_fifo_put(struct k_fifo *fifo, struct snode *item)
{
    Z_ARGS_CHECK(fifo) return NULL;

    const uint8_t key = irq_lock();

    struct k_thread *const thread = z_fifo_put(fifo, item);

    irq_unlock(key);

    return thread;
}

struct snode *k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout)
{
    Z_ARGS_CHECK(fifo) return NULL;

    const uint8_t key  = irq_lock();
    struct snode *item = slist_get(&fifo->queue);

    if (item == NULL) {
        if (z_pend_current_on(&fifo->waitqueue, timeout) == 0) {
            item = (struct snode *)z_ker.current->swap_data;
        }
    }

    irq_unlock(key);

    return item;
}

int8_t k_fifo_cancel_wait(struct k_fifo *fifo)
{
    Z_ARGS_CHECK(fifo) return -EINVAL;

    const uint8_t key = irq_lock();

    int8_t ret = (int8_t)z_cancel_all_pending(&fifo->waitqueue);

    irq_unlock(key);

    return ret;
}

bool k_fifo_is_empty(struct k_fifo *fifo)
{
    Z_ARGS_CHECK(fifo) return true;

    return k_fifo_peek_head(fifo) == NULL;
}

struct snode *k_fifo_peek_head(struct k_fifo *fifo)
{
    Z_ARGS_CHECK(fifo) return NULL;

    const uint8_t key = irq_lock();

    struct snode *item = slist_peek_head(&fifo->queue);

    irq_unlock(key);

    return item;
}

struct snode *k_fifo_peek_tail(struct k_fifo *fifo)
{
    Z_ARGS_CHECK(fifo) return NULL;

    const uint8_t key = irq_lock();

    struct snode *item = slist_peek_tail(&fifo->queue);

    irq_unlock(key);

    return item;
}
