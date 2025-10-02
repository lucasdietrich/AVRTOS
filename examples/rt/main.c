/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/usart.h>

#define PINn PIN7
#define GPIO GPIOB_DEVICE

static void thread(void *arg);

K_THREAD_DEFINE(t1, thread, 0x100, K_COOPERATIVE, NULL, '1');

int main(void)
{
    gpio_pin_init(GPIOB, PINn, DIR_OUTPUT, OUTPUT_DRIVEN_LOW);

    thread(NULL);
}

void thread(void *arg)
{
    ARG_UNUSED(arg);

    for (;;) {
        /* Writing a logic one to PINxn toggles the value of PORTxn,
         * independent on the value of DDRxn. Note that the SBI
         * instruction can be used to toggle one single bit in a port.
         */
        GPIOB->PIN = BIT(PINn);
        k_yield();
    }
}