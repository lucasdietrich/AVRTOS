/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>

#include "defines.h"

.extern z_main_stack
.extern z_avrtos_init

#if (CONFIG_KERNEL_AUTO_INIT == 1) && (CONFIG_ARDUINO_FRAMEWORK == 0)

#if CONFIG_THREAD_EXPLICIT_MAIN_STACK == 1
.section .init3,"ax",@progbits
ldi r29, hi8(Z_STACK_END_ASM(z_main_stack, CONFIG_THREAD_MAIN_STACK_SIZE))
ldi r28, lo8(Z_STACK_END_ASM(z_main_stack, CONFIG_THREAD_MAIN_STACK_SIZE))
sts SPH, r29
sts SPL, r28
#endif 
; CONFIG_THREAD_EXPLICIT_MAIN_STACK

.section .init8,"ax",@progbits
call z_avrtos_init

#endif
; CONFIG_KERNEL_AUTO_INIT