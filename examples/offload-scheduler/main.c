/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/misc/serial.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_DEBUG

static void thread_task(void *context);

K_THREAD_DEFINE(thread_a, thread_task, 0x100, K_PREEMPTIVE, NULL, 'A');
K_THREAD_DEFINE(thread_b, thread_task, 0x100, K_PREEMPTIVE, NULL, 'B');

int main(void)
{
	serial_init();

	k_sleep(K_FOREVER);
}

void thread_task(void *context)
{
	for (;;) {
		LOG_DBG("x: %u", k_ticks_get_32());
		_NOP();
		k_yield();
	}
}