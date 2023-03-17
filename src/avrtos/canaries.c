/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "canaries.h"

void z_init_thread_stack_canaries(struct k_thread *thread)
{
	for (uint8_t *addr = Z_THREAD_STACK_START_USABLE(thread);
	     addr <= (uint8_t *)thread->stack.end - Z_THREAD_STACK_VOID_SIZE;
	     addr++) {
		*addr = CONFIG_THREAD_CANARIES_SYMBOL;
	}
}

#if CONFIG_AVRTOS_LINKER_SCRIPT
extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void z_init_stacks_canaries(void)
{
	struct k_thread *thread;

	for (thread = &__k_threads_start; thread < &__k_threads_end; thread++) {
		z_init_thread_stack_canaries(thread);
	}
}
void k_dump_stack_canaries(void)
{
	serial_transmit('\n');

	for (uint8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
		k_print_stack_canaries(&(&__k_threads_start)[i]);
	}
}
#else
void z_init_stacks_canaries(void)
{
}
#endif

void *z_stack_canaries(struct k_thread *thread)
{
	uint8_t *preserved =
		Z_STACK_START_USABLE(thread->stack.end, thread->stack.size) - 1u;
	while (*(++preserved) == CONFIG_THREAD_CANARIES_SYMBOL) {
	}
	return preserved;
}

/*___________________________________________________________________________*/

#include "misc/serial.h"

void k_print_stack_canaries(struct k_thread *thread)
{
	uint8_t *addr	      = (uint8_t *)z_stack_canaries(thread);
	size_t canaries_found = addr - (uint8_t *)Z_THREAD_STACK_START_USABLE(thread);

	serial_transmit('[');
	serial_transmit(thread->symbol);
	serial_print_p(PSTR("] CANARIES until @"));
	serial_hex16((uint16_t)addr);
	serial_print_p(PSTR(" [found "));
	serial_u16(canaries_found);
	serial_print_p(PSTR("], MAX usage = "));
	serial_u16(Z_STACK_SIZE_USABLE(thread->stack.size) - canaries_found);
	serial_print_p(PSTR(" / "));
	serial_u16(Z_STACK_SIZE_USABLE(thread->stack.size));

#if CONFIG_THREAD_STACK_SENTINEL
	serial_print_p(PSTR(" + "));
	serial_u8(CONFIG_THREAD_STACK_SENTINEL_SIZE);
	serial_print_p(PSTR(" (sent)"));
#endif /* CONFIG_THREAD_STACK_SENTINEL */

	serial_transmit('\n');
}

void k_print_current_canaries(void)
{
	k_print_stack_canaries(z_current);
}