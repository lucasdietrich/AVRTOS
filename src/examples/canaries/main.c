/**
 * @file main.cpp
 * @author Dietrich Lucas (ld.adecy@gmail.com)
 * @brief Use of canaries to determine precise stacks needs for different threads
 * @version 0.1
 * @date 2021-08-06
 *
 * @copyright Copyright (c) 2021
 *
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

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void thread_led(void *p);
void thread_task1(void *p);
void thread_task2(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PREEMPTIVE, NULL, 'L');
K_THREAD_DEFINE(task1, thread_task1, 0x100, K_PREEMPTIVE, NULL, '1');
K_THREAD_DEFINE(task2, thread_task2, 0x200, K_PREEMPTIVE, NULL, '2');

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	usart_init();

	k_thread_dump_all();

	dump_stack_canaries();

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
		dump_stack_canaries();

		k_sleep(K_MSEC(1000));

		// USART_DUMP_RAM_ALL();

		// k_sleep(K_MSEC(10000));
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