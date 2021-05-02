#include "uart.h"

void usart_init()
{
  // set baud rate (function of oscillator frequency)
  UBRR0H = (uint8_t) (MH16_BAUDRATE_UBRR >> 8);
  UBRR0L = (uint8_t) MH16_BAUDRATE_UBRR;

  // enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // USART Control and Status Register n C
  //  - set Asynchronous USART : UMSEL01 = UMSEL00 = 0
  //  - 8 bit format : (3 << UCSZ00)
  //  - configure stop 1 bit : (0<<USBS0)
  UCSR0C = (0<<USBS0) | (3 << UCSZ00);
}

void usart_transmit(char data)
{
  // see datasheet : wait for empty transmit buffer
  // - UCSR0A – USART Control and Status Register n A
  // - UDRE0: USART Data Register Empty
  while (!(UCSR0A & ( 1<<UDRE0))); // while flag UDRE0 of UCSR0A is not set

  // put data into buffer
  UDR0 = data;
}

void usart_send(const char* buffer, size_t len)
{
  for(uint_fast8_t i = 0; i < len; i++)
  {
    usart_transmit(buffer[i]);
  }
}

void usart_hex(const uint8_t val)
{
  static const char alpha16[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  const char high = alpha16[val >> 4];
  const char low = alpha16[val & 0xF];

  usart_transmit(high);
  usart_transmit(low);
}

void usart_hex16(const uint16_t val)
{
  usart_hex((uint8_t) (val >> 8));
  usart_hex((uint8_t) (val & 0xFF));
}

void usart_send_hex(const uint8_t* buffer, size_t len)
{
  for(uint_fast8_t i = 0; i < len; i++)
  {
    usart_hex(buffer[i]);

    if (i != len)
    {
      if (0 == (i & 0xF))
      {
        usart_transmit('\n');
      }
      else
      {
        usart_transmit(' ');
      }
    }
  }
}