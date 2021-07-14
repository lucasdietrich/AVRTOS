/*___________________________________________________________________________*/

#include <util/delay.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/multithreading.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void thread_led(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  print_scheduled_items_list(_k_system_xtqueue);

  k_yield();

  while(1)
  {
    k_xtqueue_shift(&_k_system_xtqueue, 10);
    // print_scheduled_items_list();
    k_xtqueue_item_t *item = k_xtqueue_pop(&_k_system_xtqueue);
    if (item != NULL)
    {
      // usart_hex16((uint16_t) item->p);
      // usart_print(" expected : ");
      // usart_hex16((uint16_t) k_thread.list[1]);
      // usart_transmit('\n');


      // this function should append the thread to the list of pending threads and the scheduler choose which one to execute
      
      k_yield();
    }

    _delay_ms(10);
  }
}

void thread_led(void *p)
{
  usart_print("start");

  while (1)
  {
    k_sleep(K_MSEC(500));
    led_off();
    usart_printl("OFF");

    k_sleep(K_MSEC(500));
    led_on();
    usart_printl("ON");
  }
}

/*___________________________________________________________________________*/