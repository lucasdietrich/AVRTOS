/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "signal.h"

#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>

#include <util/atomic.h>

#define K_MODULE K_MODULE_SIGNAL

void k_signal_init(struct k_signal *sig)
{
	__ASSERT_NOTNULL(sig);

	sig->signal = 0u;
	sig->flags  = K_POLL_STATE_NOT_READY;
	dlist_init(&sig->waitqueue);
}

struct k_thread *k_signal_raise(struct k_signal *sig, uint8_t value)
{
	__ASSERT_NOTNULL(sig);

	struct k_thread *thread;

	const uint8_t key = irq_lock();

	sig->signal = value;
	sig->flags |= K_POLL_STATE_SIGNALED;

	/* TODO: unpend all threads */
	thread = z_unpend_first_thread(&sig->waitqueue);

	irq_unlock(key);

	return thread;
}

int8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout)
{
	__ASSERT_NOTNULL(sig);

	int8_t ret;

	const uint8_t key = irq_lock();

	if (TEST_BIT(sig->flags, K_POLL_STATE_SIGNALED)) {
		ret = 0;
	} else {
		ret = z_pend_current(&sig->waitqueue, timeout);
	}

	irq_unlock(key);

	return ret;
}

uint8_t k_poll_cancel_wait(struct k_signal *sig)
{
	__ASSERT_NOTNULL(sig);

	int8_t ret;

	const uint8_t key = irq_lock();

	ret = z_cancel_all_pending(&sig->waitqueue);

	irq_unlock(key);

	return ret;
}

/*___________________________________________________________________________*/