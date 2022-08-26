/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>

#if defined(__AVR_ATmega328P__)
#   define BUILTIN_LED_PORTB_BIT   0b00100000
#else
#   if defined(__AVR_ATmega2560__)
#       define BUILTIN_LED_PORTB_BIT 0b10000000	
#   else
#       define BUILTIN_LED_PORTB_BIT 0
#   endif
#endif