#ifndef _UTILS_H
#define _UTILS_H

#include <string.h>

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
void usart_dump_as_ram(const uint8_t *ram, const size_t len, const uint16_t start_addr, const uint16_t sp);

#ifdef __cplusplus
}
#endif

#endif 