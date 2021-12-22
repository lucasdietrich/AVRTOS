#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include <string.h>
#include <stdio.h>

#include "debug.h"
#include "dstruct/dlist.h"
#include "dstruct/queue.h"
#include "dstruct/debug.h"

/*___________________________________________________________________________*/

extern struct ditem *runqueue; 
extern struct titem *events_queue;

/*___________________________________________________________________________*/

uint16_t k_thread_usage(struct k_thread *th)
{
        if (NULL == th->sp) {
                return 0u;
        } else if (th == _current) {
                // stack pointer refers to the first empty addr (from end)
                // empty stack : th->stack.end == th->sp
                return ((uint16_t)th->stack.end) -
                        ((uint16_t)th->sp) - K_THREAD_STACK_VOID_SIZE;
        } else {
                // stack pointer refers to the first empty addr (from end)
                // empty stack : th->stack.end == th->sp
                return ((uint16_t)th->stack.end) - ((uint16_t)th->sp);
        }
}

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void k_thread_dbg_count(void)
{
	usart_print_p(PSTR("THREADS COUNT :"));
        usart_u8(&__k_threads_end - &__k_threads_start);
        usart_transmit('\n');
}

void k_thread_dump_hex(struct k_thread *th)
{
        usart_send_hex((const uint8_t *)th, sizeof(struct k_thread));
}

void k_thread_dump(struct k_thread *th)
{
        usart_transmit(th->symbol);
        usart_transmit(' ');
        usart_hex16((const uint16_t)th);
        if (th->coop) {
                usart_print_p(PSTR(" [COOP "));
        } else {
                usart_print_p(PSTR(" [PREE "));
        }

        usart_s8(th->priority);

        usart_print_p(PSTR("] "));

	switch (th->state) {
		case READY:
			usart_print_p(PSTR("READY  "));
			break;
		case STOPPED:
			usart_print_p(PSTR("STOPPED"));
			break;
		case PENDING:
			usart_print_p(PSTR("PENDING"));
			break;
		default:
			break;
	}

        usart_print_p(PSTR(" : SP "));

        usart_u16(k_thread_usage(th));
        usart_transmit('/');
        usart_u16(th->stack.size);
        usart_print_p(PSTR(" -| END @"));
        usart_hex16((uint16_t)th->stack.end);
        usart_transmit('\n');
}

void k_thread_dump_all(void)
{
        usart_print_p(PSTR("===== k_thread =====\n"));

        for (uint_fast8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
                k_thread_dump(&(&__k_threads_start)[i]);
        }
}


void *k_thread_get_return_addr(struct k_thread *th)
{
        if (th == _current) {
                uint16_t return_addr_reverted = *((uint16_t *)
                        ((uint16_t)th->stack.end - 2u));

                return (void *)K_SWAP_ENDIANNESS(return_addr_reverted);
        }
        return NULL;
}

int k_thread_copy_registers(struct k_thread *th, 
uint8_t *buffer, const size_t size)
{
        const uint16_t context_size = K_THREAD_STACK_VOID_SIZE;
        if ((th != _current) && (size >= context_size)) {
                memcpy(buffer, K_STACK_START(th->stack.end,
                        context_size), context_size);

                return context_size;
        }
        return -1;
}



/*___________________________________________________________________________*/


void _thread_symbol_runqueue(struct ditem *item)
{
        usart_transmit(CONTAINER_OF(item, struct k_thread, tie.runqueue)->symbol);
}

void _thread_symbol_events_queue(struct titem *item)
{
        usart_transmit(CONTAINER_OF(item, struct k_thread, tie.event)->symbol);
}

void print_runqueue(void)
{
        print_ref_dlist(runqueue, _thread_symbol_runqueue);
}

void print_events_queue(void)
{
        print_tqueue(events_queue, _thread_symbol_events_queue);
}

/*___________________________________________________________________________*/

void k_sem_debug(struct k_sem *sem)
{
        uint8_t count = sem->count;
        uint8_t limit = sem->limit;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                count = sem->count;
                limit = sem->limit;
        }

        usart_print_p(PSTR("K_SEM "));
        usart_u8(count);
        usart_transmit('/');
        usart_u8(limit);
        usart_transmit('\n');
}

/*___________________________________________________________________________*/