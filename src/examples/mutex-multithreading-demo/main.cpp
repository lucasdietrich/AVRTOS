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

K_MUTEX_DEFINE(mymutex);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  print_runqueue();
  print_events_queue();

  mutex_lock(&mymutex);

  sei();

  k_sleep(K_MSEC(5000));

  print_runqueue();
  print_events_queue();

  mutex_release(&mymutex);

  usart_printl("get back");

  print_runqueue();

  k_sleep(K_FOREVER);
}

void thread_led_on(void *p)
{
  while (1)
  {
    print_runqueue();

    while (mutex_lock_wait(&mymutex, K_NO_WAIT) != 0)
    {
      print_runqueue();

      k_sleep(K_MSEC(3000));

      usart_print("[OWM]");

      _delay_ms(1000);
    }

    usart_print("[OGM]");
    
    led_on();

    k_sleep(K_MSEC(2000));

    usart_print("[ORM]");

    mutex_release(&mymutex);
  }
}

void thread_led_off(void *p)
{
  if (mutex_lock_wait(&mymutex, K_MSEC(10000)) == 0)
  {
    usart_print("[FGM]");
  }
  else
  {
    usart_print("[FDM]");
  }

  while (1)
  {
    // if (mutex_lock_wait(&mymutex, K_NO_WAIT) == 0)
    // {
    //   usart_print("[F : got mutex]");

    //   k_sleep(K_MSEC(1000));

    //   mutex_release(&mymutex);
    // }
    // else
    // {
    //   usart_print("[F : didn't get mutex]");

    //   k_sleep(K_MSEC(1000));
    // }

    // while (mutex_lock_wait(&mymutex, K_NO_WAIT) != 0)
    // {
    //   k_sleep(K_MSEC(1000));

    //   usart_print("F : WAITING MUTEX");
    // }

    // usart_print("F : GOT MUTEX");
    
    // led_off();

    // k_sleep(K_MSEC(2000));

    // usart_print("F : RELEASE MUTEX");

    // mutex_release(&mymutex);
  }
}

void thread_idle(void *p)
{
  while(1) {
    // print_runqueue();
    // print_eventqueue();

    usart_print(";");
    _delay_ms(1000);
  }
}

/*___________________________________________________________________________*/