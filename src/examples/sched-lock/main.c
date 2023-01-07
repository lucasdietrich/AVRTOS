/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/serial.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#define USE_SCHED_LOCK_TRICK 0

/*___________________________________________________________________________*/

void thread_blink(void *p);
void thread_coop(void *p);

K_THREAD_DEFINE(blink, thread_blink, 0x100, K_PREEMPTIVE, NULL, 'B');
K_THREAD_DEFINE(coop, thread_coop, 0x100, K_COOPERATIVE, NULL, 'C');

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	z_print_runqueue();

	sei();

	while (1) {

#if USE_SCHED_LOCK_TRICK
		K_SCHED_LOCK_CONTEXT
		{
	    #else
		  k_sched_lock();
	    #endif
		  serial_printl_p(PSTR("k_sched_lock()"));

		  _delay_ms(500);

		  serial_printl_p(PSTR("k_sched_unlock()"));

	    #if USE_SCHED_LOCK_TRICK == 0
		  k_sched_unlock();
	    #else
		}
#endif

	_delay_ms(2000);
	}
}

void thread_blink(void *p)
{
	while (1) {
		serial_transmit('o');
		led_on();

		k_sleep(K_MSEC(100));

		serial_transmit('f');
		led_off();

		k_sleep(K_MSEC(100));
	}
}

void thread_coop(void *p)
{
	while (1) {
		k_sleep(K_MSEC(5000));

		serial_printl_p(PSTR("<<<< full cooperative thread"));

		_delay_ms(1000);

		serial_printl_p(PSTR(">>>>\n"));
	}
}

/*___________________________________________________________________________*/