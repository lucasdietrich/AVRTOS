/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mutex.h"

#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>

#include <avrtos/debug.h>

#include <util/atomic.h>

#define K_MODULE K_MODULE_MUTEX

void k_mutex_init(struct k_mutex *mutex)
{
        __ASSERT_NOTNULL(mutex);

        mutex->lock = 0xFFu;
        mutex->owner = NULL;
        dlist_init(&mutex->waitqueue);
}

int8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
	__ASSERT_NOTNULL(mutex);

	int8_t lock = 0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (mutex->lock != 0x00) {
			mutex->lock = 0x00;
		} else {
			lock = _k_pend_current(&mutex->waitqueue, timeout);
		}

		if (lock == 0) {
			__K_DBG_MUTEX_LOCKED(_current);    // }
			mutex->owner = _current;
		}
	}
	return lock;
}

struct k_thread *k_mutex_unlock(struct k_mutex *mutex)
{
	__ASSERT_NOTNULL(mutex);

	struct k_thread *thread = NULL;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		/* we check if the current thread actually owns the mutex */
		if (mutex->owner != _current) {
			goto ret;
		}

		__K_DBG_MUTEX_UNLOCKED(_current); // {

		/* there is a new owner, we don't need to unlock the mutex
		 * The mutex owner is changed when returning to the
		 * k_mutex_lock function.
		 */
		thread = _k_unpend_first_thread(&mutex->waitqueue);

		if (thread == NULL) {
		    /* no new owner, we need to unlock
		     * the mutex and remove the owner
		     */
			mutex->lock = 0xFFu;
			mutex->owner = NULL;
		}
	}

ret:
	return thread;
}

int8_t k_mutex_cancel_wait(struct k_mutex *mutex)
{
        __ASSERT_NOTNULL(mutex);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                return _k_cancel_all_pending(&mutex->waitqueue);
        }

        __builtin_unreachable();
}