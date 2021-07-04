#include <avr/io.h>

.global led_init
.global led_on
.global led_off

led_init:
    push r16
    ldi r16, 0b00100000	
    sts DDRB, r16
    pop r16
    ret

led_on:
    push r16
    ldi r16, 0b00100000	
    sts PORTB, r16
    pop r16
    ret
    
led_off:
    push r16
    ldi r16, 0b00000000
    sts PORTB, r16
    pop r16
    ret