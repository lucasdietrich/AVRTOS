/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _IDLE_H_
#define _IDLE_H_

#include "avrtos.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Idle thread structure
 */
extern struct k_thread z_thread_idle;

/**
 * @brief Tells if the runqueue contains the thread IDLE
 *
 * @return true
 * @return false
 */

K_NOINLINE bool k_is_cpu_idle(void);

/**
 * @brief IDLE the CPU.
 *
 * This function can be called from a thread to make it behave as an IDLE
 * thread.
 *
 * Important: if others thread a ready, they will be blocked until an interrupt
 * occurs !
 *
 * Note: This function forces interrupts to be enabled.
 */
K_NOINLINE void k_idle(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif