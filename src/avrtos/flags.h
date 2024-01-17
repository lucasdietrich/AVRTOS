/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_FLAGS_H
#define _AVRTOS_FLAGS_H

/* Implement a thread/irq-safe flags notification subscription/notification
 * system  */

#include "kernel.h"

struct k_flags {
	struct dnode _waitqueue;
	uint8_t flags;
	uint8_t reset_value;
};

#define K_FLAGS_INIT(flags_name, initial_value)                       \
	{                                                             \
		.flags = initial_value, .reset_value = initial_value, \
		._waitqueue = DLIST_INIT(flags_name._waitqueue)       \
	}

#define K_FLAGS_DEFINE(flags_name, initial_value) \
	struct k_flags flags_name = K_FLAGS_INIT(flags_name, initial_value)

typedef enum {
	/*
	 * k_flags_poll options :
	 */

	/* Wait for any bit of the flags to be set */
	K_FLAGS_SET_ANY = 1 << 0u,
	/* (not supported) Wait for all bits of the flags to be set */
	K_FLAGS_SET_ALL = 1 << 2u,
	/* (not supported) Wait for any bit of the flags to be cleared */
	K_FLAGS_CLR_ALL = 1 << 2u,
	/* (not supported) Wait for all bits of the flags to be cleared */
	K_FLAGS_CLR_ANY = 1 << 3u,
	/* Consume the flags */
	K_FLAGS_CONSUME = 1 << 4u,

	/*
	 * k_flags_notify options :
	 */

	/* Set the flags */
	K_FLAGS_SET = 1 << 5u,
	/* (not supported) Clear the flags */
	K_FLAGS_CLR = 1 << 6u,
	/* Call the scheduler if task have been wake up after notify() */
	K_FLAGS_SCHED = 1 << 7u,
} k_flags_options_t;

/**
 * @brief Initialize a flags object
 *
 * @param flags Flags object
 * @param value Initial value
 * @return int
 */
int k_flags_init(struct k_flags *flags, uint8_t value);

/**
 * @brief Poll for any bit of the flags to be set
 *
 * @param flags Flags object
 * @param mask Mask of bits to wait for
 * @param options Options
 * 		K_FLAGS_SET_ANY (required): Wait for any bit of the flags to be
 * set K_FLAGS_CONSUME: Consume the flags after notification
 * @param timeout Timeout to wait for if flags are not set
 * @return int positive value on success: Mask of bits that made the thread wake
 * up
 * @return  -EINVAL if flags is NULL
 * @return  -ENOTSUP if options is not supported
 * @return  -EAGAIN if timeout is reached
 */
int k_flags_poll(struct k_flags *flags,
		 uint8_t mask,
		 k_flags_options_t options,
		 k_timeout_t timeout);

/**
 * @brief Notify a flags object
 *
 * @param flags Flags object
 * @param mask Mask of bits to notify
 * @param options Options
 * 		K_FLAGS_SET (required): Set the flags
 * 		K_FLAGS_SCHED: Call the scheduler if task have been wake up
 * after notify()
 *
 * Note: K_FLAGS_SCHED SHOULD NOT be used in ISR context. Use return value to
 * check if a task have been wake up. Call k_yield_from_isr_cond() if return
 * value is strictly positive.
 *
 * @return int Number of tasks that have been wake up, negative value on error
 * @return  -EINVAL if flags object is NULL
 * @return  -ENOTSUP if options is not supported
 */
int k_flags_notify(struct k_flags *flags,
		   uint8_t notify_value,
		   k_flags_options_t options);

/**
 * @brief Reset a flags object
 *
 * Cancel all pending waiters and reset the flags to the initial value
 *
 * @param flags
 * @return int
 */
int k_flags_reset(struct k_flags *flags);

#endif /* _AVRTOS_FLAGS_H */