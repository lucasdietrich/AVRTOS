/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void waiting_thread(k_timeout_t *timeout);

k_timeout_t timeouts[3] = {
  K_SECONDS(2),
  K_SECONDS(1),
  K_SECONDS(1)
};

K_THREAD_DEFINE(waiter1, waiting_thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), &timeouts[0], 'A');
K_THREAD_DEFINE(waiter2, waiting_thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), &timeouts[1], 'B');
K_THREAD_DEFINE(waiter3, waiting_thread, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), &timeouts[2], 'C');
K_MUTEX_DEFINE(mymutex);

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  k_mutex_lock(&mymutex, K_NO_WAIT);

  irq_enable();

  k_sleep(K_SECONDS(1));

  k_mutex_unlock(&mymutex);

  k_sleep(K_FOREVER);
}

void waiting_thread(k_timeout_t *timeout)
{
  usart_transmit(_current->symbol);
  usart_printl(": starting");
  
  uint8_t locked = k_mutex_lock(&mymutex, *timeout);

  usart_transmit(_current->symbol);
  if (locked == 0) {
    usart_printl(": locked the mutex !");
  } else {
    usart_printl(": didn't get the mutex !");
  }

  k_sleep(K_FOREVER);
}
