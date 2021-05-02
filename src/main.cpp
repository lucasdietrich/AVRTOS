#include <avr/io.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>

/*___________________________________________________________________________*/

#include "uart.h"
#include "led.h"
#include "multithreading.h"

#include "utils.h"

/*___________________________________________________________________________*/

#define MAIN_THREAD     0
#define SECOND_THREAD   1

thread_t threads[2];

uint16_t return_sp_c(void)
{
  return SP;
}

/*___________________________________________________________________________*/

extern "C" uint16_t read_return_addr();
extern "C" uint16_t return_sp_assembler(void);
extern "C" uint16_t push_things_in_stack(void);

/*___________________________________________________________________________*/

// threadA
int main(void)
{
  led_init();
  usart_init();

  /*
  usart_show_addr(SP);

  usart_show_sp_complete();

  usart_send("\n", 1);
  usart_send("struct thread_t size = ", 23);
  usart_hex(sizeof(thread_t));
  usart_send("\n", 1);
  */

  /*___________________________________________________________________________*/

  // usart_show_addr(SP);

  /*
  uint16_t ret_addr = read_return_addr();

  usart_hex16(ret_addr);

  usart_transmit('\n');
  */

 USART_DUMP_RAM_ALL();

  /*
  usart_show_stack(SP - 2, 10);

  uint16_t current_sp = return_sp_assembler();

  usart_show_stack(current_sp - 2, 10);
  */

  /*
  uint16_t val = push_things_in_stack();
  usart_hex16(val);
  */

  /*
  uint32_t canary = 0xAAAAAAAA;
  usart_send_hex((uint8_t*) &canary, 4);
  */


  // thread_switch(&threads[MAIN_THREAD], &threads[SECOND_THREAD]);

  while(1)
  {
    // PORTB ^= 1 << 5;

    led_on();

    _delay_ms(500.0);

    led_off();
    
    _delay_ms(500.0);
  }
}

void threadB(void)
{

}