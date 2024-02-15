/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_FAULT_H_
#define _AVRTOS_FAULT_H_

#include <stdint.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

// fault codes
#define K_FAULT_ASSERT		  0
#define K_FAULT_STACK_SENTINEL	  1
#define K_FAULT_STACK_OVERFLOW	  2
#define K_FAULT_MEMORY		  3
#define K_FAULT_THREAD_TERMINATED 4
#define K_FAULT_KERNEL_HALT	  5

#define K_FAULT_ANY 0xFF

#if !defined(__ASSEMBLER__)

void __fault(uint8_t reason);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_FAULT_H_ */