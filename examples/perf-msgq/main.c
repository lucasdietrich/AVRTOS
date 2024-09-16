/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define BLOCKS_COUNT 2
#define BLOCK_SIZE	 100

K_MSGQ_DEFINE(msgq, BLOCK_SIZE, BLOCKS_COUNT);

void writer(struct k_msgq *msgq);
void reader(struct k_msgq *msgq);

K_THREAD_DEFINE(w0, writer, 0x50, K_PREEMPTIVE, &msgq, 'w');
K_THREAD_DEFINE(r0, reader, 0x50, K_PREEMPTIVE, &msgq, 'R');

void writer(struct k_msgq *msgq)
{
	static char buf[BLOCK_SIZE];
	memset(buf, 0xAA, BLOCK_SIZE);
	for (;;) {
		k_msgq_put(msgq, buf, K_FOREVER);
	}
}

void reader(struct k_msgq *msgq)
{
	static char buf[BLOCK_SIZE];
	uint8_t i = 0;
	for (;;) {
		k_msgq_get(msgq, buf, K_FOREVER);
		if (i++ == 0) serial_transmit('.');
	}
}

int main(void)
{
	serial_init();

	k_stop();
}
