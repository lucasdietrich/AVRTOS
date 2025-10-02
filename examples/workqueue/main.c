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

#define WORKQ_RUNTIME_INIT 0

void tasks_generator(void *p);
void tasks_handler(void *p);

K_THREAD_DEFINE(generator, tasks_generator, 0x100, K_PREEMPTIVE, NULL, 'G');

#if WORKQ_RUNTIME_INIT
struct k_workqueue workqueue;
struct k_thread workqueue_thread;
uint8_t workqueue_stack[0x200];
#else
K_WORKQUEUE_DEFINE(workqueue, 0x200, K_PREEMPTIVE, 'W');
#endif

struct task_t {
    struct k_work work;
    struct k_sem sem; /* we need a signal here ! semaphores are inefficient */

    /* mutex would not work because the mutex owner is the main thread and
     * the thread which would want to unlock is the workqueue thread.
     */

    uint32_t input;
};

void task_handler(struct k_work *self)
{
    struct task_t *const task = CONTAINER_OF(self, struct task_t, work);

    k_sleep(K_MSEC((k_delta_t)task->input));

    k_sched_lock();
    serial_hex16((uint16_t)self);
    serial_printl_p(PSTR(" finished"));
    k_sched_unlock();

    k_sem_give(&task->sem);
}

#define TASKS_COUNT 15
struct task_t tasks[TASKS_COUNT];

int main(void)
{
    led_init();
    serial_init();

#if WORKQ_RUNTIME_INIT
    k_workqueue_create(&workqueue, &workqueue_thread, workqueue_stack,
                       sizeof(workqueue_stack), K_PREEMPTIVE, 'W');
#endif

    k_thread_dump_all();

    for (uint_fast8_t i = 0; i < ARRAY_SIZE(tasks); i++) {
        k_sem_init(&tasks[i].sem, 1, 1);
        k_work_init(&tasks[i].work, task_handler);
        tasks[i].input = 200;
    }

    sei();

    k_sleep(K_FOREVER);
}

void tasks_generator(void *p)
{
    ARG_UNUSED(p);

    uint8_t i = 0;
    while (1) {
        if (k_sem_take(&tasks[i].sem, K_NO_WAIT) == 0) {
            serial_hex16((uint16_t)&tasks[i]);
            serial_printl_p(PSTR(" submitted"));
            k_work_submit(&workqueue, &tasks[i].work);
        }

        i = (i + 1) % ARRAY_SIZE(tasks);
    }
}