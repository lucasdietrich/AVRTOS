#include <avr/io.h>
#include <util/delay.h>
#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>

void thread_led(void *context);
void thread_coop(void *context);

uint8_t on = 1u;
uint8_t off = 0u;

K_MUTEX_DEFINE(mymutex);  // mutex protecting LED access
K_THREAD_DEFINE(ledon, thread_led, 0x50, K_PRIO_PREEMPT(K_PRIO_HIGH), (void *)&on, nullptr, 'O');
K_THREAD_DEFINE(ledoff, thread_led, 0x50, K_PRIO_PREEMPT(K_PRIO_HIGH), (void *)&off, nullptr, 'F');
K_THREAD_DEFINE(coop, thread_coop, 0x100, K_PRIO_COOP(K_PRIO_HIGH), nullptr, nullptr, 'C');

int main(void)
{
  led_init();
  usart_init();
  k_thread_dump_all();
  sei();
  k_sleep(K_FOREVER);
}

void thread_led(void *context)
{
  const uint8_t thread_led_state = *(uint8_t*)context;
  while(1)
  {
    k_mutex_lock(&mymutex, K_FOREVER);
    led_set(thread_led_state);
    usart_transmit(thread_led_state ? 'o' : 'f');
    k_sleep(K_MSEC(100));
    k_mutex_unlock(&mymutex);
  }
}

void thread_coop(void*)
{
  while(1)
  {
    k_sleep(K_MSEC(2000));
    usart_transmit('_');
    _delay_ms(500); // blocking all threads for 500ms
  }
}