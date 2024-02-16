/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_STACK_SENTINEL_H_
#define _AVRTOS_STACK_SENTINEL_H_

#include <stdbool.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Verify if stack sentinel byte(s) is(are) still intact.
 *
 * @param thread
 * @return true
 * @return false
 */
bool k_verify_stack_sentinel(struct k_thread *thread);

/**
 * @brief Check all registered stack sentinel bytes and fault on error.
 *
 * @param thread
 * @return true
 * @return false
 */
void k_assert_registered_stack_sentinel(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_STACK_SENTINEL_H_ */