/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "msgq.h"
#include <string.h>

#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_MSGQ

int8_t k_msgq_init(struct k_msgq *msgq, char *buffer, size_t msg_size, uint8_t max_msgs)
{
    Z_ARGS_CHECK(msgq && buffer && msg_size && max_msgs) return -EINVAL;

    /* List of pending threads (consumers or producers).
     * Depending on the number of messages used, we can determine if
     * the pending threads need to write (msgq full)
     * or read (msgq empty). */
    dlist_init(&msgq->waitqueue);

    msgq->max_msgs  = max_msgs;
    msgq->msg_size  = msg_size;
    msgq->used_msgs = 0;

    msgq->buf_start = buffer;
    msgq->buf_end   = buffer + ((msg_size) * (max_msgs));

    msgq->read_cursor  = buffer;
    msgq->write_cursor = buffer;

    return 0;
}

int8_t k_msgq_put(struct k_msgq *msgq, const void *data, k_timeout_t timeout)
{
    Z_ARGS_CHECK(msgq && data) return -EINVAL;

    int8_t ret;
    const uint8_t key = irq_lock();

    if (msgq->used_msgs < msgq->max_msgs) {
        /* There is space for the incoming message. */

        struct k_thread *pending_thread = z_unpend_first_thread(&msgq->waitqueue);
        if (pending_thread != NULL) {
            /* A consumer is waiting to get a message. We write the data
             * directly to the thread's buffer via swap_data.
             * This saves one memcpy operation into the msgq buffer.
             */
            memcpy(pending_thread->swap_data, data, msgq->msg_size);
        } else {
            /* No consumer is waiting to get a message. We write the data
             * to the msgq buffer. The consumer will copy it later.
             */
            memcpy(msgq->write_cursor, data, msgq->msg_size);
            msgq->write_cursor = (uint8_t *)msgq->write_cursor + msgq->msg_size;
            if (msgq->write_cursor == msgq->buf_end) {
                msgq->write_cursor = msgq->buf_start;
            }
            msgq->used_msgs++;
        }
        ret = 0;
    } else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
        /* No space is available, and the producer does not want to wait. */
        ret = -ENOMEM;
    } else {
        /* No space is available, but the producer is willing to wait. */

        /* Specifies where the data should be copied from
         * when space becomes available in the msgq.
         */
        z_ker.current->swap_data = (void *)data;

        /* Suspend the current thread until a consumer retrieves a message. */
        ret = z_pend_current_on(&msgq->waitqueue, timeout);
    }

    irq_unlock(key);

    return ret;
}

int8_t k_msgq_get(struct k_msgq *msgq, void *data, k_timeout_t timeout)
{
    Z_ARGS_CHECK(msgq && data) return -EINVAL;

    int8_t ret;
    const uint8_t key = irq_lock();

    if (msgq->used_msgs > 0) {
        /* There is a message to retrieve. */

        /* Copy the first message from the msgq to the thread. */
        memcpy(data, msgq->read_cursor, msgq->msg_size);
        msgq->read_cursor = sys_ptr_add(msgq->read_cursor, msgq->msg_size);
        if (msgq->read_cursor == msgq->buf_end) {
            msgq->read_cursor = msgq->buf_start;
        }

        struct k_thread *pending_thread = z_unpend_first_thread(&msgq->waitqueue);
        if (pending_thread != NULL) {
            /* A producer was waiting to write a message. We copy the data
             * from the thread to the msgq.
             */
            memcpy(msgq->write_cursor, pending_thread->swap_data, msgq->msg_size);
            msgq->write_cursor = sys_ptr_add(msgq->write_cursor, msgq->msg_size);
            if (msgq->write_cursor == msgq->buf_end) {
                msgq->write_cursor = msgq->buf_start;
            }
        } else {
            /* No producer was waiting to write a message,
             * so we just decrement the used_msgs counter.
             */
            msgq->used_msgs--;
        }
        ret = 0;
    } else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
        /* No message is available, and the consumer does not want to wait. */
        ret = -ENOMSG;
    } else {
        /* No message is available, but the consumer is willing to wait. */

        /* Specifies where the data should be copied
         * when a new message becomes available in the msgq.
         */
        z_ker.current->swap_data = data;

        ret = z_pend_current_on(&msgq->waitqueue, timeout);
    }

    irq_unlock(key);

    return ret;
}

int8_t k_msgq_purge(struct k_msgq *msgq)
{
    Z_ARGS_CHECK(msgq) return -EINVAL;

    int8_t ret;

    const uint8_t key = irq_lock();

    /* Reset state: no matter where the read/write cursors are. */
    msgq->write_cursor = msgq->read_cursor;
    msgq->used_msgs    = 0;

    /* Cancel all pending threads. Pending threads will be woken up
     * with -ECANCELED. */
    ret = (int8_t)z_cancel_all_pending(&msgq->waitqueue);

    irq_unlock(key);

    return ret;
}

int8_t k_msgq_peek(struct k_msgq *msgq, void *data)
{
    Z_ARGS_CHECK(msgq) return -EINVAL;

    uint8_t ret;

    const uint8_t key = irq_lock();

    if (msgq->used_msgs > 0) {
        memcpy(data, msgq->read_cursor, msgq->msg_size);

        ret = 0;
    } else {
        ret = -ENOMSG;
    }

    irq_unlock(key);

    return ret;
}

uint8_t k_msgq_num_free_get(struct k_msgq *msgq)
{
    Z_ARGS_CHECK(msgq) return -EINVAL;

    uint8_t ret;

    const uint8_t key = irq_lock();

    ret = msgq->max_msgs - msgq->used_msgs;

    irq_unlock(key);

    return ret;
}

uint8_t k_msgq_num_used_get(struct k_msgq *msgq)
{
    Z_ARGS_CHECK(msgq) return -EINVAL;

    uint8_t ret;

    const uint8_t key = irq_lock();

    ret = msgq->used_msgs;

    irq_unlock(key);

    return ret;
}
