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

void thread_led_on(void *p);
void thread_led_off(void *p);

K_THREAD_DEFINE(ledon, thread_led_on, 0x120, K_PREEMPTIVE, NULL, 'O');
K_THREAD_DEFINE(ledoff, thread_led_off, 0x140, K_PREEMPTIVE, NULL, 'F');

/*___________________________________________________________________________*/

K_MUTEX_DEFINE(mymutex);

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	usart_init();

	k_thread_dump_all();

	print_runqueue();

	sei();

	for (;;) { }
}

void thread_led_on(void *p)
{
	while (1) {
		k_mutex_lock(&mymutex, K_FOREVER);

		led_on();

		k_sleep(K_MSEC(1000));

		k_mutex_unlock(&mymutex);
	}
}

void thread_led_off(void *p)
{
	while (1) {
		k_mutex_lock(&mymutex, K_FOREVER);

		led_off();

		k_sleep(K_MSEC(1000));

		k_mutex_unlock(&mymutex);
	}
}

/*___________________________________________________________________________*/