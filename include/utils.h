#ifndef _UTILS_H
#define _UTILS_H

#include "uart.h"

#include "string.h"

/*___________________________________________________________________________*/

#define USART_DUMP_RAM_ALL() (usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP))

#define USART_SREG() (usart_hex(SREG))
#define USART_SP() (usart_hex(SP))

/*___________________________________________________________________________*/

void printf(const char * text);

void usart_dbg_hex16(const char * text, uint16_t addr);

extern "C" void usart_show_addr(uint16_t addr);

extern "C" void usart_ram_dump(uint16_t start, const size_t len, const uint16_t sp);

extern "C" void usart_dump_as_ram(const uint8_t *ram, const size_t len, const uint16_t start_addr, const uint16_t sp);

void usart_show_sp_complete(void);

#endif 