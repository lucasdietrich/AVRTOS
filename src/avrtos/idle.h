/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Idle
 *
 * The idle module in AVRTOS is responsible for managing the system's idle thread,
 * which runs when no other threads are ready to execute.
 *
 * Key Features:
 * - **Idle Thread Management**: The idle thread is automatically scheduled when the
 *   system has no other runnable threads. It operates at the lowest priority
 * - **Custom Idle Behavior**: Users can define custom behavior during idle periods by
 *   overriding the weak `z_idle_hook()` function.
 *
 * QEMU Considerations: Specific adaptations are included for running the idle
 * thread under QEMU, acknowledging potential differences from real hardware
 * behavior.
 *
 * Example Usage:
 *
 *  - When no threads are ready to run, the idle thread automatically takes over,
 *    putting the CPU into sleep mode until an interrupt occurs.
 *  - Users can implement custom IDLE strategies by overriding the idle hook
 *    function to perform additional actions during idle periods.
 *
 * Related Configuration Options:
 * - CONFIG_AVRTOS_LINKER_SCRIPT: Enables the use of the linker script
 * - CONFIG_KERNEL_THREAD_IDLE: Enables the idle thread.
 * - CONFIG_THREAD_IDLE_COOPERATIVE: Configures the idle thread to operate in cooperative
 * mode.
 * - CONFIG_KERNEL_THREAD_IDLE_ADD_STACK: Adjusts the stack size for the idle thread.
 * - CONFIG_IDLE_HOOK: Enables a custom idle hook function.
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
 * @brief Idle thread structure.
 *
 * This external structure represents the idle thread, which is used by the
 * system when no other threads are ready to run.
 */
extern struct k_thread z_thread_idle;

/**
 * @brief Check if the CPU is idle.
 *
 * This function checks whether the runqueue contains only the idle thread,
 * indicating that the CPU is currently idle.
 *
 * @return true if the CPU is idle, false otherwise.
 */
__kernel bool k_is_cpu_idle(void);

/**
 * @brief Enter idle mode.
 *
 * This function can be called from any thread to make it behave like an idle
 * thread. It should not be called from an ISR.
 *
 * When called, the function checks if other threads are ready to run:
 * - If other threads are ready, it calls `k_yield()` to allow them to run.
 * - If no other threads are ready, the MCU is put into sleep mode until an
 *   interrupt occurs.
 *
 * The function keeps the thread as "ready," meaning that if an interrupt
 * occurs or other threads finish their time slice, the thread calling k_idle()
 * will be scheduled to run again in the first place.
 * Except if k_yield_from_isr() is called from an ISR context and the current
 * thread is PREEMPTIVE.
 *
 * @note This function requires interrupts to be enabled.
 */
__kernel void k_idle(void);

#ifdef __cplusplus
}
#endif

#endif /* _IDLE_H_ */
