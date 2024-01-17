/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _IDLE_H_
#define _IDLE_H_

#include <stdbool.h>
#include <stddef.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Idle thread structure
 */
extern struct k_thread z_thread_idle;

/**
 * @brief Tells whether the runqueue contains the thread IDLE
 *
 * @return true
 * @return false
 */

__kernel bool k_is_cpu_idle(void);

/**
 * @brief IDLE the CPU.
 *
 * This function can be called from any thread to make it behave as an IDLE
 * thread.
 *
 * This function shouldn't be called from an ISR.
 *
 * If others thread a ready, the function will call k_yield() to give them
 * a chance to run. If no other thread is ready, the MCU is put in sleep mode
 * until an interrupt occurs.
 *
 * The function keeps the thread as "ready". This means that if an interrupt
 * occurs or other threads finished to consume their time slice,
 * the thread will be scheduled to run again.
 *
 * Note: This function requires interrupts to be enabled.
 */
__kernel void k_idle(void);

#ifdef __cplusplus
}
#endif

#endif