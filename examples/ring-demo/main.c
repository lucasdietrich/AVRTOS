/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

#include <avr/io.h>

static struct k_ring ring;
static uint8_t buffer[3u];

ISR(USART0_RX_vect)
{
    const char in = USART0_DEVICE->UDRn;
    char out;

    if (in >= 'a' && in <= 'z') {
        serial_transmit('>');
        serial_transmit(in);
        k_ring_push(&ring, in);
    } else {

        if (k_ring_pop(&ring, &out) == 0) {
            serial_transmit('<');
            serial_transmit(out);
        } else {
            serial_transmit('-');
        }
    }

    serial_transmit('\n');
}

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

    for (;;) {
    }
}