/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/drivers/usart.h>

K_SEM_DEFINE(sem, 0, 1);

static void monitor(void *arg);

K_THREAD_DEFINE(mid, monitor, 0x100, K_PREEMPTIVE, NULL, 'm');

ISR(TIMER3_COMPA_vect)
{
    GPIOB->PIN = BIT(5u);

    struct k_thread *unpend = k_sem_give(&sem);

    k_yield_from_isr_cond(unpend);
}

int main(void)
{
    gpiol_init(GPIOB, 0xF0u, 0x00u);

    const struct timer_config timer_cfg = {
        .counter   = TIMER_CALC_COUNTER_VALUE(500000u, 1024u),
        .mode      = TIMER_MODE_CTC,
        .prescaler = TIMER_PRESCALER_1024,
        .timsk     = BIT(OCIEnA),
    };
    ll_timer16_init(TIMER3_DEVICE, timer_get_index(TIMER3_DEVICE), &timer_cfg);

    const struct usart_config usart_cfg = USART_CONFIG_DEFAULT_115200();
    ll_usart_init(USART0_DEVICE, &usart_cfg);

    for (;;) {
        k_sem_take(&sem, K_FOREVER);

        GPIOB->PIN = BIT(7u);

        serial_printl("IRQ !");
    }
}

static void monitor(void *arg)
{
    for (;;) {
        k_dump_stack_canaries();

        k_sleep(K_SECONDS(20u));
    }
}