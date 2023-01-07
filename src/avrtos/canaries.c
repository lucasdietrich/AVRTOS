/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "canaries.h"

void z_init_thread_stack_canaries(struct k_thread *th)
{
	for (uint8_t *addr = K_THREAD_STACK_START_USABLE(th);
	     addr < (uint8_t *)th->stack.end - K_THREAD_STACK_VOID_SIZE;
	     addr++) {
		*addr = CONFIG_THREAD_CANARIES_SYMBOL;
	}
}

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void z_init_stacks_canaries(void)
{
	struct k_thread *thread;

	for (thread = &__k_threads_start; thread < &__k_threads_end; thread++) {
		z_init_thread_stack_canaries(thread);
	}
}

void *z_stack_canaries(struct k_thread *th)
{
        uint8_t *preserved = K_STACK_START_USABLE(th->stack.end, th->stack.size) - 1u;
        while (*(++preserved) == CONFIG_THREAD_CANARIES_SYMBOL) {}
        return preserved;
}

/*___________________________________________________________________________*/

#include "misc/serial.h"

void k_print_stack_canaries(struct k_thread *th)
{
        uint8_t *addr = (uint8_t *)z_stack_canaries(th);
        size_t canaries_found = addr - (uint8_t *)K_THREAD_STACK_START_USABLE(th);

        serial_transmit('[');
        serial_transmit(th->symbol);
        serial_print_p(PSTR("] CANARIES until @"));
        serial_hex16((uint16_t)addr);
        serial_print_p(PSTR(" [found "));
        serial_u16(canaries_found);
        serial_print_p(PSTR("], MAX usage = "));
        serial_u16(K_STACK_SIZE_USABLE(th->stack.size) - canaries_found);
        serial_print_p(PSTR(" / "));
        serial_u16(K_STACK_SIZE_USABLE(th->stack.size));

#if CONFIG_THREAD_STACK_SENTINEL
	serial_print_p(PSTR(" + "));
	serial_u8(CONFIG_THREAD_STACK_SENTINEL_SIZE);
	serial_print_p(PSTR(" (sentinel)"));
#endif /* CONFIG_THREAD_STACK_SENTINEL */

        serial_transmit('\n');
}

void k_print_current_canaries(void)
{
        k_print_stack_canaries(z_current);
}

void k_dump_stack_canaries(void)
{
	serial_transmit('\n');
	
        for (uint8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
                k_print_stack_canaries(&(&__k_threads_start)[i]);
        }
}