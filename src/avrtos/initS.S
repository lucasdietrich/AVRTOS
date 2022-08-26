/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>

#include "defines.h"

.extern _k_main_stack
.extern _k_avrtos_init

#if KERNEL_AUTO_INIT == 1

#if THREAD_EXPLICIT_MAIN_STACK == 1
.section .init3,"ax",@progbits
ldi r29, hi8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
ldi r28, lo8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
sts SPH, r29
sts SPL, r28
#endif 
; THREAD_EXPLICIT_MAIN_STACK

.section .init8,"ax",@progbits
call _k_avrtos_init

#endif
; KERNEL_AUTO_INIT