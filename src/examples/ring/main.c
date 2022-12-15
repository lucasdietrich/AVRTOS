/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>

#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>

static struct k_ring ring;
static uint8_t buffer[16u];

ISR(USART0_RX_vect)
{
	const char chr = USART0_DEVICE->UDRn;

	// k_ring_push(&ring, chr);
}

void thread(void *arg)
{
	uint32_t rcvd = 0u;
	for (;;) {
		const int chr = k_ring_pop(&ring);
		if (chr >= 0) {
			rcvd++;
			
			// ll_usart_sync_putc(USART0_DEVICE, chr);

			if (rcvd % 100u == 0u) {
				printf_P(PSTR("rcvd=%lu\n"), rcvd);
			}
		}

		k_yield();
	}
}

K_THREAD_DEFINE(thread_id, thread, 128u, K_PREEMPTIVE, NULL, 0u);

int main(void)
{
	k_ring_init(&ring, buffer, sizeof(buffer));

	const struct usart_config usart_config = {
		.baudrate = USART_BAUD_500000,
		.receiver = 1u,
		.transmitter = 1u,
		.mode = USART_MODE_ASYNCHRONOUS,
		.parity = USART_PARITY_NONE,
		.stopbits = USART_STOP_BITS_1,
		.databits = USART_DATA_BITS_8,
		.speed_mode = USART_SPEED_MODE_NORMAL
	};
	ll_usart_init(USART0_DEVICE, &usart_config);
	ll_usart_enable_rx_isr(USART0_DEVICE);

	// for (;;) {
	// 	const int chr = k_ring_pop(&ring);
	// 	if (chr >= 0) {
	// 		ll_usart_sync_putc(USART0_DEVICE, chr);
	// 	} else {
	// 		ll_usart_sync_putc(USART0_DEVICE, '\n');
	// 		k_sleep(K_MSEC(1000u));
	// 	}
	// }

	char chr = 'a';
	for (;;) {
		k_ring_push(&ring, chr);

		chr++;
		if (chr > 'z') {
			chr = 'a';
		}
		k_wait(K_MSEC(1u));
		// k_yield();
	}
}