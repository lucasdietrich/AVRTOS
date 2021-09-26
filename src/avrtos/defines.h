#ifndef _AVRTOS_DEFINES_H
#define _AVRTOS_DEFINES_H

/*___________________________________________________________________________*/

#include "default_config.h"

/*___________________________________________________________________________*/

// This find is include from asm files, make it compatible with ASM or user "#if !__ASSEMBLER__" preprocessor

#include "common.h"

/*___________________________________________________________________________*/

/* non standart */
#define ENOERROR        0
#define EERROR          1
#define ETIMEOUT        2

/* standart */
#define EINTR 4
#define EIO 5
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 16
#define EINVAL 22

#define EWOULDBLOCK EAGAIN


/*___________________________________________________________________________*/

// arch specific fixups

// compiler constants :
// https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html#AVR-Built-in-Macros

#if defined(__AVR_3_BYTE_PC__)
#   define _K_ARCH_STACK_SIZE_FIXUP         1
#   define _K_ARCH_PC_SIZE                  3
#else
#   if defined(__AVR_2_BYTE_PC__) 
#       define _K_ARCH_STACK_SIZE_FIXUP     0
#       define _K_ARCH_PC_SIZE              2
#   else
#       error unsupported PC size
#       define _K_ARCH_STACK_SIZE_FIXUP     0
#       define _K_ARCH_PC_SIZE              0
# endif
#endif

#define NOINLINE            __attribute__((noinline))
#define NORETURN            __attribute__((__noreturn__))
#define CODE_UNREACHABLE    __builtin_unreachable();

/*___________________________________________________________________________*/

// MAX threads
#ifdef CONFIG_THREAD_MAX
#   if CONFIG_THREAD_MAX > 1
#       define THREAD_MAX CONFIG_THREAD_MAX
#   else
#       error Cannot configure this library for less than 2 threads (CONFIG_THREAD_MAX < 2)
#   endif
#else
#   define THREAD_MAX DEFAULT_THREAD_MAX
#endif


// main thread priority
#ifdef CONFIG_THREAD_MAIN_THREAD_PRIORITY
#   define THREAD_MAIN_THREAD_PRIORITY CONFIG_THREAD_MAIN_THREAD_PRIORITY
#else
#   define THREAD_MAIN_THREAD_PRIORITY DEFAULT_THREAD_MAIN_THREAD_PRIORITY
#endif

// explicit main stack
#ifdef CONFIG_THREAD_EXPLICIT_MAIN_STACK
#   define THREAD_EXPLICIT_MAIN_STACK CONFIG_THREAD_EXPLICIT_MAIN_STACK
#else
#   define THREAD_EXPLICIT_MAIN_STACK DEFAULT_THREAD_EXPLICIT_MAIN_STACK
#endif

// main stack size (if explicit)
#ifdef CONFIG_THREAD_MAIN_STACK_SIZE
#   define THREAD_MAIN_STACK_SIZE CONFIG_THREAD_MAIN_STACK_SIZE
#else
#   define THREAD_MAIN_STACK_SIZE DEFAULT_THREAD_MAIN_STACK_SIZE
#endif

// use asm thread stack init function
#ifdef CONFIG_THREAD_USE_INIT_STACK_ASM
#   define THREAD_USE_INIT_STACK_ASM CONFIG_THREAD_USE_INIT_STACK_ASM
#else
#   define THREAD_USE_INIT_STACK_ASM DEFAULT_THREAD_USE_INIT_STACK_ASM
#endif

// use u32 time objects or u16
#ifdef CONFIG_KERNEL_HIGH_RANGE_TIME_OBJECT_U32
#   define KERNEL_HIGH_RANGE_TIME_OBJECT_U32 CONFIG_KERNEL_HIGH_RANGE_TIME_OBJECT_U32
#else
#   define KERNEL_HIGH_RANGE_TIME_OBJECT_U32 DEFAULT_KERNEL_HIGH_RANGE_TIME_OBJECT_U32
#endif

// set default SREG (interrupt flag or not)
#ifdef CONFIG_THREAD_DEFAULT_SREG
#   define THREAD_DEFAULT_SREG CONFIG_THREAD_DEFAULT_SREG
#else
#   define THREAD_DEFAULT_SREG DEFAULT_THREAD_DEFAULT_SREG
#endif

// kernel debug mode
#ifdef CONFIG_KERNEL_DEBUG
#   define KERNEL_DEBUG CONFIG_KERNEL_DEBUG
#else
#   define KERNEL_DEBUG DEFAULT_KERNEL_DEBUG
#endif

// kernel debug function mode
#ifdef CONFIG_KERNEL_API_NOINLINE
#   define KERNEL_API_NOINLINE CONFIG_KERNEL_API_NOINLINE
#else
#   define KERNEL_API_NOINLINE DEFAULT_KERNEL_API_NOINLINE
#endif

#if KERNEL_API_NOINLINE
#   define K_NOINLINE NOINLINE
#else
#   define K_NOINLINE 
#endif

// kernel scheduler debug mode
#ifdef CONFIG_KERNEL_SCHEDULER_DEBUG
#   define KERNEL_SCHEDULER_DEBUG CONFIG_KERNEL_SCHEDULER_DEBUG
#else
#   define KERNEL_SCHEDULER_DEBUG DEFAULT_KERNEL_SCHEDULER_DEBUG
#endif

// preemptive threads
#ifdef CONFIG_KERNEL_PREEMPTIVE_THREADS
#   define KERNEL_PREEMPTIVE_THREADS CONFIG_KERNEL_PREEMPTIVE_THREADS
#else
#   define KERNEL_PREEMPTIVE_THREADS DEFAULT_KERNEL_PREEMPTIVE_THREADS
#endif

//
// if preemptive threads
//
#if KERNEL_PREEMPTIVE_THREADS

// debug preempt via uart
#   ifdef CONFIG_KERNEL_DEBUG_PREEMPT_UART
#       define KERNEL_DEBUG_PREEMPT_UART CONFIG_KERNEL_DEBUG_PREEMPT_UART
#   else
#       define KERNEL_DEBUG_PREEMPT_UART DEFAULT_KERNEL_DEBUG_PREEMPT_UART
#   endif

#else 

#   define KERNEL_DEBUG_PREEMPT_UART 0

#endif

// kernel sysclock auto init
#if KERNEL_DEBUG_PREEMPT_UART

#   define KERNEL_SYSCLOCK_AUTO_INIT   0

// refactor this
#   if defined(__AVR_ATmega328P__)
#       define _K_USART_RX_vect  USART_RX_vect
#   elif defined(__AVR_ATmega2560__)
#       define _K_USART_RX_vect  USART0_RX_vect
#   else
#       warning   KERNEL_DEBUG_PREEMPT_UART enaabled, USART RX vector no configured, default = "USART_RX_vect"
#       define _K_USART_RX_vect  USART_RX_vect
#   endif

#else

#   ifdef CONFIG_KERNEL_SYSCLOCK_AUTO_INIT
#       define KERNEL_SYSCLOCK_AUTO_INIT CONFIG_KERNEL_SYSCLOCK_AUTO_INIT
#   else
#       define KERNEL_SYSCLOCK_AUTO_INIT DEFAULT_KERNEL_SYSCLOCK_AUTO_INIT
#   endif

#endif

// select hardware timer
#ifdef CONFIG_KERNEL_SYSLOCK_HW_TIMER
#   define KERNEL_SYSLOCK_HW_TIMER CONFIG_KERNEL_SYSLOCK_HW_TIMER
#else
#   define KERNEL_SYSLOCK_HW_TIMER DEFAULT_KERNEL_SYSLOCK_HW_TIMER
#endif

// preemptive threads time_slice in ms
#ifdef CONFIG_KERNEL_TIME_SLICE
#   define KERNEL_TIME_SLICE CONFIG_KERNEL_TIME_SLICE
#else
#   define KERNEL_TIME_SLICE DEFAULT_KERNEL_TIME_SLICE
#endif

// idle thread, only in preemptive mode
#if KERNEL_PREEMPTIVE_THREADS
#   ifdef CONFIG_KERNEL_THREAD_IDLE
#       define KERNEL_THREAD_IDLE CONFIG_KERNEL_THREAD_IDLE
#   else
#       define KERNEL_THREAD_IDLE DEFAULT_KERNEL_THREAD_IDLE
#   endif
#else
#   define KERNEL_THREAD_IDLE 0
#endif

#ifdef CONFIG_KERNEL_THREAD_IDLE_ADD_STACK
#   define KERNEL_THREAD_IDLE_ADD_STACK CONFIG_KERNEL_THREAD_IDLE_ADD_STACK
#else
#   define KERNEL_THREAD_IDLE_ADD_STACK DEFAULT_KERNEL_THREAD_IDLE_ADD_STACK
#endif

#ifdef CONFIG_KERNEL_ALLOW_INTERRUPT_YIELD
#   define KERNEL_ALLOW_INTERRUPT_YIELD CONFIG_KERNEL_ALLOW_INTERRUPT_YIELD
#else
#   define KERNEL_ALLOW_INTERRUPT_YIELD DEFAULT_KERNEL_ALLOW_INTERRUPT_YIELD
#endif

#ifdef CONFIG_THREAD_CANARIES
#   define THREAD_CANARIES CONFIG_THREAD_CANARIES
#else
#   define THREAD_CANARIES DEFAULT_THREAD_CANARIES
#endif

#if THREAD_CANARIES
#   ifdef CONFIG_THREAD_CANARIES_SYMBOL
#       define THREAD_CANARIES_SYMBOL CONFIG_THREAD_CANARIES_SYMBOL
#   else
#       define THREAD_CANARIES_SYMBOL DEFAULT_THREAD_CANARIES_SYMBOL
#   endif
#else
#   define THREAD_CANARIES_SYMBOL  0x00
#endif

#if THREAD_EXPLICIT_MAIN_STACK == 0 && THREAD_CANARIES == 1
#   warning unable to monitor main thread stack canaries as the main stack is defined as explicit (see THREAD_EXPLICIT_MAIN_STACK)
#endif

#ifdef CONFIG_SYSTEM_WORKQUEUE_ENABLE
#   define SYSTEM_WORKQUEUE_ENABLE CONFIG_SYSTEM_WORKQUEUE_ENABLE
#else
#   define SYSTEM_WORKQUEUE_ENABLE DEFAULT_SYSTEM_WORKQUEUE_ENABLE
#endif

#if SYSTEM_WORKQUEUE_ENABLE
#   ifdef CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE
#       define SYSTEM_WORKQUEUE_STACK_SIZE CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE
#   else
#       define SYSTEM_WORKQUEUE_STACK_SIZE DEFAULT_SYSTEM_WORKQUEUE_STACK_SIZE
#   endif

#   ifdef CONFIG_SYSTEM_WORKQUEUE_PRIORITY
#       define SYSTEM_WORKQUEUE_PRIORITY CONFIG_SYSTEM_WORKQUEUE_PRIORITY
#   else
#       define SYSTEM_WORKQUEUE_PRIORITY DEFAULT_SYSTEM_WORKQUEUE_PRIORITY
#   endif
#endif

#ifdef CONFIG_KERNEL_ASSERT
#   define KERNEL_ASSERT CONFIG_KERNEL_ASSERT
#else
#   define KERNEL_ASSERT DEFAULT_KERNEL_ASSERT
#endif

#ifdef CONFIG_KERNEL_YIELD_ON_UNPEND
#   define KERNEL_YIELD_ON_UNPEND CONFIG_KERNEL_YIELD_ON_UNPEND
#else
#   define KERNEL_YIELD_ON_UNPEND DEFAULT_KERNEL_YIELD_ON_UNPEND
#endif

#ifdef CONFIG_THREAD_ALLOW_RETURN
#   define THREAD_ALLOW_RETURN CONFIG_THREAD_ALLOW_RETURN
#else
#   define THREAD_ALLOW_RETURN DEFAULT_THREAD_ALLOW_RETURN
#endif





/*___________________________________________________________________________*/

// put all c specific definition  here

#if !__ASSEMBLER__

#include <stdint.h>
#include <stddef.h>

#if KERNEL_HIGH_RANGE_TIME_OBJECT_U32 == 1
    typedef uint32_t k_delta_ms_t;
#else
    typedef uint16_t k_delta_ms_t;
#endif

typedef struct
{
    k_delta_ms_t value;
} k_timeout_t;

#define K_TIMEOUT_EQ(t1, t2)    (t1.value == t2.value)

#define K_SECONDS(delay_s)      ((k_timeout_t){(k_delta_ms_t) (delay_s * 1000)})
#define K_MSEC(delay_ms)        ((k_timeout_t){(k_delta_ms_t) (delay_ms)})
#define K_NO_WAIT               ((k_timeout_t){(k_delta_ms_t) (0)})
#define K_FOREVER               ((k_timeout_t){(k_delta_ms_t) (-1)})
#define K_UNTIL_WAKEUP          K_FOREVER

#define HTONL(n) ((((((uint32_t)(n) & 0xFF)) << 24) |      \
               ((((uint32_t)(n) & 0xFF00)) << 8) |         \
               ((((uint32_t)(n) & 0xFF0000)) >> 8) |       \
               ((((uint32_t)(n) & 0xFF000000)) >> 24)))

#define HTONS(n) (((((uint16_t)(n) & 0xFF)) << 8) |       \
               ((((uint16_t)(n) & 0xFF00)) >> 8))

#define K_SWAP_ENDIANNESS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#define SET_BIT(x, b)  ((x) |= b)
#define CLR_BIT(x, b)  ((x) &= (~(b)))
#define TEST_BIT(x, b) ((bool) ((x) & b))

#define ARG_UNUSED(arg) ((void) arg)


#endif

/*___________________________________________________________________________*/

// 32 registers (32) + SREG (1) + return address (2)
#define K_THREAD_STACK_VOID_SIZE (35 + _K_ARCH_STACK_SIZE_FIXUP)
#define K_THREAD_STACK_MIN_SIZE K_THREAD_STACK_VOID_SIZE

// some of following macros need to be differenciate for c or asm :
// - in c files the compiler needs to know the type of stack_start in order to do arithmetic operations on poointers
// - in asm files types are not understood by compiler
#define K_STACK_END(stack_start, size) (void*) ((uint8_t*) stack_start + size - 1)
#define K_STACK_START(stack_end, size) (void*) ((uint8_t*) stack_end - size + 1)
#define K_THREAD_STACK_START(th) K_STACK_START(th->stack.end, th->stack.size)
#define K_THREAD_STACK_END(th) (th->stack.end)

#define _K_STACK_END(stack_start, size) (stack_start + size - 1)

#define _K_STACK_END_ASM(stack_start, size) _K_STACK_END(stack_start, size)

#define _K_STACK_INIT_SP(stack_end) (stack_end - K_THREAD_STACK_VOID_SIZE)

// if not casting this symbol address, the stack pointer will not be correctly set
#define _K_THREAD_STACK_START(name) ((uint8_t*)(&_k_stack_buf_##name))

#define _K_THREAD_STACK_SIZE(name) (sizeof(_k_stack_buf_##name))

#define _K_STACK_INIT_SP_FROM_NAME(name, stack_size) _K_STACK_INIT_SP(_K_STACK_END(_K_THREAD_STACK_START(name), stack_size))

#define K_THREAD        __attribute__((used, section(".k_threads")))

#define THREAD_FROM_EVENTQUEUE(item) CONTAINER_OF(item, struct k_thread, tie.runqueue)
#define THREAD_FROM_WAITQUEUE(item) CONTAINER_OF(item, struct k_thread, wany)
#define THREAD_OF_TITEM(item) CONTAINER_OF(item, struct k_thread, tie.event)

#if THREAD_ALLOW_RETURN == 1

#define _K_CORE_CONTEXT_STRUCT() struct                                    \
        {                                                                  \
            uint8_t sreg;                                                  \
            uint8_t r26_r27r30r31r28r29r1_r16[22u];                        \
            void *context;                                                 \
            thread_entry_t *entry;                                         \
            uint8_t r17r0r18_r21_3Baddrmsb[6u + _K_ARCH_STACK_SIZE_FIXUP]; \
            void *k_thread_entry;                                          \
        }

#define _K_CORE_CONTEXT(entry, context_p)                                  \
        {THREAD_DEFAULT_SREG,                                              \
         {0x00},                                                           \
         (void*) context_p,                                                \
         (thread_entry_t*) entry,                                          \
         {0x00},                                                           \
         (void*) _k_thread_entry}

#else

#define _K_CORE_CONTEXT_STRUCT() struct                                    \
        {                                                                  \
            uint8_t sreg;                                                  \
            uint8_t r26_r27r30r31r28r29r1_r16[22u];                        \
            void *context;                                                 \
            uint8_t r17r0r18_r21_3Baddrmsb[8u + _K_ARCH_STACK_SIZE_FIXUP]; \
            thread_entry_t *entry;                                         \
        }

#define _K_CORE_CONTEXT(entry, context_p)                                  \
        {THREAD_DEFAULT_SREG,                                              \
         {0x00},                                                           \
         (void*) context_p,                                                \
         {0x00},                                                           \
         (thread_entry_t*) entry}

#endif

#define _K_STACK_INITIALIZER(name, stack_size, entry, context_p)           \
    struct                                                                 \
    {                                                                      \
        uint8_t empty[stack_size - K_THREAD_STACK_VOID_SIZE];              \
        _K_CORE_CONTEXT_STRUCT() base;                                     \
    } _k_stack_buf_##name = {                                              \
        {0x00},                                                            \
       _K_CORE_CONTEXT(entry, context_p)}

#define _K_STACK_MIN_INITIALIZER(name, entry, context_p)     \
    _K_CORE_CONTEXT_STRUCT() _k_stack_buf_##name =           \
    _K_CORE_CONTEXT(entry, context_p)

#define _K_THREAD_INITIALIZER(name, stack_size, prio_flags, sym)                                             \
    struct k_thread name = {                                                                                 \
        .sp = (void *)_K_STACK_INIT_SP_FROM_NAME(name, stack_size),                                          \
        {                                                                                                    \
            .flags = K_FLAG_READY | prio_flags,                                                              \
        },                                                                                                   \
        .tie = {.runqueue = DITEM_INIT(NULL)},                                                               \
        {.wmutex = DITEM_INIT(NULL)},                                                                        \
        .swap_data = NULL,                                                                                   \
        .stack = {.end = (void *)_K_STACK_END(_K_THREAD_STACK_START(name), stack_size), .size = stack_size}, \
        .symbol = sym}

#define K_THREAD_DEFINE(name, entry, stack_size, prio_flags, context_p, symbol)                  \
    __attribute__((used)) _K_STACK_INITIALIZER(name, stack_size, entry, context_p);              \
    __attribute__((used, section(".k_threads"))) _K_THREAD_INITIALIZER(name, stack_size, prio_flags, symbol);

#define K_THREAD_MINSTACK_DEFINE(name, entry, prio_flags, context_p, symbol)                  \
    __attribute__((used)) _K_STACK_MIN_INITIALIZER(name, entry, context_p);                   \
    __attribute__((used, section(".k_threads"))) _K_THREAD_INITIALIZER(name, K_THREAD_STACK_VOID_SIZE, prio_flags, symbol);

/*___________________________________________________________________________*/

#endif