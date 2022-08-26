/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>

#include <avrtos/drivers/timer.h>
#include <avrtos/misc/uart.h>

void callback(void *dev, uint8_t tim_idx, void *arg)
{
	printf("%d : %p (%p)\n", tim_idx, dev, arg);

	if (tim_idx != 5) {
		/* stop timer counter */
		ll_timer_stop(dev);

		/* if callback takes more time than the timer period,
		* the interrupt flag will be set again by the peripheral and the callback
		* will be called a last time after this one.
		*
		* To avoid this, clear all interrupt flags.
		*/
		ll_timer_clear_irq_flags(tim_idx);
	}
}

static void app_init_timer(uint8_t tim_idx, uint32_t period_us, uint8_t flags)
{
	int ret = timer_init(tim_idx, period_us, callback, NULL, flags);
	if (ret != 0) {
		printf_P(PSTR("timer_init(%d, %lu, %p, %p, %u) err = %d\n"),
			 tim_idx, period_us, callback, NULL, flags, ret);
	}
}

int main(void)
{
	usart_init();

	uint32_t period_us = timer_get_max_period_us(5);
	printf_P(PSTR("Timer 5 max period = %lu us\n"), period_us);
	app_init_timer(5, period_us, 0U);
	timer_start(5);

	app_init_timer(4, 1LU * USEC_PER_SEC, TIMER_API_FLAG_AUTOSTART);

	app_init_timer(2, 200LU, TIMER_API_FLAG_AUTOSTART);

	k_sleep(K_FOREVER);
}