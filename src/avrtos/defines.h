#ifndef _AVRTOS_DEFINES_H
#define _AVRTOS_DEFINES_H

/*___________________________________________________________________________*/

#include "default_config.h"

/*___________________________________________________________________________*/

// This find is include from asm files, make it compatible with ASM or user "#if !__ASSEMBLER__" preprocessor

#include "common.h"

/*___________________________________________________________________________*/

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

// kernel auto init
#ifdef CONFIG_KERNEL_AUTO_INIT
#   define KERNEL_AUTO_INIT CONFIG_KERNEL_AUTO_INIT
#else
#   define KERNEL_AUTO_INIT DEFAULT_KERNEL_AUTO_INIT
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

// Sentinel
#ifdef CONFIG_THREAD_STACK_SENTINEL
#   define THREAD_STACK_SENTINEL CONFIG_THREAD_STACK_SENTINEL
#else
#   define THREAD_STACK_SENTINEL DEFAULT_THREAD_STACK_SENTINEL
#endif

#if THREAD_STACK_SENTINEL
#ifdef CONFIG_THREAD_STACK_SENTINEL_SIZE
#	define THREAD_STACK_SENTINEL_SIZE CONFIG_THREAD_STACK_SENTINEL_SIZE
#else
#	define THREAD_STACK_SENTINEL_SIZE DEFAULT_THREAD_STACK_SENTINEL_SIZE
#endif
#else
#	define THREAD_STACK_SENTINEL_SIZE	0
#endif /* THREAD_STACK_SENTINEL */

#ifdef CONFIG_THREAD_STACK_SENTINEL_SYMBOL
#   define THREAD_STACK_SENTINEL_SYMBOL CONFIG_THREAD_STACK_SENTINEL_SYMBOL
#else
#   define THREAD_STACK_SENTINEL_SYMBOL DEFAULT_THREAD_STACK_SENTINEL_SYMBOL
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

#ifdef CONFIG_THREAD_ALLOW_RETURN
#   define THREAD_ALLOW_RETURN CONFIG_THREAD_ALLOW_RETURN
#else
#   define THREAD_ALLOW_RETURN DEFAULT_THREAD_ALLOW_RETURN
#endif

#ifdef CONFIG_KERNEL_TIMERS
#   define KERNEL_TIMERS CONFIG_KERNEL_TIMERS
#else
#   define KERNEL_TIMERS DEFAULT_KERNEL_TIMERS
#endif

#ifdef CONFIG_KERNEL_EVENTS
#   define KERNEL_EVENTS CONFIG_KERNEL_EVENTS
#else
#   define KERNEL_EVENTS DEFAULT_KERNEL_EVENTS
#endif

#ifdef CONFIG_THREAD_ERRNO
#   define THREAD_ERRNO CONFIG_THREAD_ERRNO
#else
#   define THREAD_ERRNO DEFAULT_THREAD_ERRNO
#endif

#ifdef CONFIG_KERNEL_SCHED_LOCK_COUNTER
#   define KERNEL_SCHED_LOCK_COUNTER CONFIG_KERNEL_SCHED_LOCK_COUNTER
#else
#   define KERNEL_SCHED_LOCK_COUNTER DEFAULT_KERNEL_SCHED_LOCK_COUNTER
#endif

#ifdef CONFIG_KERNEL_IRQ_LOCK_COUNTER
#   define KERNEL_IRQ_LOCK_COUNTER CONFIG_KERNEL_IRQ_LOCK_COUNTER
#else
#   define KERNEL_IRQ_LOCK_COUNTER DEFAULT_KERNEL_IRQ_LOCK_COUNTER
#endif

#ifdef CONFIG_STDIO_PRINTF_TO_USART
#   define STDIO_PRINTF_TO_USART CONFIG_STDIO_PRINTF_TO_USART
#else
#   define STDIO_PRINTF_TO_USART DEFAULT_STDIO_PRINTF_TO_USART
#endif

#ifdef CONFIG_KERNEL_UPTIME
#   define KERNEL_UPTIME CONFIG_KERNEL_UPTIME
#else
#   define KERNEL_UPTIME DEFAULT_KERNEL_UPTIME
#endif

#if KERNEL_UPTIME
#    ifdef CONFIG_KERNEL_UPTIME_40BITS
#   	define KERNEL_UPTIME_40BITS CONFIG_KERNEL_UPTIME_40BITS
#    else
#   	define KERNEL_UPTIME_40BITS DEFAULT_KERNEL_UPTIME_40BITS
#   endif
#else
#   define KERNEL_UPTIME_40BITS 0
#endif

#ifdef CONFIG_KERNEL_MAX_SYSCLOCK_PERIOD_MS
#   define KERNEL_MAX_SYSCLOCK_PERIOD_MS CONFIG_KERNEL_MAX_SYSCLOCK_PERIOD_MS
#else
#   define KERNEL_MAX_SYSCLOCK_PERIOD_MS DEFAULT_KERNEL_MAX_SYSCLOCK_PERIOD_MS
#endif

#ifdef CONFIG_KERNEL_ATOMIC_API
#   define KERNEL_ATOMIC_API CONFIG_KERNEL_ATOMIC_API
#else
#   define KERNEL_ATOMIC_API DEFAULT_KERNEL_ATOMIC_API
#endif

/*___________________________________________________________________________*/

/**
 * @brief Sysclock period is :
 *  - KERNEL_TIME_SLICE if KERNEL_MAX_SYSCLOCK_PERIOD_MS is off
 *  - MIN(KERNEL_MAX_SYSCLOCK_PERIOD_MS, KERNEL_TIME_SLICE) if KERNEL_MAX_SYSCLOCK_PERIOD_MS is on
 */

#if KERNEL_UPTIME == 0 || KERNEL_MAX_SYSCLOCK_PERIOD_MS == 0
#define SYSCLOCK_PERIOD_MS  KERNEL_TIME_SLICE
#else
#if KERNEL_MAX_SYSCLOCK_PERIOD_MS >= 64
# 	error KERNEL_MAX_SYSCLOCK_PERIOD_MS must be less than 64 !
	/* because of ADIW instruction 0 <= K < 64 */
#endif
#if KERNEL_TIME_SLICE % KERNEL_MAX_SYSCLOCK_PERIOD_MS != 0
#	error KERNEL_TIME_SLICE must be multiple of KERNEL_MAX_SYSCLOCK_PERIOD_MS !
	/* because of precision */
#endif

#if KERNEL_TIME_SLICE / KERNEL_MAX_SYSCLOCK_PERIOD_MS > 255
#	error "KERNEL_TIME_SLICE / KERNEL_MAX_SYSCLOCK_PERIOD_MS must be less than 255 !"
	/* because of remaining_sysclock_ints size (1B) in arch.S */
#endif

#if KERNEL_MAX_SYSCLOCK_PERIOD_MS < KERNEL_TIME_SLICE
#define SYSCLOCK_PERIOD_MS  KERNEL_MAX_SYSCLOCK_PERIOD_MS
#else 
#define SYSCLOCK_PERIOD_MS  KERNEL_TIME_SLICE
#endif /* KERNEL_MAX_SYSCLOCK_PERIOD_MS < KERNEL_TIME_SLICE */
#endif /* KERNEL_UPTIME == 0 || KERNEL_MAX_SYSCLOCK_PERIOD_MS == 0 */

#if KERNEL_UPTIME == 1 && SYSCLOCK_PERIOD_MS > 63
/* see arch.S line #111 */
#error Because of ADIW instruction 0 <= K < 64, SYSCLOCK_PERIOD_MS must be less than 64 when KERNEL_UPTIME is enabled !
#endif /* KERNEL_UPTIME == 1 && SYSCLOCK_PERIOD_MS > 63 */

#define K_EVENTS_PERIOD_MS	KERNEL_TIME_SLICE
#define K_TIMERS_PERIOD_MS	KERNEL_TIME_SLICE

/*___________________________________________________________________________*/

// put all c specific definitions  here

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

#endif

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

/*___________________________________________________________________________*/

// 31 registers (31) + SREG (1) + return address (2 or 3)
#define K_THREAD_STACK_VOID_SIZE (35 + _K_ARCH_STACK_SIZE_FIXUP)
#define K_THREAD_STACK_MIN_SIZE (K_THREAD_STACK_VOID_SIZE + THREAD_STACK_SENTINEL_SIZE)

// some of following macros need to be differenciate for c or asm :
// - in c files the compiler needs to know the type of stack_start in order to do arithmetic operations on poointers
// - in asm files types are not understood by compiler
#define K_STACK_END(stack_start, size) (void*) ((uint8_t*) stack_start + size - 1)
#define K_STACK_START(stack_end, size) (void*) ((uint8_t*) stack_end - size + 1)
#define K_THREAD_STACK_START(th) K_STACK_START(th->stack.end, th->stack.size)
#define K_THREAD_STACK_END(th) (th->stack.end)

/* real stack start and size without counting sentinel */
#define K_STACK_START_USABLE(stack_end, size) (K_STACK_START(stack_end, size) + THREAD_STACK_SENTINEL_SIZE)
#define K_STACK_SIZE_USABLE(size) (size - THREAD_STACK_SENTINEL_SIZE)
#define K_THREAD_STACK_START_USABLE(th) K_STACK_START_USABLE(th->stack.end, th->stack.size)

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
            uint8_t r26_r27r30r31r28r29_r16[21u];                        \
            void *context;                                                 \
            thread_entry_t *entry;                                         \
            uint8_t r17r1r0r18_r21_3Baddrmsb[7u + _K_ARCH_STACK_SIZE_FIXUP]; \
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
            uint8_t r26_r27r30r31r28r29_r16[21u];                        \
            void *context;                                                 \
            uint8_t r17r1r0r18_r21_3Baddrmsb[9u + _K_ARCH_STACK_SIZE_FIXUP]; \
            thread_entry_t *entry;                                         \
        }

#define _K_CORE_CONTEXT(entry, context_p)                                  \
        {THREAD_DEFAULT_SREG,                                              \
         {0x00},                                                           \
         (void*) context_p,                                                \
         {0x00},                                                           \
         (thread_entry_t*) entry}

#endif



#if THREAD_STACK_SENTINEL
#define _K_STACK_INITIALIZER(name, stack_size, entry, context_p)           \
    struct                                                                 \
    {                                                                      \
        uint8_t stack_sentinel[THREAD_STACK_SENTINEL_SIZE];                \
        uint8_t empty[stack_size - K_THREAD_STACK_VOID_SIZE - THREAD_STACK_SENTINEL_SIZE]; \
        _K_CORE_CONTEXT_STRUCT() base;                                     \
    } _k_stack_buf_##name = {                                              \
	{THREAD_STACK_SENTINEL_SYMBOL},                                     \
        {0x00},                                                            \
       _K_CORE_CONTEXT(entry, context_p)}
#else
#define _K_STACK_INITIALIZER(name, stack_size, entry, context_p)           \
    struct                                                                 \
    {                                                                      \
        uint8_t empty[stack_size - K_THREAD_STACK_VOID_SIZE];              \
        _K_CORE_CONTEXT_STRUCT() base;                                     \
    } _k_stack_buf_##name = {                                              \
        {0x00},                                                            \
       _K_CORE_CONTEXT(entry, context_p)}
#endif /* THREAD_STACK_SENTINEL */

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
    __attribute__((used)) _K_STACK_INITIALIZER(name, stack_size, entry, context_p); \
    __attribute__((used, section(".k_threads"))) _K_THREAD_INITIALIZER(name, stack_size, prio_flags, symbol);

/*___________________________________________________________________________*/

#endif