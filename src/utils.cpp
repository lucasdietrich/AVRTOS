#include "utils.h"

void usart_show_addr(uint16_t addr)
{
    static const char message[] = "addr = ";

    usart_send(message, sizeof(message) - 1);
    usart_hex16(addr);
    usart_transmit('\n');
}

void usart_show_sp_complete(void)
{
    static const char *messages[] = {
        "Stack pointer :"
        "\n\tSP value   = ",
        "\n\tSP address = ",
    };

    // show stack pointer
    usart_send(messages[0], 30);
    usart_hex16(SP);
    usart_send(messages[1], 15);
    usart_hex16((uint16_t) &SP);
}

void usart_ram_dump(uint16_t start, size_t len, uint16_t sp)
{
    static const char limit[] = " : ";
    static const char border[] = "============\n";
    static const char here[] = "  <-- here";

    usart_send(border, sizeof(border) - 1);

    for(uint16_t i = 0; i < len; i++)
    {
        uint16_t addr = start + i;
        usart_hex16(addr);
        usart_send(limit, 3);
        usart_hex(*(uint8_t*)addr);

        if (addr == sp)
        {
            usart_send(here, sizeof(here) - 1);
        }
        usart_transmit('\n');

        if (addr == RAMEND)
        {
            break;
        }
    }

    usart_send(border, sizeof(border) - 1);
}