/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_MSGQ_H_
#define _AVRTOS_MSGQ_H_

#include <stdint.h>

#include <avrtos/dstruct/dlist.h>
#include <avrtos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_msgq {
	struct ditem waitqueue;

	size_t msg_size;
	uint8_t max_msgs;
	uint8_t used_msgs;

	void *buf_start;
	void *buf_end;

	void *read_cursor;
	void *write_cursor;
};

/*___________________________________________________________________________*/

#define K_MSGQ_INIT(msgq, p_buffer, d_msg_size, d_max_msgs)                              \
	{                                                                                \
		.waitqueue = DLIST_INIT(msgq.waitqueue), .msg_size = d_msg_size,         \
		.max_msgs = d_max_msgs, .used_msgs = 0, .buf_start = p_buffer,           \
		.buf_end     = p_buffer + (d_msg_size) * (d_max_msgs),                   \
		.read_cursor = p_buffer, .write_cursor = p_buffer,                       \
	}

#define K_MSGQ_DEFINE(msgq, p_buffer, d_msg_size, d_max_msgs)                            \
	struct k_msgq msgq = K_MSGQ_INIT(msgq, p_buffer, d_msg_size, d_max_msgs)

/*___________________________________________________________________________*/

/**
 * @brief Initialize a MsgQ
 *
 * @param msgq
 * @param buffer
 * @param msg_size
 * @param max_msgs
 * @return K_NOINLINE
 */
K_NOINLINE void
k_msgq_init(struct k_msgq *msgq, char *buffer, size_t msg_size, uint32_t max_msgs);

/**
 * @brief Try to append the message @a data to the MsgQ @a msgq.
 *
 * Try until timeout.
 *
 * Don't call from an ISR with timeout != K_NO_WAIT.
 *
 * @param msgq
 * @param data
 * @param timeout
 * @return K_NOINLINE 0 on success (ETIMEDOUT on timeout, ECANCELED on canceled
 *      ENOMSG on MsgQ full)
 */
K_NOINLINE int8_t k_msgq_put(struct k_msgq *msgq, const void *data, k_timeout_t timeout);

/**
 * @brief Try to get a message @a data to the MsgQ @a msgq.
 *
 *  Try until timeout.
 *
 * Don't call from an ISR with timeout != K_NO_WAIT.
 *
 * @param msgq
 * @param data
 * @param timeout
 * @return K_NOINLINE  0 on success (ETIMEDOUT on timeout, ECANCELED on canceled
 *      ENOMSG on no message)
 */
K_NOINLINE int8_t k_msgq_get(struct k_msgq *msgq, void *data, k_timeout_t timeout);

/**
 * @brief Cancel all thread pending on the MsgQ (reading or writing).
 * And reset the MsgQ state
 *
 * @param msgq
 * @return K_NOINLINE
 */
K_NOINLINE uint8_t k_msgq_purge(struct k_msgq *msgq);

/**
 * @brief Peek the first message of the MsgQ without removing it.
 *
 * @param msgq
 * @param data
 * @return K_NOINLINE 0 on success (ENOMSG on no message)
 */
K_NOINLINE int8_t k_msgq_peek(struct k_msgq *msgq, void *data);

/**
 * @brief Get number of free messages in the MsgQ
 *
 * @param msgq
 * @return K_NOINLINE
 */
K_NOINLINE uint8_t k_msgq_num_free_get(struct k_msgq *msgq);

/**
 * @brief Get the number of used messages in the MsgQ
 *
 * @param msgq
 * @return K_NOINLINE
 */
K_NOINLINE uint8_t k_msgq_num_used_get(struct k_msgq *msgq);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif