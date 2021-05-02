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

extern void function(uint16_t a, uint16_t b);

/*___________________________________________________________________________*/

extern "C" uint16_t read_return_addr();
extern "C" uint16_t return_sp_assembler(void);
extern "C" uint16_t push_things_in_stack(void);

/*___________________________________________________________________________*/

static uint8_t ram[20u] = {0};
static uint8_t i = 0;

// threadA
int main(void)
{
  led_init();
  usart_init();

  //////////////////////////////////////////////

  char buffer[16*8];
  memset(buffer, 0xAA, sizeof(buffer));

  USART_DUMP_RAM_ALL();

  usart_send_hex((const uint8_t*) buffer, sizeof(buffer));
 
  //////////////////////////////////////////////

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