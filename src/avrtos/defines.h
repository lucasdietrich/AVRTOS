#ifndef _AVRTOS_DEFINES_H
#define _AVRTOS_DEFINES_H

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

// This find is include from asm files, make it compatible with ASM or user "#if !__ASSEMBLER__" preprocessor
// put all c specific definition in the following file

#include "common.h"
#include "definesc.h"

/*___________________________________________________________________________*/

// MAX threads
#ifdef CONFIG_THREAD_MAX
#if CONFIG_THREAD_MAX > 1
#define K_THREAD_MAX_COUNT CONFIG_THREAD_MAX
#else
#error Cannot configure this library for less than 2 threads (CONFIG_THREAD_MAX < 2)
#endif
#else
#define K_THREAD_MAX_COUNT DEFAULT_THREAD_MAX
#endif

// main thread priority
#ifdef CONFIG_THREAD_MAIN_THREAD_PRIORITY
#define THREAD_MAIN_THREAD_PRIORITY CONFIG_THREAD_MAIN_THREAD_PRIORITY
#else
#define THREAD_MAIN_THREAD_PRIORITY DEFAULT_THREAD_MAIN_THREAD_PRIORITY
#endif

// explicit main stack
#ifdef CONFIG_THREAD_EXPLICIT_MAIN_STACK
#define THREAD_EXPLICIT_MAIN_STACK CONFIG_THREAD_EXPLICIT_MAIN_STACK
#else
#define THREAD_EXPLICIT_MAIN_STACK DEFAULT_THREAD_EXPLICIT_MAIN_STACK
#endif

// main stack size (if explicit)
#ifdef CONFIG_THREAD_MAIN_STACK_SIZE
#define THREAD_MAIN_STACK_SIZE CONFIG_THREAD_MAIN_STACK_SIZE
#else
#define THREAD_MAIN_STACK_SIZE DEFAULT_THREAD_MAIN_STACK_SIZE
#endif

// use asm thread stack init function
#ifdef CONFIG_THREAD_USE_INIT_STACK_ASM
#define THREAD_USE_INIT_STACK_ASM CONFIG_THREAD_USE_INIT_STACK_ASM
#else
#define THREAD_USE_INIT_STACK_ASM DEFAULT_THREAD_USE_INIT_STACK_ASM
#endif

// use u32 time objects or u16
#ifdef CONFIG_KERNEL_HIGH_RANGE_TIME_OBJECT_U32
#define KERNEL_HIGH_RANGE_TIME_OBJECT_U32 CONFIG_KERNEL_HIGH_RANGE_TIME_OBJECT_U32
#else
#define KERNEL_HIGH_RANGE_TIME_OBJECT_U32 DEFAULT_KERNEL_HIGH_RANGE_TIME_OBJECT_U32
#endif

// set default SREG (interrupt flag or not)
#ifdef CONFIG_THREAD_DEFAULT_SREG
#define THREAD_DEFAULT_SREG CONFIG_THREAD_DEFAULT_SREG
#else
#define THREAD_DEFAULT_SREG DEFAULT_THREAD_DEFAULT_SREG
#endif

// kernel debug mode
#ifdef CONFIG_KERNEL_DEBUG
#define KERNEL_DEBUG CONFIG_KERNEL_DEBUG
#else
#define KERNEL_DEBUG DEFAULT_KERNEL_DEBUG
#endif

// preemptive threads
#ifdef CONFIG_KERNEL_PREEMPTIVE_THREADS
#define KERNEL_PREEMPTIVE_THREADS CONFIG_KERNEL_PREEMPTIVE_THREADS
#else
#define KERNEL_PREEMPTIVE_THREADS DEFAULT_KERNEL_PREEMPTIVE_THREADS
#endif

//
// if preemptive threads
//
#if KERNEL_PREEMPTIVE_THREADS

// debug preempt via uart
#ifdef CONFIG_KERNEL_DEBUG_PREEMPT_UART
#define KERNEL_DEBUG_PREEMPT_UART CONFIG_KERNEL_DEBUG_PREEMPT_UART
#else
#define KERNEL_DEBUG_PREEMPT_UART DEFAULT_KERNEL_DEBUG_PREEMPT_UART
#endif

// kernel sysclock auto init
#if KERNEL_DEBUG_PREEMPT_UART

#define KERNEL_SYSCLOCK_AUTO_INIT   0

#else

#ifdef CONFIG_KERNEL_SYSCLOCK_AUTO_INIT
#define KERNEL_SYSCLOCK_AUTO_INIT CONFIG_KERNEL_SYSCLOCK_AUTO_INIT
#else
#define KERNEL_SYSCLOCK_AUTO_INIT DEFAULT_KERNEL_SYSCLOCK_AUTO_INIT
#endif

#endif

// preemptive threads time_slice
#ifdef CONFIG_KERNEL_TIME_SLICE
#define KERNEL_TIME_SLICE CONFIG_KERNEL_TIME_SLICE
#else
#define KERNEL_TIME_SLICE DEFAULT_KERNEL_TIME_SLICE
#endif

#if KERNEL_TIME_SLICE == 1
// 62.5
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 63 
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_256
#elif KERNEL_TIME_SLICE == 2
// 125.0
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 125
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_256
#elif KERNEL_TIME_SLICE == 3
// 187.5
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 187
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_256
#elif KERNEL_TIME_SLICE == 4
// 250.0
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 250
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_256
#elif KERNEL_TIME_SLICE == 5
// 78.125
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 78
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_1024
#elif KERNEL_TIME_SLICE == 8
// 125.0
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 125
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_1024
#elif KERNEL_TIME_SLICE == 10
// 156.25
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 156
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_1024
#elif KERNEL_TIME_SLICE == 15
// 234.375
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 234
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_1024
#elif KERNEL_TIME_SLICE == 16
// 250.0
#define KERNEL_SYSCLOCK_TIMER0_TCNT0 250
#define KERNEL_SYSCLOCK_TIMER0_PRESCALER SYSCLOCK_PRESCALER_1024
#else
#error KERNEL_TIME_SLICE must be among predefined time slices (1, 2, 3, 4, 5, 10, 15, 16)
#endif

#else

#define KERNEL_SYSCLOCK_AUTO_INIT           0
#define CONFIG_KERNEL_DEBUG_PREEMPT_UART    0
#define KERNEL_TIME_SLICE                   0
#define KERNEL_SYSCLOCK_TIMER0_TCNT0               0

#endif

/*___________________________________________________________________________*/

// 32 registers (32) + SREG (1) + return address (2)
#define K_THREAD_STACK_VOID_SIZE 35
#define K_THREAD_STACK_MIN_SIZE K_THREAD_STACK_VOID_SIZE

// some of following macros need to be differenciate for c or asm :
// - in c files the compiler needs to know the type of stack_start in order to do arithmetic operations on poointers
// - in asm files types are not understood by compiler
#define K_STACK_END(stack_start, size) (stack_start + size - 1)
#define _K_STACK_END_ASM(stack_start, size) K_STACK_END(stack_start, size)

#define _K_STACK_INIT_SP(stack_end) (stack_end - K_THREAD_STACK_VOID_SIZE)

// if not casting this symbol address, the stack pointer will not be correctly set
#define _K_THREAD_STACK_START(name) ((uint16_t)&_k_stack_buf_##name)

#define _K_THREAD_STACK_SIZE(name) (sizeof(_k_stack_buf_##name))

#define _K_STACK_INIT_SP_FROM_NAME(name, stack_size) _K_STACK_INIT_SP(K_STACK_END(_K_THREAD_STACK_START(name), stack_size))

#define _K_STACK_INITIALIZER(name, stack_size, entry, context_p) \
    struct                                                       \
    {                                                            \
        uint8_t empty[stack_size - K_THREAD_STACK_VOID_SIZE];    \
        struct                                                   \
        {                                                        \
            uint8_t sreg;                                        \
            uint8_t r25_r27r30r31r1_r16r28r29[22u];              \
            void *context;                                       \
            uint8_t r17r0r18_r23[8u];                            \
            void *ret_addr;                                      \
        } base;                                                  \
    } _k_stack_buf_##name = {                                    \
        {0x00},                                                  \
        {THREAD_DEFAULT_SREG,                                    \
         {0x00},                                                 \
         (void *)K_SWAP_ENDIANNESS(context_p),                   \
         {0x00},                                                 \
         (void *)K_SWAP_ENDIANNESS((uint16_t)entry)}}

#define _K_STACK_MIN_INITIALIZER(name, entry, context_p) \
    struct                                               \
    {                                                    \
        uint8_t sreg;                                    \
        uint8_t r25_r27r30r31r1_r16r28r29[22u];          \
        void *context;                                   \
        uint8_t r17r0r18_r23[8u];                        \
        void *ret_addr;                                  \
    } _k_stack_buf_##name = {                            \
        THREAD_DEFAULT_SREG,                             \
        {0x00},                                          \
        (void *)K_SWAP_ENDIANNESS(context_p),            \
        {0x00},                                          \
        (void *)K_SWAP_ENDIANNESS((uint16_t)entry)}

#define _K_THREAD_INITIALIZER(name, stack_size, prio_flags, local_storage_p, sym)                           \
    thread_t name = {                                                                                       \
        .sp = (void *)_K_STACK_INIT_SP_FROM_NAME(name, stack_size),                                         \
        .tie = {.runqueue = {.prev = NULL, .next = NULL}},                                                  \
        {                                                                                                   \
            .flags = K_FLAG_READY | prio_flags,                                                                            \
        },                                                                                                  \
        .stack = {.end = (void *)K_STACK_END(_K_THREAD_STACK_START(name), stack_size), .size = stack_size}, \
        .local_storage = (void *)local_storage_p,                                                           \
        .symbol = sym}

#define K_THREAD_DEFINE(name, entry, stack_size, prio_flags, context_p, local_storage_p, symbol)         \
    __attribute__((used)) static _K_STACK_INITIALIZER(name, stack_size, entry, context_p); \
    __attribute__((used, section(".k_threads"))) static _K_THREAD_INITIALIZER(name, stack_size, prio_flags, local_storage_p, symbol);

#define K_THREAD_MINSTACK_DEFINE(name, entry, prio_flags, context_p, local_storage_p, symbol)    \
    __attribute__((used)) static _K_STACK_MIN_INITIALIZER(name, entry, context_p); \
    __attribute__((used, section(".k_threads"))) static _K_THREAD_INITIALIZER(name, K_THREAD_STACK_VOID_SIZE, prio_flags, local_storage_p, symbol);

/*___________________________________________________________________________*/

#endif