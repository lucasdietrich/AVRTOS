/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file main.cpp
 * @author Dietrich Lucas (ld.adecy@gmail.com)
 * @brief Two thread increment two counters. Threads are preempted by the system
 * clock. If configuration option CONFIG_KERNEL_DEBUG_PREEMPT_UART=1 is enabled
 * : send a character over the UART to preempt the thread currently running and
 * switch to the other.
 *
 * if CONFIG_KERNEL_DEBUG_PREEMPT_UART is enabled and
 * DCONFIG_KERNEL_TIME_SLICE_US=250000 :
 * - on every 4 chars sent on the UART, the led is toggled
 * @version 0.1
 * @date 2021-08-21
 *
 * @copyright Copyright (c) 2021
 *
 */

/*___________________________________________________________________________*/

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

/*___________________________________________________________________________*/

void thread_led_toggle(void *p);
void thread_processing(void *p);

K_THREAD_DEFINE(led, thread_led_toggle, 0x100, K_PREEMPTIVE, NULL, 'L');
K_THREAD_DEFINE(task1, thread_processing, 0x100, K_COOPERATIVE, NULL, 'A');
K_THREAD_DEFINE(task2, thread_processing, 0x100, K_COOPERATIVE, NULL, 'B');

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

#if CONFIG_KERNEL_DEBUG_PREEMPT_UART
	serial_printl_p(PSTR(" Send a char over the UART to switch thread !"));
#endif

	k_sleep(K_FOREVER);
}

void thread_led_toggle(void *p)
{
	while (1) {
		led_on();

		k_sleep(K_MSEC(1000));
		// if CONFIG_KERNEL_DEBUG_PREEMPT_UART is enabled and
		// DCONFIG_KERNEL_TIME_SLICE_US=250000 :
		// - on every 4 chars sent on the UART, the led is toggled

		led_off();

		k_sleep(K_MSEC(1000));
	}
}

void thread_processing(void *p)
{
	uint32_t counter = 0;
	while (1) {
		counter++;

		if ((counter & 0xFFFFF) == 0) {
			k_sched_lock();
			serial_transmit(z_current->symbol);
			serial_print_p(PSTR(": "));
			serial_hex16(counter >> 16);
			serial_print_p(PSTR("0000\n"));
			k_sched_unlock();
		}
	}
}

/*___________________________________________________________________________*/