/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/avrtos.h>
#include <avrtos/logging.h>
#include <avrtos/misc/led.h>

#include <avr/io.h>
#define LOG_LEVEL LOG_LEVEL_DBG

ISR(TIMER1_COMPA_vect)
{
	serial_transmit('A');
}

ISR(TIMER1_COMPB_vect)
{
	serial_transmit('B');
}

ISR(TIMER1_COMPC_vect)
{
	serial_transmit('C');
}

ISR(TIMER1_OVF_vect)
{
	serial_transmit('0');
}

ISR(TIMER1_CAPT_vect)
{
	serial_transmit('=');
}

int main(void)
{
	serial_init();

	const struct timer_config config = {
		.mode	   = TIMER_MODE_FAST_PWM_ICR1,
		.counter   = 0u,
		.prescaler = TIMER_PRESCALER_1,
		.timsk	   = 0u,
	};

	/* Configure timer1 pins as output */
	gpiol_pin_init(GPIOB, 5u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
	gpiol_pin_init(GPIOB, 6u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
	gpiol_pin_init(GPIOB, 7u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

	ll_timer16_init(TIMER1_DEVICE, timer_get_index(TIMER1_DEVICE), &config);
	ll_timer16_write_reg16(&TIMER1_DEVICE->IRCN, 1u); /* Set ICR1 to 0x8000 (32768) */

	struct timer_channel_compare_config comp_conf = {
		.mode  = TIMER_CHANNEL_COMP_MODE_SET,
		.value = 0,
	};
	ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_A, &comp_conf);
	ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_B, &comp_conf);
	ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_C, &comp_conf);

	ll_timer_set_enable_int_mask(timer_get_index(TIMER1_DEVICE),
				     0x2Fu); /* Enable all interupts */

	for (;;) {
		k_sleep(K_MSEC(500u));
	}
}