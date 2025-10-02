/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <util/delay.h>

#include "avrtos/sys.h"

void thread_entry(void *_c);
void thread_canaries_entry(void *_c);

K_THREAD_DEFINE(thread, thread_entry, 0x100, K_PREEMPTIVE, NULL, 'T');

int main(void)
{
    serial_init();

    for (;;) {
        int8_t ret = k_thread_join(&thread, K_FOREVER);
        printf("Thread joined with return value %d\n", ret);

        if (ret == 0) {
            /* rebuild stack */
            k_thread_create(&thread, thread_entry,
                            K_STACK_START(thread.stack.end, thread.stack.size),
                            thread.stack.size, K_PREEMPTIVE, NULL, 'T');
            printf("Thread started again\n");
            k_thread_start(&thread);
        }
    }
}

void thread_entry(void *arg)
{
    ARG_UNUSED(arg);

    uint8_t limit = 5;

    for (uint_fast8_t i = 0; i < 5; i++) {
        printf("i: %d/%d\n", i + 1u, limit);
        k_sleep(K_MSEC(250));
    }
}