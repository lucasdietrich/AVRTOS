/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/kernel.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_DBG

#define BUILTIN_LED_PORT GPIOB_DEVICE
#define BUILTIN_LED_PIN	 (5u)

static char usart_msgq_buf[16u];
K_MSGQ_DEFINE(usart_msgq, usart_msgq_buf, 1u, 16u);

static void thread_usart(void *arg);
static void thread_led(void *arg);

K_THREAD_DEFINE(th_usart, thread_usart, 164u, K_COOPERATIVE, NULL, 'X');
K_THREAD_DEFINE(th_led, thread_led, 164u, K_COOPERATIVE, NULL, 'L');

ISR(USART0_RX_vect)
{
	const char c = USART0_DEVICE->UDRn;

	k_msgq_put(&usart_msgq, &c, K_NO_WAIT);
}

int main(void)
{
	const struct usart_config usart_config = {.baudrate    = USART_BAUD_500000,
						  .receiver    = 1u,
						  .transmitter = 1u,
						  .mode	       = USART_MODE_ASYNCHRONOUS,
						  .parity      = USART_PARITY_NONE,
						  .stopbits    = USART_STOP_BITS_1,
						  .databits    = USART_DATA_BITS_8,
						  .speed_mode  = USART_SPEED_MODE_NORMAL};
	ll_usart_init(USART0_DEVICE, &usart_config);
	ll_usart_enable_rx_isr(USART0_DEVICE);

	gpio_pin_init(BUILTIN_LED_PORT, BUILTIN_LED_PIN, GPIO_OUTPUT, 0u);

	LOG_INF("Application started");

	k_thread_dump_all();

	k_stop();
}

static void thread_usart(void *arg)
{
	char c;
	for (;;) {
		if (k_msgq_get(&usart_msgq, &c, K_FOREVER) >= 0) {
			k_show_uptime();
			LOG_INF("<inf> Received: %c", c);
		}
	}
}

static void thread_led(void *arg)
{
	for (;;) {
		k_show_uptime();
		gpio_pin_toggle(BUILTIN_LED_PORT, BUILTIN_LED_PIN);
		LOG_DBG("<dbg> toggled LED");
		k_sleep(K_MSEC(500u));
	}
}