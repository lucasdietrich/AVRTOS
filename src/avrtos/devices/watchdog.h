/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CANIOT_DEV_WATCHDOG_H_
#define _CANIOT_DEV_WATCHDOG_H_

#include <avrtos/atomic.h>

#include <avr/wdt.h>

#define WATCHDOG_TIMEOUT_MS   8000
#define WATCHDOG_TIMEOUT_WDTO WDTO_8S

/**
 * @brief Register a thread to be watched by the watchdog.
 *
 * @return uint8_t Returns the thread handle.
 */
uint8_t critical_thread_register(void);

/**
 * @brief Mark a thread as alive.
 *
 * @param thread_id thread handle.
 */
void alive(uint8_t thread_id);

#endif /* _CANIOT_DEV_WATCHDOG_H_ */