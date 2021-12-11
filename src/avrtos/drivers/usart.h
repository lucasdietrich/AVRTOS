#ifndef _AVRTOS_USART_H
#define _AVRTOS_USART_H

#include <stddef.h>
#include <avrtos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#define USART_MODE_ASYNCHRONOUS 0
#define USART_MODE_SYNCHRONOUS  1
#define USART_MODE_MSPI         2

#define USART_PARITY_NONE      0
#define USART_PARITY_EVEN      2
#define USART_PARITY_ODD       3

#define USART_STOPBITS_1        0
#define USART_STOPBITS_2        1

#define USART_DATABITS_5        0
#define USART_DATABITS_6        1
#define USART_DATABITS_7        2
#define USART_DATABITS_8        3
#define USART_DATABITS_9        7

#define USART_SPEED_MODE_NORMAL 0
#define USART_SPEED_MODE_DOUBLE 1

/* TODO make universal */
#if defined(__AVR_ATmega328P__)
#define ARCH_USART_COUNT 1
#elif defined(__AVR_ATmega2560__)
#define ARCH_USART_COUNT 4
#else
#error "Unsupported MCU"
#endif

struct usart_config
{
        uint32_t baudrate;
        uint8_t receiver: 1;
        uint8_t transmitter: 1;

        uint8_t mode: 2;

        uint8_t parity: 2;
        uint8_t stopbits: 1;
        uint8_t databits: 3;

        uint8_t speed_mode: 1;
};

#define USART_CONFIG_DEFAULT_2400() { \
        .baudrate = USART_BAUD_2400, \
        .receiver = 1, \
        .transmitter = 1, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOPBITS_1, \
        .databits = USART_DATABITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT_9600() { \
        .baudrate = USART_BAUD_9600, \
        .receiver = 1, \
        .transmitter = 1, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOPBITS_1, \
        .databits = USART_DATABITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT_115200() { \
        .baudrate = USART_BAUD_115200, \
        .receiver = 1, \
        .transmitter = 1, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOPBITS_1, \
        .databits = USART_DATABITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT_500000() { \
        .baudrate = USART_BAUD_500000, \
        .receiver = 1, \
        .transmitter = 1, \
        .mode = USART_MODE_ASYNCHRONOUS, \
        .parity = USART_PARITY_NONE, \
        .stopbits = USART_STOPBITS_1, \
        .databits = USART_DATABITS_8, \
        .speed_mode = USART_SPEED_MODE_NORMAL \
}

#define USART_CONFIG_DEFAULT USART_CONFIG_DEFAULT_500000

K_NOINLINE int usart_drv_init(uint8_t usart_id,
                              const struct usart_config *config);

K_NOINLINE int usart_drv_deinit(uint8_t usart_id);

K_NOINLINE int usart_drv_sync_putc(uint8_t usart_id, char c);

K_NOINLINE void usart0_drv_sync_putc_opt(char c);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_USART_H */