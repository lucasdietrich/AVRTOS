/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_IO_H_
#define _AVRTOS_IO_H_

#include "avrtos.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

__kernel void k_set_stdio_usart0(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_IO_H_ */