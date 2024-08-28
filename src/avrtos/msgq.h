/*
 * Copyright (c) 2022-2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Message Queues
 *
 * A message queue is a communication mechanism used by threads to exchange data
 * asynchronously. The message queue structure holds a fixed number of messages,
 * with each message being of a constant size.
 * Threads can put messages into the queue or retrieve messages from it, with
 * synchronization ensuring safe concurrent access.
 * Allocation and deallocation operations are performed in constant time (O(1)).
 *
 * Example with a message queue (msgq) containing 4 messages of 16 bytes each and the flow
 * of messages:
 *
 *  | Slot 1	      | Slot 2          | Slot 3          | Slot 4
 *  | 0x0000		  | 0x0010          | 0x0020          | 0x0030
 *  +-----------------+-----------------+-----------------+-----------------+
 *  |   Message 1     |    Message 2    |   Message 3     |   Unoccupied    |
 *  |   Read Cursor   |                 |   Write Cursor  |                 |
 *  +-----------------+-----------------+-----------------+-----------------+
 *       |                    |                   |
 *       |                    |                   |
 *       v                    v                   v
 *  Outgoing Message    Stored Message       Incoming Message would
 * 	would be copied here  				     be copied here
 *
 * Threads can block on either write or read operations depending on the availability
 * of space or messages, ensuring efficient data exchange without data loss.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 */

#ifndef _AVRTOS_MSGQ_H_
#define _AVRTOS_MSGQ_H_

#include <stdint.h>

#include "dstruct/dlist.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Kernel Message Queue structure
 *
 * This structure represents a message queue that can hold a certain number of
 * fixed-size messages. The message queue supports thread synchronization, allowing
 * threads to wait for messages or for space to put messages.
 */
struct k_msgq {
	struct dnode waitqueue; /* Waitqueue for threads waiting for a message */
	size_t msg_size;		/* Size of a message */
	uint8_t max_msgs;		/* Maximum number of messages */
	uint8_t used_msgs;		/* Number of used messages */
	void *buf_start;		/* Pointer to the start of the buffer */
	void *buf_end;			/* Pointer to the end of the buffer */
	void *read_cursor;		/* Pointer to the current read position */
	void *write_cursor;		/* Pointer to the current write position */
};

#define Z_MSGQ_INIT(_name, _buffer, _msg_size, _max_msgs)                                \
	{                                                                                    \
		.waitqueue = DLIST_INIT(_name.waitqueue), .msg_size = _msg_size,                 \
		.max_msgs = _max_msgs, .used_msgs = 0, .buf_start = _buffer,                     \
		.buf_end = _buffer + (_msg_size) * (_max_msgs), .read_cursor = _buffer,          \
		.write_cursor = _buffer,                                                         \
	}

/**
 * @brief Statically define and initialize a message queue.
 *
 * This macro defines a message queue structure and initializes it at compile time.
 * It creates a buffer for storing the messages and sets up the message queue structure.
 *
 * @param _name Name of the message queue.
 * @param _msg_size Size of each message in the queue.
 * @param _max_msgs Maximum number of messages that can be stored in the queue.
 *
 * Example usage:
 * @code
 *  K_MSGQ_DEFINE(my_msgq, sizeof(int), 10);
 * @endcode
 */
#define K_MSGQ_DEFINE(_name, _msg_size, _max_msgs)                                       \
	uint8_t z_msgq_buf_##_name[(_msg_size) * (_max_msgs)];                               \
	struct k_msgq _name = Z_MSGQ_INIT(_name, z_msgq_buf_##_name, _msg_size, _max_msgs)

/**
 * @brief Initialize a message queue.
 *
 * This function initializes a message queue with the given buffer, message size,
 * and maximum number of messages.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param msgq Pointer to the message queue structure to be initialized.
 * @param buffer Pointer to the buffer where messages will be stored.
 * @param msg_size Size of each message in the queue.
 * @param max_msgs Maximum number of messages that can be stored in the queue.
 *
 * @return 0 on success
 * 		   -EINVAL if msgq, buffer, msg_size, or max_msgs is NULL
 *
 * Example usage:
 * @code
 *  struct k_msgq my_msgq;
 *  char my_buffer[10 * sizeof(int)];
 *  k_msgq_init(&my_msgq, my_buffer, sizeof(int), 10);
 * @endcode
 */
__kernel int8_t k_msgq_init(struct k_msgq *msgq,
							char *buffer,
							size_t msg_size,
							uint8_t max_msgs);

/**
 * @brief Try to append a message to the message queue.
 *
 * This function attempts to add a message to the queue. If the queue is full,
 * the calling thread can wait until space becomes available, depending on the
 * timeout value.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @param msgq Pointer to the message queue structure.
 * @param data Pointer to the message data to be added to the queue.
 * @param timeout Timeout value specifying how long to wait if the queue is full.
 *
 * @return 0 on success
 * 		   -EINVAL if msgq or data is NULL
 * 		   -ETIMEDOUT on timeout
 * 		   -ECANCELED if canceled
 * 		   -ENOMEM if no space in the queue
 *
 * Example usage:
 * @code
 *  int my_data = 42;
 *  k_msgq_put(&my_msgq, &my_data, K_NO_WAIT);
 * @endcode
 */
__kernel int8_t k_msgq_put(struct k_msgq *msgq, const void *data, k_timeout_t timeout);

/**
 * @brief Try to retrieve a message from the message queue.
 *
 * This function attempts to get a message from the queue. If the queue is empty,
 * the calling thread can wait until a message is available, depending on the timeout
 * value.
 *
 * @param msgq Pointer to the message queue structure.
 * @param data Pointer to the buffer where the retrieved message will be stored.
 * @param timeout Timeout value specifying how long to wait if the queue is empty.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @return 0 on success
 * 		   -EINVAL if msgq or data is NULL
 * 		   -ETIMEDOUT on timeout
 * 		   -ECANCELED if canceled
 * 		   -ENOMSG if no message available
 *
 * Example usage:
 * @code
 *  int my_data;
 *  k_msgq_get(&my_msgq, &my_data, K_NO_WAIT);
 * @endcode
 */
__kernel int8_t k_msgq_get(struct k_msgq *msgq, void *data, k_timeout_t timeout);

/**
 * @brief Cancel all pending threads on the message queue and reset the queue.
 *
 * This function cancels all threads that are currently waiting on the message queue
 * (either for reading or writing) and resets the queue to an empty state.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param msgq Pointer to the message queue structure.
 *
 * @return The number of threads that were canceled
 */
__kernel int8_t k_msgq_purge(struct k_msgq *msgq);

/**
 * @brief Peek the first message in the queue without removing it.
 *
 * This function retrieves the first message from the queue without removing it,
 * allowing the message to remain in the queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param msgq Pointer to the message queue structure.
 * @param data Pointer to the buffer where the peeked message will be stored.
 *
 * @return 0 on success
 * 		   -EINVAL if msgq or data is NULL
 * 		   -ENOMSG if no message available
 */
__kernel int8_t k_msgq_peek(struct k_msgq *msgq, void *data);

/**
 * @brief Get the number of free message slots in the queue.
 *
 * This function returns the number of available message slots in the queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param msgq Pointer to the message queue structure.
 *
 * @return The number of free message slots in the queue.
 */
__kernel uint8_t k_msgq_num_free_get(struct k_msgq *msgq);

/**
 * @brief Get the number of used message slots in the queue.
 *
 * This function returns the number of messages currently in the queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param msgq Pointer to the message queue structure.
 *
 * @return The number of used message slots in the queue.
 */
__kernel uint8_t k_msgq_num_used_get(struct k_msgq *msgq);

#ifdef __cplusplus
}
#endif

#endif