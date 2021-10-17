/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#define USE_SCHED_LOCK_TRICK 0

/*___________________________________________________________________________*/

void thread_blink(void *p);
void thread_coop(void *p);

K_THREAD_DEFINE(blink, thread_blink, 0x100, K_PREEMPTIVE, NULL, 'B');
K_THREAD_DEFINE(coop, thread_coop, 0x100, K_PREEMPTIVE, NULL, 'C');

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  print_runqueue();

  sei();

  while (1)
  {
  
#if USE_SCHED_LOCK_TRICK
    K_SCHED_LOCK_CONTEXT
    {
#else
      k_sched_lock();
#endif
      usart_printl("k_sched_lock()");

      _delay_ms(500);

      usart_printl("k_sched_unlock()");

#if USE_SCHED_LOCK_TRICK == 0
      k_sched_unlock();
#else
    }
#endif

    _delay_ms(2000);
  }
}

void thread_blink(void *p)
{
  while (1)
  {
    usart_transmit('o');
    led_on();

    k_sleep(K_MSEC(100));

    usart_transmit('f');
    led_off();

    k_sleep(K_MSEC(100));
  }
}

void thread_coop(void *p)
{
  while (1)
  {
    k_sleep(K_MSEC(5000));

    usart_printl("full cooperative thread");

    _delay_ms(1000);
  }
}

/*___________________________________________________________________________*/