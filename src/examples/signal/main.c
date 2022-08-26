/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

void waiter(void *context);

K_THREAD_DEFINE(w1, waiter, 0x100, K_PREEMPTIVE, NULL, 'W');
K_SIGNAL_DEFINE(sig);


#if defined(__AVR_ATmega328P__)
#	define board_USART_RX_vect  USART_RX_vect
#elif defined(__AVR_ATmega2560__)
#	define board_USART_RX_vect  USART0_RX_vect
#endif /* __AVR_ATmega328P__ */

ISR(board_USART_RX_vect)
{
	const char rx = UDR0;
	usart_transmit(rx);

	k_signal_raise(&sig, rx);
}

int main(void)
{
	led_init();
	usart_init();

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
		usart_print(buffer);
		if (err == 0) {
			sprintf(buffer, "signal value = %d\n", sig.signal);
			usart_print(buffer);

			sig.flags = K_POLL_STATE_NOT_READY;
		}
	}
}