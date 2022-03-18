#ifndef _AVRTOS_DRIVERS_H_
#define _AVRTOS_DRIVERS_H_

#include <stddef.h>
#include <avr/io.h>

#define __IO volatile

#define AVR_IO_BASE_ADDR 0x0000U

// AVR_UART_BASE_ADDR is actually UCSR0A
#define AVR_UART_BASE_ADDR (AVR_IO_BASE_ADDR + 0x00C0U)

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
#elif defined(__AVR_ATmega2560__)
#	define ARCH_USART_COUNT 4
#else
#	warning "Unsupported MCU"
#endif



#define AVR_UARTn_BASE(n) ((UART_Device*) (AVR_UART_BASE_ADDR + (n)*sizeof(UART_Device)))

#define AVR_UARTn_INDEX(usart_dev) (usart_dev - AVR_UARTn_BASE(0))

#if ARCH_USART_COUNT > 0
#	define UART0_DEVICE AVR_UARTn_BASE(0)
#endif /* ARCH_USART_COUNT > 0 */

#if ARCH_USART_COUNT > 1
#	define UART1_DEVICE AVR_UARTn_BASE(1)
#endif /* ARCH_USART_COUNT > 1 */

#if ARCH_USART_COUNT > 2
#	define UART2_DEVICE AVR_UARTn_BASE(2)
#endif /* ARCH_USART_COUNT > 2 */

#if ARCH_USART_COUNT > 3
#	define UART3_DEVICE AVR_UARTn_BASE(2)
#endif /* ARCH_USART_COUNT > 3 */

#endif /* _AVRTOS_DRIVERS_H_ */