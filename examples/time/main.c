/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <util/delay.h>

#define K_MODULE K_MODULE_APPLICATION

void thread(void *ctx);
void processing(void *ctx);

K_THREAD_DEFINE(t1, thread, 0x100, K_PREEMPTIVE, NULL, '1');
K_THREAD_DEFINE(t2, processing, 0x100, K_PREEMPTIVE, NULL, '1');

int main(void)
{
	serial_init();

	k_thread_dump_all();

	// _delay_ms(2000);

	irq_enable();

	k_show_uptime();
	printf_P(PSTR("Startup\n"));

	uint32_t timestamp;

	for (;;) {
		// now = k_uptime_get_ms64();
		timestamp = k_time_get();

		k_show_uptime();

		printf_P(PSTR(" %lu 64ms - %lu 32ms - "), (uint32_t)k_uptime_get_ms64(),
				 k_uptime_get_ms32());

		printf_P(PSTR("%lu s - "), k_uptime_get());

		printf_P(PSTR("%lu s (timestamp)\n"), timestamp);

		k_wait(K_SECONDS(1), K_WAIT_MODE_IDLE);
	}
}

void thread(void *ctx)
{
	uint32_t sec = 100000;
	for (;;) {
		k_time_set(sec);

		sec += 100000;

		k_sleep(K_SECONDS(5));
	}
}

void processing(void *ctx)
{
	for (;;) {
	}
}