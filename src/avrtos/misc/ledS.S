/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>

#include "board.h"

.global led_init
.global led_on
.global led_off

led_init:
    ldi r24, BUILTIN_LED_PORTB_BIT
    sts DDRB, r24
    ret

led_on:
    ldi r24, BUILTIN_LED_PORTB_BIT
    sts PORTB, r24
    ret
    
led_off:
    ldi r24, 0b00000000
    sts PORTB, r24
    ret