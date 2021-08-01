/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/kernel.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void thread_led_toggle(void *p);
void thread_processing(void *p);

K_THREAD_DEFINE(led, thread_led_toggle, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'L');
K_THREAD_DEFINE(task1, thread_processing, 0x200, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, '1');
K_THREAD_DEFINE(task2, thread_processing, 0x200, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, '2');

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  k_sleep(K_FOREVER);
}

void thread_led_toggle(void *p)
{
  while (1)
  {
    print_runqueue();

    led_on();

    k_sleep(K_MSEC(1000));

    led_off();

    k_sleep(K_MSEC(1000));
  }
}

void thread_processing(void *p)
{
  uint32_t counter = 0;
  while (1)
  {
    counter++;

    if ((counter & 0xFFFFF) == 0)
    {
      usart_hex16(counter >> 16);
      usart_print("0000\n");
    }
  }
}

/*___________________________________________________________________________*/