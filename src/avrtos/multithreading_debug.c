#include "multithreading_debug.h"

#include <avr/io.h>

#include <string.h>
#include <stdio.h>

uint16_t k_thread_usage(struct thread_t *th)
{
    if (NULL == th->sp)
    {
        return 0u;
    }
    else
    {
        // stack pointer refers to the first empty addr (from end)
        // empty stack : th->stack.end == th->sp
        return ((uint16_t)th->stack.end) - ((uint16_t)th->sp);
    }
}

void k_thread_dbg_count(void)
{
    usart_print("k_thread.count = ");
    usart_u8(k_thread.count);
    usart_transmit('\n');
}

void k_thread_dump_hex(struct thread_t *th)
{
    usart_send_hex((const uint8_t *)th, sizeof(struct thread_t));
}

void k_thread_dump(struct thread_t *th)
{
    usart_transmit('[');

    if (th->priority == 0)
    {
        usart_print("DISABLED");
    }
    else
    {
        if (th->priority < 0)
        {
            usart_print("COOP ");
        }
        else
        {
            usart_print("PREE  ");
        }

        usart_s8(th->priority);
    }
    usart_print("] \t: SP ");

    usart_u16(k_thread_usage(th));
    usart_transmit('/');
    usart_u16(th->stack.size);
    usart_print(" -| END @");
    usart_hex16((uint16_t)th->stack.end);
    usart_transmit('\n');
}

void k_thread_dump_all(void)
{
    usart_print("===== k_thread =====\n");

    for (uint_fast8_t i = 0; i < k_thread.count; i++) // k_thread.count
    {
        k_thread_dump(k_thread.list[i]);
    }
}

void *k_thread_get_return_addr(struct thread_t *th)
{
    if (th == k_thread.current)
    {
        uint16_t return_addr_reverted = *((uint16_t *)((uint16_t)th->stack.end - 2u));

        return (void *)K_SWAP_LITTLE_BIG_ENDIAN(return_addr_reverted);
    }
    return NULL;
}

int k_thread_copy_registers(struct thread_t *th, char *const buffer, const size_t size)
{
    if ((th == k_thread.current) && (size >= 16))
    {
        memcpy(buffer, (void *)((uint16_t)th->stack.end - 34u), 32u); // -32 (registers) - 2 (return address)

        return 0;
    }
    return -1;
}