/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// For ATmega328PB or ATmega2560

#include <assert.h>
#include <stdint.h>
#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/misc/serial.h>
#include <avrtos/logging.h>
#include "avrtos/assert.h"
#include "avrtos/semaphore.h"

#define LOG_LEVEL LOG_LEVEL_DEBUG
#define K_MODULE K_MODULE_APPLICATION

// IPC uart
const struct usart_config usart_ipc_cfg = {
    .baudrate    = USART_BAUD_38400,
    .mode        = USART_MODE_ASYNCHRONOUS,
    .parity      = USART_PARITY_NONE,
    .stopbits    = USART_STOP_BITS_1,
    .databits    = USART_DATA_BITS_8,
    .speed_mode  = USART_SPEED_MODE_NORMAL,
};

#define USART USART1_DEVICE


static char rx_buffer[6u];
static char tx_buffer[] = "Hello World !\n";

void usart_ipc_callback(UART_Device *dev, struct usart_async_context *ctx)
{
    ARG_UNUSED(dev);

    LOG_INF("cb: %d", ctx->evt);
}


int main(void)
{
    int ret;

    // initialize IPC uart
    usart_init(USART, &usart_ipc_cfg);
    usart_set_callback(USART, usart_ipc_callback);
    usart_rx_enable(USART, rx_buffer, sizeof(rx_buffer));

    LOG_INF("USART async example");

    for (;;) {
        ret = usart_tx(USART, tx_buffer, sizeof(tx_buffer) - 1);
        Z_ASSERT_APP(ret == 0);
        k_sleep(K_MSEC(1000));
    }

    // usart_tx(USART1_DEVICE, tx_buffer, sizeof(tx_buffer) - 1);
}