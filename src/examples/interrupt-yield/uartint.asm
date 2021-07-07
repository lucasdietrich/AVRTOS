;#define __SFR_OFFSET 0
#include <avr/io.h>
#include <avr/interrupt.h>

.global USART_RX_vect
.global usart_transmit


; USART_RX_vect
USART_RX_vect:
    push r24
    push r18
    push r17

    in r17, _SFR_IO_ADDR(SREG)

; read received
    lds r18, UCSR0A    ; UCSR0A =0xC0
;    lds r17, UCSR0B (if 9 bits)
    lds r24, UDR0    ; UDR0 =0xC6

; If error, return -1
    andi r18, (1 << FE0) | (1 << DOR0) | (1 << UPE0)
    breq USART_ReceiveNoError
    ldi r24, 0x58 ; X show X if error
    
USART_ReceiveNoError:
    call usart_transmit     ; return received character

    out _SFR_IO_ADDR(SREG), r17

    pop r17
    pop r18
    pop r24
    reti

    