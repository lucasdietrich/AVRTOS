/**
 * @file main.cpp
 * @author Dietrich Lucas (ld.adecy@gmail.com)
 * @brief Measurement frequency of the led in order to know at whch frequency does the kernel switch thread
 * @version 0.1
 * @date 2021-07-04
 * 
 * @copyright Copyright (c) 2021
 */

// Measurement
// Fled frequency = F = 18.07kHz
// switch per seconds = K
//
//  K / 2 = F => K = 2F

// introducing delay to have a "round" frquency
//  T_K = 1/(2F) = 27.67µs

// with T_a = 50µs - T_K = 22.33µs
// (T_K + T_a) = 50µs -> F = 1/(2*(T_K + T_A)) = 10kHz
// measured = 10.06kHZ (=> OK)

// set to 0 to have maximum switching frequency
// set to 1 to have 10kHz switching frequency
#define SET_10kHz_SWITCHING_FREQUENCY   0

// precize calculation : cycles, etc...

/*___________________________________________________________________________*/

#include <util/delay.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>

/*___________________________________________________________________________*/

void thread_led(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, nullptr, 'O');

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();

  while(1)
  {
    led_on();
#if SET_10kHz_SWITCHING_FREQUENCY
    _delay_us(22);
#endif
    k_yield();
  }
}

void thread_led(void *p)
{
  while (1)
  {
    led_off();
#if SET_10kHz_SWITCHING_FREQUENCY
    _delay_us(22);
#endif
    k_yield();
  }
}

/*___________________________________________________________________________*/