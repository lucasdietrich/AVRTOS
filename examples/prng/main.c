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

void thread(struct k_prng *prng);

K_PRNG_DEFINE_DEFAULT(p1);
K_PRNG_DEFINE_DEFAULT(p2);
K_PRNG_DEFINE(p3, 0xABCDEF12, 0x12345678);
K_PRNG_DEFINE(p4, 0xABCDEF12, 0x12345678);

K_THREAD_DEFINE(t1, thread, 0x100, K_COOPERATIVE, &p1, 'A');
K_THREAD_DEFINE(t2, thread, 0x100, K_COOPERATIVE, &p2, 'B');
K_THREAD_DEFINE(t3, thread, 0x100, K_COOPERATIVE, &p3, 'C');

uint8_t buffer[256];

int main(void)
{
    serial_init();

    k_thread_dump_all();

    k_prng_get_buffer(&p4, buffer, sizeof(buffer));
    for (uint16_t i = 0; i < sizeof(buffer); i++) {
        serial_hex(buffer[i]);
        serial_transmit(' ');
        if ((i & 0xF) == 0xF) serial_transmit('\n');
    }
    serial_transmit('\n');

    k_sleep(K_FOREVER);
}

void thread(struct k_prng *prng)
{
    uint16_t number;

    for (;;) {
        number = k_prng_get(prng);

        serial_transmit(k_thread_get_current()->symbol);
        serial_print_p(PSTR(" : "));
        serial_hex16(number);
        serial_transmit('\n');

        k_sleep(K_MSEC(500));
    }
}
