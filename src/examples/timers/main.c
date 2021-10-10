/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

volatile uint16_t counter = 0;
volatile uint16_t counter2 = 0;

void work_handler(struct k_work* work) {
  k_sleep(K_MSEC(200));
  
  usart_print("WORKQUEUE : ");
  usart_u16(counter2);
  usart_transmit('\n');
}

K_WORK_DEFINE(mywork, work_handler);

void handler1(struct k_timer* timer)
{
  counter++;
}

void handler2(struct k_timer* timer)
{
  counter2++;
  k_system_workqueue_submit(&mywork);
}

K_TIMER_DEFINE(mytimer1, handler1, K_MSEC(100), 0);
K_TIMER_DEFINE(mytimer2, handler2, K_MSEC(100), 0);

int main(void)
{
  led_init();
  usart_init();

  k_thread_dump_all();

  k_timer_start(&mytimer2, K_NO_WAIT);

  sei();

  for (;;) {
    irq_disable();
    usart_print("MAIN : ");
    usart_u16(counter);
    usart_transmit('\n');
    irq_enable();

    k_sleep(K_MSEC(1000));
  }
}

/*___________________________________________________________________________*/