/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_CANARIES_H_
#define _AVRTOS_CANARIES_H_

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize canaries in thread stack.
 *
 * @param thread
 */
void z_init_thread_stack_canaries(struct k_thread *thread);

/**
 * @brief Automatically initialize thread canaries in stack
 * if configuration option CONFIG_THREAD_CANARIES is enabled.
 *
 * Stack is filled with symbol CONFIG_THREAD_CANARIES_SYMBOL
 *
 * Default THREAD_CANARIES_SYMBOl = 0xAA = 0b10101010
 */
__kernel void z_init_stacks_canaries(void);

/**
 * @brief Get the address of the furthest stack location used by the thread.
 * i.e. We loop until we found canaries for the thread {thread}.
 *
 * @param thread
 * @return void* Address of byte following the last found canary.
 * (Address of the furthest stack location used by the thread.)
 */
void *z_stack_canaries(struct k_thread *thread);

/**
 * @brief Pretty print found canaries in thread {thread}
 *
 * @param thread thread to check
 */
void k_print_stack_canaries(struct k_thread *thread);

/**
 * @brief Pretty print found canaries in the current thread
 *
 * [M] CANARIES until @07CE [found 468], MAX usage = 44 / 512
 */
void k_print_current_canaries(void);

/**
 * @brief Pretty print found canaries in all threads
 *
 * [M] CANARIES until @07CE [found 468], MAX usage = 44 / 512
 * [2] CANARIES until @010B [found 6], MAX usage = 506 / 512
 * [1] CANARIES until @03C7 [found 194], MAX usage = 62 / 256
 * [L] CANARIES until @04DE [found 217], MAX usage = 39 / 256
 * [K] CANARIES until @051F [found 26], MAX usage = 36 / 62
 *
 */
void k_dump_stack_canaries(void);

#ifdef __cplusplus
}
#endif

#endif