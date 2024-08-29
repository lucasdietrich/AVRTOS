/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

char chrs[2] = {'a', 'b'};
void mythread(char *ctx);
void canariesthread(void *ctx);

K_THREAD_DEFINE(t1, mythread, 0x80, K_PREEMPTIVE, &chrs[0], 'A');
K_THREAD_DEFINE(t2, mythread, 0x80, K_PREEMPTIVE, &chrs[1], 'B');
K_THREAD_DEFINE(t3, canariesthread, 0x80, K_COOPERATIVE, NULL, 'C');

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	sei();

	while (1) {
		k_show_uptime();
		serial_transmit('\n');
		printf_P(PSTR("%lu ticks : %lu ms\n"), k_ticks_get_32(), k_uptime_get_ms32());
		k_sleep(K_SECONDS(1));
	}
}

void mythread(char *ctx)
{
	uint32_t i = 0;

	while (1) {
		i++;
		serial_transmit(*ctx);
		k_sleep(K_SECONDS(1));
	}
}

void canariesthread(void *ctx)
{
	while (1) {
		k_dump_stack_canaries();
		k_sleep(K_SECONDS(30));
	}
}