#include "assert.h"

#include <avr/pgmspace.h>

#include "misc/uart.h"

void __assert(bool expression, uint16_t acode)
{
    static const char assert_msg[] PROGMEM = "***** Kernel Assertion failed *****\n  assert code = ";

    if (expression == 0)
    {
        cli();
        
        usart_print_p(assert_msg);
        usart_u8(acode >> 8);
        usart_transmit(':');
        usart_u8(acode);

        asm("jmp _exit");

        __builtin_unreachable();
    }
}