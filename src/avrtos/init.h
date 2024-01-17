/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_INIT_H_
#define _AVRTOS_INIT_H_

#include <avr/io.h>

#include "defines.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE
extern uint8_t z_mcusr;

/**
 * @brief Retrieve the reset cause
 *
 * @return uint8_t
 */
__always_inline uint8_t z_get_mcusr(void)
{
	return z_mcusr;
}
#endif	// CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE

#if CONFIG_THREAD_EXPLICIT_MAIN_STACK

// #error "(TODO) INVESTIGATE SP PROBLEM with
// CONFIG_THREAD_EXPLICIT_MAIN_STACK=1"
#define K_KERNEL_LINK_SP_INIT()                                                          \
	__attribute__((naked, used, section(".init3"))) void z_kernel_sp(void)           \
	{                                                                                \
		extern char z_main_stack[];                                              \
		SP = (uint16_t)Z_STACK_END(z_main_stack, CONFIG_THREAD_MAIN_STACK_SIZE); \
	}
#else

#define K_KERNEL_LINK_SP_INIT()

#endif

#define K_KERNEL_LINK_AVRTOS_INIT()                                              \
	__attribute__((naked, used, section(".init8"))) void k_avrtos_init(void) \
	{                                                                        \
		z_avrtos_init();                                                 \
	}

#define K_KERNEL_LINK_INIT()    \
	K_KERNEL_LINK_SP_INIT() \
	K_KERNEL_LINK_AVRTOS_INIT()

#define K_KERNEL_INIT K_KERNEL_LINK_INIT

#define K_AVRTOS_INIT K_KERNEL_INIT

/**
 * @brief Manual user AVRTOS init
 */
void z_avrtos_init(void);

#ifdef __cplusplus
}
#endif

#endif