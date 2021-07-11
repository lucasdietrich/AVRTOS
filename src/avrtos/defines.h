#ifndef _AVRTOS_DEFINES_H
#define _AVRTOS_DEFINES_H

/*___________________________________________________________________________*/

// This find is include from asm files, make it compatible with ASM or user "#if !__ASSEMBLER__" preprocessor

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
#define K_PRIO_DEFAULT                          K_PRIO_PREEMPT(8)
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

#ifdef CONFIG_HIGH_RANGE_TIME_OBJECT_U32
#define HIGH_RANGE_TIME_OBJECT_U32  CONFIG_HIGH_RANGE_TIME_OBJECT_U32
#else
#define HIGH_RANGE_TIME_OBJECT_U32  DEFAULT_HIGH_RANGE_TIME_OBJECT_U32
#endif

#ifdef CONFIG_THREAD_DEFAULT_SREG
#define THREAD_DEFAULT_SREG  CONFIG_THREAD_DEFAULT_SREG
#else
#define THREAD_DEFAULT_SREG  DEFAULT_THREAD_DEFAULT_SREG
#endif

#ifdef CONFIG_KERNEL_DEBUG
#define KERNEL_DEBUG  CONFIG_KERNEL_DEBUG
#else
#define KERNEL_DEBUG  DEFAULT_KERNEL_DEBUG
#endif

#ifdef CONFIG_KERNEL_PREEMPTIVE_THREADS
#define KERNEL_PREEMPTIVE_THREADS   CONFIG_KERNEL_PREEMPTIVE_THREADS
#else
#define KERNEL_PREEMPTIVE_THREADS   DEFAULT_KERNEL_PREEMPTIVE_THREADS
#endif

#if KERNEL_PREEMPTIVE_THREADS
#define KERNEL_TIME_SLICE_HZ            1000 / DEFAULT_KERNEL_TIME_SLICE
#endif
/*___________________________________________________________________________*/

#define K_SWAP_ENDIANNESS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

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
        {THREAD_DEFAULT_SREG,                                  \
         {0x00},                                                 \
         (void *)K_SWAP_ENDIANNESS(context_p),                                      \
         {0x00},                                                 \
         (void *)K_SWAP_ENDIANNESS((uint16_t)entry)}}

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

#if !__ASSEMBLER__

#if HIGH_RANGE_TIME_OBJECT_U32 == 1
    typedef uint32_t k_delta_ms_t;
#else
    typedef uint16_t k_delta_ms_t;
#endif

typedef struct
{
    k_delta_ms_t delay;
} k_timeout_t;

#define K_MSEC(delay) ((k_timeout_t){delay})
#define K_NO_WAIT(delay) ((k_timeout_t){0})
#define K_FOREVER(delay) ((k_timeout_t){-1})

#endif

/*___________________________________________________________________________*/

#endif