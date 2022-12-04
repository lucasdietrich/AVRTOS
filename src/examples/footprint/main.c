/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/pgmspace.h>

#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>

/* Minimum footprint (Release for ATmega2560)
	Memory region         Used Size  Region Size  %age Used
		text:        1452 B       256 KB      0.55%
		data:          31 B         8 KB      0.38%
*/
int main(void)
{
	usart_init();

	for (;;) {
		usart_print_p(PSTR("Hello\n"));
		k_wait(K_SECONDS(1));
	}
}
