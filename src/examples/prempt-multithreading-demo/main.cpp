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
void thread_idle(void *p);

K_THREAD_DEFINE(ledon, thread_led_on, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr);
K_THREAD_DEFINE(ledoff, thread_led_off, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr);
K_THREAD_DEFINE(idle, thread_idle, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  k_scheduler_init();

  led_init();
  usart_init();
  k_thread_dump_all();

  _delay_ms(5000);
  
  sei();

  while(1)
  {
    // k_thread_dump_all();
    // usart_transmit('\n');

    // k_sleep(K_MSEC(1000));
    
    usart_transmit('_'); 

    _delay_ms(1000);

    k_yield();
  }
}

void thread_led_on(void *p)
{
  while (1)
  {
    led_on();
    
    // k_thread_dump_all();
    // usart_transmit('\n');

    // k_sleep(K_MSEC(1000));

    usart_transmit('\\');

    _delay_ms(1000);

    k_yield();
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    led_off();

    // k_thread_dump_all();
    // usart_transmit('\n');

    // k_sleep(K_MSEC(1000));

    usart_transmit('/');

    _delay_ms(1000);

    k_yield();
  }
}

void thread_idle(void *p)
{
  while(1) { }
}

/*___________________________________________________________________________*/