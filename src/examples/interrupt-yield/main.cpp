/*___________________________________________________________________________*/

#include <util/delay.h>

#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/multithreading.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void thread_led_on(void *p);
void thread_led_off(void *p);

K_THREAD_DEFINE(ledon, thread_led_on, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);
K_THREAD_DEFINE(ledoff, thread_led_off, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);

/*___________________________________________________________________________*/

// ISR(USART_RX_vect)
// {
//   usart_transmit(UDR0);
// }

int main(void)
{
  led_init();
  usart_init();

  //enable UART interrupt on RX
  UCSR0B |= 1 << RXCIE0;

  usart_printl("main");

  init_sysclock();

  sei();

  while(1) {
    LOG_SREG_I();
    usart_u16(_sysclock_counter);
    usart_transmit('\n');
    // k_yield();

    _delay_ms(1000);
  }

  while(1)
  {
    k_yield();
  }
}

void thread_led_on(void *p)
{
  sei();  // default SREG is 0 need to set it TODO
  while (1)
  {
    LOG_SREG_I();
    led_on();
    usart_print("ON");
    _delay_ms(500);
    

    k_yield();
  }
}

void thread_led_off(void *p)
{
  sei();  // default SREG is 0 need to set it TODO
  while (1)
  {
    LOG_SREG_I();
    led_off();
    usart_print("OFF");
    _delay_ms(500);

    k_yield();
  }
}

/*___________________________________________________________________________*/