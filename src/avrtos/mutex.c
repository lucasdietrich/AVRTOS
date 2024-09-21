/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mutex.h"

#include <util/atomic.h>

#include "debug.h"
#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_MUTEX

#define Z_MUTEX_UNLOCKED_VALUE 0u

int8_t k_mutex_init(struct k_mutex *mutex)
{
	Z_ARGS_CHECK(mutex) return -EINVAL;

	mutex->lock	 = Z_MUTEX_UNLOCKED_VALUE;
	mutex->owner = NULL;
	dlist_init(&mutex->waitqueue);

	return 0;
}

int8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
	Z_ARGS_CHECK(mutex) return -EINVAL;

	int8_t ret		  = 0;
	const uint8_t key = irq_lock();

	if (mutex->lock == Z_MUTEX_UNLOCKED_VALUE) {
		/* Mutex is available, acquire it */
		mutex->lock = 1u;
	} else if (mutex->owner == z_ker.current) {
#if CONFIG_KERNEL_REENTRANCY
		/* Mutex is already owned by the current thread, increment lock count */
		mutex->lock++;
#endif /* CONFIG_KERNEL_REENTRANCY */
		goto exit;
	} else {
		/* Mutex is locked by another thread, wait for it to become available */
		ret = z_pend_current(&mutex->waitqueue, timeout);
	}

	if (ret == 0) {
		/* Successfully acquired the mutex */
		__Z_DBG_MUTEX_LOCKED(z_ker.current);
		mutex->owner = z_ker.current;
	}

exit:
	irq_unlock(key);
	return ret;
}

struct k_thread *k_mutex_unlock(struct k_mutex *mutex)
{
	Z_ARGS_CHECK(mutex) return NULL;

	struct k_thread *thread = NULL;
	const uint8_t key		= irq_lock();

	if (mutex->owner != z_ker.current) {
		/* Current thread does not own the mutex, cannot unlock */
		goto exit;
#if CONFIG_KERNEL_REENTRANCY
	} else if (mutex->lock > 1u) {
		/* Reentrant locking: decrement lock count instead of unlocking */
		mutex->lock--;
		goto exit;
#endif /* CONFIG_KERNEL_REENTRANCY */
	}

	__Z_DBG_MUTEX_UNLOCKED(z_ker.current);

	/* There is a new owner, we don't need to unlock the mutex as
	 * the mutex owner is changed when returning from the
	 * k_mutex_lock() function. Function to where the woken up thread
	 * will be returned.
	 */
	thread = z_unpend_first_thread(&mutex->waitqueue);
	if (thread == NULL) {
		/* No threads are waiting, fully unlock the mutex */
		mutex->lock	 = Z_MUTEX_UNLOCKED_VALUE;
		mutex->owner = NULL;
	}

exit:
	irq_unlock(key);
	return thread;
}

int8_t k_mutex_cancel_wait(struct k_mutex *mutex)
{
	__ASSERT_NOTNULL(mutex);

	int8_t ret;
	const uint8_t key = irq_lock();

	ret = (int8_t)z_cancel_all_pending(&mutex->waitqueue);

	irq_unlock(key);

	return ret;
}
