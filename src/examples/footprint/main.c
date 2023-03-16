/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/misc/serial.h>

#include <avr/pgmspace.h>

/* Minimum footprint (Release for ATmega2560)
	Memory region         Used Size  Region Size  %age Used
		text:        1448 B       256 KB      0.55%
		data:          31 B         8 KB      0.38%
*/
int main(void)
{
	serial_init();

	for (;;) {
		serial_print_p(PSTR("Hello\n"));
		k_wait(K_SECONDS(1));
	}
}
