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


#ifdef __cplusplus
}
#endif

#endif 