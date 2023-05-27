/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"
#include "kernel.h"
#include "kernel_internals.h"
#include "mutex.h"

#include <util/atomic.h>

#define K_MODULE K_MODULE_MUTEX

#define Z_MUTEX_UNLOCKED_VALUE 0x01u
#define Z_MUTEX_LOCKED_VALUE   0x00u

int8_t k_mutex_init(struct k_mutex *mutex)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!mutex) {
		return -EINVAL;
	}
#endif

	mutex->lock  = Z_MUTEX_UNLOCKED_VALUE;
	mutex->owner = NULL;
	dlist_init(&mutex->waitqueue);

	return 0;
}

int8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
	__ASSERT_NOTNULL(mutex);

	int8_t lock	  = 0;
	const uint8_t key = irq_lock();

	if (mutex->lock != Z_MUTEX_LOCKED_VALUE) {
		mutex->lock = Z_MUTEX_LOCKED_VALUE;
	} else {
		lock = z_pend_current(&mutex->waitqueue, timeout);
	}

	if (lock == 0) {
		__K_DBG_MUTEX_LOCKED(z_current);  // }
		mutex->owner = z_current;
	}

	irq_unlock(key);
	return lock;
}

struct k_thread *k_mutex_unlock(struct k_mutex *mutex)
{
	__ASSERT_NOTNULL(mutex);

	struct k_thread *thread = NULL;

	const uint8_t key = irq_lock();

	/* we check if the current thread actually owns the mutex */
	if (mutex->owner != z_current) {
		goto ret;
	}

	__K_DBG_MUTEX_UNLOCKED(z_current);  // {

	/* there is a new owner, we don't need to unlock the mutex
	 * The mutex owner is changed when returning to the
	 * k_mutex_lock function.
	 */
	thread = z_unpend_first_thread(&mutex->waitqueue);

	if (thread == NULL) {
		/* no new owner, we need to unlock
		 * the mutex and remove the owner
		 */
		mutex->lock  = Z_MUTEX_UNLOCKED_VALUE;
		mutex->owner = NULL;
	}

ret:
	irq_unlock(key);
	return thread;
}

int8_t k_mutex_cancel_wait(struct k_mutex *mutex)
{
	__ASSERT_NOTNULL(mutex);

	uint8_t ret;

	const uint8_t key = irq_lock();

	ret = z_cancel_all_pending(&mutex->waitqueue);

	irq_unlock(key);

	return (int8_t)ret;
}