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
 *      [-1] : 35/256 |037D
 * 
 * If CONFIG_THREAD_EXPLICIT_MAIN_STACK is not set, dumping main thread structure will give :
 *      [8] : 0/0 |08FF
 * 
 * @param th 
 */
void k_thread_dump(struct thread_t *th);

/**
 * @brief Pretty print all threads contains
 * 
 * ===== k_thread =====
 * [8] : 0/256 |0273
 * [-1] : 35/256 |0586
 * [-1] : 35/256 |047D
 * [-1] : 35/256 |0373
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
 * @brief Return the return addr value of itself (read_ra)
 * 
 */
uint16_t read_ra(void);

/**
 * @brief Set the stack pointer object
 * 
 * @param th 
 */
void set_stack_pointer(struct thread_t *th);

/*___________________________________________________________________________*/

void print_scheduled_item(struct k_xqueue_item_t* const item);

void print_scheduled_items_list(struct k_xqueue_item_t* root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

