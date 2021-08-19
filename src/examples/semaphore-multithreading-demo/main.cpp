/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/kernel.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

#define PERIOD_SEM_GIVE   100
#define PERIOD_SEM_TAKE   1000

/*___________________________________________________________________________*/

void waiter_entry(void*);

/*___________________________________________________________________________*/

K_SEM_DEFINE(mysem, 5, 5);
K_THREAD_DEFINE(waiter1, waiter_entry, 0x50, K_PRIO_PREEMPT(K_PRIO_LOW), NULL, NULL, 'A');
K_THREAD_DEFINE(waiter2, waiter_entry, 0x50, K_PRIO_PREEMPT(K_PRIO_LOW), NULL, NULL, 'B');
K_THREAD_DEFINE(waiter3, waiter_entry, 0x50, K_PRIO_PREEMPT(K_PRIO_LOW), NULL, NULL, 'C');
K_THREAD_DEFINE(waiter4, waiter_entry, 0x50, K_PRIO_PREEMPT(K_PRIO_LOW), NULL, NULL, 'D');
K_THREAD_DEFINE(waiter5, waiter_entry, 0x50, K_PRIO_PREEMPT(K_PRIO_LOW), NULL, NULL, 'E');

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  k_thread_dump_all();
  
  sei();

  while(1)
  {
    k_sleep(K_MSEC(PERIOD_SEM_GIVE));

    k_sched_lock();
    usart_print("M: giving a semaphore");
    k_sem_debug(&mysem);
    k_sem_give(&mysem);
    k_sched_unlock();
  }
}

void waiter_entry(void*)
{
  while(1)
  {
    k_sem_take(&mysem, K_FOREVER);

    k_sched_lock();
    usart_transmit(k_current->symbol);
    usart_printl(": got a semaphore !");
    k_sched_unlock();

    k_sleep(K_MSEC(PERIOD_SEM_TAKE));
  }
}

/*___________________________________________________________________________*/