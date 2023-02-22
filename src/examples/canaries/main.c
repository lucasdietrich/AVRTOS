/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Configuration options
 *  - CONFIG_THREAD_CANARIES=1
 *  - CONFIG_THREAD_EXPLICIT_MAIN_STACK=1
 *
 * Logs :
 *
 * ...
 *
 * [M] CANARIES until @07CE [found 468], MAX usage = 44 / 512
 * [2] CANARIES until @0113 [found 14], MAX usage = 498 / 512
 * [1] CANARIES until @03C7 [found 194], MAX usage = 62 / 256
 * [L] CANARIES until @04DE [found 217], MAX usage = 39 / 256
 * [K] CANARIES until @051F [found 26], MAX usage = 36 / 62
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

static uint8_t dthread_stack[0x40u];
static struct k_thread dthread;

void thread_led(void *p);
void thread_task1(void *p);
void thread_task2(void *p);
void dthread_entry(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PREEMPTIVE, NULL, 'L');
K_THREAD_DEFINE(task1, thread_task1, 0x100, K_PREEMPTIVE, NULL, '1');
K_THREAD_DEFINE(task2, thread_task2, 0x200, K_PREEMPTIVE, NULL, '2');

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	k_dump_stack_canaries();

	k_thread_create(&dthread,
			dthread_entry,
			dthread_stack,
			sizeof(dthread_stack),
			K_PREEMPTIVE,
			&dthread,
			'D');
	k_thread_start(&dthread);

	k_sleep(K_FOREVER);
}

void dthread_entry(void *p)
{
	struct k_thread *thread = (struct k_thread *)p;

	(void)thread;

	k_sleep(K_FOREVER);
}

void thread_led(void *p)
{
	while (1) {
		led_on();

		k_sleep(K_MSEC(1000));

		led_off();
	}
}

void thread_task1(void *p)
{
	while (1) {
		k_dump_stack_canaries();

		k_sleep(K_MSEC(1000));
	}
}

void thread_task2(void *p)
{
	uint16_t blocks = 0;

	while (1) {
		blocks = (blocks + 1) % (0x200 - 30);

		if (blocks != 0) {
			cli();
			SP -= (blocks - 1);
			*((uint8_t *)SP--) = 0xBB;
			sei();

			// at this moment we need the most of the stack
			k_yield();

			cli();
			SP += blocks;
			sei();
		}

		k_sleep(K_MSEC(50));
	}
}

/*___________________________________________________________________________*/