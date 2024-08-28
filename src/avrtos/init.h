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
 * @brief Retrieve the reset cause.
 *
 * @return uint8_t The value of the MCU status register (MCUSR).
 */
__always_inline uint8_t z_get_mcusr(void)
{
	return z_mcusr;
}
#endif // CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE

#if CONFIG_THREAD_EXPLICIT_MAIN_STACK

// #error "(TODO) INVESTIGATE SP PROBLEM with CONFIG_THREAD_EXPLICIT_MAIN_STACK=1"

/**
 * @brief Set the stack pointer to the end of the main stack.
 *
 * This macro defines a function that initializes the stack pointer (SP) to
 * point to the end of the main stack, as defined by `CONFIG_THREAD_MAIN_STACK_SIZE`.
 * This function is placed in the `.init3` section of the AVR program, ensuring
 * it is executed early in the startup sequence (before the C++ constructors).
 */
#define K_KERNEL_LINK_SP_INIT()                                                          \
	__attribute__((naked, used, section(".init3"))) void z_kernel_sp(void)               \
	{                                                                                    \
		extern char z_main_stack[];                                                      \
		SP = (uint16_t)Z_STACK_END(z_main_stack, CONFIG_THREAD_MAIN_STACK_SIZE);         \
	}
#else

/**
 * @brief Empty macro for cases where the default main stack is used.
 */
#define K_KERNEL_LINK_SP_INIT()

#endif

/**
 * @brief Declare function to automatically initialize AVRTOS.
 *
 * This macro defines a function that calls `z_avrtos_init`, the main initialization
 * function for the AVRTOS kernel. This function is placed in the `.init8` section
 * of the AVR program, not strong requirements but it is executed after the
 * copy of the `.data` and `.bss` sections and before the main function.
 */
#define K_KERNEL_LINK_AVRTOS_INIT()                                                      \
	__attribute__((naked, used, section(".init8"))) void k_avrtos_init(void)             \
	{                                                                                    \
		z_avrtos_init();                                                                 \
	}

/**
 * @brief Initialize the kernel and set up the stack pointer.
 *
 * This macro combines the stack pointer initialization (if applicable) and
 * the AVRTOS kernel initialization into a single macro for convenience.
 */
#define K_KERNEL_LINK_INIT()                                                             \
	K_KERNEL_LINK_SP_INIT()                                                              \
	K_KERNEL_LINK_AVRTOS_INIT()

/**
 * @brief Alias for initializing the AVRTOS kernel.
 */
#define K_KERNEL_INIT K_KERNEL_LINK_INIT

/**
 * @brief Alias for initializing the AVRTOS kernel.
 */
#define K_AVRTOS_INIT K_KERNEL_INIT

/**
 * @brief Perform manual initialization of the AVRTOS kernel.
 *
 * This function is the main entry point for manually initializing the AVRTOS
 * kernel. It must be called if automatic initialization is disabled, when:
 * - `CONFIG_AVRTOS_LINKER_SCRIPT` is disabled
 * - or  `CONFIG_KERNEL_AUTO_INIT` is disabled
 * - or the framework does not provide a mechanism to call the `k_avrtos_init`
 * function early (Arduino does through `initVariant`).
 */
void z_avrtos_init(void);

#ifdef __cplusplus
}
#endif

#endif
