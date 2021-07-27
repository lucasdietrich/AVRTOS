/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/kernel.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void thread_led_on(void *p);
void thread_led_off(void *p);
void thread_idle(void *p);

K_THREAD_DEFINE(ledon, thread_led_on, 0x120, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'O');
K_THREAD_DEFINE(ledoff, thread_led_off, 0x140, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'F');
K_THREAD_DEFINE(idle, thread_idle, 0x160, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'I');

/*___________________________________________________________________________*/

extern struct thread_t __k_threads_start;
extern struct thread_t __k_threads_end;

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  print_runqueue();

  usart_print("\n__k_threads_start = 0x");
  usart_hex16((uint16_t) &__k_threads_start);
  usart_print("\n__k_threads_end = 0x");
  usart_hex16((uint16_t) &__k_threads_end);
  usart_print("\n__k_threads_end - __k_threads_start = 0x");
  usart_hex16(((uint16_t) &__k_threads_end) - ((uint16_t) &__k_threads_start));
  usart_print("\nsizeof(struct thread_t) = 0x");
  usart_hex16((uint16_t) sizeof(struct thread_t));

  sei();

  while(1)
  {

  }
}

void thread_led_on(void *p)
{
  while(1)
  {
    led_on();
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    led_off();
  }
}

void thread_idle(void *p)
{
  while(1) {
    usart_print("S");
    _delay_ms(1000);
  }
}

/*___________________________________________________________________________*/