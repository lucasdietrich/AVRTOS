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
  
  sei();

  while(1)
  {
    usart_print("M : "); 
    print_runqueue();

    _delay_ms(5000);
    
    k_sleep(K_MSEC(50));

    // k_yield();
  }
}

// still a problem when having preemp interrupt

void thread_led_on(void *p)
{
  while (1)
  {
    usart_print("O : "); 
    print_runqueue();

    led_on();

    _delay_ms(5000);

    k_yield();
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    usart_print("F : "); 
    print_runqueue();

    led_off();

    _delay_ms(5000);

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