/**
 * @file main.cpp
 * @author Dietrich Lucas (ld.adecy@gmail.com)
 * @brief Two thread increment two counters. Threads are preempted by the system clock.
 * If configuration option KERNEL_DEBUG_PREEMPT_UART=1 is enabled :
 * send a character over the UART to preempt the thread currently running and switch to the other.
 * 
 * if KERNEL_DEBUG_PREEMPT_UART is enabled and DCONFIG_KERNEL_TIME_SLICE=250000 :
 * - on every 4 chars sent on the UART, the led is toggled
 * @version 0.1
 * @date 2021-08-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void thread_led_toggle(void *p);
void thread_processing(void *p);

K_THREAD_DEFINE(led, thread_led_toggle, 0x100, K_PREEMPTIVE, NULL, 'L');
K_THREAD_DEFINE(task1, thread_processing, 0x100, K_PREEMPTIVE, NULL, 'A');
K_THREAD_DEFINE(task2, thread_processing, 0x100, K_PREEMPTIVE, NULL, 'B');

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

#if KERNEL_DEBUG_PREEMPT_UART
  usart_printl_p(PSTR(" Send a char over the UART to switch thread !"));
#endif

  k_sleep(K_FOREVER);
}

void thread_led_toggle(void *p)
{
  while (1)
  {
    led_on();

    k_sleep(K_MSEC(1000));  
    // if KERNEL_DEBUG_PREEMPT_UART is enabled and DCONFIG_KERNEL_TIME_SLICE=250000 :
    // - on every 4 chars sent on the UART, the led is toggled

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
      k_sched_lock();
      usart_transmit(_current->symbol);
      usart_print_p(PSTR(": "));
      usart_hex16(counter >> 16);
      usart_print_p(PSTR("0000\n"));
      k_sched_unlock();
    }
  }
}

/*___________________________________________________________________________*/