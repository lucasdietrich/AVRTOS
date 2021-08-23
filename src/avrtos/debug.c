#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include <string.h>
#include <stdio.h>

#include "debug.h"
#include "dstruct/tqueue.h"

/*___________________________________________________________________________*/

extern struct ditem *runqueue; 
extern struct titem *events_queue;

/*___________________________________________________________________________*/

uint16_t k_thread_usage(struct k_thread *th)
{
    if (NULL == th->sp)
    {
        return 0u;
    }
    else if(th == k_current)
    {
        // stack pointer refers to the first empty addr (from end)
        // empty stack : th->stack.end == th->sp
        return ((uint16_t)th->stack.end) - ((uint16_t)th->sp) - K_THREAD_STACK_VOID_SIZE;
    }
    else
    {
        // stack pointer refers to the first empty addr (from end)
        // empty stack : th->stack.end == th->sp
        return ((uint16_t)th->stack.end) - ((uint16_t)th->sp);
    }
}

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void k_thread_dbg_count(void)
{
    static const char string_1[] PROGMEM = "THREADS COUNT : _k_thread_count = ";
    usart_print_p(string_1);
    usart_u8(_k_thread_count);
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
    usart_hex16((const uint16_t) th);
    if (th->coop)
    {
        usart_print(" [COOP ");
    }
    else
    {
        usart_print(" [PREE ");
    }

    usart_s8(th->priority);

    usart_print("] ");

   static const char strings[4][8] PROGMEM = {
        "STOPPED",
        "RUNNING",
        "READY  ",
        "WAITING",
    };

    usart_print_p(strings[th->state]);

    usart_print(" : SP ");

    usart_u16(k_thread_usage(th));
    usart_transmit('/');
    usart_u16(th->stack.size);
    usart_print(" -| END @");
    usart_hex16((uint16_t)th->stack.end);
    usart_transmit('\n');
}

void k_thread_dump_all(void)
{
    static const char string[] PROGMEM = "===== k_thread =====\n";
    usart_print_p(string);

    for (uint_fast8_t i = 0; i < _k_thread_count; i++)
    {
        k_thread_dump(&(&__k_threads_start)[i]);
    }
}


void *k_thread_get_return_addr(struct k_thread *th)
{
    if (th == k_current)
    {
        uint16_t return_addr_reverted = *((uint16_t *)((uint16_t)th->stack.end - 2u));

        return (void *)K_SWAP_ENDIANNESS(return_addr_reverted);
    }
    return NULL;
}

int k_thread_copy_registers(struct k_thread *th, char *const buffer, const size_t size)
{
    if ((th == k_current) && (size >= 16))
    {
        memcpy(buffer, (void *)((uint16_t)th->stack.end - 34u), 32u); // -32 (registers) - 2 (return address)

        return 0;
    }
    return -1;
}



/*___________________________________________________________________________*/

#include "dstruct/dlist.h"
#include "dstruct/queue.h"
#include "dstruct/debug.h"

void _thread_symbol_runqueue(struct ditem * item)
{
    usart_transmit(CONTAINER_OF(item, struct k_thread, tie.runqueue)->symbol);
}

void _thread_symbol_events_queue(struct titem * item)
{
    usart_transmit(CONTAINER_OF(item, struct k_thread, tie.event)->symbol);
}

void print_runqueue(void)
{
    print_dlist(runqueue, _thread_symbol_runqueue);
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

    usart_print("K_SEM ");
    usart_u8(count);
    usart_transmit('/');
    usart_u8(limit);
    usart_transmit('\n');
}

/*___________________________________________________________________________*/