#include <avr/io.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>

/*___________________________________________________________________________*/

#include "uart.h"
#include "led.h"
#include "multithreading.h"
#include "utils.h"

#include "debug.h"

/*___________________________________________________________________________*/

#define MAIN_THREAD     0
#define SECOND_THREAD   1

thread_t threads[2];

void thread(void *p);

/*___________________________________________________________________________*/

void(*function_p)(void) = testfunction;

typedef struct {
  uint32_t context = 0x89ABCDEF;
} context_t;

static context_t thread_context;

// threadA
int main(void)
{
  led_init();
  usart_init();
  
  //////////////////////////////////////////////

  thread_create(&threads[SECOND_THREAD], thread, THREAD_2ND_STACK_LOC, (void*) &thread_context);

  usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP);

  //////////////////////////////////////////////

  usart_dbg_hex16("thread 2 address : ", (uint16_t) thread);
  usart_dbg_hex16("thread 2 stack context : ", (uint16_t) &thread_context);
  usart_dbg_hex16("thread 2 stack pointer address : ", (uint16_t) threads[SECOND_THREAD].sp);
  usart_dbg_hex16("thread 2 stack base address : ", THREAD_2ND_STACK_LOC);

  //////////////////////////////////////////////


  while(1)
  {
    usart_dbg_hex16("#1 loop : ", SP);

    led_on();

    _delay_ms(100.0);

    led_off();

    _delay_ms(100.0);

    thread_switch(&threads[MAIN_THREAD], &threads[SECOND_THREAD]);
  }
}

/*___________________________________________________________________________*/


void thread(void *p)
{
  usart_send_hex((const char*) (uint8_t*) &((context_t*)p)->context, 4);

  while(1)
  {
    usart_dbg_hex16("#2 loop SP = ", SP);

    led_on();

    _delay_ms(500.0);

    led_off();

    _delay_ms(500.0);

    thread_switch(&threads[SECOND_THREAD], &threads[MAIN_THREAD]);
  }
}