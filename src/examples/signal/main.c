/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

void waiter(void *context);

K_THREAD_DEFINE(w1, waiter, 0x100, K_PREEMPTIVE, NULL, 'W');
K_SIGNAL_DEFINE(sig);

ISR(USART0_RX_vect)
{
	const char rx = UDR0;
	serial_transmit(rx);

	struct k_thread *thread = k_signal_raise(&sig, rx);

	k_yield_from_isr_cond(thread);
}

int main(void)
{
	led_init();
	serial_init();

	/* enable RX interrupt */
	SET_BIT(UCSR0B, 1 << RXCIE0);

	k_thread_dump_all();

	k_sleep(K_FOREVER);
}

void waiter(void *context)
{
	char buffer[128];

	printf_P(PSTR("Press any key: "));

	k_sleep(K_SECONDS(5));

	for (;;) {
		int8_t err = k_poll_signal(&sig, K_SECONDS(3));
		sprintf(buffer, "k_poll_signal returned err = %d\n", err);
		serial_print(buffer);
		if (err == 0) {
			sprintf(buffer, "signal value = %d\n", sig.signal);
			serial_print(buffer);

			sig.flags = K_POLL_STATE_NOT_READY;
		}
	}
}