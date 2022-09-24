/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_USART_H
#define _AVRTOS_USART_H

#include <stddef.h>
#include <avrtos/kernel.h>

#include <avrtos/drivers.h>

#ifdef __cplusplus
extern "C" {
#endif

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


/* TODO make universal */
#if defined(__AVR_ATmega328P__)
#	define ARCH_USART_COUNT 1
#elif defined(__AVR_ATmega328PB__)
#	define ARCH_USART_COUNT 2
#elif defined(__AVR_ATmega2560__)
#	define ARCH_USART_COUNT 4
#else
#	warning "Unsupported MCU"
#endif


#define AVR_USARTn_BASE(n) ((UART_Device*) (AVR_USART_BASE_ADDR + (n)*sizeof(UART_Device)))

#define AVR_USARTn_INDEX(usart_dev) (usart_dev - AVR_USARTn_BASE(0))

#if ARCH_USART_COUNT > 0
#	define USART0_DEVICE AVR_USARTn_BASE(0)
#endif /* ARCH_USART_COUNT > 0 */

#if ARCH_USART_COUNT > 1
#	define USART1_DEVICE AVR_USARTn_BASE(1)
#endif /* ARCH_USART_COUNT > 1 */

#if ARCH_USART_COUNT > 2
#	define USART2_DEVICE AVR_USARTn_BASE(2)
#endif /* ARCH_USART_COUNT > 2 */

#if ARCH_USART_COUNT > 3
#	define USART3_DEVICE AVR_USARTn_BASE(2)
#endif /* ARCH_USART_COUNT > 3 */

/* see ATmega328p datasheet page 190 */
#define USART_BAUD_2400       2400
#define USART_BAUD_4800       4800
#define USART_BAUD_9600       9600
#define USART_BAUD_14400      14400
#define USART_BAUD_19200      19200
#define USART_BAUD_28800      28800
#define USART_BAUD_38400      38400
#define USART_BAUD_57600      57600
#define USART_BAUD_76800      76800
#define USART_BAUD_115200     115200
#define USART_BAUD_230400     230400 
#define USART_BAUD_250000     250000
#define USART_BAUD_500000     500000
#define USART_BAUD_1000000    1000000

#define USART_CALC_UBRRn(baudrate) (((F_CPU >> 4) / baudrate) - 1)
#define USART_CALC_SPEED_MODE_UBRRn(baudrate) (((F_CPU >> 3) / baudrate) - 1)

typedef enum {
	USART_MODE_ASYNCHRONOUS = 0,
	USART_MODE_SYNCHRONOUS = 1,
	USART_MODE_MSPI = 2,
} usart_mode_t;

typedef enum {
	USART_PARITY_NONE = 0,
	USART_PARITY_EVEN = 2,
	USART_PARITY_ODD = 3,
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

struct usart_config
{
        uint32_t baudrate;
        uint8_t receiver: 1;
        uint8_t transmitter: 1;

        usart_mode_t mode: 2;
        usart_parity_t parity: 2;
        usart_stop_bits_t stopbits: 1;
        usart_data_bits_t databits: 3;
        usart_speed_mode_t speed_mode: 1;
};

#define USART_CONFIG_DEFAULT_2400() { \
        .baudrate = USART_BAUD_2400, \
        .receiver = 1u, \
        .transmitter = 1u, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOP_BITS_1, \
        .databits = USART_DATA_BITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT_9600() { \
        .baudrate = USART_BAUD_9600, \
        .receiver = 1u, \
        .transmitter = 1u, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOP_BITS_1, \
        .databits = USART_DATA_BITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT_115200() { \
        .baudrate = USART_BAUD_115200, \
        .receiver = 1u, \
        .transmitter = 1u, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOP_BITS_1, \
        .databits = USART_DATA_BITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT_500000() { \
        .baudrate = USART_BAUD_500000, \
        .receiver = 1u, \
        .transmitter = 1u, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOP_BITS_1, \
        .databits = USART_DATA_BITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT USART_CONFIG_DEFAULT_500000

// drivers API
K_NOINLINE void usart_ll_drv_init(UART_Device *dev,
				  const struct usart_config *config);

K_NOINLINE int usart_drv_init(UART_Device *dev,
			      const struct usart_config *config);

K_NOINLINE int usart_drv_deinit(UART_Device *dev);

K_NOINLINE int usart_drv_sync_putc(UART_Device *dev, char c);

K_NOINLINE void usart0_drv_sync_putc_opt(char c);

// ASYNC API

struct usart_async_context;

typedef enum {
	USART_EVENT_RX_COMPLETE = 0,
	USART_EVENT_TX_COMPLETE = 1,
	USART_EVENT_ERROR = 2,
} usart_event_t;

typedef void (*usart_async_callback_t)(UART_Device *dev,
				       struct usart_async_context *ctx);

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

K_NOINLINE int usart_set_callback(UART_Device *dev, usart_async_callback_t cb);

K_NOINLINE int usart_rx_disable(UART_Device *dev);

K_NOINLINE int usart_rx_enable(UART_Device *dev, void *buf, size_t size);

K_NOINLINE int usart_tx(UART_Device *dev, const void *buf, size_t size);	

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_USART_H */