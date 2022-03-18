#ifndef _AVRTOS_DRIVERS_H_
#define _AVRTOS_DRIVERS_H_

#include <stddef.h>
#include <avr/io.h>

#define __IO volatile

#define AVR_IO_BASE_ADDR 0x0000U

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

// UCSRnA
#define AVR_UARTn_BASE(n) ((UART_Device*) (AVR_IO_BASE_ADDR + 0x00C0U + (n)*sizeof(UART_Device)))

#define UART0_DEVICE AVR_UARTn_BASE(0)
#define UART1_DEVICE AVR_UARTn_BASE(1)
#define UART2_DEVICE AVR_UARTn_BASE(2)
#define UART3_DEVICE AVR_UARTn_BASE(2)

#endif /* _AVRTOS_DRIVERS_H_ */