#include "canaries.h"

extern struct k_thread __k_threads_start;

void _k_init_thread_canaries(void)
{
    for (uint8_t i = 0; i < _k_thread_count; i++)
    {
        struct k_thread *const th = &(&__k_threads_start)[i];

        void* const stack_end = th->stack.end;
        const size_t stack_size = th->stack.size;
        uint8_t* addr = K_STACK_START(stack_end, stack_size);
        
        while (addr < (uint8_t*) stack_end - K_THREAD_STACK_VOID_SIZE)
        {
            *addr++ = THREAD_CANARIES_SYMBOL;
        }
    }
}

void* _k_thread_canaries(struct k_thread *th)
{
    uint8_t* preserved = K_STACK_START(th->stack.end, th->stack.size) - 1u;
    while (*(++preserved) == THREAD_CANARIES_SYMBOL) { }
    return preserved;
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void print_thread_canaries(struct k_thread *th)
{
    uint8_t* addr = (uint8_t*) _k_thread_canaries(th);
    size_t canaries_size = addr - (uint8_t*) K_THREAD_STACK_START(th);

    usart_transmit('[');
    usart_transmit(th->symbol);
    usart_print("] CANARIES until @");
    usart_hex16((uint16_t) addr);
    usart_print(" [found ");
    usart_u16(canaries_size);
    usart_print("], MAX usage = ");
    usart_u16(th->stack.size - canaries_size);
    usart_print(" / ");
    usart_u16(th->stack.size);
    usart_transmit('\n');
}

void print_current_canaries(void)
{
    print_thread_canaries(k_current);
}

void dump_threads_canaries(void)
{
    for (uint8_t i = 0; i < _k_thread_count; i++)
    {
        print_thread_canaries(&(&__k_threads_start)[i]);
    }
}