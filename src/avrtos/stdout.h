/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_STDIO_H_
#define _AVRTOS_STDIO_H_

#include <stdio.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set up the standard I/O to use serial USART.
 *
 * This function configures the standard I/O to communicate via USART. This allows
 * the function `printf`, to use USART for input and output.
 *
 * The configuration option `CONFIG_STDIO_USART` automatically selects the
 * USART to use for standard I/O.
 */
__kernel void k_set_stdio_serial(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_STDIO_H_ */
