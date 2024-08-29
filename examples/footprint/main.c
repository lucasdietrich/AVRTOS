/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/misc/serial.h>

#include <avr/pgmspace.h>

/* Footprint (Release for ATmega2560)
	Memory region         Used Size  Region Size  %age Used
				text:        1408 B       256 KB      0.54%
				data:          31 B         8 KB      0.38%
*/
int main(void)
{
	serial_init();

	for (;;) {
		serial_print_p(PSTR("Hello\n"));
		k_wait(K_SECONDS(1), K_WAIT_MODE_IDLE);
	}
}
