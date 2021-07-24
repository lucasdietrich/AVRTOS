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
K_THREAD_DEFINE(idle, thread_idle, 0x100, K_PRIO_PREEMPT(8), nullptr, nullptr, 'I');

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  usart_printl("strart");
  
  k_thread_dump_all();
  
  sei();

  while(1)
  {
    // print_runqueue();

    // _delay_ms(1000);

    k_yield();
    
    // k_sleep(K_MSEC(50));

    // k_yield();
  }
}

// still a problem when having preemp interrupt

void thread_led_on(void *p)
{
  while (1)
  {
    print_runqueue();

    led_on();

    k_sleep(K_MSEC(1000));

    // _delay_ms(1000);

    // k_yield();
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    print_runqueue();

    led_off();

    k_sleep(K_MSEC(1000));

    // _delay_ms(1000);

    // k_yield();
  }
}

void thread_idle(void *p)
{
  while(1) {
    usart_printl("IDLE");
    _delay_ms(1000);
  }
}

/*___________________________________________________________________________*/