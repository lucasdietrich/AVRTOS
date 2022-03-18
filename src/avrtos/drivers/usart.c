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

static void set_ubrr(UART_Device *dev, uint16_t ubrr)
{
	dev->UBRRnH = (uint8_t)(ubrr >> 8);
	dev->UBRRnL = (uint8_t)ubrr;
}


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
        uint16_t ubrr = calculate_ubrr(baudrate, speed_mode);

        set_ubrr(dev, ubrr);
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
        if ((config->databits & 0x4u) && (config->databits != USART_DATABITS_9)) {
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