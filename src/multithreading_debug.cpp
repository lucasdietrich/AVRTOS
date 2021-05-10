#include "multithreading_debug.h"

uint16_t k_thread_usage(thread_t *th)
{
    return (uint16_t) (th->stack.end - (uint16_t) th->sp);
}

void k_thread_dbg_count(void)
{
    usart_print("k_thread.count = ");
    usart_u8(k_thread.count);
    usart_transmit('\n');
}

void k_thread_dump_hex(thread_t *th)
{
    usart_send_hex((const uint8_t*) th, sizeof(thread_t));
}

/*
    TYP:
        2 [-1] : 15/256
*/
void k_thread_dump(thread_t *th)
{
    usart_u8(th->tid);
    usart_transmit(' ');
    usart_transmit('[');
    usart_s8(th->priority);
    usart_print("] : ");
    usart_u16(k_thread_usage(th));
    usart_transmit('/');
    usart_u16(th->stack.size);
    usart_transmit('\n');
}
void k_thread_dump_all(void)
{
    usart_print("===== k_thread =====\n");

    usart_print("  current idx = ");
    usart_u8(k_thread.current_idx);
    usart_transmit('/');
    usart_u8(k_thread.count);
    usart_transmit('\n');
    
    usart_print("{idx}[prio] : {usage}/{size}\n");

    for (uint_fast8_t i = 0; i < k_thread.count; i++)
    {
        k_thread_dump(k_thread.list[i]);
    }
}