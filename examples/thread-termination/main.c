/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <util/delay.h>

struct k_thread *volatile s_thread = NULL;

void thread_entry(void *_c);
void thread_canaries_entry(void *_c);

K_THREAD_DEFINE(thread, thread_entry, 0x100, K_PREEMPTIVE, &s_thread, 'T');

K_THREAD_DEFINE(
    thread_canaries, thread_canaries_entry, 0x100, K_PREEMPTIVE, &s_thread, 'C');

K_SEM_DEFINE(mysem, 0, 1);

int main(void)
{
    serial_init();

    irq_enable();

    for (;;) {
        k_sem_take(&mysem, K_FOREVER);

        serial_print_p(PSTR("Thread terminated : "));
        serial_hex16(s_thread->symbol);
        serial_transmit('\n');

        /* rebuild stack */
        k_thread_create(s_thread, thread_entry,
                        K_STACK_START(s_thread->stack.end, s_thread->stack.size),
                        s_thread->stack.size, K_PREEMPTIVE, NULL, 'T');
        serial_printl_p(PSTR("Thread started again"));
        k_thread_start(s_thread);
    }

    k_sleep(K_FOREVER);
}

void thread_entry(void *_c)
{
    s_thread = k_thread_get_current();

    for (uint_fast8_t i = 0; i < 5; i++) {
        serial_printl_p(PSTR("Hello !"));
        k_sleep(K_MSEC(250));
    }

    /* make sure the cpu is not released
     * until total termination of the thread */
    k_sched_lock();
    k_sem_give(&mysem);
}

void thread_canaries_entry(void *_c)
{
    for (;;) {
        k_thread_dump_all();
        k_dump_stack_canaries();

        k_sleep(K_SECONDS(30));
    }
}