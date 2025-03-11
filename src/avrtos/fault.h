/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Fault
 *
 * This header defines the fault reason codes and a function to trigger a *fault.
 *
 * Fault is a critical error which is willingly trigger by the kernel when
 * it reaches an unexpected or unrecoverable state.
 *
 * It is possible to define the hook function `__fault_hook` that is called
 * before the debug information is printed.
 *
 * TODOs:
 * - Modify __assert to use __fault.
 *
 * Related configuration options:
 * - CONFIG_KERNEL_FAULT_VERBOSITY: Allows to control the verbosity of the fault
 *   debug information.
 */

#ifndef _AVRTOS_FAULT_H_
#define _AVRTOS_FAULT_H_

#include <stdint.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

// Fault codes
#define K_FAULT_ASSERT			  0 // Assertion failure
#define K_FAULT_STACK_SENTINEL	  1 // Stack sentinel value corrupted
#define K_FAULT_STACK_OVERFLOW	  2 // Stack overflow detected
#define K_FAULT_MEMORY			  3 // Memory-related fault
#define K_FAULT_THREAD_TERMINATED 4 // Thread terminated unexpectedly
#define K_FAULT_KERNEL_HALT		  5 // Kernel halt due to critical error
#define K_FAULT_RUST_PANIC        6 // Rust panic

#define K_FAULT_ANY 0xFF

/**
 * @brief fault function alias
 */
#define panic __fault(K_FAULT_ANY)

#if !defined(__ASSEMBLER__)

/**
 * @brief Fault handler function
 *
 * The `__fault` function disables interrupts, print some debug information using
 * the system serial port, and the program execution is halted:
 * 1. cli
 * 2. * print debug information *
 * 3. jmp _exit
 *
 * @param reason The fault code indicating the type of fault.
 */
void __fault(uint8_t reason);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_FAULT_H_ */
