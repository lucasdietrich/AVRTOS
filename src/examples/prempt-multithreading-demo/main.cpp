/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/multithreading.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void thread_led_on(void *p);
void thread_led_off(void *p);

K_THREAD_DEFINE(ledon, thread_led_on, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr);
K_THREAD_DEFINE(ledoff, thread_led_off, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  k_thread_dump_all();
  
  sei();

  while(1)
  {
    k_thread_dump_all();

    usart_transmit('\n');
    usart_transmit('_');

    _delay_ms(1000);
  }
}

void thread_led_on(void *p)
{
  while (1)
  {
    led_on();
    
    k_thread_dump_all();

    usart_transmit('\n');
    usart_transmit('\\');

    _delay_ms(1000);
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    led_off();

    k_thread_dump_all();

    usart_transmit('\n');
    usart_transmit('/');

    _delay_ms(1000);
  }
}

/*___________________________________________________________________________*/