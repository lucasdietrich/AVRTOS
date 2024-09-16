/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/logging.h>
#include <avrtos/misc/led.h>

#include <avrtos.h>
#define LOG_LEVEL LOG_LEVEL_DBG

K_MSGQ_DEFINE(usart_msgq, 1u, 16u);

static struct k_thread thread_usart;
static uint8_t thread_usart_stack[164u];
static void thread_usart_task(void *arg);

static struct k_thread thread_led;
static uint8_t thread_led_stack[164u];
static void thread_led_task(void *arg);

ISR(USART0_RX_vect)
{
	const char c = USART0_DEVICE->UDRn;

	k_msgq_put(&usart_msgq, &c, K_NO_WAIT);
}

void setup(void)
{
	const struct usart_config usart_config = {
		.baudrate	 = USART_BAUD_9600,
		.receiver	 = 1u,
		.transmitter = 1u,
		.mode		 = USART_MODE_ASYNCHRONOUS,
		.parity		 = USART_PARITY_NONE,
		.stopbits	 = USART_STOP_BITS_1,
		.databits	 = USART_DATA_BITS_8,
		.speed_mode	 = USART_SPEED_MODE_NORMAL,
	};
	ll_usart_init(USART0_DEVICE, &usart_config);
	ll_usart_enable_rx_isr(USART0_DEVICE);

	led_init();

	k_thread_create(&thread_usart, thread_usart_task, thread_usart_stack,
					sizeof(thread_usart_stack), K_COOPERATIVE, NULL, 'X');
	k_thread_create(&thread_led, thread_led_task, thread_led_stack,
					sizeof(thread_led_stack), K_COOPERATIVE, NULL, 'L');

	LOG_INF("Application started");

	k_thread_dump(&thread_usart);
	k_thread_dump(&thread_led);

	k_thread_start(&thread_usart);
	k_thread_start(&thread_led);
}

void loop(void)
{
	/* loop() is executed in the context of the main thread which is cooperative,
	 * therefore we need to give the other threads a chance to run, this is done
	 * through the k_idle() function.
	 */
	k_idle();
}

static void thread_usart_task(void *arg)
{
	char c;
	for (;;) {
		if (k_msgq_get(&usart_msgq, &c, K_FOREVER) >= 0) {
			k_show_uptime();
			LOG_INF("<inf> Received: %c", c);
		}
	}
}

static void thread_led_task(void *arg)
{
	for (;;) {
		k_show_uptime();
		led_toggle();
		LOG_DBG("<dbg> toggled LED");

		/* As serial baudrate is very slow, the thread will take a lot of time
		 * to print the uptime and debug message, therefore the LED will blink
		 * slower than 2Hz.
		 *
		 * I advise to use a faster baudrate (e.g. 115200) or comment out the
		 * k_show_uptime() and LOG_DBG() calls.
		 */
		k_sleep(K_MSEC(500u));
	}
}