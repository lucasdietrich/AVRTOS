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

K_THREAD_DEFINE(ledon, thread_led_on, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr, 'O');
K_THREAD_DEFINE(ledoff, thread_led_off, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr, 'F');
// K_THREAD_DEFINE(idle, thread_idle, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  print_runqueue();
  
  sei();

  while(1)
  {
    print_runqueue();

    usart_transmit('_'); 

    _delay_ms(1000);
    
    k_sleep(K_MSEC(1000));

    // k_yield();
  }
}

void thread_led_on(void *p)
{
  while (1)
  {
    print_runqueue();

    led_on();

    usart_transmit('\\');

    _delay_ms(1000);

    k_yield();
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    print_runqueue();

    led_off();

    usart_transmit('/');

    _delay_ms(1000);

    k_yield();
  }
}

void thread_idle(void *p)
{
  while(1) {
    k_yield();
  }
}

/*___________________________________________________________________________*/