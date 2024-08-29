/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEBUG_H
#define _AVRTOS_DEBUG_H

/**
 * Debugging utilities
 */

#include <avr/pgmspace.h>

#include "kernel.h"
#include "misc/serial.h"
#include "mutex.h"
#include "semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Kernel Debug Macros
//

#define __Z_DBG_HELPER_TH(thread, chr)                                                   \
	serial_transmit(chr);                                                                \
	serial_transmit(thread->symbol)

#define __Z_DBG_HELPER_TH_R(thread, chr)                                                 \
	serial_transmit(thread->symbol);                                                     \
	serial_transmit(chr)

#if CONFIG_KERNEL_SCHEDULER_DEBUG

#define __Z_DBG_SCHED_LOCK(thread)		__Z_DBG_HELPER_TH(thread, '[')
#define __Z_DBG_SCHED_UNLOCK()			serial_transmit(']')
#define __Z_DBG_SCHED_EVENT(thread)		__Z_DBG_HELPER_TH(thread, '!')
#define __Z_DBG_SCHED_SUSPENDED(thread) serial_transmit('~')
#define __Z_DBG_SCHED_NEXT_THREAD()		serial_transmit('>')
#define __Z_DBG_SCHED_SKIP_IDLE()		serial_print_p(PSTR("p"))
#define __Z_DBG_SCHED_NEXT(thread)		serial_transmit(thread->symbol)
#define __Z_DBG_WAKEUP(thread)			__Z_DBG_HELPER_TH(thread, '@')

#define __Z_DBG_MUTEX_LOCKED(thread)   __Z_DBG_HELPER_TH(thread, '}')
#define __Z_DBG_MUTEX_UNLOCKED(thread) __Z_DBG_HELPER_TH_R(thread, '{')
#define __Z_DBG_MUTEX_WAIT(thread)	   __Z_DBG_HELPER_TH(thread, '#')

#define __Z_DBG_SEM_TAKE(thread) __Z_DBG_HELPER_TH(thread, ')')
#define __Z_DBG_SEM_GIVE(thread) __Z_DBG_HELPER_TH_R(thread, '(')
#define __Z_DBG_SEM_WAIT(thread) __Z_DBG_HELPER_TH(thread, '$')

#else

#define __Z_DBG_SCHED_LOCK(thread)
#define __Z_DBG_SCHED_UNLOCK()
#define __Z_DBG_SCHED_EVENT(thread)
#define __Z_DBG_SCHED_SUSPENDED(thread)
#define __Z_DBG_SCHED_NEXT_THREAD()
#define __Z_DBG_SCHED_SKIP_IDLE()
#define __Z_DBG_SCHED_NEXT(thread)
#define __Z_DBG_WAKEUP(thread)

#define __Z_DBG_MUTEX_LOCKED(thread)
#define __Z_DBG_MUTEX_UNLOCKED(thread)
#define __Z_DBG_MUTEX_WAIT(thread)

#define __Z_DBG_SEM_TAKE(thread)
#define __Z_DBG_SEM_GIVE(thread)
#define __Z_DBG_SEM_WAIT(thread)

#endif

/* Kernel GPIO Debug */

#if CONFIG_KERNEL_DEBUG_GPIO == 1
#define __Z_DBG_GPIO_PORT  GPIOA_DEVICE
#define __Z_DBG_GPIO_0_PIN 0u
#define __Z_DBG_GPIO_1_PIN 1u
#define __Z_DBG_GPIO_2_PIN 2u
#define __Z_DBG_GPIO_3_PIN 3u
#elif CONFIG_KERNEL_DEBUG_GPIO == 2
#define __Z_DBG_GPIO_PORT  GPIOA_DEVICE
#define __Z_DBG_GPIO_0_PIN 4u
#define __Z_DBG_GPIO_1_PIN 5u
#define __Z_DBG_GPIO_2_PIN 6u
#define __Z_DBG_GPIO_3_PIN 7u
#elif CONFIG_KERNEL_DEBUG_GPIO == 3
#define __Z_DBG_GPIO_PORT  GPIOB_DEVICE
#define __Z_DBG_GPIO_0_PIN 0u
#define __Z_DBG_GPIO_1_PIN 1u
#define __Z_DBG_GPIO_2_PIN 2u
#define __Z_DBG_GPIO_3_PIN 3u
#elif CONFIG_KERNEL_DEBUG_GPIO == 4
#define __Z_DBG_GPIO_PORT  GPIOB_DEVICE
#define __Z_DBG_GPIO_0_PIN 4u
#define __Z_DBG_GPIO_1_PIN 5u
#define __Z_DBG_GPIO_2_PIN 6u
#define __Z_DBG_GPIO_3_PIN 7u
#elif CONFIG_KERNEL_DEBUG_GPIO == 5
#define __Z_DBG_GPIO_PORT  GPIOC_DEVICE
#define __Z_DBG_GPIO_0_PIN 0u
#define __Z_DBG_GPIO_1_PIN 1u
#define __Z_DBG_GPIO_2_PIN 2u
#define __Z_DBG_GPIO_3_PIN 3u
#elif CONFIG_KERNEL_DEBUG_GPIO == 6
#define __Z_DBG_GPIO_PORT  GPIOC_DEVICE
#define __Z_DBG_GPIO_0_PIN 4u
#define __Z_DBG_GPIO_1_PIN 5u
#define __Z_DBG_GPIO_2_PIN 6u
#define __Z_DBG_GPIO_3_PIN 7u
#elif CONFIG_KERNEL_DEBUG_GPIO == 7
#define __Z_DBG_GPIO_PORT  GPIOD_DEVICE
#define __Z_DBG_GPIO_0_PIN 0u
#define __Z_DBG_GPIO_1_PIN 1u
#define __Z_DBG_GPIO_2_PIN 2u
#define __Z_DBG_GPIO_3_PIN 3u
#elif CONFIG_KERNEL_DEBUG_GPIO == 8
#define __Z_DBG_GPIO_PORT  GPIOD_DEVICE
#define __Z_DBG_GPIO_0_PIN 4u
#define __Z_DBG_GPIO_1_PIN 5u
#define __Z_DBG_GPIO_2_PIN 6u
#define __Z_DBG_GPIO_3_PIN 7u
#elif CONFIG_KERNEL_DEBUG_GPIO >= 9
#error "Invalid CONFIG_KERNEL_DEBUG_GPIO_PINS_RANGE value"
#endif

#if CONFIG_KERNEL_DEBUG_GPIO > 0
#include "drivers/gpio.h"
#define __Z_DBG_GPIO_INIT(_port, _pin)                                                   \
	gpiol_pin_init(_port, _pin, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_HIGH)
#define __Z_DBG_GPIO_SET(_port, _pin)	 gpiol_pin_write_state(_port, _pin, 1u)
#define __Z_DBG_GPIO_CLEAR(_port, _pin)	 gpiol_pin_write_state(_port, _pin, 0u)
#define __Z_DBG_GPIO_TOGGLE(_port, _pin) gpiol_pin_toggle(_port, _pin)
#else
#define __Z_DBG_GPIO_INIT(_port, _pin)
#define __Z_DBG_GPIO_SET(_port, _pin)
#define __Z_DBG_GPIO_CLEAR(_port, _pin)
#define __Z_DBG_GPIO_TOGGLE(_port, _pin)
#endif

#define __Z_DBG_GPIO_0_INIT() __Z_DBG_GPIO_INIT(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_0_PIN)
#define __Z_DBG_GPIO_1_INIT() __Z_DBG_GPIO_INIT(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_1_PIN)
#define __Z_DBG_GPIO_2_INIT() __Z_DBG_GPIO_INIT(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_2_PIN)
#define __Z_DBG_GPIO_3_INIT() __Z_DBG_GPIO_INIT(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_3_PIN)

#define __Z_DBG_GPIO_0_TOGGLE() __Z_DBG_GPIO_TOGGLE(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_0_PIN)
#define __Z_DBG_GPIO_1_TOGGLE() __Z_DBG_GPIO_TOGGLE(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_1_PIN)
#define __Z_DBG_GPIO_2_TOGGLE() __Z_DBG_GPIO_TOGGLE(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_2_PIN)
#define __Z_DBG_GPIO_3_TOGGLE() __Z_DBG_GPIO_TOGGLE(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_3_PIN)

#define __Z_DBG_GPIO_0_SET() __Z_DBG_GPIO_SET(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_0_PIN)
#define __Z_DBG_GPIO_1_SET() __Z_DBG_GPIO_SET(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_1_PIN)
#define __Z_DBG_GPIO_2_SET() __Z_DBG_GPIO_SET(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_2_PIN)
#define __Z_DBG_GPIO_3_SET() __Z_DBG_GPIO_SET(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_3_PIN)

#define __Z_DBG_GPIO_0_CLEAR() __Z_DBG_GPIO_CLEAR(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_0_PIN)
#define __Z_DBG_GPIO_1_CLEAR() __Z_DBG_GPIO_CLEAR(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_1_PIN)
#define __Z_DBG_GPIO_2_CLEAR() __Z_DBG_GPIO_CLEAR(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_2_PIN)
#define __Z_DBG_GPIO_3_CLEAR() __Z_DBG_GPIO_CLEAR(__Z_DBG_GPIO_PORT, __Z_DBG_GPIO_3_PIN)

#if CONFIG_KERNEL_DEBUG_GPIO_SYSTICK == 0
#define __Z_DBG_SYSTICK_ENTER()
#define __Z_DBG_SYSTICK_EXIT()
#elif CONFIG_KERNEL_DEBUG_GPIO_SYSTICK == 1
#define __Z_DBG_SYSTICK_ENTER() __Z_DBG_GPIO_3_TOGGLE()
#define __Z_DBG_SYSTICK_EXIT()
#elif CONFIG_KERNEL_DEBUG_GPIO_SYSTICK == 2
#define __Z_DBG_SYSTICK_ENTER() __Z_DBG_GPIO_3_SET()
#define __Z_DBG_SYSTICK_EXIT()	__Z_DBG_GPIO_3_CLEAR()
#else
#error "Invalid CONFIG_KERNEL_DEBUG_GPIO_SYSTICK value"
#endif

/**
 * @brief Current thread stack usage
 *
 * Getting the stack usage of the main thread if
 * CONFIG_THREAD_EXPLICIT_MAIN_STACK is not set will return 0 by default :
 *
 * @param thread
 * @return uint16_t
 */
uint16_t k_thread_usage(struct k_thread *thread);

/**
 * @brief Print the current number of threads
 */
void k_thread_dbg_count(void);

/**
 * @brief Dump the thread structure content as hexadecimal
 *
 * @param thread
 */
void k_thread_dump_hex(struct k_thread *thread);

/**
 * @brief Pretty print the thread structure content :
 *
 * [priority] : stack usage/stack size | stack end address
 *
 * e.g:
 * M 0614 [PREE 0] RUNNING : SP 0/512 -| END @0856
 *
 * @param thread
 */
void k_thread_dump(struct k_thread *thread);

/**
 * @brief Pretty print all threads contains
 *
 * M 0614 [PREE 0] RUNNING : SP 0/512 -| END @0856
 * R 0624 [PREE 1] READY   : SP 35/512 -| END @0307
 * W 0634 [PREE 1] READY   : SP 35/512 -| END @0507
 * K 0644 [PREE 3] READY   : SP 35/62 -| END @0545
 */
void k_thread_dump_all(void);

/**
 * @brief Return the "return address" (stored in stack) of a suspended thread
 *
 * @warning NOT TESTED
 *
 * @param thread
 * @return void*
 */
void *z_thread_get_return_addr(struct k_thread *thread);

/**
 * @brief Copy in buffer the 32 registers (stored in stack) of a suspended
 * thread
 *
 * buffer[0] > buffer[5] : r31 > r26
 * buffer[6], buffer[7] = r24, r25
 * buffer[8] > buffer[31] : r23 > r0
 *
 * @see k_yield, z_thread_stack_create, Z_STACK_INITIALIZER
 *
 * @warning r24, r25 are not pushed in the same way as other registers
 *
 * @warning NOT TESTED
 *
 * @param thread
 * @param buffer
 * @param size
 * @return int8_t
 */
// int8_t k_thread_copy_registers(struct k_thread *thread, uint8_t * buffer, const
// size_t size);

/**
 * @brief Return the stack pointer value (after CALL)
 *
 */
uint16_t z_read_sp(void);

/**
 * @brief Return the return addr value of itself (z_read_ra), helps to determine
 * current running thread
 *
 * Depends on architecture (PC can be 2 or 3 bytes)
 */
#if defined(__AVR_3_BYTE_PC__)
uint32_t z_read_ra(void);
#else
uint16_t z_read_ra(void);
#endif

/**
 * @brief Set the stack pointer object
 *
 * @param thread
 */
void z_set_stack_pointer(struct k_thread *thread);

/**
 * @brief Read SREG register
 *
 * SREG: Status Register
 * C: Carry Flag
 * Z: Zero Flag
 * N: Negative Flag
 * V: Two's complement overflow indicator
 * S: N, V, For signed tests
 * H: Half Carry Flag
 * T: Transfer bit used by BLD and BST instructions
 * I: Global Interrupt Enable/Disable Flag
 *
 * @return uint8_t
 */
uint8_t z_read_sreg(void);

#define Z_LOG_SREG()                                                                     \
	serial_u8(SREG);                                                                     \
	serial_transmit(';');
#define Z_LOG_SREG2()                                                                    \
	serial_u8(z_read_sreg());                                                            \
	serial_transmit(';');
#define Z_LOG_SREG_I()                                                                   \
	serial_transmit('[');                                                                \
	serial_u8(SREG >> 7);                                                                \
	serial_transmit(']');

void z_thread_symbol_runqueue(struct dnode *item);

void z_thread_symbol_events_queue(struct titem *item);

void z_print_events_queue(void);

void z_print_runqueue(void);

//
// Debug mutex
//

void z_mutex_debug(struct k_mutex *mutex);

//
// Debug mutex
//

void z_sem_debug(struct k_sem *sem);

#ifdef __cplusplus
}
#endif

#endif
