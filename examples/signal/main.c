/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

void waiter(void *context);

K_THREAD_DEFINE(w1, waiter, 0x100, K_PREEMPTIVE, NULL, 'W');
K_SIGNAL_DEFINE(sig);

ISR(USART0_RX_vect)
{
    const char rx = UDR0;
    serial_transmit(rx);

    int8_t ret = k_signal_raise(&sig, rx);

    if (ret > 0) k_yield_from_isr();
}

int main(void)
{
    led_init();
    serial_init();

    /* enable RX interrupt */
    SET_BIT(UCSR0B, 1 << RXCIE0);

    k_thread_dump_all();

    k_sleep(K_FOREVER);
}

void waiter(void *arg)
{
    ARG_UNUSED(arg);

    printf_P(PSTR("Press any key: "));

    k_sleep(K_SECONDS(1));

    for (;;) {
        int8_t err = k_poll_signal(&sig, K_SECONDS(3));
        printf("k_poll_signal returned err = %d\n", err);
        if (err == 0) {
            printf("signal value = %d\n", sig.signal);

            sig.flags = K_POLL_STATE_NOT_READY;
        }
    }
}