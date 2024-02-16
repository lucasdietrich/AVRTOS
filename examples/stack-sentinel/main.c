/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/avrtos.h>
#include <avrtos/misc/serial.h>

#include <avr/sleep.h>

void thread(void *p);

K_THREAD_DEFINE(
	t1, thread, Z_THREAD_STACK_MIN_SIZE + 100, K_PREEMPTIVE, NULL, '1');

void write_stack(uint16_t size, uint8_t pattern)
{
	uint8_t stack_buffer[size];
	for (uint16_t i = 0; i < size; i++) {
		stack_buffer[i] = pattern;
	}
	ARG_UNUSED(stack_buffer);
}

int main(void)
{
	serial_init();
	k_thread_dump_all();

	uint32_t counter = 0u;

	for (;;) {
		bool success = k_verify_stack_sentinel(&t1);
		k_show_uptime();
		printf_P(PSTR("%lu: Sentinel status = %d\n"), counter++, success ? 1 : 0);

		k_dump_stack_canaries();
		k_thread_dump_all();

		k_sleep(K_SECONDS(1));
	}
}

void thread(void *p)
{
	uint16_t size = 0;
	for (;;) {
		write_stack(size, 0xFF);
		size += 5u;
		k_sleep(K_NEXT_TICK);
	}
}