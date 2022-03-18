#include "usart.h"

#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

/* same for all USARTs */
#define TXENn TXEN0
#define RXENn RXEN0
#define UMSELn0 UMSEL00
#define UMSELn1 UMSEL01
#define UPMn0 UPM00
#define UPMn1 UPM01
#define USBSn USBS0
#define UCSZn0 UCSZ00
#define UCSZn1 UCSZ01
#define UCSZn2 UCSZ02
#define UDREn UDRE0

/* check U2Xn bit which allow to double the speed */
static uint16_t calculate_ubrr(uint32_t baudrate, bool speed_mode)
{
        /* TODO use switch case or array ? */
        if (speed_mode) {
                return USART_CALC_SPEED_MODE_UBRRn(baudrate);
        } else {
                return USART_CALC_UBRRn(baudrate);
        }
}

static void set_baudrate(UART_Device *dev,
                         uint32_t baudrate,
                         bool speed_mode)
{
        const uint16_t ubrr = calculate_ubrr(baudrate, speed_mode);

	dev->UBRRnH = (uint8_t)(ubrr >> 8);
	dev->UBRRnL = (uint8_t)ubrr;
};

int usart_drv_init(UART_Device *dev,
                   const struct usart_config *config)
{
        if (dev == NULL) {
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

        /* set baudrate */
        set_baudrate(dev, config->baudrate, config->speed_mode);

        /* enable receiver and transmitter */
        uint8_t ucsrnb = 0u;
        if (config->transmitter)
                SET_BIT(ucsrnb, BIT(TXENn));
        if (config->receiver)
                SET_BIT(ucsrnb, BIT(RXENn));
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

        return 0;
}

int usart_drv_deinit(UART_Device *dev)
{
        if (dev == NULL) {
                return -EINVAL;
        }

	dev->UCSRnA = 0U;
	dev->UCSRnB = 0U;
	dev->UCSRnC = 0U;

        return 0;
}

/*
 * Problem is that execution time is much longer than the old "usart_transmit",
 * because of fetch IO address for the proper USART from flash.
 */

int usart_drv_sync_putc(UART_Device *dev, char c)
{
	if (dev == NULL) {
		return -EINVAL;
	}

        /* wait for empty transmit buffer */
	while (!(dev->UCSRnA & BIT(UDREn)));

        /* put data into HW buffer, sends the data */
	dev->UDRn = c;

        return 0;
}

/* as fast as usart_transmit */
void usart0_drv_sync_putc_opt(char c)
{
	while (!(UART0_DEVICE->UCSRnA & BIT(UDREn)));
	UDR0 = c;
}

#if DRIVERS_UART_ASYNC

// size should be configurable
static struct usart_async_context usart_async_contexts[ARCH_USART_COUNT];

static struct usart_async_context* usart_get_async_context(UART_Device *dev)
{
	return &usart_async_contexts[AVR_UARTn_INDEX(dev)];
}

static void rx_interrupt(UART_Device *dev)
{
	const char chr = dev->UDRn;

	struct usart_async_context *ctx = usart_get_async_context(dev);

	ctx->buf.data[ctx->buf.cur++] = chr;

	if (ctx->buf.cur == ctx->buf.size) {
		ctx->rx_callback(dev, ctx);

		ctx->buf.cur = 0U;
	}
}

// ISR(USART0_RX_vect)
// {
// 	rx_interrupt(UART0_DEVICE);
// }

ISR(USART1_RX_vect)
{
	rx_interrupt(UART1_DEVICE);
}

// ISR(USART2_RX_vect)
// {
// 	rx_interrupt(UART0_DEVICE);
// }

// ISR(USART3_RX_vect)
// {
// 	rx_interrupt(UART0_DEVICE);
// }

// same callback for all USARTs
int usart_set_callback(UART_Device *dev, usart_rx_callback cb)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	usart_async_contexts[AVR_UARTn_INDEX(dev)].rx_callback = cb;

	return 0;
}

int usart_rx_enable(UART_Device *dev, void *buf, size_t buffer_size)
{
	if (dev == NULL) {
		return -EINVAL;
	}

	usart_async_contexts[AVR_UARTn_INDEX(dev)].buf.data = (uint8_t*) buf;
	usart_async_contexts[AVR_UARTn_INDEX(dev)].buf.size = buffer_size;
	usart_async_contexts[AVR_UARTn_INDEX(dev)].buf.cur = 0U;

	/* enable receiver */
	SET_BIT(dev->UCSRnB, BIT(RXENn));

	return 0;
}

#endif /* DRIVERS_UART_ASYNC */