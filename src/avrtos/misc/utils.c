#include "utils.h"

static const char utils_ram_limit[] = " : ";
static const char utils_ram_border[] = "============\n";
static const char utils_ram_here[] = "  <-- here";

static const char utils_addr_limit[] = " = ";

// always before rampdump
void usart_dump_addr(const char * name, void* addr)
{
    usart_transmit('@');

    usart_send(name, strlen(name));
    usart_send(utils_addr_limit, strlen(utils_addr_limit));
    usart_hex16((uint16_t) addr);

    usart_transmit('\n');
}

void usart_ram_dump(uint16_t start, const size_t len, const uint16_t sp)
{
    usart_transmit('\n');
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