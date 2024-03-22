/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <avrtos/debug.h>
#include <avrtos/avrtos.h>
#include <avrtos/logging.h>

#include <avrtos/drivers/timer.h>

#define LOG_LEVEL LOG_LEVEL_DBG
#define STACK_SIZE 0x200u

static void task(void *p)
{
	while (1) {
		LOG_DBG("Task %p", z_current);
		k_sleep(K_MSEC(100u));
	}
}

// K_THREAD_DEFINE(t1, task, 0x100, K_COOPERATIVE, NULL, '1');
// K_THREAD_DEFINE(t2, task, 0x100, K_COOPERATIVE, NULL, '2');
// K_THREAD_DEFINE(t3, task, 0x100, K_COOPERATIVE, NULL, '3');

int main(void)
{
	serial_init();

	for (;;) {
		LOG_INF("%u", ll_timer16_get_tcnt(TIMER1_DEVICE));
		// z_cpu_block_us(1000u);
	}

	k_stop();
}