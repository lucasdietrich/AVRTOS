/**
 * @file main.cpp
 * @author Dietrich Lucas (ld.adecy@gmail.com)
 * @brief Measurement frequency of the led in order to know at whch frequency does the kernel switch thread
 * @version 0.1
 * @date 2021-07-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/*___________________________________________________________________________*/

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/multithreading.h"

/*___________________________________________________________________________*/

void thread_led(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  while(1)
  {
    led_on();
    k_yield();
  }
}

void thread_led(void *p)
{
  while (1)
  {
    led_off();
    k_yield();
  }
}

/*___________________________________________________________________________*/