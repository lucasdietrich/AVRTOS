/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/dstruct/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define MEM_SLAB_COMPILATION_TIME 1

struct block {
    struct snode tie;
    uint8_t data[0x20];
};

#define BLOCK_COUNT 10u
#define BLOCK_SIZE  sizeof(struct block)

void block_read(uint8_t *buffer, uint16_t size)
{
    for (uint16_t i = 0u; i < size; i++) {
        serial_hex(buffer[i]);
        serial_transmit(' ');
    }
    serial_transmit('\n');
}

void block_write(uint8_t *buffer, uint16_t size, uint8_t symb)
{
    for (uint16_t i = 0u; i < size; i++) {
        buffer[i] = symb;
    }
}

void consumer_thread(void *);

K_FIFO_DEFINE(fifo);
K_THREAD_DEFINE(consumer, consumer_thread, 0x50, K_PREEMPTIVE, NULL, 'A');

#if MEM_SLAB_COMPILATION_TIME
K_MEM_SLAB_DEFINE(myslab, BLOCK_SIZE, BLOCK_COUNT);
#else
uint8_t buffer[BLOCK_SIZE * BLOCK_COUNT];
struct k_mem_slab myslab;
#endif

int main(void)
{
    led_init();
    serial_init();

#if !MEM_SLAB_COMPILATION_TIME
    k_mem_slab_init(&myslab, buffer, BLOCK_SIZE, BLOCK_COUNT);
#endif

    k_thread_dump_all();

    irq_enable();

    uint8_t x = 0;
    while (1) {
        struct block *mem;
        int8_t ret = k_mem_slab_alloc(&myslab, (void **)&mem, K_FOREVER);
        if (ret == 0) {
            k_sched_lock();
            serial_print_p(PSTR("Allocated : "));
            serial_hex16((uint16_t)mem);
            serial_transmit('\n');
            k_sched_unlock();

            block_write(mem->data, sizeof(mem->data), x++);
            k_fifo_put(&fifo, &mem->tie);
        }
    }
}

void consumer_thread(void *context)
{
    while (1) {
        struct block *mem = (struct block *)k_fifo_get(&fifo, K_FOREVER);
        if (mem != NULL) {
            k_sched_lock();
            serial_print_p(PSTR("Received mem slab at address "));
            serial_hex16((uint16_t)mem);
            serial_print_p(PSTR(" data = "));
            block_read(mem->data, sizeof(mem->data));
            k_sched_unlock();

            k_mem_slab_free(&myslab, (void *)mem);
        }

        k_sleep(K_SECONDS(1));
    }
}
