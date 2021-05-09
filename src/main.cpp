#include <avr/io.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>

/*___________________________________________________________________________*/

#include "uart.h"
#include "led.h"
#include "utils.h"

#include "multithreading.h"
#include "mutex.h"

#include "debug.h"


/*___________________________________________________________________________*/

//
// Multithreading
//

#define MAIN_THREAD     0
#define SECOND_THREAD   1

static thread_t threads[2];

void thread(void *p);

typedef struct {
  uint32_t context = 0x89ABCDEF;
} context_t;

static context_t thread_context;

/*___________________________________________________________________________*/

//
// Mutex
//

static mutex_t my_mutex;

/*___________________________________________________________________________*/

// threadA
int main(void)
{
  led_init();
  usart_init();

  //////////////////////////////////////////////

  mutex_define(&my_mutex);

  //////////////////////////////////////////////

  thread_create(&threads[SECOND_THREAD], thread, THREAD_2ND_STACK_LOC, (void*) &thread_context);

  // usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP);

  //////////////////////////////////////////////

  usart_dbg_hex16("thread 2 address : ", (uint16_t) thread);
  usart_dbg_hex16("thread 2 stack context : ", (uint16_t) &thread_context);
  usart_dbg_hex16("thread 2 stack pointer address : ", (uint16_t) threads[SECOND_THREAD].sp);
  usart_dbg_hex16("thread 2 stack base address : ", THREAD_2ND_STACK_LOC);

  //////////////////////////////////////////////

  while(1)
  {
    usart_dbg_hex16("#1 loop SP = ", SP);

    led_on();

    _delay_ms(500.0);

    led_off();

    _delay_ms(500.0);

    // lock the mutex 1 time/2
    if (mutex_lock(&my_mutex) == 0)
    {
      // do nothing
    }
    else
    {
      mutex_release(&my_mutex);
    }

    thread_switch(&threads[MAIN_THREAD], &threads[SECOND_THREAD]);
  }
}

/*___________________________________________________________________________*/


void thread(void *p)
{
  usart_print("Thread 2 start up, context is :\n");

  usart_send_hex((const uint8_t*) &((context_t*)p)->context, 4);

  usart_print("\n");

  while(1)
  {
    // do only if mutex is available
    if (mutex_lock(&my_mutex) == 0)
    {
      usart_dbg_hex16("#2 loop SP = ", SP);

      led_on();

      _delay_ms(100.0);

      led_off();

      _delay_ms(100.0);

      mutex_release(&my_mutex);
    }

    thread_switch(&threads[SECOND_THREAD], &threads[MAIN_THREAD]);
  }
}