#include "utils.h"

static const char utils_ram_limit[] = " : ";
static const char utils_ram_border[] = "============\n";
static const char utils_ram_here[] = "  <-- here";

void usart_dbg_hex16(const char * text, uint16_t addr)
{
    usart_print(text);

    usart_hex16(addr);
    usart_transmit('\n');
}

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

void usart_ram_dump(uint16_t start, const size_t len, const uint16_t sp)
{
    usart_send(utils_ram_border, sizeof(utils_ram_border) - 1);

    for(uint16_t i = 0; i < len; i++)
    {
        uint16_t addr = start + i;
        usart_hex16(addr);
        usart_send(utils_ram_limit, 3);
        usart_hex(*((uint8_t*)start + i));

        if (addr == sp)
        {
            usart_send(utils_ram_here, sizeof(utils_ram_here) - 1);
        }
        usart_transmit('\n');

        if (addr == RAMEND)
        {
            break;
        }
    }

    usart_send(utils_ram_border, sizeof(utils_ram_border) - 1);
}

void usart_dump_as_ram(const uint8_t *ram, const size_t len, const uint16_t start_addr, const uint16_t sp)
{
    usart_send(utils_ram_border, sizeof(utils_ram_border) - 1);

    for(uint16_t i = 0; i < len; i++)
    {
        uint16_t addr = start_addr + i;
        usart_hex16(addr);
        usart_send(utils_ram_limit, 3);
        usart_hex(ram[i]);

        if (addr == sp)
        {
            usart_send(utils_ram_here, sizeof(utils_ram_here) - 1);
        }
        usart_transmit('\n');

        if (addr == RAMEND)
        {
            break;
        }
    }

    usart_send(utils_ram_border, sizeof(utils_ram_border) - 1);
}