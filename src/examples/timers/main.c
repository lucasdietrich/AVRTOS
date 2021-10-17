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

void handler1(struct k_timer* timer);
void handler2(struct k_timer* timer);
void thread2(void *context);
void work_handler(struct k_work* work);

K_WORK_DEFINE(mywork, work_handler);

K_TIMER_DEFINE(mytimer1, handler1, K_MSEC(100), 0);
K_TIMER_DEFINE(mytimer2, handler2, K_MSEC(100), K_TIMER_STOPPED);

K_THREAD_DEFINE(th2, thread2, 0x100, K_PRIO_PREEMPT(K_PRIO_MAX), NULL, 'A');

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

void handler1(struct k_timer* timer)
{
  counter++;
}

void handler2(struct k_timer* timer)
{
  counter2++;
  k_system_workqueue_submit(&mywork);
}

void work_handler(struct k_work* work)
{
  k_sleep(K_MSEC(200));

  usart_print("WORKQUEUE : ");
  usart_u16(counter2);
  usart_transmit('\n');
}

void thread2(void *context)
{
    k_sleep(K_SECONDS(5));
    k_timer_stop(&mytimer1);
    k_sleep(K_FOREVER);
}

/*___________________________________________________________________________*/