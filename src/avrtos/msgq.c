/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "msgq.h"
#include <string.h>

#include "kernel.h"
#include "kernel_internals.h"

#define K_MODULE K_MODULE_MSGQ

int8_t k_msgq_init(struct k_msgq *msgq, char *buffer, size_t msg_size, uint32_t max_msgs)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!msgq || !buffer || !msg_size || !max_msgs) {
		return -EINVAL;
	}
#endif

	/* list of pending thread (on writing XOR on reading)
	 * depending on the nuber of messages used, we can assert that
	 * the pending threads need to write (msgq full)
	 * or read (msgq empty) */
	dlist_init(&msgq->waitqueue);

	msgq->max_msgs	= max_msgs;
	msgq->msg_size	= msg_size;
	msgq->used_msgs = 0;

	msgq->buf_start = buffer;
	msgq->buf_end	= buffer + ((msg_size) * (max_msgs));

	msgq->read_cursor  = buffer;
	msgq->write_cursor = buffer;

	return 0;
}

int8_t k_msgq_put(struct k_msgq *msgq, const void *data, k_timeout_t timeout)
{
	__ASSERT_NOTNULL(msgq);
	__ASSERT_NOTNULL(data);

	int8_t ret;

	const uint8_t key = irq_lock();

	if (msgq->used_msgs < msgq->max_msgs) {
		struct k_thread *pending_thread = z_unpend_first_thread(&msgq->waitqueue);
		if (pending_thread != NULL) {
			/* a thread is waiting to get a msg, we write directly
			 * the data to the thread buffer. Passed through
			 * swap_data
			 */
			memcpy(pending_thread->swap_data, data, msgq->msg_size);
		} else {
			/* no thread waiting on a msg, appending the msg to
			 * the msgq buffer */
			memcpy(msgq->write_cursor, data, msgq->msg_size);
			msgq->write_cursor = msgq->write_cursor + msgq->msg_size;
			if (msgq->write_cursor == msgq->buf_end) {
				msgq->write_cursor = msgq->buf_start;
			}
			msgq->used_msgs++;
		}
		ret = 0;
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		ret = -ENOMSG;
	} else {
		/* tells from were the data should be copied
		 * where there will be space in the msgq
		 */
		z_current->swap_data = (void *)data;

		ret = z_pend_current(&msgq->waitqueue, timeout);
	}

	irq_unlock(key);

	return ret;
}

__always_inline static int8_t z_msgq_get(struct k_msgq *msgq,
					 void *data,
					 k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(msgq);
	__ASSERT_NOTNULL(data);

	if (msgq->used_msgs > 0) {
		/* copy first message from the msgq to the thread */
		memcpy(data, msgq->read_cursor, msgq->msg_size);
		msgq->read_cursor = msgq->read_cursor + msgq->msg_size;
		if (msgq->read_cursor == msgq->buf_end) {
			msgq->read_cursor = msgq->buf_start;
		}
		msgq->used_msgs--;

		struct k_thread *pending_thread = z_unpend_first_thread(&msgq->waitqueue);
		if (pending_thread != NULL) {
			/* a thread is waiting to write a msg,
			 * we copy the data from the thread to the msgq
			 */
			memcpy(msgq->write_cursor, pending_thread->swap_data,
			       msgq->msg_size);
			msgq->write_cursor = msgq->write_cursor + msgq->msg_size;
			if (msgq->write_cursor == msgq->buf_end) {
				msgq->write_cursor = msgq->buf_start;
			}
			msgq->used_msgs++;
		}
		return 0;
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		return -ENOMSG;
	} else {
		/* tells were the data should be copied
		 * where there will be a new message in the msgq
		 */
		z_current->swap_data = data;

		return z_pend_current(&msgq->waitqueue, timeout);
	}
}

int8_t k_msgq_get(struct k_msgq *msgq, void *data, k_timeout_t timeout)
{
	int8_t ret;

	__ASSERT_NOTNULL(msgq);
	__ASSERT_NOTNULL(data);

	const uint8_t key = irq_lock();

	if (msgq->used_msgs > 0) {
		/* copy first message from the msgq to the thread */
		memcpy(data, msgq->read_cursor, msgq->msg_size);
		msgq->read_cursor = msgq->read_cursor + msgq->msg_size;
		if (msgq->read_cursor == msgq->buf_end) {
			msgq->read_cursor = msgq->buf_start;
		}
		msgq->used_msgs--;

		struct k_thread *pending_thread = z_unpend_first_thread(&msgq->waitqueue);
		if (pending_thread != NULL) {
			/* a thread is waiting to write a msg,
			 * we copy the data from the thread to the msgq
			 */
			memcpy(msgq->write_cursor, pending_thread->swap_data,
			       msgq->msg_size);
			msgq->write_cursor = msgq->write_cursor + msgq->msg_size;
			if (msgq->write_cursor == msgq->buf_end) {
				msgq->write_cursor = msgq->buf_start;
			}
			msgq->used_msgs++;
		}
		ret = 0;
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		ret = -ENOMSG;
	} else {
		/* tells were the data should be copied
		 * where there will be a new message in the msgq
		 */
		z_current->swap_data = data;

		ret = z_pend_current(&msgq->waitqueue, timeout);
	}

	irq_unlock(key);

	return ret;
}

uint8_t k_msgq_purge(struct k_msgq *msgq)
{
	__ASSERT_NOTNULL(msgq);

	uint8_t ret;

	const uint8_t key = irq_lock();

	/* reset state: no matter where the read/write cursor are */
	msgq->write_cursor = msgq->read_cursor;
	msgq->used_msgs	   = 0;

	ret = z_cancel_all_pending(&msgq->waitqueue);

	irq_unlock(key);

	return ret;
}

int8_t k_msgq_peek(struct k_msgq *msgq, void *data)
{
	__ASSERT_NOTNULL(msgq);

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
	__ASSERT_NOTNULL(msgq);

	uint8_t ret;

	const uint8_t key = irq_lock();

	ret = msgq->max_msgs - msgq->used_msgs;

	irq_unlock(key);

	return ret;
}
uint8_t k_msgq_num_used_get(struct k_msgq *msgq)
{
	__ASSERT_NOTNULL(msgq);

	uint8_t ret;

	const uint8_t key = irq_lock();

	ret = msgq->used_msgs;

	irq_unlock(key);

	return ret;
}
