/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "usart.h"
#include <stdbool.h>

#include <avr/io.h>
#include <avr/pgmspace.h>

#define DRIVERS_UART_ASYNC                                                 \
	((CONFIG_DRIVERS_USART0_ASYNC) || (CONFIG_DRIVERS_USART1_ASYNC) || \
	 (CONFIG_DRIVERS_USART2_ASYNC) || (CONFIG_DRIVERS_USART3_ASYNC))

/* same for all USARTs */
#define TXENn TXEN0
#define RXENn RXEN0
#define UDREn UDRE0

#define UMSELn0 UMSEL00
#define UMSELn1 UMSEL01
#define UPMn0	UPM00
#define UPMn1	UPM01
#define USBSn	USBS0
#define UCSZn0	UCSZ00
#define UCSZn1	UCSZ01
#define UCSZn2	UCSZ02
#define UDREn	UDRE0

#define MPCMn MPCM0
#define U2Xn  U2X0
#define UPEn  UPE0
#define DORn  DOR0
#define FEn   FE0
#define UDREn UDRE0
#define TXCn  TXC0
#define RXCn  RXC0

#define K_MODULE K_MODULE_DRIVERS_USART

#define CONFIG_USART_BAUDRATE_CALC_ROUNDING 1

/* check U2Xn bit which allow to double the speed */
static uint16_t calculate_ubrr(uint32_t baudrate, bool speed_mode)
{
#if CONFIG_USART_BAUDRATE_CALC_ROUNDING
	const uint8_t q = (speed_mode) ? 2u : 3u;
	uint16_t div	= ((F_CPU >> q) / baudrate);

	if (div & 1u) {
		div += 2u;
	}

	return (div >> 1u) - 1u;
#else
	if (speed_mode) {
		return USART_CALC_SPEED_MODE_UBRRn(baudrate);
	} else {
		return USART_CALC_UBRRn(baudrate);
	}
#endif
}

static void set_baudrate(UART_Device *dev, uint32_t baudrate, bool speed_mode)
{
	const uint16_t ubrr = calculate_ubrr(baudrate, speed_mode);

	dev->UBRRnH = (uint8_t)(ubrr >> 8);
	dev->UBRRnL = (uint8_t)ubrr;
}

void ll_usart_init(UART_Device *dev, const struct usart_config *config)
{
	/* set baudrate */
	set_baudrate(dev, config->baudrate, config->speed_mode);

	/* enable receiver and transmitter */
	uint8_t ucsrnb = 0u;
	if (config->transmitter) SET_BIT(ucsrnb, BIT(TXENn));
	if (config->receiver) SET_BIT(ucsrnb, BIT(RXENn));

#if DRIVERS_UART_ASYNC
	/* enable interrupt */
	SET_BIT(ucsrnb, BIT(RXCIEn));
	SET_BIT(ucsrnb, BIT(TXCIEn));
#endif

	dev->UCSRnB = ucsrnb;

	uint8_t ucsrc = 0u;
	/* set USART mode asynchrone */
	SET_BIT(ucsrc, (0 << UMSELn0) | (0 << UMSELn1));
	/* set parity mode */
	SET_BIT(ucsrc, config->parity << UPMn0);
	/* Set stop bit */
	SET_BIT(ucsrc, config->stopbits << USBSn);
	/* set frame format */
	SET_BIT(ucsrc, config->databits << UCSZn0);
	dev->UCSRnC = ucsrc;
}

int usart_init(UART_Device *dev, const struct usart_config *config)
{
	if ((dev == NULL) || (config == NULL)) {
		return -EINVAL;
	}

	/* Only asynchrone mode is supported */
	if (config->mode != USART_MODE_ASYNCHRONOUS) {
		return -EINVAL;
	}

	/* Invalid parity mode (reserved) */
	if (config->parity == 1u) {
		return -EINVAL;
	}

	/* Invalid frame format (reserved) */
	if ((config->databits & 0x4u) && (config->databits != USART_DATA_BITS_9)) {
		return -EINVAL;
	}

	ll_usart_init(dev, config);

	return 0;
}

int usart_deinit(UART_Device *dev)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	dev->UCSRnA = 0U;
	dev->UCSRnB = 0U;
	dev->UCSRnC = 0U;

	return 0;
}

void ll_usart_sync_putc(UART_Device *dev, char c)
{
	/* wait for empty transmit buffer */
	while (!(dev->UCSRnA & BIT(UDREn)))
		;

	/* put data into HW buffer, sends the data */
	dev->UDRn = c;
}

int ll_usart_sync_getc(UART_Device *dev)
{
	int ret = -EAGAIN;

	/* get and return received data from HW buffer */
	if (dev->UCSRnA & BIT(RXCn)) {
		ret = dev->UDRn;
	}

	return ret;
}

int usart_send(UART_Device *dev, const char *buf, int len)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (dev == NULL) {
		return -EINVAL;
	}
#endif

	for (int i = 0; i < len; i++) {
		ll_usart_sync_putc(dev, buf[i]);
	}

	return len;
}

/*
 * Problem is that execution time is much longer than the old "serial_transmit",
 * because of fetch IO address for the proper USART from flash.
 */

int usart_sync_putc(UART_Device *dev, char c)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (dev == NULL) {
		return -EINVAL;
	}
#endif

	ll_usart_sync_putc(dev, c);

	return 0;
}

__kernel int usart_getc(UART_Device *dev)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (dev == NULL) {
		return -EINVAL;
	}
#endif

	return ll_usart_sync_getc(dev);
}

/* as fast as serial_transmit */
void usart0_sync_putc(char c)
{
	while (!(USART0_DEVICE->UCSRnA & BIT(UDREn)))
		;
	UDR0 = c;
}

__kernel int usart0_getc(void)
{
	if (!(USART0_DEVICE->UCSRnA & BIT(RXCn))) {
		return -EAGAIN;
	}

	return UDR0;
}

#if DRIVERS_UART_ASYNC

// size should be configurable
static struct usart_async_context usart_async_contexts[ARCH_USART_COUNT];

static inline struct usart_async_context *usart_get_async_context(UART_Device *dev)
{
	return &usart_async_contexts[AVR_USARTn_INDEX(dev)];
}

static void rx_interrupt(UART_Device *dev)
{
	const char chr			= dev->UDRn;
	struct usart_async_context *ctx = usart_get_async_context(dev);

	ctx->rx.buf[ctx->rx.cur++] = chr;
	if (ctx->rx.cur == ctx->rx.size) {
		__ASSERT_FALSE(ctx->callback == NULL);

		ctx->evt = USART_EVENT_RX_COMPLETE;
		ctx->callback(dev, ctx);
		ctx->rx.cur = 0U;
	}
}

static void tx_interrupt(UART_Device *dev)
{
	struct usart_async_context *ctx = usart_get_async_context(dev);

	if (ctx->tx.cur == ctx->tx.size) {
		__ASSERT_FALSE(ctx->callback == NULL);

		ctx->evt = USART_EVENT_TX_COMPLETE;
		ctx->callback(dev, ctx);
		ctx->tx.size = 0U;
	}
}

static void udre_interrupt(UART_Device *dev)
{
	struct usart_async_context *ctx = usart_get_async_context(dev);

	/* if there are more data to send */
	if (ctx->tx.cur < ctx->tx.size) {
		dev->UDRn = ctx->tx.buf[ctx->tx.cur++];
	} else {
		CLR_BIT(dev->UCSRnB, BIT(UDRIEn));
	}
}

#if CONFIG_DRIVERS_USART0_ASYNC
ISR(USART0_RX_vect)
{
	rx_interrupt(USART0_DEVICE);
}

ISR(USART0_TX_vect)
{
	tx_interrupt(USART0_DEVICE);
}

ISR(USART0_UDRE_vect)
{
	udre_interrupt(USART0_DEVICE);
}
#endif /* CONFIG_DRIVERS_USART0_ASYNC */

#if CONFIG_DRIVERS_USART1_ASYNC
ISR(USART1_RX_vect)
{
	rx_interrupt(USART1_DEVICE);
}

ISR(USART1_TX_vect)
{
	tx_interrupt(USART1_DEVICE);
}

ISR(USART1_UDRE_vect)
{
	udre_interrupt(USART1_DEVICE);
}
#endif /* CONFIG_DRIVERS_USART1_ASYNC */

#if CONFIG_DRIVERS_USART2_ASYNC
ISR(USART2_RX_vect)
{
	rx_interrupt(USART2_DEVICE);
}

ISR(USART2_TX_vect)
{
	tx_interrupt(USART2_DEVICE);
}

ISR(USART2_UDRE_vect)
{
	udre_interrupt(USART2_DEVICE);
}
#endif /* CONFIG_DRIVERS_USART2_ASYNC */

#if CONFIG_DRIVERS_USART3_ASYNC
ISR(USART3_RX_vect)
{
	rx_interrupt(USART3_DEVICE);
}

ISR(USART3_TX_vect)
{
	tx_interrupt(USART3_DEVICE);
}

ISR(USART3_UDRE_vect)
{
	udre_interrupt(USART3_DEVICE);
}
#endif /* CONFIG_DRIVERS_USART3_ASYNC */

int usart_set_callback(UART_Device *dev, usart_async_callback_t cb)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	usart_async_contexts[AVR_USARTn_INDEX(dev)].callback = cb;

	return 0;
}

int usart_rx_enable(UART_Device *dev, void *buf, size_t size)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	usart_async_contexts[AVR_USARTn_INDEX(dev)].rx.buf  = (uint8_t *)buf;
	usart_async_contexts[AVR_USARTn_INDEX(dev)].rx.size = size;
	usart_async_contexts[AVR_USARTn_INDEX(dev)].rx.cur  = 0U;

	/* enable receiver */
	SET_BIT(dev->UCSRnB, BIT(RXENn));

	return 0;
}

int usart_rx_disable(UART_Device *dev)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	/* disable receiver */
	CLR_BIT(dev->UCSRnB, BIT(RXENn));

	return 0;
}

int usart_tx(UART_Device *dev, const void *buf, size_t size)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	usart_async_contexts[AVR_USARTn_INDEX(dev)].tx.buf  = (const uint8_t *)buf;
	usart_async_contexts[AVR_USARTn_INDEX(dev)].tx.size = size;
	usart_async_contexts[AVR_USARTn_INDEX(dev)].tx.cur  = 0U;

	/* enable transmitter */
	SET_BIT(dev->UCSRnB, BIT(UDRIEn));

	return 0;
}

#endif /* DRIVERS_UART_ASYNC */