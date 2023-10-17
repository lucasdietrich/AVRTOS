/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_WORKQUEUE
#define _AVRTOS_WORKQUEUE

#include <stddef.h>

#include "event.h"
#include "fifo.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct k_work;

typedef void (*k_work_handler_t)(struct k_work *);

struct k_work {
	struct snode _tie;
	k_work_handler_t handler;
};

#define K_WORK_INIT(work_handler)                              \
	{                                                      \
		._tie = SNODE_INIT(), .handler = work_handler, \
	}
#define K_WORK_INITIALIZER(work_handler) K_WORK_INIT(work_handler)

#define K_WORK_DEFINE(work_name, work_handler) \
	struct k_work work_name = K_WORK_INIT(work_handler)

struct k_workqueue {
	struct k_fifo q;
	uint8_t flags;
};

#define k_workq k_workqueue

#define K_WORKQUEUE_DEFINE(_name, _stack_size, _prio_flags, _symbol)                  \
	struct k_workqueue _name = {                                                  \
		.q     = K_FIFO_INIT(_name.q),                                        \
		.flags = 0u,                                                          \
	};                                                                            \
	K_THREAD_DEFINE(z_workq_##_name, z_workqueue_entry, _stack_size, _prio_flags, \
			&_name, _symbol)

//
// Workqueue internal
//

__kernel void z_workqueue_entry(struct k_workqueue *const workqueue);

/**
 * @brief Create a workqueue thread at runtime.
 *
 * @param workqueue workqueue struct
 * @param thread thread struct
 * @param stack stack buffer
 * @param stack_size stack size
 * @param prio_flags thread priority and flags
 * @param symbol thread symbol
 * @return __kernel 0 if success, negative error code otherwise
 */
__kernel int8_t k_workqueue_create(struct k_workqueue *workqueue,
				   struct k_thread *thread,
				   uint8_t *stack,
				   size_t stack_size,
				   uint8_t prio_flags,
				   char symbol);

/**
 * @brief Initialize a workqueue item at runtime.
 * k_work items can also be defined at compilation time using K_WORK_DEFINE
 *
 * @param work work item
 * @param handler handler
 */
__kernel void k_work_init(struct k_work *work, k_work_handler_t handler);

/**
 * @brief Submit a work item to the desired workqueue.
 *
 * If the work item has already been queued but didn't get processed, the
 * work is not sent a new time.
 *
 * A work item started being processed can be submitted again.
 *
 * A work item can be sent from an interrupt handler.
 * With "signals", this is the recommanded way to handle interrupts.
 *
 * Assumptions :
 *  - workqueue is not null
 *  - work is not null
 *  - work->handler is not null
 *
 * @param workqueue
 * @param work
 */
__kernel bool k_work_submit(struct k_workqueue *workqueue, struct k_work *work);

/**
 * @brief Configure the workqueue to release the cpu after each work item
 * being processed.
 *
 * Prevent cooperative thread workqueue from keeping the cpu for too long
 * if a lot of work items need to be processed.
 *
 * @param workqueue
 */
__kernel void k_workqueue_enable_yieldeach(struct k_workqueue *workqueue);

/**
 * @brief Configure the workqueue to not release the cpu after each work
 * item if there are more to be processed.
 *
 * Assumptions :
 *  - work is not null
 *  - work->handler is not null
 *
 * @see Undo k_workqueue_enable_yieldeach
 *
 * @param workqueue
 */
__kernel void k_workqueue_disable_yieldeach(struct k_workqueue *workqueue);

//
// System workqueue
//

/**
 * @brief Send a work item to the system workqueue
 *
 * @param work
 */
bool k_system_workqueue_submit(struct k_work *work);

/**
 * @brief Delayable workqueue item.
 * - Underlying work item to queue after timeout
 * - Event object triggering the work item
 * - Workqueue to queue the work item
 */
struct k_work_delayable {
	struct k_work work;
	struct k_event _event;
	struct k_workqueue *_workqueue;
};

#define K_WORK_DELAYABLE_INIT(work_handler)                                      \
	{                                                                        \
		.work = K_WORK_INIT(work_handler), ._event = K_EVENT_INIT(NULL), \
		._workqueue = NULL,                                              \
	}

#define K_WORK_DELAYABLE_DEFINE(name, work_handler) \
	struct k_work_delayable name = K_WORK_DELAYABLE_INIT(work_handler)

/**
 * @brief Initialize a delayable workqueue item at runtime.
 * k_work_delayable items can also be defined at compilation time using
 * K_WORK_DELAYABLE_DEFINE
 *
 * @param work work item
 * @param handler handler
 */
__kernel void k_work_delayable_init(struct k_work_delayable *dwork,
				    k_work_handler_t handler);

/**
 * @brief Schedule a delayable work item to be queued after the given timeout.
 *
 * If the work item has already been scheduled or queued for processing, the
 * work is not sent a new time and the function returns -EAGAIN.
 *
 * @param workqueue Workqueue to queue the work item
 * @param dwork Delayable work item
 * @param timeout Timeout before the work item is queued
 * @return 0 if success, negative error code otherwise
 */
__kernel int k_work_delayable_schedule(struct k_workqueue *workqueue,
				       struct k_work_delayable *dwork,
				       k_timeout_t timeout);

/**
 * @brief Schedule a delayable work item for the system workqueue.
 *
 * @see k_work_delayable_schedule
 *
 * @param dwork
 * @param timeout
 * @return __kernel
 */
__kernel int k_system_work_delayable_schedule(struct k_work_delayable *dwork,
					      k_timeout_t timeout);

/**
 * @brief Cancel a delayable work item which has been scheduled but not already
 * queued for processing.
 *
 * If the work item is not yet scheduled, the function returns -EAGAIN.
 * - If the work item is scheduled, the submission is cancelled and the function
 * returns 0.
 * - If the work item not schedule but still in the queue for processing, the
 * function returns -EBUSY.
 * - If the work item is not schedule and not in the queue for processing, the
 * function returns -EAGAIN.
 *
 * @param dwork
 * @return 0 if success, negative error code otherwise
 */
__kernel int k_work_delayable_cancel(struct k_work_delayable *dwork);

#ifdef __cplusplus
}
#endif

#endif