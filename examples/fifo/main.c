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

k_timeout_t timeouts[] = {
    K_FOREVER, // K_MSEC(2000)
    K_FOREVER, // K_MSEC(1500)
    K_FOREVER, // K_MSEC(1500)
};

void consumer_thread(k_timeout_t *p_timeout);

K_FIFO_DEFINE(fifo);
K_THREAD_DEFINE(consumer1, consumer_thread, 0x50, K_PREEMPTIVE, &timeouts[0], 'A');
K_THREAD_DEFINE(consumer2, consumer_thread, 0x50, K_PREEMPTIVE, &timeouts[1], 'B');
K_THREAD_DEFINE(consumer3, consumer_thread, 0x50, K_PREEMPTIVE, &timeouts[2], 'C');

struct item {
    char chr;
    struct snode tie;
};

struct item letters[] = {{'1'}, {'2'}, {'3'}, {'4'}, {'5'}};

int main(void)
{
    led_init();
    serial_init();

    k_thread_dump_all();

    irq_enable();

    uint8_t pos = 0u;
    while (1) {
        serial_transmit(k_thread_get_current()->symbol);
        serial_print_p(PSTR(" : Send = "));
        serial_transmit(letters[pos].chr);
        serial_transmit('\n');

        k_fifo_put(&fifo, &letters[pos].tie);

        pos = (pos + 1) % ARRAY_SIZE(letters);

        k_sleep(K_MSEC(500u));
    }
}

void consumer_thread(k_timeout_t *p_timeout)
{
    while (1) {
        struct snode *tie = k_fifo_get(&fifo, *p_timeout);
        serial_transmit(k_thread_get_current()->symbol);
        if (tie != NULL) {
            serial_print_p(PSTR(" : Received = "));
            serial_transmit(CONTAINER_OF(tie, struct item, tie)->chr);
            serial_transmit('\n');
        } else {
            serial_printl_p(PSTR(" : Failed to get a fifo item"));
        }
        k_yield();
    }
}
