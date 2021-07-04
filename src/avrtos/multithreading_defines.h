#ifndef _MULTITHREADING_DEFINES_H
#define _MULTITHREADING_DEFINES_H

/*___________________________________________________________________________*/

// Include user configuration

// CONFIG_THREAD_MAX
// CONFIG_THREAD_MAIN_THREAD_PRIORITY
// CONFIG_THREAD_EXPLICIT_MAIN_STACK
// CONFIG_THREAD_MAIN_STACK_SIZE
// CONFIG_THREAD_USE_INIT_STACK_ASM

/*___________________________________________________________________________*/

#include "default_config.h"

/*___________________________________________________________________________*/

#define K_PRIO_PREEMPT(p)                       (p)
#define K_PRIO_COOP(p)                          (-p)
#define K_PRIO_DEFAULT                          K_PRIO_COOP(1)
#define K_STOPPED                               0

/*___________________________________________________________________________*/


#ifdef CONFIG_THREAD_MAX
#if CONFIG_THREAD_MAX > 1
#define K_THREAD_MAX_COUNT CONFIG_THREAD_MAX
#else
#error Cannot configure this library for less than 2 threads (CONFIG_THREAD_MAX < 2)
#endif
#else
#define K_THREAD_MAX_COUNT DEFAULT_THREAD_MAX
#endif

#ifdef CONFIG_THREAD_MAIN_THREAD_PRIORITY
#define THREAD_MAIN_THREAD_PRIORITY CONFIG_THREAD_MAIN_THREAD_PRIORITY
#else
#define THREAD_MAIN_THREAD_PRIORITY DEFAULT_THREAD_MAIN_THREAD_PRIORITY
#endif

#ifdef CONFIG_THREAD_EXPLICIT_MAIN_STACK
#define THREAD_EXPLICIT_MAIN_STACK CONFIG_THREAD_EXPLICIT_MAIN_STACK
#else
#define THREAD_EXPLICIT_MAIN_STACK DEFAULT_THREAD_EXPLICIT_MAIN_STACK
#endif

#ifdef CONFIG_THREAD_MAIN_STACK_SIZE
#define THREAD_MAIN_STACK_SIZE CONFIG_THREAD_MAIN_STACK_SIZE
#else
#define THREAD_MAIN_STACK_SIZE DEFAULT_THREAD_MAIN_STACK_SIZE
#endif

#ifdef CONFIG_THREAD_USE_INIT_STACK_ASM
#define THREAD_USE_INIT_STACK_ASM CONFIG_THREAD_USE_INIT_STACK_ASM
#else
#define THREAD_USE_INIT_STACK_ASM DEFAULT_THREAD_USE_INIT_STACK_ASM
#endif

/*___________________________________________________________________________*/

#define K_SWAP_LITTLE_BIG_ENDIAN(u16) (((uint16_t)u16 << 8) | ((uint16_t)u16 >> 8))

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// From datasheet
// Bit 7 6 5 4 3 2 1 0
// 0x3F (0x5F) I T H S V N Z C SREG
// Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
// Initial Value 0 0 0 0 0 0 0 0
#define K_THREAD_DEFAULT_SREG 0x00

// 32 registers (32) + SREG (1) + return address (2)
#define K_THREAD_STACK_VOID_SIZE 35u
#define K_THREAD_STACK_MIN_SIZE K_THREAD_STACK_VOID_SIZE

// some of following macros need to be differenciate for c or asm :
// - in c files the compiler needs to know the type of stack_start in order to do arithmetic operations on poointers
// - in asm files types are not understood by compiler
#define K_STACK_END(stack_start, size) (stack_start + size - 1)
#define _K_STACK_END_ASM(stack_start, size) K_STACK_END(stack_start, size)

#define _K_STACK_INIT_SP(stack_end) (stack_end - K_THREAD_STACK_VOID_SIZE)

// if not casting this symbol address, the stack pointer will not be correctly set
#define _K_THREAD_STACK_START(name) ((uint16_t) &_k_stack_buf_##name)

#define _K_THREAD_STACK_SIZE(name) (sizeof(_k_stack_buf_##name))

#define _K_STACK_INIT_SP_FROM_NAME(name, stack_size) _K_STACK_INIT_SP(K_STACK_END(_K_THREAD_STACK_START(name), stack_size))

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

#define _K_THREAD_INITIALIZER(name, stack_size, prio, local_storage_p)                                      \
    thread_t name = {                                                                                       \
        .sp = (void *)_K_STACK_INIT_SP_FROM_NAME(name, stack_size),                                         \
        .priority = prio,                                                                                   \
        .stack = {.end = (void *)K_STACK_END(_K_THREAD_STACK_START(name), stack_size), .size = stack_size}, \
        .local_storage = (void *)local_storage_p}

#define K_THREAD_DEFINE(name, entry, stack_size, prio, context_p, local_storage_p)         \
    __attribute__((used)) static _K_STACK_INITIALIZER(name, stack_size, entry, context_p); \
    __attribute__((used, section(".k_threads"))) static _K_THREAD_INITIALIZER(name, stack_size, prio, local_storage_p);

/*___________________________________________________________________________*/


#endif