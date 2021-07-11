/*___________________________________________________________________________*/

#include <util/delay.h>

#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/multithreading.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void thread_led_on(void *p);
void thread_led_off(void *p);
void exit(uint8_t err);

K_THREAD_DEFINE(ledon, thread_led_on, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);
K_THREAD_DEFINE(ledoff, thread_led_off, 0x100, K_PRIO_DEFAULT, nullptr, nullptr);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  // USART_DUMP_RAM_ALL();
  // USART_DUMP_CORE();

  k_thread_dump_all();

  init_sysclock();

  sei();

  // uint16_t ra = read_ra();
  // usart_hex16(ra);

  USART_DUMP_CORE();

  while(1)
  {
    usart_transmit('_');

    k_yield();
  }
}

void thread_led_on(void *p)
{
  while (1)
  {
    LOG_SREG_I();
    led_on();
    usart_transmit('O');
    _delay_ms(1000);

    // k_yield();
  }
}

void thread_led_off(void *p)
{
  while (1)
  {
    LOG_SREG_I();
    if ((SREG & (1 << SREG_I)) == 0)
    {
      exit(0);
    }
    led_off();
    usart_transmit('F');
    _delay_ms(1000);
    

    // k_yield();
  }
}

void exit(uint8_t err)
{
  USART_DUMP_CORE();

  k_thread_dump_all();

  usart_printl("stop");

  while(1) { }
}

/*___________________________________________________________________________*/