#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

void thread(void *p);

K_THREAD_DEFINE(ledon, thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'T');

/*___________________________________________________________________________*/

K_MUTEX_DEFINE(mymutex);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  print_runqueue();

  k_mutex_lock(&mymutex, K_NO_WAIT);

  sei();

  k_sleep(K_SECONDS(4));

  k_mutex_unlock(&mymutex);

  k_sleep(K_FOREVER);
}

void thread(void *p)
{
  uint8_t lock = k_mutex_lock(&mymutex, K_SECONDS(3));

  if (lock)
  {
    usart_printl("Didn't get the mutex ...");
  }
  else
  {
    usart_printl("Got the mutex !");
  }
  
  k_sleep(K_FOREVER);
}

/*___________________________________________________________________________*/