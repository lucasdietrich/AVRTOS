#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>

/*___________________________________________________________________________*/

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/utils.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

uint8_t on = 1u;
uint8_t off = 0u;

void thread_led(void *p);
void thread_monitor(void *p);

#define THREAD_PREPROCESSOR 0

#if THREAD_PREPROCESSOR
K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, (void *)&on, 'O');
K_THREAD_DEFINE(ledoff, thread_led, 0x100, K_PRIO_DEFAULT, (void *)&off, 'F');
K_THREAD_DEFINE(monitor, thread_monitor, 0x100, K_PRIO_DEFAULT, NULL, 'R');
#else
K_THREAD static struct k_thread O;
K_THREAD static struct k_thread F;
K_THREAD static struct k_thread R;

static char stack1[0x100u];
static char stack2[0x100u];
static char stack3[0x100u];
#endif

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
 

#if !THREAD_PREPROCESSOR  
  k_thread_create(&O, thread_led, stack1, sizeof(stack1), K_PRIO_DEFAULT, (void *)&on, 'O');
  k_thread_create(&F, thread_led, stack2, sizeof(stack2), K_PRIO_DEFAULT, (void *)&off, 'F');
  k_thread_create(&R, thread_monitor, stack3, sizeof(stack3), K_PRIO_DEFAULT, NULL, 'R');

  k_start(&O);
  k_start(&F);
  k_start(&R);
#endif

  print_runqueue();

  // USART_DUMP_RAM_ALL();
  k_thread_dump_all();

  while(1)
  {
    usart_printl_p(PSTR("::main"));

    k_yield();
  }
}

/*___________________________________________________________________________*/

// use thread local storage
void inthread_setled(uint8_t state)
{
  if (state == 0)
  {
    led_off();
    usart_printl_p(PSTR("::thread off"));
  }
  else
  {
    led_on();
    usart_printl_p(PSTR("::thread on"));
  }
}

void thread_led(void *p)
{
  while (1)
  {
    inthread_setled(*(uint8_t*) p);

    _delay_ms(500);

    k_yield();
  }
}

void thread_monitor(void *p)
{
  while(1)
  {
    usart_printl_p(PSTR("::monitoring"));

    k_yield();
  }
}

/*___________________________________________________________________________*/