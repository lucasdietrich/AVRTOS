/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

void waiting_thread(k_timeout_t *timeout);

k_timeout_t timeouts[3] = {K_SECONDS(2), K_SECONDS(1), K_SECONDS(1)};

K_THREAD_DEFINE(waiter1, waiting_thread, 0x100, K_PREEMPTIVE, &timeouts[0], 'A');
K_THREAD_DEFINE(waiter2, waiting_thread, 0x100, K_PREEMPTIVE, &timeouts[1], 'B');
K_THREAD_DEFINE(waiter3, waiting_thread, 0x100, K_PREEMPTIVE, &timeouts[2], 'C');
K_MUTEX_DEFINE(mymutex);

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	k_mutex_lock(&mymutex, K_NO_WAIT);

	irq_enable();

	k_sleep(K_SECONDS(1));

	k_mutex_unlock(&mymutex);

	k_sleep(K_FOREVER);
}

void waiting_thread(k_timeout_t *timeout)
{
	serial_transmit(z_current->symbol);
	serial_printl_p(PSTR(": starting"));

	uint8_t locked = k_mutex_lock(&mymutex, *timeout);

	serial_transmit(z_current->symbol);
	if (locked == 0) {
		serial_printl_p(PSTR(": locked the mutex !"));
	} else {
		serial_printl_p(PSTR(": didn't get the mutex !"));
	}

	k_sleep(K_FOREVER);
}
