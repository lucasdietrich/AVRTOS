/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEFINES_H
#define _AVRTOS_DEFINES_H

/*___________________________________________________________________________*/

#include "default_config.h"

/*___________________________________________________________________________*/

// This find is include from asm files, make it compatible with ASM or user "#if !defined(__ASSEMBLER__)" preprocessor

#include "common.h"

/*___________________________________________________________________________*/

// main thread priority
#if defined(CONFIG_THREAD_MAIN_COOPERATIVE)
#   define THREAD_MAIN_COOPERATIVE CONFIG_THREAD_MAIN_COOPERATIVE
#else
#   define THREAD_MAIN_COOPERATIVE DEFAULT_THREAD_MAIN_COOPERATIVE
#endif

// explicit main stack
#if defined(CONFIG_THREAD_EXPLICIT_MAIN_STACK)
#   define THREAD_EXPLICIT_MAIN_STACK CONFIG_THREAD_EXPLICIT_MAIN_STACK
#else
#   define THREAD_EXPLICIT_MAIN_STACK DEFAULT_THREAD_EXPLICIT_MAIN_STACK
#endif

// main stack size (if explicit)
#if defined(CONFIG_THREAD_MAIN_STACK_SIZE)
#   define THREAD_MAIN_STACK_SIZE CONFIG_THREAD_MAIN_STACK_SIZE
#else
#   define THREAD_MAIN_STACK_SIZE DEFAULT_THREAD_MAIN_STACK_SIZE
#endif

// set default SREG (interrupt flag or not)
#if defined(CONFIG_THREAD_DEFAULT_SREG)
#   define THREAD_DEFAULT_SREG CONFIG_THREAD_DEFAULT_SREG
#else
#   define THREAD_DEFAULT_SREG DEFAULT_THREAD_DEFAULT_SREG
#endif

// set priority mechanism
#if defined(CONFIG_TREAD_PRIO_MULTIQ)
#   define TREAD_PRIO_MULTIQ CONFIG_TREAD_PRIO_MULTIQ
#else
#   define TREAD_PRIO_MULTIQ DEFAULT_TREAD_PRIO_MULTIQ
#endif


// kernel debug mode
#if defined(CONFIG_KERNEL_DEBUG)
#   define KERNEL_DEBUG CONFIG_KERNEL_DEBUG
#else
#   define KERNEL_DEBUG DEFAULT_KERNEL_DEBUG
#endif

// kernel debug function mode
#if defined(CONFIG_KERNEL_API_NOINLINE)
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
#if defined(CONFIG_KERNEL_SCHEDULER_DEBUG)
#   define KERNEL_SCHEDULER_DEBUG CONFIG_KERNEL_SCHEDULER_DEBUG
#else
#   define KERNEL_SCHEDULER_DEBUG DEFAULT_KERNEL_SCHEDULER_DEBUG
#endif

// compare threads after scheduler call
#if defined(CONFIG_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH)
#	define KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH CONFIG_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH
#else
#	define KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH DEFAULT_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH
#endif

// preemptive threads
#if defined(CONFIG_KERNEL_PREEMPTIVE_THREADS)
#   define KERNEL_PREEMPTIVE_THREADS CONFIG_KERNEL_PREEMPTIVE_THREADS
#else
#   define KERNEL_PREEMPTIVE_THREADS DEFAULT_KERNEL_PREEMPTIVE_THREADS
#endif

// kernel auto init
#if defined(CONFIG_KERNEL_AUTO_INIT)
#   define KERNEL_AUTO_INIT CONFIG_KERNEL_AUTO_INIT
#else
#   define KERNEL_AUTO_INIT DEFAULT_KERNEL_AUTO_INIT
#endif

// select hardware timer
#if defined(CONFIG_KERNEL_SYSLOCK_HW_TIMER)
#	define _DESIRED_SYSLOCK_HW_TIMER CONFIG_KERNEL_SYSLOCK_HW_TIMER
#else
#   	define _DESIRED_SYSLOCK_HW_TIMER DEFAULT_KERNEL_SYSLOCK_HW_TIMER
#endif

// in case of qemu emulator, check if the timer is available
#if defined(__QEMU__) && (_DESIRED_SYSLOCK_HW_TIMER != 1U)
#	warning "QEMU emulator detected, only timer 1 is supported (forced)"
#	define KERNEL_SYSLOCK_HW_TIMER 1U
#else
#	define KERNEL_SYSLOCK_HW_TIMER _DESIRED_SYSLOCK_HW_TIMER
#endif

// select sysclock period us when precision mode disabled
#if defined(CONFIG_KERNEL_SYSCLOCK_PERIOD_US)
#   define KERNEL_SYSCLOCK_PERIOD_US CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#else
#   define KERNEL_SYSCLOCK_PERIOD_US DEFAULT_KERNEL_SYSCLOCK_PERIOD_US
#endif

#if defined(CONFIG_KERNEL_TICKS_40BITS)
#   define KERNEL_TICKS_40BITS CONFIG_KERNEL_TICKS_40BITS
#else
#   define KERNEL_TICKS_40BITS DEFAULT_KERNEL_TICKS_40BITS
#endif

// preemptive threads time_slice in us
#if defined(CONFIG_KERNEL_TIME_SLICE_US)
#   define KERNEL_TIME_SLICE_US CONFIG_KERNEL_TIME_SLICE_US
#else
#   define KERNEL_TIME_SLICE_US DEFAULT_KERNEL_TIME_SLICE_US
#endif

//
// debug preempt via uart
//
#if defined(CONFIG_KERNEL_DEBUG_PREEMPT_UART)
#	define KERNEL_DEBUG_PREEMPT_UART CONFIG_KERNEL_DEBUG_PREEMPT_UART
#else
#	define KERNEL_DEBUG_PREEMPT_UART DEFAULT_KERNEL_DEBUG_PREEMPT_UART
#endif

#if defined(CONFIG_KERNEL_THREAD_IDLE)
#   define KERNEL_THREAD_IDLE CONFIG_KERNEL_THREAD_IDLE
#else
#   define KERNEL_THREAD_IDLE DEFAULT_KERNEL_THREAD_IDLE
#endif

#if KERNEL_PREEMPTIVE_THREADS
#	ifdef CONFIG_THREAD_IDLE_COOPERATIVE
#   		define THREAD_IDLE_COOPERATIVE CONFIG_THREAD_IDLE_COOPERATIVE
#	else
#   		define THREAD_IDLE_COOPERATIVE DEFAULT_THREAD_IDLE_COOPERATIVE
#	endif /* CONFIG_THREAD_IDLE_COOPERATIVE */
#else
#   	define THREAD_IDLE_COOPERATIVE 1
#endif /* KERNEL_PREEMPTIVE_THREADS */

#if defined(CONFIG_KERNEL_THREAD_IDLE_ADD_STACK)
#   define KERNEL_THREAD_IDLE_ADD_STACK CONFIG_KERNEL_THREAD_IDLE_ADD_STACK
#else
#   define KERNEL_THREAD_IDLE_ADD_STACK DEFAULT_KERNEL_THREAD_IDLE_ADD_STACK
#endif

#if defined(CONFIG_THREAD_CANARIES)
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
#if defined(CONFIG_THREAD_STACK_SENTINEL)
#   define THREAD_STACK_SENTINEL CONFIG_THREAD_STACK_SENTINEL
#else
#   define THREAD_STACK_SENTINEL DEFAULT_THREAD_STACK_SENTINEL
#endif

#if THREAD_STACK_SENTINEL
#if defined(CONFIG_THREAD_STACK_SENTINEL_SIZE)
#	define THREAD_STACK_SENTINEL_SIZE CONFIG_THREAD_STACK_SENTINEL_SIZE
#else
#	define THREAD_STACK_SENTINEL_SIZE DEFAULT_THREAD_STACK_SENTINEL_SIZE
#endif
#else
#	define THREAD_STACK_SENTINEL_SIZE	0
#endif /* THREAD_STACK_SENTINEL */

#if defined(CONFIG_THREAD_STACK_SENTINEL_SYMBOL)
#   define THREAD_STACK_SENTINEL_SYMBOL CONFIG_THREAD_STACK_SENTINEL_SYMBOL
#else
#   define THREAD_STACK_SENTINEL_SYMBOL DEFAULT_THREAD_STACK_SENTINEL_SYMBOL
#endif


// #if THREAD_EXPLICIT_MAIN_STACK == 0 && THREAD_CANARIES == 1
// #   warning unable to monitor main thread stack canaries as the main stack is defined as explicit (see THREAD_EXPLICIT_MAIN_STACK)
// #endif

#if defined(CONFIG_SYSTEM_WORKQUEUE_ENABLE)
#   define SYSTEM_WORKQUEUE_ENABLE CONFIG_SYSTEM_WORKQUEUE_ENABLE
#else
#   define SYSTEM_WORKQUEUE_ENABLE DEFAULT_SYSTEM_WORKQUEUE_ENABLE
#endif

#if SYSTEM_WORKQUEUE_ENABLE

#	ifdef CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE
#		define SYSTEM_WORKQUEUE_STACK_SIZE CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE
#	else
#		define SYSTEM_WORKQUEUE_STACK_SIZE DEFAULT_SYSTEM_WORKQUEUE_STACK_SIZE
#	endif

#	ifdef CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE
#		 define SYSTEM_WORKQUEUE_COOPERATIVE CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE
#	else
#		define SYSTEM_WORKQUEUE_COOPERATIVE DEFAULT_SYSTEM_WORKQUEUE_COOPERATIVE
#	endif

#	if SYSTEM_WORKQUEUE_COOPERATIVE
#		define SYSTEM_WORKQUEUE_PRIORITY			K_COOPERATIVE
#	else
#		define SYSTEM_WORKQUEUE_PRIORITY			K_PREEMPTIVE
#	endif /* SYSTEM_WORKQUEUE_COOPERATIVE */

#endif

#if defined(CONFIG_KERNEL_ASSERT)
#   define KERNEL_ASSERT CONFIG_KERNEL_ASSERT
#else
#   define KERNEL_ASSERT DEFAULT_KERNEL_ASSERT
#endif

#if defined(CONFIG_KERNEL_TIMERS)
#   define KERNEL_TIMERS CONFIG_KERNEL_TIMERS
#else
#   define KERNEL_TIMERS DEFAULT_KERNEL_TIMERS
#endif

#if defined(CONFIG_KERNEL_EVENTS)
#   define KERNEL_EVENTS CONFIG_KERNEL_EVENTS
#else
#   define KERNEL_EVENTS DEFAULT_KERNEL_EVENTS
#endif

#if defined(CONFIG_THREAD_ERRNO)
#   define THREAD_ERRNO CONFIG_THREAD_ERRNO
#else
#   define THREAD_ERRNO DEFAULT_THREAD_ERRNO
#endif

#if defined(CONFIG_KERNEL_SCHED_LOCK_COUNTER)
#   define KERNEL_SCHED_LOCK_COUNTER CONFIG_KERNEL_SCHED_LOCK_COUNTER
#else
#   define KERNEL_SCHED_LOCK_COUNTER DEFAULT_KERNEL_SCHED_LOCK_COUNTER
#endif

#if defined(CONFIG_KERNEL_IRQ_LOCK_COUNTER)
#   define KERNEL_IRQ_LOCK_COUNTER CONFIG_KERNEL_IRQ_LOCK_COUNTER
#else
#   define KERNEL_IRQ_LOCK_COUNTER DEFAULT_KERNEL_IRQ_LOCK_COUNTER
#endif

#if defined(CONFIG_STDIO_PRINTF_TO_USART)
#   define STDIO_PRINTF_TO_USART CONFIG_STDIO_PRINTF_TO_USART
#else
#   define STDIO_PRINTF_TO_USART DEFAULT_STDIO_PRINTF_TO_USART
#endif

#if defined(CONFIG_LOGGING_SUBSYSTEM)
#	define LOGGING_SUBSYSTEM CONFIG_LOGGING_SUBSYSTEM
#else
#	define LOGGING_SUBSYSTEM DEFAULT_LOGGING_SUBSYSTEM
#endif /* CONFIG_LOGGING_SUBSYSTEM */

#if LOGGING_SUBSYSTEM && (STDIO_PRINTF_TO_USART < 0)
#	error "Logging subsystem requires STDIO_PRINTF_TO_USART to be defined"
#endif

#if defined(CONFIG_FD_MAX_COUNT)
#   define FD_MAX_COUNT CONFIG_FD_MAX_COUNT
#else
#   define FD_MAX_COUNT DEFAULT_FD_MAX_COUNT
#endif

#if defined(CONFIG_KERNEL_UPTIME)
#   define KERNEL_UPTIME CONFIG_KERNEL_UPTIME
#else
#   define KERNEL_UPTIME DEFAULT_KERNEL_UPTIME
#endif

#if defined(CONFIG_KERNEL_TIME)
#   define KERNEL_TIME CONFIG_KERNEL_TIME
#else
#   define KERNEL_TIME DEFAULT_KERNEL_TIME
#endif

#if defined(CONFIG_KERNEL_ATOMIC_API)
#   define KERNEL_ATOMIC_API CONFIG_KERNEL_ATOMIC_API
#else
#   define KERNEL_ATOMIC_API DEFAULT_KERNEL_ATOMIC_API
#endif

#if defined(CONFIG_KERNEL_THREAD_TERMINATION_TYPE)
#   define KERNEL_THREAD_TERMINATION_TYPE CONFIG_KERNEL_THREAD_TERMINATION_TYPE
#else
#   define KERNEL_THREAD_TERMINATION_TYPE DEFAULT_KERNEL_THREAD_TERMINATION_TYPE
#endif /* CONFIG_KERNEL_THREAD_TERMINATION_TYPE */

#if defined(CONFIG_KERNEL_DELAY_OBJECT_U32)
#   define KERNEL_DELAY_OBJECT_U32 CONFIG_KERNEL_DELAY_OBJECT_U32
#else
#   define KERNEL_DELAY_OBJECT_U32 DEFAULT_KERNEL_DELAY_OBJECT_U32
#endif /* CONFIG_KERNEL_DELAY_OBJECT_U32 */

#if defined(CONFIG_DRIVERS_USART0_ASYNC)
#	define DRIVERS_USART0_ASYNC CONFIG_DRIVERS_USART0_ASYNC
#else
#	define DRIVERS_USART0_ASYNC DEFAULT_DRIVERS_USART0_ASYNC
#endif /* CONFIG_DRIVERS_USART0_ASYNC */

#if defined(CONFIG_DRIVERS_USART1_ASYNC)
#	define DRIVERS_USART1_ASYNC CONFIG_DRIVERS_USART1_ASYNC
#else
#	define DRIVERS_USART1_ASYNC DEFAULT_DRIVERS_USART1_ASYNC
#endif /* CONFIG_DRIVERS_USART1_ASYNC */

#if defined(CONFIG_DRIVERS_USART2_ASYNC)
#	define DRIVERS_USART2_ASYNC CONFIG_DRIVERS_USART2_ASYNC
#else
#	define DRIVERS_USART2_ASYNC DEFAULT_DRIVERS_USART2_ASYNC
#endif /* CONFIG_DRIVERS_USART2_ASYNC */

#if defined(CONFIG_DRIVERS_USART3_ASYNC)
#	define DRIVERS_USART3_ASYNC CONFIG_DRIVERS_USART3_ASYNC
#else
#	define DRIVERS_USART3_ASYNC DEFAULT_DRIVERS_USART3_ASYNC
#endif /* CONFIG_DRIVERS_USART3_ASYNC */

#define DRIVERS_UART_ASYNC ((DRIVERS_USART0_ASYNC) || \
	(DRIVERS_USART1_ASYNC) || \
	(DRIVERS_USART2_ASYNC) || \
	(DRIVERS_USART3_ASYNC))	

#if defined(CONFIG_INTERRUPT_POLICY)
#	define INTERRUPT_POLICY CONFIG_INTERRUPT_POLICY
#else
#	define INTERRUPT_POLICY DEFAULT_INTERRUPT_POLICY
#endif /* CONFIG_INTERRUPT_POLICY */

#if defined(CONFIG_DRIVERS_TIMER0_API)
#	define DRIVERS_TIMER0_API CONFIG_DRIVERS_TIMER0_API
#else
#	define DRIVERS_TIMER0_API DEFAULT_DRIVERS_TIMER0_API
#endif /* CONFIG_DRIVERS_TIMER0_API */

// timer1
#if defined(CONFIG_DRIVERS_TIMER1_API)
#	define DRIVERS_TIMER1_API CONFIG_DRIVERS_TIMER1_API
#else
#	define DRIVERS_TIMER1_API DEFAULT_DRIVERS_TIMER1_API
#endif /* CONFIG_DRIVERS_TIMER1_API */


// timer2
#if defined(CONFIG_DRIVERS_TIMER2_API)
#	define DRIVERS_TIMER2_API CONFIG_DRIVERS_TIMER2_API
#else
#	define DRIVERS_TIMER2_API DEFAULT_DRIVERS_TIMER2_API
#endif /* CONFIG_DRIVERS_TIMER2_API */

// timer3
#if defined(CONFIG_DRIVERS_TIMER3_API)
#	define DRIVERS_TIMER3_API CONFIG_DRIVERS_TIMER3_API
#else
#	define DRIVERS_TIMER3_API DEFAULT_DRIVERS_TIMER3_API
#endif /* CONFIG_DRIVERS_TIMER3_API */

// timer4
#if defined(CONFIG_DRIVERS_TIMER4_API)
#	define DRIVERS_TIMER4_API CONFIG_DRIVERS_TIMER4_API
#else
#	define DRIVERS_TIMER4_API DEFAULT_DRIVERS_TIMER4_API
#endif /* CONFIG_DRIVERS_TIMER4_API */


#if defined(CONFIG_DRIVERS_TIMER5_API)
#	define DRIVERS_TIMER5_API CONFIG_DRIVERS_TIMER5_API
#else
#	define DRIVERS_TIMER5_API DEFAULT_DRIVERS_TIMER5_API
#endif /* CONFIG_DRIVERS_TIMER5_API */

#define DRIVERS_TIMERS_API ((DRIVERS_TIMER0_API) || \
	(DRIVERS_TIMER1_API) || \
	(DRIVERS_TIMER2_API) || \
	(DRIVERS_TIMER3_API) || \
	(DRIVERS_TIMER4_API) || \
	(DRIVERS_TIMER5_API))

#if defined(CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG)
#	define KERNEL_SYSTICK_GPIOB_DEBUG CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
#else
#	define KERNEL_SYSTICK_GPIOB_DEBUG DEFAULT_KERNEL_SYSTICK_GPIOB_DEBUG
#endif /* CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG */

#if defined(CONFIG_KERNEL_CHECKS)
#	define KERNEL_CHECKS CONFIG_KERNEL_CHECKS
#else
#	define KERNEL_CHECKS DEFAULT_KERNEL_CHECKS
#endif /* CONFIG_KERNEL_CHECKS */


/*___________________________________________________________________________*/

#if KERNEL_UPTIME
#	define KERNEL_TICKS			1
#	if KERNEL_TICKS_40BITS
#		define KERNEL_TICKS_SIZE 	5
#	else
#		define KERNEL_TICKS_SIZE 	4
#	endif
#else
#	define KERNEL_TICKS			0
#	define KERNEL_TICKS_SIZE 		0
#endif

#if KERNEL_TIME_SLICE_US < KERNEL_SYSCLOCK_PERIOD_US
#	error [UNSUPPORTED] KERNEL_TIME_SLICE_US < KERNEL_SYSCLOCK_PERIOD_US
#elif KERNEL_TIME_SLICE_US > KERNEL_SYSCLOCK_PERIOD_US
#	define KERNEL_TIME_SLICE_MULTIPLE_TICKS 1

#	if KERNEL_TIME_SLICE_US % KERNEL_SYSCLOCK_PERIOD_US != 0
#		warning [WARNING] KERNEL_TIME_SLICE_US must be a multiple of KERNEL_SYSCLOCK_PERIOD_US
#	endif 

#	if KERNEL_TIME_SLICE_US / KERNEL_SYSCLOCK_PERIOD_US <= 255
#		define KERNEL_TIME_SLICE_TICKS	(KERNEL_TIME_SLICE_US / KERNEL_SYSCLOCK_PERIOD_US)
#	else 
#		error "Time slice too different compared to KERNEL_SYSCLOCK_PERIOD_US\
		KERNEL_TIME_SLICE_US / KERNEL_SYSCLOCK_PERIOD_US > 255"
#	endif 
#else 
#	define KERNEL_TIME_SLICE_MULTIPLE_TICKS 	0
#	define KERNEL_TIME_SLICE_TICKS			1
#endif /* KERNEL_TIME_SLICE_US != KERNEL_SYSCLOCK_PERIOD_US */


#define K_EVENTS_PERIOD_TICKS	KERNEL_TIME_SLICE_TICKS
#define K_TIMERS_PERIOD_TICKS	KERNEL_TIME_SLICE_TICKS

#define KERNEL_TICK_PERIOD_US	KERNEL_SYSCLOCK_PERIOD_US
#define KERNEL_TICK_PERIOD_MS	(KERNEL_TICK_PERIOD_US / 1000ULL)

#define K_TICKS_US		KERNEL_SYSCLOCK_PERIOD_US
#define K_TICKS_PER_SECOND	(((float)1000000ULL) / KERNEL_SYSCLOCK_PERIOD_US)
#define K_TICKS_PER_MS		(((float)1000ULL) / KERNEL_SYSCLOCK_PERIOD_US)

// put all c specific definitions  here

#if !defined(__ASSEMBLER__)

#include <stdint.h>
#include <stddef.h>

#if KERNEL_DELAY_OBJECT_U32
typedef uint32_t k_ticks_t;
#else 
typedef uint16_t k_ticks_t;
#endif /* KERNEL_DELAY_OBJECT_U32 */

typedef k_ticks_t k_delta_t;

typedef struct
{
    k_ticks_t value;
} k_timeout_t;

#endif

#define MSEC_PER_SEC		(1000LU)
#define USEC_PER_MSEC		(1000LU)
#define USEC_PER_SEC		(USEC_PER_MSEC * MSEC_PER_SEC)

#define K_TIMEOUT_TICKS(t)	(t.value)
#define K_TIMEOUT_EQ(t1, t2)    (K_TIMEOUT_TICKS(t1) == K_TIMEOUT_TICKS(t2))
#define K_TIMEOUT_MS(t)		((uint32_t) (K_TIMEOUT_TICKS(t) / K_TICKS_PER_MS))
#define K_TIMEOUT_SECONDS(t)	((uint32_t) (K_TIMEOUT_TICKS(t) / K_TICKS_PER_SECOND))

#ifndef __cplusplus

#define K_SECONDS(seconds)      ((k_timeout_t){.value = K_TICKS_PER_SECOND * seconds})
#define K_MSEC(milliseconds)    ((k_timeout_t){.value = K_TICKS_PER_MS * milliseconds})
#define K_NO_WAIT               ((k_timeout_t){.value = (k_ticks_t) 0})
#define K_NEXT_TICK             ((k_timeout_t){.value = (k_ticks_t) 1})
#define K_FOREVER               ((k_timeout_t){.value = (k_ticks_t) -1})
#define K_UNTIL_WAKEUP          K_FOREVER

#define K_IMMEDIATE 	   	K_NEXT_TICK

#else

#define K_SECONDS(seconds)      ((k_timeout_t){.value = static_cast<k_ticks_t>(K_TICKS_PER_SECOND * seconds) })
#define K_MSEC(milliseconds)    ((k_timeout_t){.value = static_cast<k_ticks_t>(K_TICKS_PER_MS * milliseconds) })
#define K_NO_WAIT               ((k_timeout_t){.value = (k_ticks_t) 0})
#define K_FOREVER               ((k_timeout_t){.value = (k_ticks_t) -1})
#define K_UNTIL_WAKEUP          K_FOREVER

#endif /* __cplusplus */
/*___________________________________________________________________________*/

// arch specific fixups

#if defined(__AVR_ATmega328P__)
#define USART0_RX_vect USART_RX_vect
#define USART0_RX_vect_num USART_RX_vect_num
#define USART0_TX_vect USART_TX_vect
#define USART0_TX_vect_num USART_TX_vect_num
#define USART0_UDRE_vect USART_UDRE_vect
#define USART0_UDRE_vect_num USART_UDRE_vect_num
#endif /* __AVR_ATmega328P__ */


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

/* stack sentinel */

#if THREAD_STACK_SENTINEL
#	define _K_STACK_SENTINEL_REGISTER(stack_symb)	__attribute__((used, section(".k_sentinels"))) void *_k_sent_##stack_symb = (void *)(&stack_symb)
#else
#	define _K_STACK_SENTINEL_REGISTER(stack_symb)
#endif /* THREAD_STACK_SENTINEL */

/*___________________________________________________________________________*/

// 31 registers (31) + SREG (1) + return address (2 or 3)


#if !defined(__ASSEMBLER__)
#define _K_CALLSAVED_CTX_SIZE	  sizeof(struct _k_callsaved_ctx)
#else 
#define _K_CALLSAVED_CTX_SIZE	  (19U + _K_ARCH_PC_SIZE)
#endif 

// call-clobbered (or call-used) registers
#define _K_CALLUSED_CTX_SIZE	  sizeof(struct _k_callused_ctx)

// interrupt context
#define _K_INTCTX_SIZE		  sizeof(struct _k_intctx)

#define K_THREAD_STACK_VOID_SIZE 	_K_CALLSAVED_CTX_SIZE
#define K_THREAD_CTX_START(stack_end)	((struct _k_callsaved_ctx *)((uint8_t*)stack_end - _K_CALLSAVED_CTX_SIZE + 1U))

#define K_THREAD_STACK_MIN_SIZE (K_THREAD_STACK_VOID_SIZE + THREAD_STACK_SENTINEL_SIZE)

// some of following macros need to be differenciate for c or asm :
// - in c files the compiler needs to know the type of stack_start in order to do arithmetic operations on pointers
// - in asm files types are not understood by compiler
#define K_STACK_END(stack_start, size) (uint8_t*) ((uint8_t*) (stack_start) + (size) - 1)
#define K_STACK_START(stack_end, size) (uint8_t*) ((uint8_t*) (stack_end) - (size) + 1)
#define K_THREAD_STACK_START(th) K_STACK_START(th->stack.end, th->stack.size)
#define K_THREAD_STACK_END(th) (th->stack.end)

/* real stack start and size without counting sentinel */
#define K_STACK_START_USABLE(stack_end, size) (K_STACK_START(stack_end, size) + THREAD_STACK_SENTINEL_SIZE)
#define K_STACK_SIZE_USABLE(size) (size - THREAD_STACK_SENTINEL_SIZE)
#define K_THREAD_STACK_START_USABLE(th) K_STACK_START_USABLE(th->stack.end, th->stack.size)

#define _K_STACK_END(stack_start, size) ((stack_start) + (size) - 1)

#define _K_STACK_END_ASM(stack_start, size) _K_STACK_END(stack_start, size)

#define _K_STACK_INIT_SP(stack_end) (stack_end - _K_CALLSAVED_CTX_SIZE)

// if not casting this symbol address, the stack pointer will not be correctly set
#define _K_THREAD_STACK_START(name) ((uint8_t*)(&_k_stack_buf_##name))

#define _K_THREAD_STACK_SIZE(name) (sizeof(_k_stack_buf_##name))

#define _K_STACK_INIT_SP_FROM_NAME(name, stack_size) _K_STACK_INIT_SP(_K_STACK_END(_K_THREAD_STACK_START(name), stack_size))

#define K_THREAD        __attribute__((used, section(".k_threads")))

#define THREAD_FROM_EVENTQUEUE(item) CONTAINER_OF(item, struct k_thread, tie.runqueue)
#define THREAD_FROM_WAITQUEUE(item) CONTAINER_OF(item, struct k_thread, wany)
#define THREAD_OF_TITEM(item) CONTAINER_OF(item, struct k_thread, tie.event)

#define _K_CORE_CONTEXT_INIT(entry, ctx, __entry) \
(struct _k_callsaved_ctx) { \
	.sreg = 0x00, \
	{ .regs = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U} }, \
	{ .init_sreg = THREAD_DEFAULT_SREG }, \
	{ .thread_entry = (thread_entry_t) entry }, \
	{ .thread_context = (void*) ctx }, \
	{ .pc = (void*) __entry } \
}

#define _K_STACK_INITIALIZER(name, stack_size, entry, ctx) \
    struct \
    { \
        uint8_t empty[(stack_size) - _K_CALLSAVED_CTX_SIZE]; \
        struct _k_callsaved_ctx core; \
    } _k_stack_buf_##name = { \
        {0x00}, \
       _K_CORE_CONTEXT_INIT(entry, ctx, _k_thread_entry), \
    }

#define _K_STACK_MINIMAL_INITIALIZER(name, entry, ctx) \
    struct _k_callsaved_ctx _k_stack_buf_##name = _K_CORE_CONTEXT_INIT(entry, ctx, _k_thread_entry)

#define _K_THREAD_INITIALIZER(_name, stack_size, prio_flag, sym)                                              \
    struct k_thread _name = {                                                                                 \
        .sp = (void *)_K_STACK_INIT_SP_FROM_NAME(_name, stack_size),                                          \
        {                                                                                                    \
            .flags = K_FLAG_READY | prio_flag,                                                               \
        },                                                                                                   \
        .tie = {.runqueue = DITEM_INIT(NULL)},                                                               \
        {.wmutex = DITEM_INIT(NULL)},                                                                        \
        .swap_data = NULL,                                                                                   \
        .stack = {.end = (void *)_K_STACK_END(_K_THREAD_STACK_START(_name), stack_size), .size = (stack_size)}, \
        .symbol = sym}

#define K_THREAD_DEFINE(name, entry, stack_size, prio_flag, context_p, symbol)                  \
    __attribute__((used)) _K_STACK_INITIALIZER(name, stack_size, entry, context_p); \
    __attribute__((used, section(".k_threads"))) _K_THREAD_INITIALIZER(name, stack_size, prio_flag, symbol); \
    _K_STACK_SENTINEL_REGISTER(_k_stack_buf_##name);

#define K_THREAD_MINIMAL_DEFINE(name, entry, prio_flag, context_p, symbol)                  \
    __attribute__((used)) _K_STACK_MINIMAL_INITIALIZER(name, entry, context_p); \
    __attribute__((used, section(".k_threads"))) _K_THREAD_INITIALIZER(name, _K_CALLSAVED_CTX_SIZE, prio_flag, symbol); \
    _K_STACK_SENTINEL_REGISTER(_k_stack_buf_##name);

/*___________________________________________________________________________*/

#endif