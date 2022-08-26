/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_FAULT_H_
#define _AVRTOS_FAULT_H_

#include <stdint.h>

#include "avrtos.h"

#ifdef __cplusplus
extern "C" {
#endif

#define K_FAULT_ASSERT    	0
#define K_FAULT_SENTINEL	1
#define K_FAULT_MEMORY		2
#define K_THREAD_TERMINATED	3
#define K_FAULT_KERNEL_HALT	4

#define K_FAULT 		0xFF

#if !defined(__ASSEMBLER__)

void __fault(uint8_t reason);

#endif

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_FAULT_H_ */