#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

// if this example reset, there is a problem when mutex lock timeout check K_FLAG_TIMER_EXPIRED flag

void thread(void *p);
void threadp(void *p);

K_THREAD_DEFINE(thread1, thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), NULL, '1');
K_THREAD_DEFINE(thread2, thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), NULL, '2');
K_THREAD_DEFINE(thread3, thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), NULL, '3');
K_THREAD_DEFINE(thread4, threadp, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), NULL, '4');

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

  k_sleep(K_SECONDS(3));

  k_mutex_unlock(&mymutex);

  k_sleep(K_FOREVER);
}

void thread(void *p)
{
  uint8_t lock = k_mutex_lock(&mymutex, K_SECONDS(5));  // change this timeout

  usart_transmit(_current->symbol);

  if (lock)
  {
    usart_printl(" : Didn't get the mutex ...");
  }
  else
  {
    usart_printl(" : Got the mutex !");

    k_sleep(K_SECONDS(1));

    k_mutex_unlock(&mymutex);
  }
  
  k_sleep(K_FOREVER);
}

void threadp(void *p)
{
  uint8_t lock = k_mutex_lock(&mymutex, K_SECONDS(9));  // change this timeout

  usart_transmit(_current->symbol);

  if (lock)
  {
    usart_printl(" : Didn't get the mutex ...");
  }
  else
  {
    usart_printl(" : Got the mutex !");

    k_sleep(K_SECONDS(1));

    k_mutex_unlock(&mymutex);
  }

  
  k_sleep(K_FOREVER);
}

/*___________________________________________________________________________*/