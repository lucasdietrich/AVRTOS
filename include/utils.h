#ifndef _UTILS_H
#define _UTILS_H

#include "uart.h"

#define USART_DUMP_RAM_ALL() (usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP))

extern "C" void usart_show_addr(uint16_t addr);

extern "C" void usart_ram_dump(uint16_t start, size_t len, uint16_t sp);

void usart_show_sp_complete(void);

#endif 