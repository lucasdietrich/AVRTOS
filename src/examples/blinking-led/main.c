/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>

#include <avr/io.h>

#if defined(__AVR_ATmega328P__)
#	define LED_DDR DDRB
#	define LED_PORT PORTB
#	define LED_PIN 5U
#elif defined(__AVR_ATmega2560__)
#	define LED_DDR DDRB
#	define LED_PORT PORTB
#	define LED_PIN 7U
#else
#	error "Unsupported MCU"
#endif

#define PERIOD_MS 200U

int main(void)
{
	LED_DDR |= _BV(LED_PIN);
	LED_PORT &= ~_BV(LED_PIN);

	for (;;) {
		LED_PORT |= _BV(LED_PIN);
		k_sleep(K_MSEC(PERIOD_MS));
		LED_PORT &= ~_BV(LED_PIN);
		k_sleep(K_MSEC(PERIOD_MS));
	}
}