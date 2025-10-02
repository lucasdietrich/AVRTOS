/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
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

#include "avrtos/sys.h"

#define TASKS_COUNT 10u

struct periodic_task {
    struct k_timer timer;
    uint16_t counter; // counter
};

int periodic_task_handler(struct k_timer *timer)
{
    struct periodic_task *task = CONTAINER_OF(timer, struct periodic_task, timer);

    task->counter++;

    return 0; // continue timer
}

static struct periodic_task tasks[TASKS_COUNT];

int mytimer1_handler(struct k_timer *timer)
{
    ARG_UNUSED(timer);

    printf("mytimer1_handler()\n");

    return -1; // stop timer
}

K_TIMER_DEFINE(mytimer1, mytimer1_handler, K_MSEC(500), 0);

int main(void)
{
    led_init();
    serial_init();

    k_thread_dump_all();

    for (uint8_t i = 0; i < ARRAY_SIZE(tasks); i++) {
        k_timer_init(&tasks[i].timer, periodic_task_handler, K_MSEC(100lu * (i + 1)),
                     K_NO_WAIT);
    }

    for (;;) {
        for (uint8_t i = 0; i < ARRAY_SIZE(tasks); i++) {
            printf("Task %u counter = %u\n", i, tasks[i].counter);
        }

        k_dump_stack_canaries();

        printf("Restart mytimer1\n");
        k_timer_start(&mytimer1, mytimer1.timeout);

        k_sleep(K_SECONDS(5));
    }
}