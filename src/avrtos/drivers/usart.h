/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_USART_H
#define _AVRTOS_DRIVERS_USART_H

#include <stddef.h>

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UDRIEn UDRIE0
#define TXCIEn TXCIE0
#define RXCIEn RXCIE0

// AVR_USART_BASE_ADDR is actually UCSR0A
#define AVR_USART_BASE_ADDR (AVR_IO_BASE_ADDR + 0x00C0U)

typedef struct {
    __IO uint8_t UCSRnA;
    __IO uint8_t UCSRnB;
    __IO uint8_t UCSRnC;
    __IO uint8_t _reserved1; // internal/unused register
    __IO uint8_t UBRRnL;
    __IO uint8_t UBRRnH;
    __IO uint8_t UDRn;
    __IO uint8_t _reserved2;
} UART_Device;

#define AVR_USARTn_BASE(n)                                                               \
    ((UART_Device *)(AVR_USART_BASE_ADDR + (n) * sizeof(UART_Device)))
#define AVR_USARTn_INDEX(usart_dev) (usart_dev - AVR_USARTn_BASE(0))

#if defined(UDR5)
#define ARCH_USART_COUNT 6
#elif defined(UDR4)
#define ARCH_USART_COUNT 5
#elif defined(UDR3)
#define ARCH_USART_COUNT 4
#elif defined(UDR2)
#define ARCH_USART_COUNT 3
#elif defined(UDR1)
#define ARCH_USART_COUNT 2
#elif defined(UDR0)
#define ARCH_USART_COUNT 1
#else
#warning "Unsupported MCU"
#define ARCH_USART_COUNT 0
#endif

#if ARCH_USART_COUNT > 0
#define USART0_DEVICE AVR_USARTn_BASE(0)
#endif /* ARCH_USART_COUNT > 0 */

#if ARCH_USART_COUNT > 1
#define USART1_DEVICE AVR_USARTn_BASE(1)
#endif /* ARCH_USART_COUNT > 1 */

#if ARCH_USART_COUNT > 2
#define USART2_DEVICE AVR_USARTn_BASE(2)
#endif /* ARCH_USART_COUNT > 2 */

#if ARCH_USART_COUNT > 3
#define USART3_DEVICE AVR_USARTn_BASE(2)
#endif /* ARCH_USART_COUNT > 3 */

/* see ATmega328p datasheet page 190 */
#define USART_BAUD_2400    2400lu
#define USART_BAUD_4800    4800lu
#define USART_BAUD_9600    9600lu
#define USART_BAUD_14400   14400lu
#define USART_BAUD_19200   19200lu
#define USART_BAUD_28800   28800lu
#define USART_BAUD_38400   38400lu
#define USART_BAUD_57600   57600lu
#define USART_BAUD_76800   76800lu
#define USART_BAUD_115200  115200lu
#define USART_BAUD_230400  230400lu
#define USART_BAUD_250000  250000lu
#define USART_BAUD_500000  500000lu
#define USART_BAUD_1000000 1000000lu

#define USART_CALC_UBRRn(baudrate)            (((F_CPU >> 4) / baudrate) - 1)
#define USART_CALC_SPEED_MODE_UBRRn(baudrate) (((F_CPU >> 3) / baudrate) - 1)

typedef enum {
    USART_MODE_ASYNCHRONOUS = 0,
    USART_MODE_SYNCHRONOUS  = 1,
    USART_MODE_MSPI         = 2,
} usart_mode_t;

typedef enum {
    USART_PARITY_NONE = 0,
    USART_PARITY_EVEN = 2,
    USART_PARITY_ODD  = 3,
} usart_parity_t;

typedef enum {
    USART_STOP_BITS_1 = 0,
    USART_STOP_BITS_2 = 1,
} usart_stop_bits_t;

typedef enum {
    USART_DATA_BITS_5 = 0,
    USART_DATA_BITS_6 = 1,
    USART_DATA_BITS_7 = 2,
    USART_DATA_BITS_8 = 3,
    USART_DATA_BITS_9 = 7
} usart_data_bits_t;

typedef enum {
    USART_SPEED_MODE_NORMAL = 0,
    USART_SPEED_MODE_DOUBLE = 1
} usart_speed_mode_t;

struct usart_config {
    uint32_t baudrate;
    uint8_t receiver : 1;
    uint8_t transmitter : 1;

    usart_mode_t mode : 2;
    usart_parity_t parity : 2;
    usart_stop_bits_t stopbits : 1;
    usart_data_bits_t databits : 3;
    usart_speed_mode_t speed_mode : 1;
};

#define USART_CONFIG_DEFAULT_2400()                                                      \
    {                                                                                    \
        .baudrate = USART_BAUD_2400, .receiver = 1u, .transmitter = 1u,                  \
        .mode = USART_MODE_ASYNCHRONOUS, .parity = USART_PARITY_NONE,                    \
        .stopbits = USART_STOP_BITS_1, .databits = USART_DATA_BITS_8,                    \
        .speed_mode = USART_SPEED_MODE_NORMAL                                            \
    }

#define USART_CONFIG_DEFAULT_9600()                                                      \
    {                                                                                    \
        .baudrate = USART_BAUD_9600, .receiver = 1u, .transmitter = 1u,                  \
        .mode = USART_MODE_ASYNCHRONOUS, .parity = USART_PARITY_NONE,                    \
        .stopbits = USART_STOP_BITS_1, .databits = USART_DATA_BITS_8,                    \
        .speed_mode = USART_SPEED_MODE_NORMAL                                            \
    }

#define USART_CONFIG_DEFAULT_115200()                                                    \
    {                                                                                    \
        .baudrate = USART_BAUD_115200, .receiver = 1u, .transmitter = 1u,                \
        .mode = USART_MODE_ASYNCHRONOUS, .parity = USART_PARITY_NONE,                    \
        .stopbits = USART_STOP_BITS_1, .databits = USART_DATA_BITS_8,                    \
        .speed_mode = USART_SPEED_MODE_NORMAL                                            \
    }

#define USART_CONFIG_DEFAULT_500000()                                                    \
    {                                                                                    \
        .baudrate = USART_BAUD_500000, .receiver = 1u, .transmitter = 1u,                \
        .mode = USART_MODE_ASYNCHRONOUS, .parity = USART_PARITY_NONE,                    \
        .stopbits = USART_STOP_BITS_1, .databits = USART_DATA_BITS_8,                    \
        .speed_mode = USART_SPEED_MODE_NORMAL                                            \
    }

#define USART_CONFIG_DEFAULT USART_CONFIG_DEFAULT_115200

// drivers API
__kernel void ll_usart_init(UART_Device *dev, const struct usart_config *config);

__kernel int8_t usart_init(UART_Device *dev, const struct usart_config *config);

__kernel int8_t usart_deinit(UART_Device *dev);

__kernel void ll_usart_sync_putc(UART_Device *dev, char c);

__kernel uint8_t ll_usart_sync_getc(UART_Device *dev);

__always_inline void ll_usart_enable_rx_isr(UART_Device *dev)
{
    SET_BIT(dev->UCSRnB, BIT(RXCIEn));
}

__always_inline void ll_usart_disable_rx_isr(UART_Device *dev)
{
    CLR_BIT(dev->UCSRnB, BIT(RXCIEn));
}

__always_inline void ll_usart_enable_tx_isr(UART_Device *dev)
{
    SET_BIT(dev->UCSRnB, BIT(TXCIEn));
}

__always_inline void ll_usart_disable_tx_isr(UART_Device *dev)
{
    CLR_BIT(dev->UCSRnB, BIT(TXCIEn));
}

__always_inline void ll_usart_enable_udre_isr(UART_Device *dev)
{
    SET_BIT(dev->UCSRnB, BIT(UDRIEn));
}

__always_inline void ll_usart_disable_udre_isr(UART_Device *dev)
{
    CLR_BIT(dev->UCSRnB, BIT(UDRIEn));
}

__kernel int8_t usart_send(UART_Device *dev, const char *buf, size_t len);

// ASYNC API

struct usart_async_context;

typedef enum {
    USART_EVENT_RX_COMPLETE = 0,
    USART_EVENT_TX_COMPLETE = 1,
    USART_EVENT_ERROR       = 2,
} usart_event_t;

typedef void (*usart_async_callback_t)(UART_Device *dev, struct usart_async_context *ctx);

struct usart_async_context {
    usart_event_t evt;

    usart_async_callback_t callback;

    struct {
        uint8_t *buf;
        size_t size;
        size_t cur;
    } rx;

    struct {
        const uint8_t *buf;
        size_t size;
        size_t cur;
    } tx;
};
// typedef struct usart_event_t;

__kernel int8_t usart_set_callback(UART_Device *dev, usart_async_callback_t cb);

__kernel int8_t usart_rx_disable(UART_Device *dev);

__kernel int8_t usart_rx_enable(UART_Device *dev, void *buf, size_t size);

__kernel int8_t usart_tx(UART_Device *dev, const void *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_DRIVERS_USART_H */