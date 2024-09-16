/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

int timer_handler(struct k_timer *timer)
{
	printf("uptime: %lu seconds\n", k_uptime_get());

	return 0;
}

K_TIMER_DEFINE(mstimer, timer_handler, K_MSEC(1000), 1000);

int main(void)
{
	serial_init();

	led_init();

	k_thread_dump_all();

	for (;;) {
		led_toggle();

		k_sleep(K_MSEC(5000));

		k_dump_stack_canaries();
	}
}

void thread(void *ctx);

K_THREAD_DEFINE(ta, thread, 0x200, K_PREEMPTIVE, NULL, 'A');

void thread(void *ctx)
{
	uint64_t last = 0L;
	uint64_t now  = 0L;

	const uint32_t period = 1000LLU;

	for (;;) {
		now = k_uptime_get_ms64();
		if (now - last >= period) {
			last = now;
			k_show_uptime();
			printf("now = %lu %lu\n", (uint32_t)(now >> 32), (uint32_t)now);
		}
	}
}