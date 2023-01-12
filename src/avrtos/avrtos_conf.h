/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEFAULT_CONFIG_H
#define _AVRTOS_DEFAULT_CONFIG_H

#include "common.h"

#include <avr/io.h>

//
// This file contains all the default configuration options of the kernel
//
// You may need to clear the build directory if you modify this configuration
// file
//
// Few constants defined here may be used in a asm file, avoid the use of the
// "unsigned" flag ("u") at the end of numbers
// e.g. prefer : CONFIG_THREAD_MAIN_STACK_SIZE 0x100
// to CONFIG_THREAD_MAIN_STACK_SIZE 0x100u
//

//
// Define the main thread type (coop/prempt) and priority
//
// 0: Main thread is preemptive
// 1: Main thread is cooperative
//
#ifndef CONFIG_THREAD_MAIN_COOPERATIVE
#define CONFIG_THREAD_MAIN_COOPERATIVE 0
#endif

//
// Interrupt policy on main thread startup
//
// 0: Interrupts are disabled
// 1: Interrupts are enabled
// 2: Interrupts are enabled but scheduler is locked
//
#ifndef CONFIG_INTERRUPT_POLICY
#define CONFIG_INTERRUPT_POLICY 0
#endif

//
// Tells whether the main stack location is at RAMEND (0) or stack is allocated
// in a dedicated buffer (1).
//
// 0: Main stack is allocated at RAMEND (undetermined size)
// 1: Main stack is allocated in a dedicated buffer (size known at compile time)
//
// IMPORTANT NOTE: If you're using the heap, you must set this to 0 as stdlib
// malloc use the stack pointer to make checks on the remaining heap size.
// i.e. : With explicit main stack, a buffer is allocated (for main thread stack), 
// before the heap. stdlib malloc() will then constantly fails while stack pointer
// is beyond the limit (actually on the other side of the heap).
// The checker, expect main stack to be near to RAMEND, in upper memory regions.
//
#ifndef CONFIG_THREAD_EXPLICIT_MAIN_STACK
#define CONFIG_THREAD_EXPLICIT_MAIN_STACK 0
#endif

//
// This configuration option defines the size of the main stack.
// If canaries are enabled, the total stack will be filled with the canary
// value.
//
// If CONFIG_THREAD_EXPLICIT_MAIN_STACK is enabled, the main stack is  allocated
// in a dedicated buffer in the .data section.
//
// If CONFIG_THREAD_EXPLICIT_MAIN_STACK=1 make sure main stack size does not
// exceed the remaining RAM size.
//
// If heap grows down, canaries might be invalidated if expected stack size is
// too big.
//
#ifndef CONFIG_THREAD_MAIN_STACK_SIZE
#define CONFIG_THREAD_MAIN_STACK_SIZE 0x200
#endif

//
// Tells whether malloc is expected to be used in main thread or not
// This option is a guard to make sure you set
// DEFAULT_THREAD_EXPLICIT_MAIN_STACK=1 while using malloc in main thread
//
// 0: malloc is not used in main thread
// 1: malloc is used in main thread
//
#ifndef CONFIG_USE_STDLIB_HEAP_MALLOC_MAIN
#define CONFIG_USE_STDLIB_HEAP_MALLOC_MAIN 0
#endif

//
// Tells whether malloc is expected to be used in other threads or not
//
// 0: malloc is not used in other threads
// 1: malloc is used in other threads
//
#ifndef CONFIG_USE_STDLIB_HEAP_MALLOC_THREAD
#define CONFIG_USE_STDLIB_HEAP_MALLOC_THREAD 0
#endif

//
// Default SREG value for other thread on stack creation.
// Main thread default SREG is always 0
//
// From datasheet
//  Bit 7 6 5 4 3 2 1 0
//  0x3F (0x5F) I T H S V N Z C SREG
//  Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
//  Initial Value 0 0 0 0 0 0 0 0
//
// Is it greatly recommended to keep interrupts enabled when starting other
// threads, otherwise interrupts should be re-enabled in the thread code if time
// critical operations are performed.
//
// 0: Interrupts disabled when threads starts (other than main thread)
// (1 << SREG_I): Interrupts enabled when threads starts (other than main
// thread)
//
#ifndef CONFIG_THREAD_DEFAULT_SREG
#define CONFIG_THREAD_DEFAULT_SREG (1 << SREG_I)
#endif

//
// Allow, or not thread termination
// Disabling the option (0) saves up to 2/3 bytes of stack per thread.
// (depending on the architecture)
//
// 0: Thread termination is not permitted (hardfault)
// 1: Thread termination is enabled
// -1: Thread termination is not permitted (kernel panic)
//
#ifndef CONFIG_KERNEL_THREAD_TERMINATION_TYPE
#define CONFIG_KERNEL_THREAD_TERMINATION_TYPE 0
#endif

//
// Enable support for thread priorities using a multiple queues mechanism
//
// 0: Thread priorities are not supported
// 1: Thread priorities are supported
//
#ifndef CONFIG_TREAD_PRIO_MULTIQ
#define CONFIG_TREAD_PRIO_MULTIQ 0
#endif

//
// Compare threads addresses before thread switch to avoid unnecessary switch
// to the same context.
//
// Note: inefficient if at least two threads are always ready)
//
// 0: Do not compare threads addresses before switch
// 1: Compare threads addresses before switch
//
#ifndef CONFIG_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH
#define CONFIG_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH 1
#endif

//
// Enable cooperative threads. This feature allows threads to not be preempted
// by the scheduler when they are ready.
//
// 0: Cooperative threads are disabled
// 1: Cooperative threads are enabled
//
#ifndef CONFIG_KERNEL_COOPERATIVE_THREADS
#define CONFIG_KERNEL_COOPERATIVE_THREADS 1
#endif

//
// Sysclock period when precision mode is disabled ("llu" suffix is important)
//
// Reducing this value can cause some precision loss
//
// Range depends on the hardware timer resolution  and prescaler
//
// Usually 1ms is a good value
//
#ifndef CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#define CONFIG_KERNEL_SYSCLOCK_PERIOD_US 1000llu
#endif

//
// Time slice in milliseconds (0 if using SYSCLOCK period)
//
// Interval between two context switches of preemptive threads
//
// Range depends on CONFIG_KERNEL_SYSCLOCK_PERIOD_US.
// Usually using CONFIG_KERNEL_SYSCLOCK_PERIOD_US value is a good choice
//
#ifndef CONFIG_KERNEL_TIME_SLICE_US
#define CONFIG_KERNEL_TIME_SLICE_US 1000llu
#endif

//
// Select Hardware timer used for kernel sysclock
//
// ATmega328P/...
// 0: Timer 0 (8 bits)
// 1: Timer 1 (16 bits)
// 2: Timer 2 (8 bits)
//
// ATmega2560/ATmega328PB/...
// 3: Timer 3 (16 bits)
// 4: Timer 4 (16 bits)
// 5: Timer 5 (16 bits)
//
#ifndef CONFIG_KERNEL_SYSLOCK_HW_TIMER
#define CONFIG_KERNEL_SYSLOCK_HW_TIMER 1
#endif

//
// Use 40 bits for ticks counter size (instead of 32 bits)
//
// Using 40 bits counter size allows to have a longer period before overflow
// e.g. with 1ms sysclock period, 40 bits counter allows to have a period of
// 34 years before overflow instead of 49 days with 32 bits counter.
//
// 0: 32 bits counter size
// 1: 40 bits counter size
//
#ifndef CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
#define CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS 1
#endif

//
// Use 32 bits for delay objects (k_timeout_t / k_ticks_t / k_delta_t)
//
// Disabling this object can save some flash and RAM space.
// - Adding two U32 objects instead of U16 needs twice as much code.
// - There are many wait function
// - IMPORTANT : Increasing the sysclock period can help to reduce the
//   maximum number we need to store for a delay.
//
// This configuration is unrelated to time functions, it only applies when
//  giving a timeout to a kernel function (as k_sleep, k_mutex_lock, ...)
//
// 0: Delay objects are 16 bits
// 1: Delay objects are 32 bits
//
#ifndef CONFIG_KERNEL_DELAY_OBJECT_U32
#define CONFIG_KERNEL_DELAY_OBJECT_U32 0
#endif

//
// Kernel auto initialisation.
//
// This option might not work if the avrtos is linked as a library in some
// environments (e.g. PlatformIO). In this case you should either call
// kernel_init() manually or use the z_avrtos_init() function manually.
// Or link auto initialization function with K_KERNEL_LINK_AVRTOS_INIT() macro.
//
// 0: Kernel auto initialisation is disabled
// 1: Kernel auto initialisation is enabled
//
#ifndef CONFIG_KERNEL_AUTO_INIT
#define CONFIG_KERNEL_AUTO_INIT 1
#endif

//
// Tells whether the kernel should define a idle thread to enable other threads
// to sleep. If disabled, at least one thread must be always ready, otherwise
// a fault will be triggered.
//
// 0: Kernel idle thread is disabled
// 1: Kernel idle thread is enabled
//
#ifndef CONFIG_KERNEL_THREAD_IDLE
#define CONFIG_KERNEL_THREAD_IDLE 1
#endif

//
// Kernel thread idle addtionnal stack (for interrupt handles stack)
// In some cases, the idle thread stack is not enough to execute heavy interrupt
// handlers. This option allows to add some additional stack to the idle thread.
//
// Minimum advised value is 0x50
//
#ifndef CONFIG_KERNEL_THREAD_IDLE_ADD_STACK
#define CONFIG_KERNEL_THREAD_IDLE_ADD_STACK 0x50
#endif

//
// Tells whether IDLE thread is preemptive or cooperative
// Note: If preemptive, additonal stack is allocated for thread
//
// 0: Kernel idle thread is preemptive
// 1: Kernel idle thread is cooperative
//
#ifndef CONFIG_THREAD_IDLE_COOPERATIVE
#define CONFIG_THREAD_IDLE_COOPERATIVE 0
#endif

//
// Enable thread canaries. Threads stacks are filled with a canary symbol
// to detect stack overflows.
//
// There is only little overhead when initializing the stacks. Always good to
// enable to high stack usage.
//
// 0: Thread canaries are disabled
// 1: Thread canaries are enabled
//
#ifndef CONFIG_THREAD_CANARIES
#define CONFIG_THREAD_CANARIES 0
#endif

//
// Define thread canaries symbol
//
// 0xAA or 0x55 are good values
//
#ifndef CONFIG_THREAD_CANARIES_SYMBOL
#define CONFIG_THREAD_CANARIES_SYMBOL 0xAA
#endif

//
// Tells whether a sentinel should be added at the end of the thread stack.
// When corrupted, the sentinel will help to detect stack overflows.
//
// 0: Thread sentinel is disabled
// 1: Thread sentinel is enabled
//
#ifndef CONFIG_THREAD_STACK_SENTINEL
#define CONFIG_THREAD_STACK_SENTINEL 0
#endif

//
// Define thread sentinel size
//
// 1 is a good value
//
#ifndef CONFIG_THREAD_STACK_SENTINEL_SIZE
#define CONFIG_THREAD_STACK_SENTINEL_SIZE 1
#endif

//
// Define thread sentinel symbol
//
// 0xAA or 0x55 are good values
//
#ifndef CONFIG_THREAD_STACK_SENTINEL_SYMBOL
#define CONFIG_THREAD_STACK_SENTINEL_SYMBOL 0xAA
#endif

//
// Enable system workqueue
//
// 0: System workqueue is disabled
// 1: System workqueue is enabled
//
#ifndef CONFIG_SYSTEM_WORKQUEUE_ENABLE
#define CONFIG_SYSTEM_WORKQUEUE_ENABLE 0
#endif

//
// Define system workqueue stack size
// Workqueue stack size depends on the maximum stack required by the work
// handlers when executed.
//
#ifndef CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE
#define CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE 0x200
#endif

//
// Tells whether the system workqueue should be executed cooperatively
//
// 0: System workqueue is preemptive
// 1: System workqueue is cooperative
//
#ifndef CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE
#define CONFIG_SYSTEM_WORKQUEUE_COOPERATIVE 0
#endif

//
// Enable kernel assertion test for debug purpose
//
// 0: Kernel assertion is disabled
// 1: Kernel assertion is enabled
//
#ifndef CONFIG_KERNEL_ASSERT
#define CONFIG_KERNEL_ASSERT 0
#endif

//
// Enable kernel arguments checks
//
// 0: Kernel arguments checks are disabled
// 1: Kernel arguments checks are enabled
//
#ifndef CONFIG_KERNEL_ARGS_CHECKS
#define CONFIG_KERNEL_ARGS_CHECKS 0
#endif

//
// Enable timers support
// - This feature needs additionnal stack for threads
//
// 0: Kernel timers are disabled
// 1: Kernel timers are enabled
//
#ifndef CONFIG_KERNEL_TIMERS
#define CONFIG_KERNEL_TIMERS 0
#endif

//
// Enable events support
// - This feature needs additionnal stack for threads
//
// 0: Kernel events are disabled
// 1: Kernel events are enabled
//
#ifndef CONFIG_KERNEL_EVENTS
#define CONFIG_KERNEL_EVENTS 0
#endif

//
// Allow to schedule an events with K_NO_WAIT
// This will cause the event callback to be executed immediately
//
// 0: Require a non-zero timeout to schedule an event
// 1: Allow to schedule an event with K_NO_WAIT
//
#ifndef CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT
#define CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT 1
#endif

//
// Enabled thread errno
//
// 0: Thread errno is disabled
// 1: Thread errno is enabled
//
#ifndef CONFIG_THREAD_ERRNO
#define CONFIG_THREAD_ERRNO 0
#endif

//
// Enable scheduler lock counter for each thread to allow
// k_sched_lock() to be called recursively.
//
// Note: This option leads to performance overhead when locking/unlocking
// the scheduler, enable it only if needed.
//
// 0: Scheduler lock counter is disabled
// 1: Scheduler lock counter is enabled
//
#ifndef CONFIG_KERNEL_SCHED_LOCK_COUNTER
#define CONFIG_KERNEL_SCHED_LOCK_COUNTER 0
#endif

//
// Enable IRQ lock counter for each thread to allow
// irq_enable() to be called recursively.
//
// If enabled, calling irq_disable() doesn't garantee that interrupts
// are actually enabled again.
//
// Note: This option leads to performance overhead when enabling/disabling irqs,
// enable it only if needed.
//
// 0: IRQ lock counter is disabled
// 1: IRQ lock counter is enabled
//
#ifndef CONFIG_KERNEL_IRQ_LOCK_COUNTER
#define CONFIG_KERNEL_IRQ_LOCK_COUNTER 0
#endif

//
// Tells to which USART printf function output should be redirected.
// Disable the option by setting to -1.
//
// -1: printf is disabled
// 0: printf is redirected to USART0
// 1: printf is redirected to USART1 if exists
// 2: printf is redirected to USART2 if exists
// 3: printf is redirected to USART3 if exists
// 4: printf is redirected to USART4 if exists
// 5: printf is redirected to USART5 if exists
// 6: printf is redirected to USART6 if exists
//
#ifndef CONFIG_STDIO_PRINTF_TO_USART
#define CONFIG_STDIO_PRINTF_TO_USART -1
#endif

//
// Enable logging subsystem
//
// 0: Logging subsystem is disabled
// 1: Logging subsystem is enabled
//
#ifndef CONFIG_LOGGING_SUBSYSTEM
#define CONFIG_LOGGING_SUBSYSTEM 1
#endif

//
// Enable uptime counter
//
// 0: Uptime counter is disabled
// 1: Uptime counter is enabled
//
#ifndef CONFIG_KERNEL_UPTIME
#define CONFIG_KERNEL_UPTIME 0
#endif

//
// Enable atomic API
//
// 0: Atomic API is disabled
// 1: Atomic API is enabled
//
#ifndef CONFIG_KERNEL_ATOMIC_API
#define CONFIG_KERNEL_ATOMIC_API 0
#endif

//
// Use UART0 RX interrupt as preemptive signal
// Note: Reserved for debug purpose
//
// 0: UART0 RX interrupt is not used as preemptive signal
// 1: UART0 RX interrupt is used as preemptive signal
//
#ifndef CONFIG_KERNEL_DEBUG_PREEMPT_UART
#define CONFIG_KERNEL_DEBUG_PREEMPT_UART 0
#endif

//
//  Enable Kernel debug for function, that set some of them noinline
//
// 0: Kernel function inlining is disabled
// 1: Kernel function inlining is enabled
//
#ifndef CONFIG_KERNEL_API_NOINLINE
#define CONFIG_KERNEL_API_NOINLINE 0
#endif

//
//  Enable Kernel Debug features
//
// 0: Kernel Debug is disabled
// 1: Kernel Debug is enabled
//
#ifndef CONFIG_KERNEL_DEBUG
#define CONFIG_KERNEL_DEBUG 0
#endif

//
// Enable Kernel Debug in scheduler
//
// 0: Kernel Debug in scheduler is disabled
// 1: Kernel Debug in scheduler is enabled
//
#ifndef CONFIG_KERNEL_SCHEDULER_DEBUG
#define CONFIG_KERNEL_SCHEDULER_DEBUG 0
#endif

//
// Maximum number of file descriptors
//
// 0: No file descriptor
// n: Maximum number of file descriptors
//
#ifndef CONFIG_FD_MAX_COUNT
#define CONFIG_FD_MAX_COUNT 0
#endif

//
// Enable asynchronous support for USART0
//
// 0: USART0 asynchronous support is disabled
// 1: USART0 asynchronous support is enabled
//
#ifndef CONFIG_DRIVERS_USART0_ASYNC
#define CONFIG_DRIVERS_USART0_ASYNC 0
#endif

//
// Enable asynchronous support for USART1
//
// 0: USART1 asynchronous support is disabled
// 1: USART1 asynchronous support is enabled
//
#ifndef CONFIG_DRIVERS_USART1_ASYNC
#define CONFIG_DRIVERS_USART1_ASYNC 0
#endif

//
// Enable asynchronous support for USART2
//
// 0: USART2 asynchronous support is disabled
// 1: USART2 asynchronous support is enabled
//
#ifndef CONFIG_DRIVERS_USART2_ASYNC
#define CONFIG_DRIVERS_USART2_ASYNC 0
#endif

//
// Enable asynchronous support for USART3
//
// 0: USART3 asynchronous support is disabled
// 1: USART3 asynchronous support is enabled
//
#ifndef CONFIG_DRIVERS_USART3_ASYNC
#define CONFIG_DRIVERS_USART3_ASYNC 0
#endif

//
// Enable high level support for timer0
//
// 0: High level support for timer0 is disabled
// 1: High level support for timer0 is enabled
//
#ifndef CONFIG_DRIVERS_TIMER0_API
#define CONFIG_DRIVERS_TIMER0_API 0
#endif

//
// Enable high level support for timer1
//
// 0: High level support for timer1 is disabled
// 1: High level support for timer1 is enabled
//
#ifndef CONFIG_DRIVERS_TIMER1_API
#define CONFIG_DRIVERS_TIMER1_API 0
#endif

//
// Enable high level support for timer2
//
// 0: High level support for timer2 is disabled
// 1: High level support for timer2 is enabled
//
#ifndef CONFIG_DRIVERS_TIMER2_API
#define CONFIG_DRIVERS_TIMER2_API 0
#endif

//
// Enable high level support for timer3
//
// 0: High level support for timer3 is disabled
// 1: High level support for timer3 is enabled
//
#ifndef CONFIG_DRIVERS_TIMER3_API
#define CONFIG_DRIVERS_TIMER3_API 0
#endif

//
// Enable high level support for timer4
//
// 0: High level support for timer4 is disabled
// 1: High level support for timer4 is enabled
//
#ifndef CONFIG_DRIVERS_TIMER4_API
#define CONFIG_DRIVERS_TIMER4_API 0
#endif

//
// Enable high level support for timer5
//
// 0: High level support for timer5 is disabled
// 1: High level support for timer5 is enabled
//
#ifndef CONFIG_DRIVERS_TIMER5_API
#define CONFIG_DRIVERS_TIMER5_API 0
#endif

//
// Debug systick using given GPIO pin of PORTB
//
// 0: Systick debug is disabled
// n: Systick debug is enabled on given GPIO pin of PORTB
//
#ifndef CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
#define CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG 0
#endif

#endif