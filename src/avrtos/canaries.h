/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Stack canaries
 *
 * The kernel provides stack usage monitoring using canaries, which are used to detect
 * stack overflows by measuring how much of the stack has been used by finding the
 * remaining canaries.
 *
 * If the configuration option CONFIG_THREAD_CANARIES is enabled, the kernel will
 * fill the stack with the value specified by CONFIG_THREAD_CANARIES_SYMBOL.
 *
 * As the stack grows, the canaries are overwritten with the stack data. The kernel
 * can then determine the maximum stack usage by finding the last canary that has not
 * been overwritten.
 *
 * This can be coupled with the stack sentinel feature.
 *
 * Related configuration options:
 * - CONFIG_THREAD_CANARIES: Enable to use stack canaries for stack usage monitoring.
 * - CONFIG_AVRTOS_LINKER_SCRIPT: Enable to use the linker script to statically
 *   initialize stack canaries for all threads.
 * - CONFIG_THREAD_CANARIES_SYMBOL: The symbol used to fill the stack canaries.
 */

#ifndef _AVRTOS_CANARIES_H_
#define _AVRTOS_CANARIES_H_

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize stack canaries for a specific thread.
 *
 * This function initializes the stack canaries for the given thread.
 *
 * @param thread Pointer to the thread whose stack canaries are to be initialized.
 */
void z_init_thread_stack_canaries(struct k_thread *thread);

/**
 * @brief Automatically initialize stack canaries for all threads.
 *
 * This function automatically initializes stack canaries for all threads
 * defined in the .k_threads section.
 */
__kernel void z_init_stacks_canaries(void);

/**
 * @brief Get the address of the furthest used stack location in a thread.
 *
 * This function checks the stack of the specified thread and returns the
 * address of the byte following the last found canary. This represents
 * the furthest stack location that has been used by the thread, which
 * helps in determining stack usage.
 *
 * The stack usage can then be calculated with:
 *
 *   stack_usage = Z_STACK_SIZE_USABLE(thread->stack.size) - z_stack_canaries(thread);
 *
 * @param thread Pointer to the thread whose stack usage is to be checked.
 * @return void* Address of the byte following the last canary found,
 * representing the furthest used stack location.
 */
void *z_stack_canaries(struct k_thread *thread);

/**
 * @brief Print stack canary information for a specific thread.
 *
 * This function prints detailed information about the stack canaries
 * found in the specified thread.
 *
 * @param thread Pointer to the thread whose stack canaries are to be printed.
 */
void k_print_stack_canaries(struct k_thread *thread);

/**
 * @brief Print stack canary information for the current thread.
 *
 * This function prints detailed information about the stack canaries
 * found in the current thread.
 */
void k_print_current_canaries(void);

/**
 * @brief Print stack canary information for all threads.
 *
 * This function prints detailed information about the stack canaries
 * found in all threads defined in the .k_threads section.
 */
void k_dump_stack_canaries(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_CANARIES_H_ */
