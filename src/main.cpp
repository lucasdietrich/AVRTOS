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
#include "semaphore.h"

#include "debug.h"
#include "multithreading_debug.h"

#include <avr/pgmspace.h>

/*___________________________________________________________________________*/

//
// Multithreading
//

#define MAIN_THREAD     0
#define SECOND_THREAD   1

static thread_t mythread1;
static thread_t mythread2;

void thread_ledon(void *p);
void thread_ledoff(void *p);

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  // main stack size = 0x300

  k_thread_create(&mythread1, thread_ledon, RAMEND - 0x300, 0x100, K_PRIO_DEFAULT, nullptr);
  k_thread_create(&mythread2, thread_ledoff, RAMEND - 0x300 - 0x100, 0x100, K_PRIO_DEFAULT, nullptr);

  k_thread_dbg_count();

  k_thread_dump_hex(k_thread.current);
  k_thread_dump_hex(k_thread.list[k_thread.current_idx]);

  k_thread_dump_all();

  while(1) {
    k_yield();
  }

  while(1)
  {
    usart_printl("::main");

    k_thread_dump_all();

    k_yield();
  }
}

/*___________________________________________________________________________*/

void thread_ledon(void *p)
{
  while(1)
  {
    usart_printl("::thread_ledon");

    led_on();

    _delay_ms(500);

    k_yield();
  }
}

void thread_ledoff(void *p)
{
  while(1)
  {
    usart_printl("::thread_ledoff");

    led_off();

    _delay_ms(500);

    k_yield();
  }
}


/*___________________________________________________________________________*/