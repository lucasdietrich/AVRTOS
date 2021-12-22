#include "uart.h"

#include <avr/pgmspace.h>

inline void _usart_init(const uint16_t baudrate_ubrr)
{
  // set baud rate (function of oscillator frequency)
        UBRR0H = (uint8_t)(baudrate_ubrr >> 8) & 0xF;
        UBRR0L = (uint8_t)baudrate_ubrr;

        // enable receiver and transmitter
        UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (0 << RXCIE0);

        // USART Control and Status Register n C
        //  - set Asynchronous USART : UMSEL01 = UMSEL00 = 0
        //  - 8 bit format : (3 << UCSZ00)
        //  - configure stop 1 bit : (0<<USBS0)
        //  - parity none (0 << UPM01) (if EVEN .pio monitor_flags = --parity E) : 
        // https://docs.platformio.org/en/latest/core/userguide/device/cmd_monitor.html#cmd-device-monitor
        UCSR0C = (0 << UPM01) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
}

void usart_init()
{
        _usart_init(UBRR);
}

void usart_transmit(char data)
{
  // see datasheet : wait for empty transmit buffer
  // - UCSR0A – USART Control and Status Register n A
  // - UDRE0: USART Data Register Empty
        while (!(UCSR0A & (1 << UDRE0))); // while flag UDRE0 of UCSR0A is not set

        // put data into buffer
        UDR0 = data;
}

void usart_send(const char *buffer, size_t len)
{
        for (uint_fast8_t i = 0; i < len; i++) {
                usart_transmit(buffer[i]);
        }
}

static char figure2hex(uint8_t value)
{
	return (value < 10) ? value + '0' : value + 'A' - 10;
}

void usart_u8(const uint8_t val)
{
        const char hundred = figure2hex(val / 100);
        const char ten = figure2hex((val / 10) % 10);
        const char unit = figure2hex(val % 10);

        if (val >= 100) {
                usart_transmit(hundred);
        }
        if (val >= 10) {
                usart_transmit(ten);
        }

        usart_transmit(unit);
}

void  usart_u16(uint16_t val)
{
        char digits[5];

        uint8_t first_digit = 4u;

        for (uint_fast8_t p = 0; p < 5; p++) {
                char cur = figure2hex(val % 10);

                val /= 10;

                digits[4u - p] = cur;

                if (cur != '0') {
                        first_digit = 4u - p;
                }
        }

        usart_send(&digits[first_digit], 5 - first_digit);
}

void usart_s8(const int8_t val)
{
        uint8_t u8_val;

        if (val == 0) {
                usart_transmit('0');

                return;
        } else if (val < 0) {
                u8_val = (uint8_t)(-val);
                usart_transmit('-');
        } else {
                u8_val = (uint8_t)(val);
        }
        usart_u8(u8_val);
}

void usart_hex(const uint8_t val)
{
        const char high = figure2hex(val >> 4);
        const char low = figure2hex(val & 0xF);

        usart_transmit(high);
        usart_transmit(low);
}

void usart_hex16(const uint16_t val)
{
        usart_hex((uint8_t)(val >> 8));
        usart_hex((uint8_t)val);
}

void usart_send_hex(const uint8_t *buffer, size_t len)
{
        for (uint_fast8_t i = 0; i < len; i++) {
                usart_hex(buffer[i]);

                if (0xF == (i & 0xF)) {
                        usart_transmit('\n'); // EOL every 16 chars
                } else {
                        usart_transmit(' ');
                }
        }
}

void usart_print(const char *text)
{
        usart_send(text, strlen(text));
}

void usart_printl(const char *text)
{
        usart_print(text);
        usart_transmit('\n');
}

void usart_send_p(const char *buffer, size_t len)
{
        for (uint_fast16_t i = 0; i < len; i++) {
                usart_transmit(pgm_read_byte(&buffer[i]));
        }
}

void usart_print_p(const char *text)
{
        usart_send_p(text, strlen_P(text));
}

void usart_printl_p(const char *text)
{
        usart_print_p(text);
        usart_transmit('\n');
}