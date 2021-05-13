#include "uart.h"

static const char usart_alpha16[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

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

void usart_u8(const uint8_t val)
{
  const char hundred = usart_alpha16[(val / 100)];
  const char ten = usart_alpha16[(val / 10) % 10];
  const char unit = usart_alpha16[val % 10];

  if (val > 100)
  {
    usart_transmit(hundred);
  }
  if (val > 10)
  {
    usart_transmit(ten);
  }
  
  usart_transmit(unit);
}

void  usart_u16(uint16_t val)
{
  // return usart_hex16(val);

  char digits[5];

  uint8_t first_digit = 4u;

  for (uint_fast8_t p = 0; p < 5; p++)
  {
    char cur = usart_alpha16[val % 10];

    val /= 10;

    digits[4u - p] = cur;

    if (cur != '0')
    {
      first_digit = 4u - p;
    }
  }

  usart_send(&digits[first_digit], 5 - first_digit);
}

void usart_s8(const int8_t val)
{
  uint8_t u8_val;
  
  if (val == 0)
  {
    usart_transmit('0');

    return;
  }
  else if (val < 0)
  {
    u8_val = (uint8_t) (-val);
    usart_transmit('-');
  }
  else
  {
    u8_val = (uint8_t) (val);
  }
  usart_u8(u8_val);
}

void usart_hex(const uint8_t val)
{
  const char high = usart_alpha16[val >> 4];
  const char low = usart_alpha16[val & 0xF];

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
      if (0xF == (i & 0xF))
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

void usart_print(const char * text)
{
    usart_send(text, strlen(text));
}

void usart_printl(const char * text)
{
  usart_print(text);
  usart_transmit('\n');
}