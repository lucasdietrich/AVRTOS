/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

#include <avr/io.h>

void thread(void *arg);

K_FLAGS_DEFINE(flags, 0u);

K_THREAD_DEFINE(thread1, thread, 128u, K_PREEMPTIVE, NULL, 't');

ISR(USART0_RX_vect)
{
    const char chr = USART0_DEVICE->UDRn;
    ll_usart_sync_putc(USART0_DEVICE, chr);

    uint8_t notify = 0u;

    switch (chr) {
    case 'a':
        notify |= 1u << 0u;
        break;
    case 'b':
        notify |= 1u << 1u;
        break;
    case 'c':
        notify |= 1u << 2u;
        break;
    case 'd':
        notify |= 1u << 3u;
        break;
    case 'e':
        notify |= 1u << 4u;
        break;
    case 'f':
        notify |= 1u << 5u;
        break;
    case 'g':
        notify |= 1u << 6u;
        break;
    case 'h':
        notify |= 1u << 7u;
        break;
    default:
        break;
    }

    int8_t ret = k_flags_notify(&flags, notify, K_FLAGS_SET);

    if (ret > 0) {
        k_yield_from_isr();
    }
}

int main(void)
{
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
        printf_P(PSTR("<main> poll\n"));
        uint8_t val = 0xFE;
        int ret     = k_flags_poll(&flags, &val, K_FLAGS_SET_ANY, K_FOREVER);
        printf_P(PSTR("<main> ret: %d val: %u\n"), ret, val);

        k_dump_stack_canaries();

        k_sleep(K_SECONDS(1u));
    }
}

void thread(void *arg)
{
    for (;;) {
        uint8_t val = 0x0F;
        int ret =
            k_flags_poll(&flags, &val, K_FLAGS_SET_ANY | K_FLAGS_CONSUME, K_FOREVER);
        printf_P(PSTR("<thread> ret: %d val: %u\n"), ret, val);
    }
}