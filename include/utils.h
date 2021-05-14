#ifndef _UTILS_H
#define _UTILS_H

#include <string.h>

#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#define USART_DUMP_RAM_ALL() (usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP))

#define USART_SREG() (usart_hex(SREG))
#define USART_SP() (usart_hex(SP))

/*___________________________________________________________________________*/

void usart_dump_addr(const char * name, void* addr);
void usart_dbg_hex16(const char * text, uint16_t addr);
void usart_show_addr(uint16_t addr);
void usart_ram_dump(uint16_t start, const size_t len, const uint16_t sp);
void usart_dump_as_ram(const uint8_t *ram, const size_t len, const uint16_t start_addr, const uint16_t sp);
void usart_show_sp_complete(void);

#ifdef __cplusplus
}
#endif

#endif 