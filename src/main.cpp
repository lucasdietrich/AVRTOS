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

/*___________________________________________________________________________*/

//
// Multithreading
//

#define MAIN_THREAD     0
#define SECOND_THREAD   1

static thread_t threads[2];

void thread(void *p);

/*___________________________________________________________________________*/

// threadA
int main(void)
{
  led_init();
  usart_init();

  thread_create(&threads[SECOND_THREAD], thread, THREAD_2ND_STACK_LOC, nullptr);

  while(1)
  {


    thread_switch(&threads[MAIN_THREAD], &threads[SECOND_THREAD]);
  }
}

/*___________________________________________________________________________*/

void thread(void *p)
{

  while(1)
  {


    thread_switch(&threads[SECOND_THREAD], &threads[MAIN_THREAD]);
  }
}

/*___________________________________________________________________________*/