/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_SIGNAL_H_
#define _AVRTOS_SIGNAL_H_

#include <stdbool.h>

#include <avr/io.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define K_POLL_STATE_NOT_READY 0x00
#define K_POLL_STATE_SIGNALED  0x01

#define K_SIGNAL_INIT(sig)                                     \
	{                                                      \
		.signal = 0u, .flags = K_POLL_STATE_NOT_READY, \
		.waitqueue = DLIST_INIT(sig.waitqueue)         \
	}

#define K_SIGNAL_DEFINE(signal_name) \
	struct k_signal signal_name = K_SIGNAL_INIT(signal_name)

#define K_SIGNAL_SET_UNREADY(signal) (signal)->flags = K_POLL_STATE_NOT_READY

struct k_signal {
	uint8_t signal;
	uint8_t flags;
	struct dnode waitqueue;
};

/**
 * @brief Initialize a signal object.
 *
 * @param sig
 */
__kernel int8_t k_signal_init(struct k_signal *sig);

/**
 * @brief Wake up the first thread (TODO all threads) polling on the signal.
 *
 * @param sig
 * @param value
 * @return The thread that was woken up, NULL otherwise.
 */
__kernel struct k_thread *k_signal_raise(struct k_signal *sig, uint8_t value);

/**
 * @brief Poll on a signal.
 *
 * Signal must be cleared manually with K_SIGNAL_SET_UNREADY.
 *
 * TODO: Return the signal value that made the thread ready.
 *
 * @param sig
 * @param timeout
 * @return 0 on success, negative error code otherwise.
 */
__kernel int8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout);

/**
 * @brief Cancel the wait of all threads polling on the signal.
 *
 * @param sig
 * @return Number of threads that were woken up.
 */
__kernel uint8_t k_poll_cancel_wait(struct k_signal *sig);

#ifdef __cplusplus
}
#endif

#endif