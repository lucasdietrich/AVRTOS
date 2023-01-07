/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEBUG_H
#define _AVRTOS_DEBUG_H

#include "misc/serial.h"

#include "avrtos.h"

#ifdef __cplusplus
extern "C" {
#endif


/*___________________________________________________________________________*/

//
// Kernel Debug Macros
//

#define __K_DBG_HELPER_TH(th, chr) \
    serial_transmit(chr);           \
    serial_transmit(th->symbol)

#define __K_DBG_HELPER_TH_R(th, chr) \
    serial_transmit(th->symbol);      \
    serial_transmit(chr)

#if CONFIG_KERNEL_SCHEDULER_DEBUG

#define __K_DBG_SCHED_LOCK(th)      __K_DBG_HELPER_TH(th, '[')
#define __K_DBG_SCHED_UNLOCK()      serial_transmit(']')
#define __K_DBG_SCHED_EVENT(th)     __K_DBG_HELPER_TH(th, '!')
#define __K_DBG_SCHED_SUSPENDED(th)     serial_transmit('~')
#define __K_DBG_SCHED_NEXT_THREAD()     serial_transmit('>')
#define __K_DBG_SCHED_SKIP_IDLE()   serial_print_p(PSTR("p"))
#define __K_DBG_SCHED_NEXT(th)      serial_transmit(th->symbol)
#define __K_DBG_WAKEUP(th)          __K_DBG_HELPER_TH(th, '@')

#define __K_DBG_MUTEX_LOCKED(th)    __K_DBG_HELPER_TH(th, '}')
#define __K_DBG_MUTEX_UNLOCKED(th)  __K_DBG_HELPER_TH_R(th, '{')
#define __K_DBG_MUTEX_WAIT(th)      __K_DBG_HELPER_TH(th, '#')

#define __K_DBG_SEM_TAKE(th)        __K_DBG_HELPER_TH(th, ')')
#define __K_DBG_SEM_GIVE(th)        __K_DBG_HELPER_TH_R(th, '(')
#define __K_DBG_SEM_WAIT(th)        __K_DBG_HELPER_TH(th, '$')

#else

#define __K_DBG_SCHED_LOCK(th)
#define __K_DBG_SCHED_UNLOCK()
#define __K_DBG_SCHED_EVENT(th)
#define __K_DBG_SCHED_SUSPENDED(th)
#define __K_DBG_SCHED_NEXT_THREAD()
#define __K_DBG_SCHED_SKIP_IDLE()
#define __K_DBG_SCHED_NEXT(th)
#define __K_DBG_WAKEUP(th)

#define __K_DBG_MUTEX_LOCKED(th)
#define __K_DBG_MUTEX_UNLOCKED(th)
#define __K_DBG_MUTEX_WAIT(th)

#define __K_DBG_SEM_TAKE(th)
#define __K_DBG_SEM_GIVE(th)
#define __K_DBG_SEM_WAIT(th)

#endif


/*___________________________________________________________________________*/

/**
 * @brief Current thread stack usage
 * 
 * Getting the stack usage of the main thread if CONFIG_THREAD_EXPLICIT_MAIN_STACK is not set will return 0 by default :
 *  
 * @param th
 * @return uint16_t 
 */
uint16_t k_thread_usage(struct k_thread *th);

/**
 * @brief Print the current number of threads
 */
void k_thread_dbg_count(void);

/**
 * @brief Dump the thread structure content as hexadecimal
 * 
 * @param th 
 */
void k_thread_dump_hex(struct k_thread *th);

/**
 * @brief Pretty print the thread structure content :
 * 
 * [priority] : stack usage/stack size | stack end address
 * 
 * e.g:
 * M 0614 [PREE 0] RUNNING : SP 0/512 -| END @0856 
 * 
 * @param th 
 */
void k_thread_dump(struct k_thread *th);

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
 * @param th 
 * @return void* 
 */
void *z_thread_get_return_addr(struct k_thread *th);

/**
 * @brief Copy in buffer the 32 registers (stored in stack) of a suspended thread
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
 * @param th 
 * @param buffer 
 * @param size 
 * @return int 
 */
// int k_thread_copy_registers(struct k_thread *th, uint8_t * buffer, const size_t size);

/*___________________________________________________________________________*/

/**
 * @brief Return the stack pointer value (after CALL)
 * 
 */
uint16_t z_read_sp(void);

/**
 * @brief Return the return addr value of itself (z_read_ra), helps to determine current running thread
 * 
 * Depends on arch (PC can be 2 or 3 bytes)
 */
#if defined(__AVR_3_BYTE_PC__)
uint32_t z_read_ra(void);
#else
uint16_t z_read_ra(void);
#endif

/**
 * @brief Set the stack pointer object
 * 
 * @param th 
 */
void z_set_stack_pointer(struct k_thread *th);

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

#define Z_LOG_SREG() serial_u8(SREG); serial_transmit(';');
#define Z_LOG_SREG2() serial_u8(z_read_sreg()); serial_transmit(';');
#define Z_LOG_SREG_I() serial_transmit('['); serial_u8(SREG >> 7); serial_transmit(']');

/*___________________________________________________________________________*/

void z_thread_symbol_runqueue(struct ditem * item);

void z_thread_symbol_events_queue(struct titem * item);

void z_print_events_queue(void);

void z_print_runqueue(void);

/*___________________________________________________________________________*/

//
// Debug mutex
//

void z_mutex_debug(struct k_mutex *mutex);

/*___________________________________________________________________________*/

//
// Debug mutex
//

void z_sem_debug(struct k_sem *sem);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

