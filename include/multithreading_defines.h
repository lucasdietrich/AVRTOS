#ifndef _MULTITHREADING_DEFINES_H
#define _MULTITHREADING_DEFINES_H

/*___________________________________________________________________________*/

// Include user configuration

#include "config.h"

/*___________________________________________________________________________*/

//
// Multithreading Configuration
//

#define THREAD_DEFAULT_MAX                      4
#define THREAD_DEFAULT_MAIN_THREAD_PRIORITY     K_PRIO_PREEMPT(8)
#define THREAD_DEFAULT_EXPLICIT_MAIN_STACK      1
#define THREAD_DEFAULT_MAIN_STACK_SIZE          0x400
#define THREAD_DEFAULT_USE_INIT_STACK_ASM       1

/*___________________________________________________________________________*/

#ifdef CONFIG_THREAD_MAX
#if CONFIG_THREAD_MAX > 1
#define K_THREAD_MAX_COUNT CONFIG_THREAD_MAX
#else
#error Cannot configure this library for less than 2 threads (CONFIG_THREAD_MAX < 2)
#endif
#else
#define K_THREAD_MAX_COUNT THREAD_DEFAULT_MAX
#endif

#ifdef CONFIG_THREAD_MAIN_THREAD_PRIORITY
#define THREAD_MAIN_THREAD_PRIORITY   CONFIG_THREAD_MAIN_THREAD_PRIORITY
#else
#define THREAD_MAIN_THREAD_PRIORITY   THREAD_DEFAULT_MAIN_THREAD_PRIORITY
#endif

#ifdef CONFIG_THREAD_EXPLICIT_MAIN_STACK
#define THREAD_EXPLICIT_MAIN_STACK CONFIG_THREAD_EXPLICIT_MAIN_STACK
#else
#define THREAD_EXPLICIT_MAIN_STACK THREAD_DEFAULT_EXPLICIT_MAIN_STACK
#endif

#ifdef CONFIG_THREAD_MAIN_STACK_SIZE
#define THREAD_MAIN_STACK_SIZE  CONFIG_THREAD_MAIN_STACK_SIZE
#else
#define THREAD_MAIN_STACK_SIZE  THREAD_DEFAULT_MAIN_STACK_SIZE
#endif

#ifdef CONFIG_THREAD_USE_INIT_STACK_ASM
#define THREAD_USE_INIT_STACK_ASM   CONFIG_THREAD_USE_INIT_STACK_ASM
#else
#define THREAD_USE_INIT_STACK_ASM   THREAD_DEFAULT_USE_INIT_STACK_ASM
#endif

/*___________________________________________________________________________*/

#define K_SWAP_LITTLE_BIG_ENDIAN(u16) (((uint16_t)u16 << 8) | ((uint16_t)u16 >> 8))

// From datasheet
// Bit 7 6 5 4 3 2 1 0
// 0x3F (0x5F) I T H S V N Z C SREG
// Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
// Initial Value 0 0 0 0 0 0 0 0
#define K_THREAD_DEFAULT_SREG 0x00

#define K_THREAD_STACK_VOID_SIZE 35u
#define K_THREAD_STACK_MIN_SIZE K_THREAD_STACK_VOID_SIZE

#define K_PRIO_PREEMPT(p) (p)
#define K_PRIO_COOP(p) (-p)
#define K_PRIO_DEFAULT K_PRIO_COOP(1)
#define K_STOPPED 0

#define _K_STACK_END_ASM(stack_start, size) ((stack_start + size - 1))

#define K_STACK_END(stack_start, size) ((void *)((uint16_t)stack_start + size - 1))
#define K_STACK_START(stack_end, size) ((void *)((uint16_t)stack_end - size + 1))

#define K_STACK_INIT_SP(stack_end) ((uint16_t)stack_end - K_THREAD_STACK_VOID_SIZE)

#define K_THREAD_STACK_NAME(name) (&_k_stack_buf_##name)

#define K_STACK_INIT_SP_FROM_NAME(name, stack_size) K_STACK_INIT_SP(K_STACK_END(K_THREAD_STACK_NAME(name), stack_size))

#define K_THREAD_STACK_DEFINE(name, size) char _k_stack_buf_##name[size]

#define K_THREAD_STACK_SIZE(name) (sizeof(_k_stack_buf_##name))

#define _K_STACK_INITIALIZER(name, stack_size, entry, context_p) \
    struct                                                       \
    {                                                            \
        uint8_t empty[stack_size - K_THREAD_STACK_VOID_SIZE];    \
        struct                                                   \
        {                                                        \
            uint8_t sreg;                                        \
            uint8_t r26r31[6u];                                  \
            void *context;                                       \
            uint8_t r0r23[24u];                                  \
            void *ret_addr;                                      \
        } base;                                                  \
    } _k_stack_buf_##name = {                                    \
        {0x00},                                                  \
        {K_THREAD_DEFAULT_SREG,                                  \
         {0x00},                                                 \
         (void *)context_p,                                      \
         {0x00},                                                 \
         (void *)K_SWAP_LITTLE_BIG_ENDIAN((uint16_t)entry)}}

// in order to remove the K_SWAP_LITTLE_BIG_ENDIAN define here we need to pop and push registers in an inverted way

#define _K_THREAD_INITIALIZER(name, stack_size, prio, local_storage_p)                            \
    {                                                                                             \
        .sp = (void *)K_STACK_INIT_SP_FROM_NAME(name, stack_size),                                \
        .priority = prio,                                                                         \
        .stack = {.end = K_STACK_END(K_THREAD_STACK_NAME(name), stack_size), .size = stack_size}, \
        .local_storage = (void *)local_storage_p                                                  \
    }

#define K_THREAD_DEFINE(name, entry, stack_size, prio, context_p, local_storage_p)         \
    __attribute__((used)) static _K_STACK_INITIALIZER(name, stack_size, entry, context_p); \
    __attribute__((used)) static thread_t name = _K_THREAD_INITIALIZER(name, stack_size, prio, local_storage_p);

/*___________________________________________________________________________*/


#endif