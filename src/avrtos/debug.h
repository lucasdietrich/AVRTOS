#ifndef _AVRTOS_DEBUG_H
#define _AVRTOS_DEBUG_H

#include "misc/uart.h"
#include "misc/utils.h"

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
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
uint16_t k_thread_usage(struct thread_t *th);

/**
 * @brief Print the current number of threads
 */
void k_thread_dbg_count(void);

/**
 * @brief Dump the thread structure content as hexadecimal
 * 
 * @param th 
 */
void k_thread_dump_hex(struct thread_t *th);

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
void k_thread_dump(struct thread_t *th);

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
void *k_thread_get_return_addr(struct thread_t *th);

/**
 * @brief Copy in buffer the 32 registers (stored in stack) of a suspended thread
 * 
 * buffer[0] > buffer[5] : r31 > r26
 * buffer[6], buffer[7] = r24, r25
 * buffer[8] > buffer[31] : r23 > r0
 * 
 * @see k_yield, _k_thread_stack_create, _K_STACK_INITIALIZER
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
int k_thread_copy_registers(struct thread_t *th, char *const buffer, const size_t size);

/*___________________________________________________________________________*/

/**
 * @brief Return the stack pointer value (after CALL)
 * 
 */
uint16_t read_sp(void);

/**
 * @brief Return the return addr value of itself (read_ra), helps to determine current running thread
 * 
 * Depends on arch (PC can be 2 or 3 bytes)
 */
#if __AVR_3_BYTE_PC__
uint32_t read_ra(void);
#else
uint16_t read_ra(void);
#endif

/**
 * @brief Set the stack pointer object
 * 
 * @param th 
 */
void set_stack_pointer(struct thread_t *th);

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
uint8_t read_sreg(void);

#define LOG_SREG() usart_u8(SREG); usart_transmit(';');
#define LOG_SREG2() usart_u8(read_sreg()); usart_transmit(';');
#define LOG_SREG_I() usart_transmit('['); usart_u8(SREG >> 7); usart_transmit(']');

/*___________________________________________________________________________*/

void _thread_symbol_runqueue(struct ditem * item);

void _thread_symbol_events_queue(struct titem * item);

void print_events_queue(void);

void print_runqueue(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

