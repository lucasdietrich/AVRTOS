/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/kernel.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void task_handler(struct k_work* self)
{
  k_sleep(K_SECONDS(1));

  usart_printl("Hello");
}

K_WORK_DEFINE(work, task_handler);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  sei();

  while(true)
  {
    k_system_workqueue_submit(&work);
    k_system_workqueue_submit(&work);

    k_sleep(K_SECONDS(2));
  }
}

/*___________________________________________________________________________*/