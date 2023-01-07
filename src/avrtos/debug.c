/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"
#include "dstruct/debug.h"
#include "dstruct/dlist.h"
#include "dstruct/queue.h"

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

/*___________________________________________________________________________*/

extern struct ditem *z_runqueue;
extern struct titem *z_events_queue;

/*___________________________________________________________________________*/

uint16_t k_thread_usage(struct k_thread *th)
{
	if (NULL == th->sp) {
		return 0u;
	} else if (th == z_current) {
		// stack pointer refers to the first empty addr (from end)
		// empty stack : th->stack.end == th->sp
		uint16_t sp;
		const uint8_t lock = irq_lock();
		sp		   = SP;
		irq_unlock(lock);
		return ((uint16_t)th->stack.end) - sp;
	} else {
		// stack pointer points to the top of the stack
		// empty stack : th->stack.end == th->sp
		return ((uint16_t)th->stack.end) - ((uint16_t)th->sp);
	}
}

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void k_thread_dbg_count(void)
{
	serial_print_p(PSTR("THREADS COUNT :"));
	serial_u8(&__k_threads_end - &__k_threads_start);
	serial_transmit('\n');
}

void k_thread_dump_hex(struct k_thread *th)
{
	serial_send_hex((const uint8_t *)th, sizeof(struct k_thread));
}

void k_thread_dump(struct k_thread *th)
{
	serial_transmit(th->symbol);
	serial_print_p(PSTR(" 0x"));
	serial_hex16((const uint16_t)th);

	serial_transmit(' ');

	switch (th->state) {
	case K_READY:
		serial_print_p(PSTR("READY  "));
		break;
	case K_STOPPED:
		serial_print_p(PSTR("STOPPED"));
		break;
	case K_PENDING:
		serial_print_p(PSTR("PENDING"));
		break;
	case K_IDLE:
		serial_print_p(PSTR("IDLE   "));
		break;
	default:
		break;
	}

	serial_transmit(' ');

	serial_transmit((th->flags & K_MASK_PRIO) == K_COOPERATIVE ? 'C' : 'P');
	serial_transmit(' ');
	serial_transmit((th->flags & K_MASK_PRIO) == K_FLAG_PRIO_HIGH ? '0' : '1');
	serial_transmit(' ');
	serial_transmit(th->sched_lock ? 'S' : '_');
	serial_transmit(th->timer_expired ? 'X' : '_');
	serial_transmit(th->pend_canceled ? 'Y' : '_');
	serial_transmit(th->wakeup_schd ? 'W' : '_');

	serial_print_p(PSTR(" : SP "));
	serial_u16(k_thread_usage(th));
	serial_transmit('/');
	serial_u16(th->stack.size);
	serial_print_p(PSTR(":0x"));
	serial_hex16((uint16_t)th->stack.end);
	serial_transmit('\n');
}

void k_thread_dump_all(void)
{
	serial_print_p(PSTR("===== k_thread =====\n"));

	for (uint_fast8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
		k_thread_dump(&(&__k_threads_start)[i]);
	}
}

void *z_thread_get_return_addr(struct k_thread *th)
{
	if (th == z_current) {
		uint16_t return_addr_reverted =
			*((uint16_t *)((uint16_t)th->stack.end - 2u));

		return (void *)K_SWAP_ENDIANNESS(return_addr_reverted);
	}
	return NULL;
}

/*___________________________________________________________________________*/

void z_thread_symbol_runqueue(struct ditem *item)
{
	serial_transmit(CONTAINER_OF(item, struct k_thread, tie.runqueue)->symbol);
}

void z_thread_symbol_events_queue(struct titem *item)
{
	serial_transmit(CONTAINER_OF(item, struct k_thread, tie.event)->symbol);
}

void z_print_runqueue(void)
{
	/* TODO */
}

void z_print_events_queue(void)
{
	print_tqueue(z_events_queue, z_thread_symbol_events_queue);
}

/*___________________________________________________________________________*/

void z_sem_debug(struct k_sem *sem)
{
	uint8_t count = sem->count;
	uint8_t limit = sem->limit;

	const uint8_t lock = irq_lock();
	count		   = sem->count;
	limit		   = sem->limit;
	irq_unlock(lock);

	serial_print_p(PSTR("K_SEM "));
	serial_u8(count);
	serial_transmit('/');
	serial_u8(limit);
	serial_transmit('\n');
}

/*___________________________________________________________________________*/