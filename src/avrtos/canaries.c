#include "canaries.h"

extern struct k_thread __k_threads_start;

void _k_init_stack_canaries(void)
{
        for (uint_fast8_t i = 0; i < _k_thread_count; i++) {
                struct k_thread *const th = &(&__k_threads_start)[i];

                void *const stack_end = th->stack.end;
                const size_t stack_size = th->stack.size;
		uint8_t *addr = K_STACK_START_USABLE(stack_end, stack_size);

                while (addr < (uint8_t *)stack_end - K_THREAD_STACK_VOID_SIZE) {
                        *addr++ = THREAD_CANARIES_SYMBOL;
                }
        }
}

void *_k_stack_canaries(struct k_thread *th)
{
        uint8_t *preserved = K_STACK_START_USABLE(th->stack.end, th->stack.size) - 1u;
        while (*(++preserved) == THREAD_CANARIES_SYMBOL) {}
        return preserved;
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void print_stack_canaries(struct k_thread *th)
{
        uint8_t *addr = (uint8_t *)_k_stack_canaries(th);
        size_t canaries_found = addr - (uint8_t *)K_THREAD_STACK_START_USABLE(th);

        usart_transmit('[');
        usart_transmit(th->symbol);
        usart_print_p(PSTR("] CANARIES until @"));
        usart_hex16((uint16_t)addr);
        usart_print_p(PSTR(" [found "));
        usart_u16(canaries_found);
        usart_print_p(PSTR("], MAX usage = "));
        usart_u16(K_STACK_SIZE_USABLE(th->stack.size) - canaries_found);
        usart_print_p(PSTR(" / "));
        usart_u16(K_STACK_SIZE_USABLE(th->stack.size));

#if THREAD_STACK_SENTINEL
	usart_print_p(PSTR(" + "));
	usart_u8(THREAD_STACK_SENTINEL_SIZE);
	usart_print_p(PSTR(" (sentinel)"));
#endif /* THREAD_STACK_SENTINEL */

        usart_transmit('\n');
}

void print_current_canaries(void)
{
        print_stack_canaries(_current);
}

void dump_stack_canaries(void)
{
        for (uint8_t i = 0; i < _k_thread_count; i++) {
                print_stack_canaries(&(&__k_threads_start)[i]);
        }
}