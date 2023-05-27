/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/avrtos.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

void waiting_thread(void *p);
void monitoring_thread(void *p);

K_THREAD_DEFINE(waiting, waiting_thread, 0x200, K_PREEMPTIVE, NULL, 'W');
K_THREAD_DEFINE(monitoring, monitoring_thread, 0x200, K_PREEMPTIVE, NULL, 'R');

K_FIFO_DEFINE(myfifo);
K_MEM_SLAB_DEFINE(myslab, 2u + 1u, 10u);

static uint8_t usart_read_rx(void)
{
	uint8_t status = UCSR0A;
	// uint8_t resh = UCSR0B;
	uint8_t resl = UDR0;

	if (status & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
		serial_transmit('x');
		resl = 0;
	}

	return resl;
}

// atmega2560
ISR(USART0_RX_vect)
{
	// UART0 RX buffer must be read before enabling interrupts again
	char recv = usart_read_rx();

	void *mem;
	if (k_mem_slab_alloc(&myslab, &mem, K_NO_WAIT) == 0) {

		/* first two bytes are use by the fifo */
		*(char *)(mem + 2u) = recv;
		k_fifo_put(&myfifo, mem);
	} else {
		/* no memory slab available */
		serial_transmit('X');
	}
}

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	/* set UART RX interrupt */
	UCSR0B |= 1 << RXCIE0;

	irq_enable();

	k_sleep(K_FOREVER);
}

void waiting_thread(void *p)
{
	while (1) {
		void *mem = k_fifo_get(&myfifo, K_SECONDS(5));
		if (mem != NULL) {
			serial_print_p(PSTR("Got a letter from the UART : "));
			serial_transmit(*(char *)(mem + 2u));
			serial_transmit('\n');

			/* This delay emulates a delay in the process of the
			 * input letter. If you send letters faster than you
			 * process them, there will be not enough memory blocks
			 * and 'X' will appear in the UART.*/
			k_sleep(K_MSEC(200));

			k_mem_slab_free(&myslab, mem);
		} else {
			serial_printl_p(
				PSTR("Didn't get a letter from the UART in time."));
		}
	}
}

void monitoring_thread(void *p)
{
	while (1) {
		k_sleep(K_MSEC(30000));

		z_print_runqueue();
		z_print_events_queue();
		k_dump_stack_canaries();
	}
}
