;#define __SFR_OFFSET 0
#include <avr/io.h>
#include <avr/interrupt.h>

.global USART_RX_vect
.global usart_transmit

; when calling function, you must push all registers covered by the arguments,; even not completely uint8_t require r24 but cover also r25 
; the function will use r25 without pushing on the stack, admitting that it as been saved by ther caller
_USART_RX_vect:
    push r24
    push r25
    lds r24, UDR0
    lds r24, 0x5A
    call usart_transmit
    pop r25
    pop r24
    reti

; USART_RX_vect
USART_RX_vect:
    push r24
    push r25
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
    pop r25
    pop r24
    reti

    