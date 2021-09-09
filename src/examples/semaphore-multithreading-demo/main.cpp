/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

#define CRASH_TEST  1

#if CRASH_TEST
#define PERIOD_SEM_GIVE 1000
#define PERIOD_SEM_TAKE PERIOD_SEM_GIVE
#else
#define PERIOD_SEM_GIVE   1000
#define PERIOD_SEM_TAKE   1000
#endif

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
#if !KERNEL_SCHEDULER_DEBUG
    usart_print("M: giving a semaphore ");
#endif
    // k_sem_debug(&mysem);
    k_sched_unlock();

    k_sem_give(&mysem);
  }
}

void waiter_entry(void*)
{
  while(1)
  {
    uint8_t dbg_sem = k_sem_take(&mysem, K_FOREVER);

    if(dbg_sem == 0)
    {
      k_sched_lock();
#if !KERNEL_SCHEDULER_DEBUG
      usart_transmit(_current->symbol);
      usart_printl(": got a semaphore !");
#endif
      k_sched_unlock();

      k_sleep(K_MSEC(PERIOD_SEM_TAKE));
    }
    else
    {
      usart_printl("DIDN'T TOOK A SEMAPHORE, KERNEL PROBLEM");
    }
  }
}

/*___________________________________________________________________________*/