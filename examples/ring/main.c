/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

#include <avr/io.h>

static struct k_ring ring;
static uint8_t buffer[5u];

ISR(USART0_RX_vect)
{
    const char chr = USART0_DEVICE->UDRn;

    k_ring_push(&ring, chr);
}

void thread(void *arg)
{
    ARG_UNUSED(arg);

    char chr;
    uint32_t rcvd = 0u;
    for (;;) {
        if (k_ring_pop(&ring, &chr) == 0) {
            rcvd++;

            ll_usart_sync_putc(USART0_DEVICE, chr);

            if (rcvd % 100u == 0u) {
                printf_P(PSTR("[rcvd=%lu]\n"), rcvd);
            }
        }

        k_sleep(K_MSEC(100u));

        k_yield();
    }
}

K_THREAD_DEFINE(thread_id, thread, 128u, K_PREEMPTIVE, NULL, 0u);

int main(void)
{
    k_ring_init(&ring, buffer, sizeof(buffer));

    const struct usart_config usart_config = {
        .baudrate    = USART_BAUD_115200,
        .receiver    = 1u,
        .transmitter = 1u,
        .mode        = USART_MODE_ASYNCHRONOUS,
        .parity      = USART_PARITY_NONE,
        .stopbits    = USART_STOP_BITS_1,
        .databits    = USART_DATA_BITS_8,
        .speed_mode  = USART_SPEED_MODE_NORMAL,
    };
    ll_usart_init(USART0_DEVICE, &usart_config);
    ll_usart_enable_rx_isr(USART0_DEVICE);

    char chr = 'a';
    for (;;) {
        k_ring_push(&ring, chr);

        chr++;
        if (chr > 'z') {
            chr = 'a';
        }
        k_wait(K_MSEC(500u), K_WAIT_MODE_IDLE);
    }

    k_sleep(K_FOREVER);
}