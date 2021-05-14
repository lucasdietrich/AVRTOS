#ifndef _DEBUG_H
#define _DEBUG_H

#include <avr/io.h>

#include <string.h>
#include <stdio.h>

#include "uart.h"
#include "utils.h"
#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

void testfunction(void); 

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

/*___________________________________________________________________________*/

void set_stack_pointer(struct thread_t *th);

/**
 * @brief Write the stack pointer value (after CALL and pushes in function) to the structure th
 * 
 * first byte is SPL 
 * second byte is SPH
 * 
 * @see assembler
 * 
 * @param th 
 */
void show_stack_pointer(struct thread_t *th);

/*___________________________________________________________________________*/

/**
 * @brief loop to calculate return value
 * 
 * testloop_assembler(23) > 23
 */
uint8_t testloop_assembler(uint8_t a);

/**
 * @brief shift 1bit to right
 * 
 * Example : testshift_assembler(0x5555) -> 0x2aaa
 */
uint16_t testshift_assembler(uint16_t a);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 