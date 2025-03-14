/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define BLOCKS 9

void thread(void *p);
void thread_time(void *ctx);

#define STACK_SIZE  0x80
#define THREAD_PRIO K_PREEMPTIVE

// in ms
#define THREAD_MAX_WAIT           255
#define COLLECT_BLOCK_START_DELAY 3000

K_THREAD_DEFINE(w0, thread, STACK_SIZE, THREAD_PRIO, NULL, '0');
K_THREAD_DEFINE(w1, thread, STACK_SIZE, THREAD_PRIO, NULL, '1');
K_THREAD_DEFINE(w2, thread, STACK_SIZE, THREAD_PRIO, NULL, '2');
K_THREAD_DEFINE(w3, thread, STACK_SIZE, THREAD_PRIO, NULL, '3');
K_THREAD_DEFINE(w4, thread, STACK_SIZE, THREAD_PRIO, NULL, '4');
K_THREAD_DEFINE(w5, thread, STACK_SIZE, THREAD_PRIO, NULL, '5');
K_THREAD_DEFINE(w6, thread, STACK_SIZE, THREAD_PRIO, NULL, '6');
K_THREAD_DEFINE(w7, thread, STACK_SIZE, THREAD_PRIO, NULL, '7');
K_THREAD_DEFINE(w8, thread, STACK_SIZE, THREAD_PRIO, NULL, '8');
K_THREAD_DEFINE(w9, thread, STACK_SIZE, THREAD_PRIO, NULL, '9');
K_THREAD_DEFINE(time, thread_time, 0x80, K_COOPERATIVE, NULL, 'T');

K_MEM_SLAB_DEFINE(myslab, 0x10, BLOCKS);

static uint8_t ms(struct k_prng *prng)
{
    return k_prng_get(prng) & THREAD_MAX_WAIT; /* set 0x7 to test perfomance */
}

static void debug(void *mem, int8_t rc)
{
    // printf_P(PSTR("cur=%c mem=0x%x rc=%d\n"), k_thread_get_current()->symbol,
    // 		 (unsigned int)mem, rc);
}

static void *alloc(k_timeout_t timeout)
{
    void *mem = NULL;
    int8_t rc = k_mem_slab_alloc(&myslab, &mem, timeout);
    debug(mem, rc);

    return mem;
}

void thread(void *p)
{
    K_PRNG_DEFINE_DEFAULT(prng);

    for (;;) {
        void *m = alloc(K_MSEC(ms(&prng)));
        if (m != NULL) {
            k_sleep(K_MSEC(ms(&prng)));
            k_mem_slab_free(&myslab, m);
        }
    }
}

void thread_time(void *ctx)
{
    for (;;) {
        k_show_uptime();
        printf_P(PSTR("\n"));

        k_sleep(K_SECONDS(1));
    }
}

int main(void)
{
    /* interrupts are disabled in this thread */

    led_init();
    serial_init();

    k_thread_dump_all();

    void *blocks[BLOCKS];
    uint8_t count = 0;

    for (;;) {
        /* collect all blocks */
        while (count < BLOCKS) {
            k_sleep(K_MSEC(COLLECT_BLOCK_START_DELAY));
            blocks[count++] = alloc(K_FOREVER);
            printf_P(PSTR("Collected 1 more memory slab (%d/%d) !\n"), count, BLOCKS);
        }

        printf_P(PSTR("Collected all memory slabs !\n"));
        for (uint8_t i = 0; i < BLOCKS; i++) {
            debug(blocks[i], 0);
        }

        k_dump_stack_canaries();

        K_SECONDS(1);

        /* free all blocks */
        for (uint8_t i = 0; i < BLOCKS; i++) {
            k_mem_slab_free(&myslab, blocks[i]);
        }
        count = 0;
    }
}