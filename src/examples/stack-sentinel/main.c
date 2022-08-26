/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>
#include <avr/sleep.h>

void thread(void *p);

// ISR(USART_RX_vect) {
// 	char c = UDR0;
// 	usart_transmit('n');
// }


K_THREAD_DEFINE(t1, thread, K_THREAD_STACK_MIN_SIZE + 0x100 - 237 - 2, K_PREEMPTIVE, NULL, '1');

int main(void)
{
	usart_init();

	// UCSR0B = 1 << RXCIE0;

	k_thread_dump_all();

	for (;;) {
		bool success = k_verify_stack_sentinel(&t1);

		k_show_uptime();

		printf_P(PSTR("Sentinel status = %d\n"), success ? 1 : 0);

		dump_stack_canaries();

		k_thread_dump_all();

		k_sleep(K_SECONDS(1));
	}
}

void thread(void *p)
{
	for (;;) {
		sleep_cpu();
	}
}