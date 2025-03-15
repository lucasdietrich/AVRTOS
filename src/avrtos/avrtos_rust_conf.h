/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_SYS_RUST_DEFAULT_CONFIG_H
#define _AVRTOS_SYS_RUST_DEFAULT_CONFIG_H

/*
 * Adapter configuration in an rust environment
 * This file is included by avrtos_conf.h
 */

/* A linker script is provided by the rust environment */
#ifndef CONFIG_AVRTOS_LINKER_SCRIPT
#define CONFIG_AVRTOS_LINKER_SCRIPT 1
#endif

/* Kernel must be explicitly initialized in rust environment */
#ifndef CONFIG_KERNEL_AUTO_INIT
#define CONFIG_KERNEL_AUTO_INIT 0
#endif

#ifndef CONFIG_KERNEL_DELAY_OBJECT_U32
#define CONFIG_KERNEL_DELAY_OBJECT_U32 0
#endif

#ifndef CONFIG_AVRTOS_BANNER
#define CONFIG_AVRTOS_BANNER "*** AVRTOS RUST ***\n"
#endif

#ifndef CONFIG_THREAD_CANARIES
// Enabling this option breaks the execution
#define CONFIG_THREAD_CANARIES 1
#endif

// TODO how to configure ??
#ifndef CONFIG_KERNEL_GLOBAL_ALLOCATOR_SIZE
#define CONFIG_KERNEL_GLOBAL_ALLOCATOR_SIZE 4096
#endif

// TODO keep ??
#ifndef CONFIG_STDIO_PRINTF_TO_USART
#define CONFIG_STDIO_PRINTF_TO_USART 0
#endif

// Allow threads to termiante
#ifndef CONFIG_KERNEL_THREAD_TERMINATION_TYPE
#define CONFIG_KERNEL_THREAD_TERMINATION_TYPE 1
#endif

#endif /* _AVRTOS_SYS_RUST_DEFAULT_CONFIG_H */