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

/*___________________________________________________________________________*/

void(*function_p)(void) = testfunction;

// threadA
int main(void)
{
  led_init();
  usart_init();

  //////////////////////////////////////////////

  function_p();

  usart_show_addr((uint16_t) main << 1);        // fixed addr
  usart_show_addr((uint16_t) function_p << 1);  // fixed addr

  //////////////////////////////////////////////

  while(1)
  {
    led_on();

    _delay_ms(500.0);

    led_off();
    
    _delay_ms(500.0);
  }
}

/*___________________________________________________________________________*/


void threadB(void)
{

}