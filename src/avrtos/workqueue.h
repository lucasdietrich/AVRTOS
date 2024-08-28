/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Workqueues
 *
 * A workqueue executes deferred tasks or background processing. Workqueues allow you
 * to offload work from interrupt handlers or other high-priority contexts to a
 * lower-priority thread, helping to keep time-sensitive operations responsive and
 * efficient.
 *
 * Workqueues operate by maintaining a queue of work items, each of which specifies a
 * handler function to be executed. Work items can be submitted to a workqueue for
 * processing, either immediately or after a specified delay, allowing for flexible
 * scheduling of tasks. Once a work item is processed, the workqueue can continue with
 * the next item or yield the CPU, depending on its configuration.
 *
 * Key Features:
 * - **Immediate and Delayed Work Items**: Work items can be processed immediately or
 *   scheduled to run after a specified timeout, providing precise control over task
 *   timing.
 * - **System and Custom Workqueues**: The system provides a global workqueue, but
 *   users can also define custom workqueues tailored to specific application needs.
 * - **Flexible Configuration**: Workqueues can be configured to yield the CPU after
 *   processing each item, preventing long-running workqueues from blocking other tasks.
 * - **Interrupt-Safe Submission**: Work items can be submitted from interrupt contexts.
 *
 * Example Usage:
 *
 *  - A work item can be created and submitted to a workqueue to perform background
 *    processing without blocking the main application flow.
 *  - Delayed work items allow tasks to be scheduled for future execution, useful for
 *    implementing time-based events or retries.
 *
 * Related configuration options:
 * 	- CONFIG_SYSTEM_WORKQUEUE_ENABLE: Enable the system workqueue for global work
 * processing.
 *  - CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE: Size of the stack for the system workqueue
 * thread.
 *  - CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE: Use cooperative scheduling for the system
 * workqueue.
 *  - CONFIG_WORKQUEUE_DELAYABLE: Enable support for delayable work items in workqueues.
 * 	  							  Requires CONFIG_KERNEL_EVENT.
 */

#ifndef _AVRTOS_WORKQUEUE_H_
#define _AVRTOS_WORKQUEUE_H_

#include <stddef.h>

#include "event.h"
#include "fifo.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct k_work;

/**
 * @brief Work handler function type.
 *
 * This function type defines the signature for work handler functions that
 * are executed by a workqueue when a work item is processed.
 *
 * @param work Pointer to the work item being processed.
 */
typedef void (*k_work_handler_t)(struct k_work *);

/**
 * @brief Work item structure.
 *
 * A `k_work` structure represents a unit of work to be processed by a workqueue.
 * It contains a handler function that is invoked when the work item is processed.
 */
struct k_work {
	struct snode _tie;		  ///< Node for linking work items in the queue.
	k_work_handler_t handler; ///< Handler function to process the work item.
};

/**
 * @brief Statically initialize a work item.
 *
 * This macro initializes a `k_work` structure with the specified work handler.
 *
 * @param work_handler The function that will handle the work item.
 */
#define Z_WORK_INIT(work_handler)                                                        \
	{                                                                                    \
		._tie = SNODE_INIT(), .handler = work_handler,                                   \
	}

/**
 * @brief Statically define and initialize a work item.
 *
 * This macro defines and initializes a `k_work` structure with the specified handler.
 *
 * @param work_name Name of the work item.
 * @param work_handler Function to handle the work item.
 */
#define K_WORK_DEFINE(work_name, work_handler)                                           \
	struct k_work work_name = Z_WORK_INIT(work_handler)

/**
 * @brief Workqueue structure.
 *
 * A workqueue is responsible for managing and processing a queue of work items.
 * It runs in its own thread and processes items in the order they are submitted.
 */
struct k_workqueue {
	struct k_fifo q; ///< Queue for storing work items.
	uint8_t flags;	 ///< Workqueue flags for configuration.
};

/**
 * @brief Macro alias for `k_workqueue`.
 *
 * This macro provides an alias for the `k_workqueue` structure.
 */
#define k_workq k_workqueue

/**
 * @brief Statically define and initialize a workqueue.
 *
 * This macro defines a workqueue and creates a thread to process its work items.
 *
 * @param _name Name of the workqueue.
 * @param _stack_size Size of the stack for the workqueue thread.
 * @param _prio_flags Priority and flags for the workqueue thread.
 * @param _symbol Symbol to represent the workqueue thread.
 */
#define K_WORKQUEUE_DEFINE(_name, _stack_size, _prio_flags, _symbol)                     \
	struct k_workqueue _name = {                                                         \
		.q	   = Z_FIFO_INIT(_name.q),                                                   \
		.flags = 0u,                                                                     \
	};                                                                                   \
	K_THREAD_DEFINE(z_workq_##_name, z_workqueue_entry, _stack_size, _prio_flags,        \
					&_name, _symbol)

//
// Workqueue internal
//

/**
 * @brief Entry function for workqueue threads.
 *
 * This function runs in the context of the workqueue's thread and processes
 * work items from the workqueue's queue.
 *
 * @param workqueue Pointer to the workqueue structure.
 */
__kernel void z_workqueue_entry(struct k_workqueue *const workqueue);

/**
 * @brief Create a workqueue thread at runtime.
 *
 * This function initializes a workqueue and creates a thread to process work items.
 * 
 * Safety: This function is safe but discouraged to call from an ISR context.
 *
 * @param workqueue Pointer to the workqueue structure.
 * @param thread Pointer to the thread structure to be used for the workqueue.
 * @param stack Pointer to the stack memory for the workqueue thread.
 * @param stack_size Size of the stack memory.
 * @param prio_flags Priority and flags for the workqueue thread.
 * @param symbol Symbol to represent the workqueue thread.
 * @return 0 on success, or a negative error code on failure.
 * @return -EINVAL if any of the arguments are invalid.
 */
__kernel int8_t k_workqueue_create(struct k_workqueue *workqueue,
								   struct k_thread *thread,
								   uint8_t *stack,
								   size_t stack_size,
								   uint8_t prio_flags,
								   char symbol);

/**
 * @brief Initialize a work item at runtime.
 *
 * This function initializes a work item with the specified handler function.
 * Work items can also be defined statically using `K_WORK_DEFINE`.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param work Pointer to the work item structure.
 * @param handler Function to handle the work item.
 */
__kernel void k_work_init(struct k_work *work, k_work_handler_t handler);

/**
 * @brief Submit a work item to a workqueue.
 *
 * This function submits a work item to the specified workqueue for processing.
 * If the work item is already in the queue and has not been processed, it will
 * not be added again. A work item that has started processing can be resubmitted.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param workqueue Pointer to the workqueue structure.
 * @param work Pointer to the work item to submit.
 * @return `true` if the work item was successfully submitted, `false` otherwise.
 */
__kernel bool k_work_submit(struct k_workqueue *workqueue, struct k_work *work);

/**
 * @brief Enable yield after each work item is processed.
 *
 * This function configures the workqueue to yield the CPU after each work item
 * is processed. This is useful for preventing a cooperative thread workqueue
 * from monopolizing the CPU if there are many work items to process.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param workqueue Pointer to the workqueue structure.
 */
__kernel void k_workqueue_enable_yieldeach(struct k_workqueue *workqueue);

/**
 * @brief Disable yield after each work item is processed.
 *
 * This function configures the workqueue to continue processing work items without
 * yielding the CPU between items, if there are more items to process.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param workqueue Pointer to the workqueue structure.
 */
__kernel void k_workqueue_disable_yieldeach(struct k_workqueue *workqueue);

//
// System workqueue
//

/**
 * @brief Submit a work item to the system workqueue.
 *
 * This function submits a work item to the system-wide workqueue for processing.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param work Pointer to the work item to submit.
 * @return `true` if the work item was successfully submitted, `false` otherwise.
 */
bool k_system_workqueue_submit(struct k_work *work);

/**
 * @brief Delayable work item structure.
 *
 * A delayable work item allows for a work item to be scheduled to run after a
 * specified timeout. The delayable work item includes the underlying work item,
 * an event object for triggering the work, and a pointer to the workqueue that
 * will process the work.
 */
struct k_work_delayable {
	struct k_work work;				///< Underlying work item.
	struct k_event _event;			///< Event object to trigger the work item.
	struct k_workqueue *_workqueue; ///< Workqueue that will process the work item.
};

/**
 * @brief Statically initialize a delayable work item.
 *
 * This macro initializes a delayable work item with the specified work handler.
 *
 * @param work_handler The function that will handle the work item.
 */
#define K_WORK_DELAYABLE_INIT(work_handler)                                              \
	{                                                                                    \
		.work	= K_WORK_INIT(work_handler),                                             \
		._event = K_EVENT_INIT(z_delayable_work_trigger), ._workqueue = NULL,            \
	}

/**
 * @brief Statically define and initialize a delayable work item.
 *
 * This macro defines and initializes a delayable work item with the specified handler.
 *
 * @param name Name of the delayable work item.
 * @param work_handler Function to handle the work item.
 */
#define K_WORK_DELAYABLE_DEFINE(name, work_handler)                                      \
	struct k_work_delayable name = Z_WORK_DELAYABLE_INIT(work_handler)

/**
 * @brief Initialize a delayable work item at runtime.
 *
 * This function initializes a delayable work item with the specified handler function.
 * Delayable work items can also be defined statically using `K_WORK_DELAYABLE_DEFINE`.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param dwork Pointer to the delayable work item structure.
 * @param handler Function to handle the work item.
 */
__kernel void k_work_delayable_init(struct k_work_delayable *dwork,
									k_work_handler_t handler);

/**
 * @brief Schedule a delayable work item to be queued after a timeout.
 *
 * This function schedules a delayable work item to be added to the workqueue after
 * the specified timeout. If the work item is already scheduled or queued, the function
 * returns -EBUSY.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param workqueue Pointer to the workqueue structure.
 * @param dwork Pointer to the delayable work item.
 * @param timeout Timeout before the work item is queued.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_work_delayable_schedule(struct k_workqueue *workqueue,
										  struct k_work_delayable *dwork,
										  k_timeout_t timeout);

/**
 * @brief Schedule a delayable work item for the system workqueue.
 *
 * This function schedules a delayable work item to be added to the system workqueue
 * after the specified timeout.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param dwork Pointer to the delayable work item.
 * @param timeout Timeout before the work item is queued.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_system_work_delayable_schedule(struct k_work_delayable *dwork,
												 k_timeout_t timeout);

/**
 * @brief Cancel a scheduled delayable work item.
 *
 * This function cancels a delayable work item that has been scheduled but not yet queued
 * for processing. If the work item is in the queue or already being processed, the
 * function returns -EBUSY.
 * 
 * Safety: This function is safe to call from an ISR context.
 *
 * @param dwork Pointer to the delayable work item.
 * @return 0 on success, or a negative error code on failure.
 */
__kernel int8_t k_work_delayable_cancel(struct k_work_delayable *dwork);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_WORKQUEUE_H_ */
