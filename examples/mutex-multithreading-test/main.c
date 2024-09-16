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

void thread(void *p);
void threadp(void *p);

K_THREAD_DEFINE(thread1, thread, 0x100, K_PREEMPTIVE, NULL, '1');
K_THREAD_DEFINE(thread2, thread, 0x100, K_PREEMPTIVE, NULL, '2');
K_THREAD_DEFINE(thread3, thread, 0x100, K_PREEMPTIVE, NULL, '3');
K_THREAD_DEFINE(thread4, threadp, 0x100, K_PREEMPTIVE, NULL, '4');

K_MUTEX_DEFINE(mymutex);

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	z_print_runqueue();

	k_mutex_lock(&mymutex, K_NO_WAIT);

	sei();

	k_sleep(K_SECONDS(3));

	k_mutex_unlock(&mymutex);

	k_sleep(K_FOREVER);
}

void thread(void *p)
{
	uint8_t lock = k_mutex_lock(&mymutex, K_SECONDS(5)); // change this timeout

	serial_transmit(z_current->symbol);

	if (lock) {
		serial_printl_p(PSTR(" : Didn't get the mutex ..."));
	} else {
		serial_printl_p(PSTR(" : Got the mutex !"));

		k_sleep(K_SECONDS(1));

		k_mutex_unlock(&mymutex);
	}

	k_sleep(K_FOREVER);
}

void threadp(void *p)
{
	uint8_t lock = k_mutex_lock(&mymutex, K_SECONDS(9)); // change this timeout

	serial_transmit(z_current->symbol);

	if (lock) {
		serial_printl_p(PSTR(" : Didn't get the mutex ..."));
	} else {
		serial_printl_p(PSTR(" : Got the mutex !"));

		k_sleep(K_SECONDS(1));

		k_mutex_unlock(&mymutex);
	}

	k_sleep(K_FOREVER);
}