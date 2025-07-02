/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/logging.h>
#include <avr/iom2560.h>
#include <avr/pgmspace.h>

#include "avrtos/defines.h"
#include "avrtos/deprecated.h"
#include "avrtos/fault.h"
#include "avrtos/kernel.h"
#include "avrtos/semaphore.h"
#include "avrtos/tickless.h"
#include "gpio.h"
#include "serial.h"

#define LOG_LEVEL LOG_LEVEL_DBG

struct th_data {
    uint8_t led;
    uint16_t ms;
};

static void task(void *arg)
{
    struct th_data *data = (struct th_data *)arg;
    printf_P(PSTR("Task started, blinking LED %u every %u ms\n"), data->led, data->ms);
    
    k_timeout_t timeout = K_MSEC(data->ms);

    for (;;) {
        gpiol_pin_toggle(GPIOF, data->led);
        k_sleep(timeout);
    }
}

static const struct th_data task_data[] = {
    { .led = 4, .ms = 75u },
    { .led = 5, .ms = 150u },
    { .led = 6, .ms = 250u },
    { .led = 7, .ms = 1200u },
};

K_THREAD_DEFINE(t1, task, 0x100, K_COOPERATIVE, (void*)&task_data[0], 'a');
K_THREAD_DEFINE(t2, task, 0x100, K_COOPERATIVE, (void*)&task_data[1], 'b');
K_THREAD_DEFINE(t3, task, 0x100, K_COOPERATIVE, (void*)&task_data[2], 'c');
K_THREAD_DEFINE(t4, task, 0x100, K_COOPERATIVE, (void*)&task_data[3], 'c');

K_SEM_DEFINE(sem, 0, 1);

void print_time(void)
{
    struct z_tickless_timespec tls;
    struct timespec ts;

    z_tickless_time_get(&tls);
    z_tickless_spec_convert(&tls, &ts);

    printf_P(PSTR("Tickless time: "));
    serial_u32(ts.tv_sec);
    printf_P(PSTR("."));
    printf_P(PSTR("%03u\n"), ts.tv_msec);
}

ISR(TIMER4_COMPA_vect)
{
    // uint32_t et = z_tickless_elapsed_since_last_sp();
    // serial_u32(et);
    // printf_P(PSTR(" ticks %u ms\n"), et / (Z_TICK_US * 1000u));

    // printf_P(PSTR("tim1 tcnt: %u\n"),
    //          ll_timer16_get_tcnt(TIMER1_DEVICE));


    gpiol_pin_write_state(GPIOF, 4, 0u);

    // printf_P(PSTR("irq\n"));
    struct k_thread *thread = k_sem_give(&sem);
    ll_timer16_stop(TIMER4_DEVICE);
    k_yield_from_isr_cond(thread);
}

int main(void)
{
    gpiol_pin_init(GPIOF, 4u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 5u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 6u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 7u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    const struct timer_config config = {
        .mode = TIMER_MODE_CTC,
        .prescaler = TIMER_PRESCALER_256,
        .counter = TIMER_CALC_COUNTER_VALUE(50000u, 256u),
        .timsk = BIT(OCIEnA)
    };

    printf_P(PSTR("AVRTOS Tickless Example\n"));

    k_timeout_t timeout = K_MSEC(100u);
    serial_u32(timeout.value);
    printf_P(PSTR(" ticks = 100ms \n"));
    
    for (uint_fast16_t i = 0;;i++) {
        printf_P(PSTR("=================\nstarting timer\n"));
        k_sem_init(&sem, 0, 1);
        ll_timer16_deinit(TIMER4_DEVICE, TIMER4_INDEX);
        gpiol_pin_write_state(GPIOF, 4, 1u);
        ll_timer16_init(TIMER4_DEVICE, TIMER4_INDEX, &config);
        
        int ret = k_sem_take(&sem, timeout);
        printf_P(PSTR("sem: %d\n"), ret);

        // print_time();
        k_sleep(K_MSEC(1000u));

        // if (i >= 6) {
            // __fault(K_FAULT_ANY);
        // }
    }

    k_stop();
}