/*___________________________________________________________________________*/

#include <util/delay.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/multithreading.h"
#include "avrtos/multithreading_debug.h"

/*___________________________________________________________________________*/

void thread_led(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  print_scheduled_items_list();

  k_yield();

  k_scheduled_item_t *item;

  while(1)
  {
    _k_schedule_time_passed(10);
    // print_scheduled_items_list();
    item = _k_schedule_pop_first_expired();
    if (item != NULL)
    {
      // usart_hex16((uint16_t) item->p);
      // usart_print(" expected : ");
      // usart_hex16((uint16_t) k_thread.list[1]);
      // usart_transmit('\n');

      k_yield();
    }

    _delay_ms(10);
  }
}

void thread_led(void *p)
{
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