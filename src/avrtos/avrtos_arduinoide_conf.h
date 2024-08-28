/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ARDUINO_IDE_DEFAULT_CONFIG_H
#define _AVRTOS_ARDUINO_IDE_DEFAULT_CONFIG_H

/*
 * Adapter configuration in an arduino environment
 * This file is included by avrtos_conf.h
 */

#if !defined(ARDUINO_ARCH_AVR)
#error "This library only supports 8bits AVR-based MCU"
#endif

/* If using Arduino IDE, it's not possible to use a custom linker script
 * so force option to 0 */
#ifndef CONFIG_AVRTOS_LINKER_SCRIPT
#define CONFIG_AVRTOS_LINKER_SCRIPT 0
#endif

#ifndef CONFIG_KERNEL_AUTO_INIT
#define CONFIG_KERNEL_AUTO_INIT 1
#endif

/* Features */
#ifndef CONFIG_KERNEL_TIMERS
#define CONFIG_KERNEL_TIMERS 1
#endif

#ifndef CONFIG_KERNEL_EVENTS
#define CONFIG_KERNEL_EVENTS 1
#endif

#ifndef CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT
#define CONFIG_KERNEL_EVENTS_ALLOW_NO_WAIT 1
#endif

#ifndef CONFIG_KERNEL_ATOMIC_API
#define CONFIG_KERNEL_ATOMIC_API 1
#endif

/* Enable ticks counter to calculate uptime */
#ifndef CONFIG_KERNEL_UPTIME
#define CONFIG_KERNEL_UPTIME 1
#endif

/* Disable because requires a dedicated thread
 * user can still spawn workqueues manually
 */
#ifndef CONFIG_SYSTEM_WORKQUEUE_ENABLE
#define CONFIG_SYSTEM_WORKQUEUE_ENABLE 0
#endif

/* Allow to use printf() which prints to serial (UART0) */
#ifndef CONFIG_STDIO_PRINTF_TO_USART
#define CONFIG_STDIO_PRINTF_TO_USART 0
#endif

#ifndef CONFIG_THREAD_MAIN_STACK_SIZE
#define CONFIG_THREAD_MAIN_STACK_SIZE 512
#endif

/* Many Arduino libraries use malloc() on init */
#ifndef CONFIG_USE_STDLIB_HEAP_MALLOC_MAIN
#define CONFIG_USE_STDLIB_HEAP_MALLOC_MAIN 1
#endif

/* Allow threads to terminates in order to not confuse the user if they stops */
#ifndef CONFIG_KERNEL_THREAD_TERMINATION_TYPE
#define CONFIG_KERNEL_THREAD_TERMINATION_TYPE 1
#endif

/* Use timer 2 for kernel syslock
 * timer0 is used by Arduino framework for millis() and delay()
 */
#ifndef CONFIG_KERNEL_SYSLOCK_HW_TIMER
#define CONFIG_KERNEL_SYSLOCK_HW_TIMER 2
#endif

/* Allow timeout/delays bigger than 65535ms */
#ifndef CONFIG_KERNEL_DELAY_OBJECT_U32
#define CONFIG_KERNEL_DELAY_OBJECT_U32 1
#endif

/* Spawn IDLE thread */
#ifndef CONFIG_KERNEL_THREAD_IDLE
#define CONFIG_KERNEL_THREAD_IDLE 1
#endif

/* Add additional stack in case interrupts handlers need a lot of stack */
#ifndef CONFIG_KERNEL_THREAD_IDLE_ADD_STACK
#define CONFIG_KERNEL_THREAD_IDLE_ADD_STACK 96
#endif

/* Enable canaries by default, easier to benchmark stack usage */
#ifndef CONFIG_THREAD_CANARIES
#define CONFIG_THREAD_CANARIES 1
#endif

/* Disable kernel checks/asserts */
#ifndef CONFIG_THREAD_STACK_SENTINEL
#define CONFIG_THREAD_STACK_SENTINEL 0
#endif

#ifndef CONFIG_KERNEL_ASSERT
#define CONFIG_KERNEL_ASSERT 0
#endif

#ifndef CONFIG_KERNEL_ARGS_CHECKS
#define CONFIG_KERNEL_ARGS_CHECKS 0
#endif

/* Auto init serial */
#ifndef CONFIG_SERIAL_AUTO_INIT
#define CONFIG_SERIAL_AUTO_INIT 1
#endif

/* Default serial baudrate */
#ifndef CONFIG_SERIAL_USART_BAUDRATE
#define CONFIG_SERIAL_USART_BAUDRATE 9600lu
#endif

/* Keep loop() as cooperative thread,
 * as k_yield() is invoked between each loop iteration
 */
#ifndef CONFIG_THREAD_MAIN_COOPERATIVE
#define CONFIG_THREAD_MAIN_COOPERATIVE 1
#endif

/* 1ms sysclock period */
#ifndef CONFIG_KERNEL_SYSCLOCK_PERIOD_US
#define CONFIG_KERNEL_SYSCLOCK_PERIOD_US 1000llu
#endif

/* 1ms time slice */
#ifndef CONFIG_KERNEL_TIME_SLICE_US
#define CONFIG_KERNEL_TIME_SLICE_US 1000llu
#endif

#endif /* _AVRTOS_ARDUINO_IDE_DEFAULT_CONFIG_H */