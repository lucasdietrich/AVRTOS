#include <stddef.h>

#include "sysclock.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "misc/uart.h"

volatile uint8_t _sysclock_counter = 0;

// 100Hz timer
// ISR(TIMER2_OVF_vect)
// {
//     TCNT2 = 256 - 156;

//     usart_transmit('.');

//     // _sysclock_counter++;
// }

void init_sysclock(void)
{
    // normal mode
    TCCR2A &= ~(1 << WGM20 | 1 << WGM21 | 1 << WGM22);

    // prescaler = 1024
    TCCR2B |= (1 << CS20 | 1 << CS21 | 1 << CS22);

    // set counter
    TCNT2 = 256 - 156;

    // enable interrupt mask, start timer
    TIMSK2 |= 1 << TOIE2;
}