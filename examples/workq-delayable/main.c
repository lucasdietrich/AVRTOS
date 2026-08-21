/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Press any character on USART0 to schedule a work item
 * that will print "Work done !" after 1 second.
 * If the character 'c' is pressed, the scheduled work item
 * is cancelled.
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/misc/serial.h>
#include "avrtos/kernel.h"
#include "avrtos/ring.h"

#define TIMEOUT_MS 5000

struct MyStruct {
    struct k_work_delayable work;
    struct k_sem sem;
};

static struct MyStruct my_struct;

void work_handler(struct k_work *work)
{
    struct MyStruct *my_struct = CONTAINER_OF(work, struct MyStruct, work);
    k_sem_give(&my_struct->sem);
}

K_RING_DEFINE(ring, 16);
K_SEM_DEFINE(uart_sem, 0, 1);

ISR(USART0_RX_vect)
{
    uint8_t c = USART0_DEVICE->UDRn;
    k_ring_push(&ring, c);
    
    struct k_thread *thread = k_sem_give(&uart_sem);
    if (thread)
        k_yield_from_isr();
}

static void usart_task(void *arg)
{
    struct MyStruct *ms = (struct MyStruct *)arg;

    int8_t ret;
    char c;

    for (;;) {
        ret = k_ring_pop(&ring, &c);
        if (ret) {
            k_sem_take(&uart_sem, K_FOREVER);
            continue;
        }

        if (c == 'c' || c == 'C') {
            printf_P(PSTR("Cancelling scheduled work"));
            ret = k_work_delayable_cancel(&ms->work);
        } else if (c == 's' || c == 'S') {
            printf_P(PSTR("Scheduling work"));
            ret = k_system_work_delayable_schedule(&ms->work, K_MSEC(TIMEOUT_MS));
        } else if (c == 'r' || c == 'R') {
            printf_P(PSTR("Rescheduling work"));
            ret = k_system_work_delayable_reschedule(&ms->work, K_MSEC(TIMEOUT_MS));
        } else {
            printf_P(PSTR("Unknown command: %c, use 's' to schedule, 'r' to reschedule, "
                          "'c' to cancel\n"),
                     c);
            continue;
        }

        if (ret >= 0) {
            printf_P(PSTR(" OK (%d)\n"), ret);
        } else {
            printf_P(PSTR(" Error: %d\n"), ret);
        }
    }
}

K_THREAD_DEFINE(usart, usart_task, 0x200, K_PREEMPTIVE, &my_struct, 'u');

int main(void)
{
    ll_usart_enable_rx_isr(USART0_DEVICE);
    k_work_delayable_init(&my_struct.work, work_handler);
    k_sem_init(&my_struct.sem, 0, 1);

    for (;;) {
        k_sem_take(&my_struct.sem, K_FOREVER);
        printf_P(PSTR("Work done !\n"));
    }
}