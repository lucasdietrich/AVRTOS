/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

.global z_interrupts

z_interrupts:
	/*
	 * Helper function, which returns 1 if interrupts are enabled, 0 otherwise.
	 */
	ldi r24, 0x00
	lds r25, SREG
	sbrc r25, SREG_I
	ldi r24, 0x01
	ret


#if CONFIG_KERNEL_TICKS_COUNTER
.global k_ticks_get_32
.global k_ticks_get_64
k_ticks_get_32:
	lds	r26, SREG
	cli
	lds 	r22, z_ker + 2
	lds 	r23, z_ker + 3
	lds 	r24, z_ker + 4
	sts	SREG, r26
	lds 	r25, z_ker + 5
	ret

k_ticks_get_64:
	lds	r26, SREG
	cli
	lds 	r18, z_ker
	lds 	r19, z_ker + 3
	lds 	r20, z_ker + 4
	lds 	r21, z_ker + 5
	sts	SREG, r26
#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
	lds	r22, z_ker + 5
#else
	ldi	r22, 0x00
#endif /* CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS */
	ldi	r23, 0x00
	ldi	r24, 0x00
	ldi	r25, 0x00
	ret
#endif /* CONFIG_KERNEL_TICKS_COUNTER */