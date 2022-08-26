/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

#define BLOCKS_COUNT    10
#define BLOCK_SIZE      0x20

char buffer[BLOCKS_COUNT * BLOCK_SIZE];
K_MSGQ_DEFINE(msgq, buffer, BLOCK_SIZE, BLOCKS_COUNT);

/*___________________________________________________________________________*/

#define WRITER_TIMEOUT  500
#define WRITER_DELAY    250

#define READER_TIMEOUT  1000
#define READER_DELAY    1000

#define PURGE_PERIOD    10000

void writer(struct k_msgq *msgq);
void reader(struct k_msgq *msgq);

K_THREAD_DEFINE(w0, writer, 0x50, K_PREEMPTIVE, &msgq, 'w');
K_THREAD_DEFINE(w1, writer, 0x50, K_PREEMPTIVE, &msgq, 'W');
K_THREAD_DEFINE(r0, reader, 0x50, K_PREEMPTIVE, &msgq, 'r');
K_THREAD_DEFINE(r1, reader, 0x50, K_PREEMPTIVE, &msgq, 'R');

/*___________________________________________________________________________*/

void writer(struct k_msgq *msgq)
{
	int8_t ret;
	char buf[BLOCK_SIZE];

	for (;;) {
		(*(uint16_t *)buf)++;

		ret = k_msgq_put(msgq, buf, K_MSEC(WRITER_TIMEOUT));
		usart_transmit(_current->symbol);

		if (ret == 0) {
			usart_transmit(' ');
			usart_u16(*(uint16_t *)buf);
			usart_transmit('\n');

			k_sleep(K_MSEC(WRITER_DELAY));
		} else if (ret == -ECANCEL) {
			usart_print_p(PSTR(" canceled\n"));
		} else {
			usart_print_p(PSTR(" !\n"));
		}
	}
}

void reader(struct k_msgq *msgq)
{
	int8_t ret;
	char buf[BLOCK_SIZE];

	for (;;) {
		ret = k_msgq_get(msgq, buf, K_MSEC(READER_TIMEOUT));
		usart_transmit(_current->symbol);
		if (ret == 0) {

			usart_transmit(' ');
			usart_u16(*(uint16_t *)buf);
			usart_transmit('\n');

			k_sleep(K_MSEC(READER_DELAY));
		} else if (ret == -ECANCEL) {
			usart_print_p(PSTR(" canceled\n"));
		} else {
			usart_print_p(PSTR(" !\n"));
		}
	}
}

int main(void)
{
	/* interrupts are disabled in this thread */

	led_init();
	usart_init();

	k_thread_dump_all();

	for (;;) {
		k_sleep(K_MSEC(PURGE_PERIOD));

		k_msgq_purge(&msgq);
	}
}

/*___________________________________________________________________________*/