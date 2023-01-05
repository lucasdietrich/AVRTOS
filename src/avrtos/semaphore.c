/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "semaphore.h"

#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>
#include <avrtos/debug.h>

#include <util/atomic.h>

void k_sem_init(struct k_sem *sem, uint8_t initial_count, uint8_t limit)
{
        sem->limit = limit;
        sem->count = MIN(limit, initial_count);
        dlist_init(&sem->waitqueue);
}

int8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
	int8_t get = 0x00;

	const uint8_t key = irq_lock();

	if (sem->count != 0) {
		sem->count--;
	} else {
		get = _k_pend_current(&sem->waitqueue, timeout);
	}

	irq_unlock(key);

	if (get == 0) {
		__K_DBG_SEM_TAKE(_current);
	}

	return get;
}

struct k_thread *k_sem_give(struct k_sem *sem)
{
	struct k_thread *thread;

	const uint8_t key = irq_lock();

	__K_DBG_SEM_GIVE(_current);

	/* if a thread is thread is pending on a semaphore
	 * it means that its count is necessary 0. So we don't
	 * need to check if we reached the limit.
	 */

	thread = _k_unpend_first_thread(&sem->waitqueue);

	/* If there is a thread pending on a semaphore,
	 * we to give the semaphore directly to the thread
	 */
	if (thread == NULL) {
		if (sem->count != sem->limit) {
			sem->count++;
		}
	}

	irq_unlock(key);

	return thread;
}