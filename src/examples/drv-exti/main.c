/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/drivers/exti.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/kernel.h>

ISR(INT0_vect)
{
	/* Generate signal on measurement line */
	gpio_pin_toggle(GPIOB_DEVICE, PIN7);

	/* Generate a soft int on INT1 */
	gpio_pin_toggle(GPIOD_DEVICE, PIN1);
}

ISR(INT1_vect)
{
	/* Generate signal on measurement line */
	gpio_pin_toggle(GPIOB_DEVICE, PIN6);
}

ISR(PCINT2_vect)
{
	/* Generate signal on measurement line */
	gpio_pin_toggle(GPIOB_DEVICE, PIN5);
}

int main(void)
{
	/* Init INT gpios */
	gpio_init(GPIOB_DEVICE, 0xFF, 0x00);

	/* Init measurements gpios */
	gpio_init(GPIOD_DEVICE, 0xFF, 0x00);
	gpio_pin_set_direction(GPIOD_DEVICE, PIN0, GPIO_OUTPUT);
	gpio_pin_write_state(GPIOD_DEVICE, PIN0, STATE_LOW);
	gpio_pin_set_direction(GPIOD_DEVICE, PIN1, GPIO_OUTPUT);
	gpio_pin_write_state(GPIOD_DEVICE, PIN1, STATE_LOW);

#if defined(GPIOH_DEVICE)
	/* Unecessary, just to have clean signals on my lines */
	gpio_init(GPIOH_DEVICE, 0xFF, 0x00);
#endif

	/* Configure INT0 */
	exti_configure(INT0, ISC_EDGE);
	exti_clear_flag(INT0);
	exti_enable(INT0);

	/* Configure INT1 */
	exti_configure(INT1, ISC_EDGE);
	exti_clear_flag(INT1);
	exti_enable(INT1);

#if defined(GPIOK)
	/* Configure GPIOK */
	gpio_init(GPIOK, 0xFF, 0x00);
#endif

	/* Configure PCINT16-23 */
	pci_configure(PCINT_16_23, 0xFFu);
	pci_clear_flag(PCINT_16_23);
	pci_enable(PCINT_16_23);

	/* Enable interrupts */
	irq_enable();

	for (;;) {
		/* Generate a soft int on INT0 */
		gpio_pin_toggle(GPIOD_DEVICE, PIN0);
		k_sleep(K_MSEC(1u));
	}
}