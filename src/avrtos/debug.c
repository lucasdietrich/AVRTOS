/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "dstruct/debug.h"
#include "dstruct/dlist.h"
#include "kernel_private.h"

uint16_t k_thread_usage(struct k_thread *thread)
{
    if (NULL == thread->sp) {
        return 0u;
    } else if (thread == z_ker.current) {
        // stack pointer refers to the first empty addr (from end)
        // empty stack : thread->stack.end == thread->sp
        uint16_t sp;
        const uint8_t lock = irq_lock();
        sp                 = SP;
        irq_unlock(lock);
        return ((uint16_t)thread->stack.end) - sp;
    } else {
        // stack pointer points to the top of the stack
        // empty stack : thread->stack.end == thread->sp
        return ((uint16_t)thread->stack.end) - ((uint16_t)thread->sp);
    }
}

#if CONFIG_AVRTOS_LINKER_SCRIPT

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void k_thread_dbg_count(void)
{
    serial_print_p(PSTR("THREADS COUNT :"));
    serial_u8(&__k_threads_end - &__k_threads_start);
    serial_transmit('\n');
}

void k_thread_dump_all(void)
{
    serial_print_p(PSTR("===== k_thread =====\n"));

    for (uint_fast8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
        k_thread_dump(&(&__k_threads_start)[i]);
    }
}

#endif

void k_thread_dump_hex(struct k_thread *thread)
{
    serial_send_hex((const uint8_t *)thread, sizeof(struct k_thread));
}

void k_thread_dump(struct k_thread *thread)
{
    serial_transmit(thread->symbol);
    serial_print_p(PSTR(" 0x"));
    serial_hex16((const uint16_t)thread);

    serial_transmit(' ');

    switch (thread->flags & Z_THREAD_STATE_MSK) {
    case Z_THREAD_STATE_READY:
        serial_print_p(PSTR("READY  "));
        break;
    case Z_THREAD_STATE_STOPPED:
        serial_print_p(PSTR("STOPPED"));
        break;
    case Z_THREAD_STATE_PENDING:
        serial_print_p(PSTR("PENDING"));
        break;
    case Z_THREAD_STATE_IDLE:
        serial_print_p(PSTR("IDLE   "));
        break;
    default:
        break;
    }

    serial_transmit(' ');

    serial_transmit((thread->flags & Z_THREAD_PRIO_COOP_MSK) == Z_THREAD_PRIO_COOP ? 'C'
                                                                                   : 'P');
    serial_transmit(' ');
    serial_transmit(
        (thread->flags & Z_THREAD_PRIO_LEVEL_MSK) == Z_THREAD_PRIO_HIGH ? '0' : '1');
    serial_transmit(' ');
    serial_transmit(thread->flags & Z_THREAD_SCHED_LOCKED_MSK ? 'S' : '_');
    serial_transmit(thread->flags & Z_THREAD_TIMER_EXPIRED_MSK ? 'X' : '_');
    serial_transmit(thread->flags & Z_THREAD_PEND_CANCELED_MSK ? 'Y' : '_');
    serial_transmit(thread->flags & Z_THREAD_WAKEUP_SCHED_MSK ? 'W' : '_');

    serial_print_p(PSTR(" : SP "));
    serial_u16(k_thread_usage(thread));
    serial_transmit('/');
    serial_u16(thread->stack.size);
    serial_print_p(PSTR(":0x"));
    serial_hex16((uint16_t)thread->stack.end);
    serial_transmit('\n');
}

void *z_thread_get_return_addr(struct k_thread *thread)
{
    if (thread == z_ker.current) {
        uint16_t return_addr_reverted = *((uint16_t *)((uint16_t)thread->stack.end - 2u));

        return (void *)K_SWAP_ENDIANNESS(return_addr_reverted);
    }
    return NULL;
}

void z_thread_symbol_runqueue(struct dnode *item)
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
    print_tqueue(z_ker.timeouts_queue, z_thread_symbol_events_queue);
}

void z_sem_debug(struct k_sem *sem)
{
    uint8_t count = sem->count;
    uint8_t limit = sem->limit;

    const uint8_t lock = irq_lock();
    count              = sem->count;
    limit              = sem->limit;
    irq_unlock(lock);

    serial_print_p(PSTR("K_SEM "));
    serial_u8(count);
    serial_transmit('/');
    serial_u8(limit);
    serial_transmit('\n');
}