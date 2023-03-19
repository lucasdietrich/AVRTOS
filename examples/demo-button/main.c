/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/drivers/exti.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>

K_SEM_DEFINE(button_sem, 0, 1);

ISR(INT0_vect)
{
	struct k_thread *thread;

	led_toggle();
	
	thread = k_sem_give(&button_sem);

	k_yield_from_isr_cond(thread);
}

int main(void)
{
	serial_init();

	led_init();
	led_off();

	gpiol_pin_init(GPIOD, PIN0, GPIO_INPUT, PIN_NO_PULLUP);

	/* Configure INT0 to trigger on falling edge */
	exti_configure(INT0, ISC_FALLING);
	exti_clear_flag(INT0);
	exti_enable(INT0);

	k_thread_dump_all();

	for (;;) {
		k_sem_take(&button_sem, K_FOREVER);

		k_show_uptime();
		printf_P(PSTR("Button pressed\n"));
	}
}