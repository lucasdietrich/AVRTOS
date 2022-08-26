/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <string.h>
#include <avr/io.h>

#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Dump the entire RAM over the USART
 * 
 * The ram can be handled by "handle_ram_dump.py" python script
 */
#define USART_DUMP_RAM_ALL() (usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP))

/**
 * @brief Print the SREG register over the USART
 */
#define USART_SREG() (usart_hex(SREG))

/**
 * @brief Print the Stack Pointer (SP) register over the USART
 */
#define USART_SP() (usart_hex(SP))

/*___________________________________________________________________________*/

/**
 * @brief Dump an address over the USART
 * 
 * The address can be handled by "handle_ram_dump.py" python script
 * 
 * @param name 
 * @param addr 
 */
void usart_dump_addr(const char * name, void* addr);

/**
 * @brief Dump a section of the RAM over the USART
 * 
 * The ram can be handled by "handle_ram_dump.py" python script
 * 
 * @param start 
 * @param len 
 * @param sp 
 */
void usart_ram_dump(uint16_t start, const size_t len, const uint16_t sp);

/**
 * @brief Dump a buffer, interpreted as RAM, over the USART
 * 
 * The ram can be handled by "handle_ram_dump.py" python script
 * 
 * @param start 
 * @param len 
 * @param sp 
 */
void usart_dump_as_ram(const uint8_t *ram, const size_t len,
        const uint16_t start_addr, const uint16_t sp);

/*___________________________________________________________________________*/

/**
 * @brief 
 * 
 */
#define USART_DUMP_CORE() (usart_core_dump())

// important -> no parameters
/**
 * @brief Print cpu state in the uart 
 *  - SP before calling the function,
 *  - return address = PC just after calling the function
 *  - r0 -> r31
 *  - SREG flags
 * 
 * <<<<0355 04A8 00 00 00 50 53 49 20 52 56 41 01 10 0F 95 41 42 78 03 00 09 01 01 0D 00 0D 00 82 03 00 09 30 01 02>>>>
 * <<<< SP   PC  r0 r1 r2 r3 .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. SREG>>>>
 */
void usart_core_dump(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 