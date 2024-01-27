/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEFINES_H
#define _AVRTOS_DEFINES_H

#include "avrtos_conf.h"
#include "errno.h"
#include "sys.h"

#define AVRTOS_VERSION_MAJOR	1
#define AVRTOS_VERSION_MINOR	2
#define AVRTOS_VERSION_REVISION 0

#if CONFIG_USE_STDLIB_HEAP_MALLOC_MAIN && CONFIG_THREAD_EXPLICIT_MAIN_STACK
#error                                                                                   \
	"CONFIG_USE_STDLIB_HEAP_MALLOC_MAIN and CONFIG_THREAD_EXPLICIT_MAIN_STACK are incompatible"
#endif

#if CONFIG_KERNEL_API_NOINLINE
#define __kernel __noinline
#else
#define __kernel
#endif

#if CONFIG_AVRTOS_LINKER_SCRIPT
#define Z_LINK_KERNEL_SECTION(_section) Z_LINK_SECTION_USED(_section)
#else
#define Z_LINK_KERNEL_SECTION(_section) __attribute__((used))
#endif

#if !CONFIG_KERNEL_COOPERATIVE_THREADS && CONFIG_THREAD_IDLE_COOPERATIVE
#error "CONFIG_THREAD_IDLE_COOPERATIVE is required with CONFIG_KERNEL_COOPERATIVE_THREADS"
#endif

#if !CONFIG_KERNEL_COOPERATIVE_THREADS && CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE
#error                                                                                   \
	"CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE is required with CONFIG_KERNEL_COOPERATIVE_THREADS"
#endif

#if CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE
#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY K_COOPERATIVE
#else
#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY K_PREEMPTIVE
#endif

#if CONFIG_KERNEL_ARGS_CHECKS
#define Z_ARGS_CHECK(_cond) if (!(_cond))
#else
#define Z_ARGS_CHECK(_cond) if (0)
#endif

/*___________________________________________________________________________*/

// Timing

#if CONFIG_KERNEL_UPTIME
#define CONFIG_KERNEL_TICKS_COUNTER 1
#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
#define CONFIG_KERNEL_TICKS_COUNTER_SIZE 5
#else
#define CONFIG_KERNEL_TICKS_COUNTER_SIZE 4
#endif
#else
#define CONFIG_KERNEL_TICKS_COUNTER		 0
#define CONFIG_KERNEL_TICKS_COUNTER_SIZE 0
#endif

#if !CONFIG_KERNEL_UPTIME && CONFIG_KERNEL_TIME_API
#error "CONFIG_KERNEL_TIME_API requires CONFIG_KERNEL_UPTIME"
#endif

#if CONFIG_KERNEL_TIME_SLICE_US < CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#error[UNSUPPORTED] CONFIG_KERNEL_TIME_SLICE_US < CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#elif CONFIG_KERNEL_TIME_SLICE_US > CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#define CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS 1

#if CONFIG_KERNEL_TIME_SLICE_US % CONFIG_KERNEL_SYSCLOCK_PERIOD_US != 0
#warning[WARNING] CONFIG_KERNEL_TIME_SLICE_US must be a multiple of CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#endif

#if CONFIG_KERNEL_TIME_SLICE_US / CONFIG_KERNEL_SYSCLOCK_PERIOD_US <= 255
#define CONFIG_KERNEL_TIME_SLICE_TICKS                                                   \
	(CONFIG_KERNEL_TIME_SLICE_US / CONFIG_KERNEL_SYSCLOCK_PERIOD_US)
#else
#error "Time slice too different compared to CONFIG_KERNEL_SYSCLOCK_PERIOD_US\
		CONFIG_KERNEL_TIME_SLICE_US / CONFIG_KERNEL_SYSCLOCK_PERIOD_US > 255"
#endif
#else
#define CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS 0
#define CONFIG_KERNEL_TIME_SLICE_TICKS			1
#endif /* CONFIG_KERNEL_TIME_SLICE_US != CONFIG_KERNEL_SYSCLOCK_PERIOD_US */

#define K_EVENTS_PERIOD_TICKS CONFIG_KERNEL_TIME_SLICE_TICKS
#define K_TIMERS_PERIOD_TICKS CONFIG_KERNEL_TIME_SLICE_TICKS

#define KERNEL_TICK_PERIOD_US CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#define KERNEL_TICK_PERIOD_MS (KERNEL_TICK_PERIOD_US / 1000ULL)

#define K_TICKS_US		   CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#define K_TICKS_PER_SECOND (((float)1000000ULL) / CONFIG_KERNEL_SYSCLOCK_PERIOD_US)
#define K_TICKS_PER_MS	   (((float)1000ULL) / CONFIG_KERNEL_SYSCLOCK_PERIOD_US)
#define K_TICKS_PER_USEC   (((float)1ULL) / CONFIG_KERNEL_SYSCLOCK_PERIOD_US)

// put all c specific definitions  here

#if !defined(__ASSEMBLER__)

#include <stddef.h>
#include <stdint.h>

#if CONFIG_KERNEL_DELAY_OBJECT_U32
typedef uint32_t k_ticks_t;
#else
typedef uint16_t k_ticks_t;
#endif /* CONFIG_KERNEL_DELAY_OBJECT_U32 */

typedef k_ticks_t k_delta_t;

typedef struct {
	k_ticks_t value;
} k_timeout_t;

#endif

#define MSEC_PER_SEC  (1000LU)
#define USEC_PER_MSEC (1000LU)
#define USEC_PER_SEC  (USEC_PER_MSEC * MSEC_PER_SEC)

#define K_TIMEOUT_TICKS(t)	 (t.value)
#define K_TIMEOUT_EQ(t1, t2) (K_TIMEOUT_TICKS(t1) == K_TIMEOUT_TICKS(t2))
#define K_TIMEOUT_MS(t)		 ((uint32_t)(K_TIMEOUT_TICKS(t) / K_TICKS_PER_MS))
#define K_TIMEOUT_SECONDS(t) ((uint32_t)(K_TIMEOUT_TICKS(t) / K_TICKS_PER_SECOND))

#ifndef __cplusplus
#define K_TICKS(ticks) ((k_timeout_t){.value = (k_ticks_t)ticks})
#else
#define K_TICKS(ticks) ((k_timeout_t){.value = static_cast<k_ticks_t>(ticks)})
#endif /* __cplusplus */

#define K_SECONDS(seconds)	 K_TICKS(K_TICKS_PER_SECOND *seconds)
#define K_MSEC(milliseconds) K_TICKS(K_TICKS_PER_MS *milliseconds)
#define K_USEC(microseconds) K_TICKS(K_TICKS_PER_USEC *microseconds)
#define K_NO_WAIT			 K_TICKS(0)
#define K_NEXT_TICK			 K_TICKS(1)
#define K_FOREVER			 K_TICKS(-1)
#define K_UNTIL_WAKEUP		 K_FOREVER

#define K_IMMEDIATE K_NEXT_TICK

/*___________________________________________________________________________*/

// MCU specific fixups

#if !defined(USART0_RX_vect) && defined(USART_RX_vect)
#define USART0_RX_vect		 USART_RX_vect
#define USART0_RX_vect_num	 USART_RX_vect_num
#define USART0_TX_vect		 USART_TX_vect
#define USART0_TX_vect_num	 USART_TX_vect_num
#define USART0_UDRE_vect	 USART_UDRE_vect
#define USART0_UDRE_vect_num USART_UDRE_vect_num
#endif /* __AVR_ATmega328P__ */

// compiler constants :
// https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html#AVR-Built-in-Macros

#if defined(__AVR_3_BYTE_PC__)
#define Z_ARCH_STACK_SIZE_FIXUP 1
#define Z_ARCH_PC_SIZE			3
#else
#if defined(__AVR_2_BYTE_PC__)
#define Z_ARCH_STACK_SIZE_FIXUP 0
#define Z_ARCH_PC_SIZE			2
#else
#error unsupported PC size
#define Z_ARCH_STACK_SIZE_FIXUP 0
#define Z_ARCH_PC_SIZE			0
#endif
#endif

/* stack sentinel */

#if CONFIG_THREAD_STACK_SENTINEL
#define Z_STACK_SENTINEL_REGISTER(stack_symb)                                            \
	Z_LINK_KERNEL_SECTION(.k_sentinels)                                                  \
	void *z_sent_##stack_symb = (void *)(&stack_symb)
#else
#define Z_STACK_SENTINEL_REGISTER(stack_symb)
#endif /* CONFIG_THREAD_STACK_SENTINEL */

/*___________________________________________________________________________*/

// 31 registers (31) + SREG (1) + return address (2 or 3)

#if !defined(__ASSEMBLER__)
#define Z_CALLSAVED_CTX_SIZE sizeof(struct z_callsaved_ctx)
#else
#define Z_CALLSAVED_CTX_SIZE (19U + Z_ARCH_PC_SIZE)
#endif

// call-clobbered (or call-used) registers
#define Z_CALLUSED_CTX_SIZE sizeof(struct z_callused_ctx)

// interrupt context
#define Z_INTCTX_SIZE sizeof(struct z_intctx)

#define Z_THREAD_STACK_VOID_SIZE Z_CALLSAVED_CTX_SIZE
#define Z_THREAD_CTX_START(stack_end)                                                    \
	((struct z_callsaved_ctx *)((uint8_t *)stack_end - Z_CALLSAVED_CTX_SIZE + 1U))

#define Z_THREAD_STACK_MIN_SIZE                                                          \
	(Z_THREAD_STACK_VOID_SIZE + CONFIG_THREAD_STACK_SENTINEL_SIZE)

// some of following macros need to be differenciate for c or asm :
// - in c files the compiler needs to know the type of stack_start in order to
// do arithmetic operations on pointers
// - in asm files types are not understood by compiler
#define K_STACK_END(stack_start, size) (uint8_t *)((uint8_t *)(stack_start) + (size)-1)
#define K_STACK_START(stack_end, size) (uint8_t *)((uint8_t *)(stack_end) - (size) + 1)
#define K_THREAD_STACK_START(thread)   K_STACK_START(thread->stack.end, thread->stack.size)
#define K_THREAD_STACK_END(thread)	   (thread->stack.end)

/* real stack start and size without counting sentinel */
#if CONFIG_THREAD_STACK_SENTINEL
#define Z_STACK_START_USABLE(stack_end, size)                                            \
	(K_STACK_START(stack_end, size) + CONFIG_THREAD_STACK_SENTINEL_SIZE)
#define Z_STACK_SIZE_USABLE(size) ((size)-CONFIG_THREAD_STACK_SENTINEL_SIZE)
#else
#define Z_STACK_START_USABLE(stack_end, size) K_STACK_START(stack_end, size)
#define Z_STACK_SIZE_USABLE(size)			  (size)
#endif

#define Z_THREAD_STACK_START_USABLE(thread)                                              \
	Z_STACK_START_USABLE(thread->stack.end, thread->stack.size)

#define Z_STACK_END(stack_start, size) ((stack_start) + (size)-1)

#define Z_STACK_END_ASM(stack_start, size) Z_STACK_END(stack_start, size)

#define Z_STACK_INIT_SP(stack_end) (stack_end - Z_CALLSAVED_CTX_SIZE)

// if not casting this symbol address, the stack pointer will not be correctly
// set
#define Z_THREAD_STACK_START(name) ((uint8_t *)(&z_stack_buf_##name))

#define Z_THREAD_STACK_SIZE(name) (sizeof(z_stack_buf_##name))

#define Z_STACK_INIT_SP_FROM_NAME(name, stack_size)                                      \
	Z_STACK_INIT_SP(Z_STACK_END(Z_THREAD_STACK_START(name), stack_size))

#define K_THREAD Z_LINK_KERNEL_SECTION(.k_threads)

#define Z_THREAD_FROM_EVENTQUEUE(item) CONTAINER_OF(item, struct k_thread, tie.runqueue)
#define Z_THREAD_FROM_WAITQUEUE(item)  CONTAINER_OF(item, struct k_thread, wany)
#define Z_THREAD_OF_TITEM(item)		   CONTAINER_OF(item, struct k_thread, tie.event)

#if Z_ARCH_PC_SIZE == 3
#define Z_CORE_CONTEXT_ARCH_PC_INIT(_entry)                                              \
	{                                                                                    \
		.pch = 0x00u, .pc = (void *)_entry,                                              \
	}
#else
#define Z_CORE_CONTEXT_ARCH_PC_INIT(_entry)                                              \
	{                                                                                    \
		.pc = (void *)_entry,                                                            \
	}
#endif

#define Z_CORE_CONTEXT_INIT(entry, ctx, _entry)                                          \
	(struct z_callsaved_ctx)                                                             \
	{                                                                                    \
		.sreg = 0x00, {.regs = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U}},    \
		{.init_sreg = CONFIG_THREAD_DEFAULT_SREG},                                       \
		{.thread_entry = (k_thread_entry_t)entry}, {.thread_context = (void *)ctx},      \
		Z_CORE_CONTEXT_ARCH_PC_INIT(_entry),                                             \
	}

#define Z_STACK_INITIALIZER(name, stack_size, entry, ctx)                                \
	struct {                                                                             \
		uint8_t empty[(stack_size)-Z_CALLSAVED_CTX_SIZE];                                \
		struct z_callsaved_ctx core;                                                     \
	} z_stack_buf_##name = {                                                             \
		{0x00},                                                                          \
		Z_CORE_CONTEXT_INIT(entry, ctx, z_thread_entry),                                 \
	}

#define Z_STACK_MINIMAL_INITIALIZER(name, entry, ctx)                                    \
	struct z_callsaved_ctx z_stack_buf_##name =                                          \
		Z_CORE_CONTEXT_INIT(entry, ctx, z_thread_entry)

#define Z_THREAD_INITIALIZER(_name, stack_size, _flags, sym)                             \
	struct k_thread _name = {                                                            \
		.sp	   = (void *)Z_STACK_INIT_SP_FROM_NAME(_name, stack_size),                   \
		.flags = _flags,                                                                 \
		.tie   = {.runqueue = DITEM_INIT(NULL)},                                         \
		{.wany = DITEM_INIT(NULL)},                                                      \
		.swap_data = NULL,                                                               \
		.stack =                                                                         \
			{                                                                            \
				.end  = (void *)Z_STACK_END(Z_THREAD_STACK_START(_name), stack_size),    \
				.size = (stack_size),                                                    \
			},                                                                           \
		.symbol = sym}

#define K_THREAD_DEFINE(name, entry, stack_size, prio_flag, context_p, symbol)           \
	Z_THREAD_DEFINE(name, entry, stack_size, prio_flag, context_p, symbol, 1)

#define K_THREAD_MINIMAL_DEFINE(name, entry, prio_flag, context_p, symbol)               \
	Z_THREAD_MINIMAL_DEFINE(name, entry, prio_flag, context_p, symbol, 1)

#if CONFIG_AVRTOS_LINKER_SCRIPT

#define Z_THREAD_DEFINE(name, entry, stack_size, prio_flag, context_p, symbol,           \
						auto_start)                                                      \
	__attribute__((used)) Z_STACK_INITIALIZER(name, stack_size, entry, context_p);       \
	Z_LINK_KERNEL_SECTION(.k_threads)                                                    \
	Z_THREAD_INITIALIZER(name, stack_size,                                               \
						 (auto_start ? Z_THREAD_STATE_READY : Z_THREAD_STATE_STOPPED) |  \
							 prio_flag,                                                  \
						 symbol);                                                        \
	Z_STACK_SENTINEL_REGISTER(z_stack_buf_##name)

#define Z_THREAD_MINIMAL_DEFINE(name, entry, prio_flag, context_p, symbol, auto_start)   \
	__attribute__((used)) Z_STACK_MINIMAL_INITIALIZER(name, entry, context_p);           \
	Z_LINK_KERNEL_SECTION(.k_threads)                                                    \
	Z_THREAD_INITIALIZER(name, Z_CALLSAVED_CTX_SIZE,                                     \
						 (auto_start ? Z_THREAD_STATE_READY : Z_THREAD_STATE_STOPPED) |  \
							 prio_flag,                                                  \
						 symbol);                                                        \
	Z_STACK_SENTINEL_REGISTER(z_stack_buf_##name)

#else

#define Z_THREAD_DEFINE(name, entry, stack_size, prio_flag, context_p, symbol,           \
						auto_start)                                                      \
	__STATIC_ASSERT(0u, "Static thread (K_THREAD_DEFINE) creation is not "               \
						"supported");

#define Z_THREAD_MINIMAL_DEFINE(name, entry, prio_flag, context_p, symbol, auto_start)   \
	Z_THREAD_DEFINE(name, entry, Z_CALLSAVED_CTX_SIZE, prio_flag, context_p, symbol,     \
					auto_start)

#endif

/*___________________________________________________________________________*/

/* Note: Willingly not adding unsigned "u" suffix to numbers
 * because these flags are used in assembly code
 */

#define Z_THREAD_STATE_POS		   0
#define Z_THREAD_SCHED_LOCKED_POS  2
#define Z_THREAD_PRIO_POS		   3
#define Z_THREAD_TIMER_EXPIRED_POS 5
#define Z_THREAD_PEND_CANCELED_POS 6
#define Z_THREAD_WAKEUP_SCHED_POS  7

#define Z_THREAD_STATE_MSK		   (3 << Z_THREAD_STATE_POS)
#define Z_THREAD_SCHED_LOCKED_MSK  (1 << Z_THREAD_SCHED_LOCKED_POS)
#define Z_THREAD_PRIO_MSK		   (3 << Z_THREAD_PRIO_POS)
#define Z_THREAD_PRIO_COOP_MSK	   (2 << Z_THREAD_PRIO_POS)
#define Z_THREAD_PRIO_LEVEL_MSK	   (1 << Z_THREAD_PRIO_POS)
#define Z_THREAD_TIMER_EXPIRED_MSK (1 << Z_THREAD_TIMER_EXPIRED_POS)
#define Z_THREAD_PEND_CANCELED_MSK (1 << Z_THREAD_PEND_CANCELED_POS)
#define Z_THREAD_WAKEUP_SCHED_MSK  (1 << Z_THREAD_WAKEUP_SCHED_POS)

/* the thread is not running and is not in the runqueue,
 * it can be stopped/started with k_thread_stop/k_thread_start functions.
 */
#define Z_THREAD_STATE_STOPPED (0 << Z_THREAD_STATE_POS)

/* the thread is (yet/still) ready for execution and is the runqueue
 */
#define Z_THREAD_STATE_READY (1 << Z_THREAD_STATE_POS)

/* The thread is pending for an event, it may be in the time queue
 * (events_queue) but it is not in the runqueue. It can be wake up with
 * function z_wake_up()
 */
#define Z_THREAD_STATE_PENDING (2 << Z_THREAD_STATE_POS)

/* This flag is reserved for IDLE thread only (if enabled),
 * it is used to know whether the thread the IDLE thread or a normal thread
 */
#define Z_THREAD_STATE_IDLE (3 << Z_THREAD_STATE_POS)

/* Thread is cooperative and cannot be preempted by other threads */
#define Z_THREAD_PRIO_COOP (2 << Z_THREAD_PRIO_POS)

/* Thread is preemptible and can be preempted by other threads */
#define Z_THREAD_PRIO_PREEMPT (0 << Z_THREAD_PRIO_POS)

/* Thread priority level is high, it will be scheduled before
 * threads with low priority
 */
#define Z_THREAD_PRIO_HIGH (1 << Z_THREAD_PRIO_POS)

/* Thread priority level is low, it will be scheduled after
 * threads with high priority
 */
#define Z_THREAD_PRIO_LOW (0 << Z_THREAD_PRIO_POS)

/* Default thread priority level for cooperative threads */
#define K_COOPERATIVE (Z_THREAD_PRIO_COOP | Z_THREAD_PRIO_LOW)

/* Default thread priority level for preemptible threads */
#define K_PREEMPTIVE (Z_THREAD_PRIO_PREEMPT | Z_THREAD_PRIO_LOW)

/* Default thread priority level */
#define K_PRIO_DEFAULT (K_COOPERATIVE | Z_THREAD_PRIO_LOW)

/*___________________________________________________________________________*/

#endif