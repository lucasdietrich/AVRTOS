#include <avr/io.h>

#include <avrtos/defines.h>

.global read_sp
.global read_ra
.global set_stack_pointer
.global read_sreg

;_____________________________________________________________________________;

; prempt debug

#if DEFAULT_KERNEL_DEBUG_PREEMPT_UART

.extern _k_interrupt_yield
.global USART_RX_vect

; when calling function, you must push all registers covered by the arguments, even not completely uint8_t require r24 but cover also r25 
; the function will use r25 without pushing on the stack, admitting that it as been saved by ther caller

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
    breq USART_Continue
    ldi r24, 0x58 ; X show X if error
    
USART_Continue:
    call usart_transmit     ; return received character

    out _SFR_IO_ADDR(SREG), r17

    pop r17
    pop r18
    pop r25
    pop r24

    jmp _k_preempt_routine

#endif

;_____________________________________________________________________________;

; return the return address of when ther callee calls it
read_ra:
    pop r25
    pop r24

    push r24
    push r25

    ; shift the address (addr_real = addr_cpu << 1)
    add r24, r24
    adc r25, r25

    ret

read_sp:
    lds r24, SPL
    lds r25, SPH
    ret

; structure address in r24, r25
set_stack_pointer:
    push r26
    push r27

    movw r26, r24

    lds r24, SPL
    lds r25, SPH

    st X+, r24
    st X+, r25

    pop r27
    pop r26

    ret

read_sreg:
    lds r24,SREG
    ret

;_____________________________________________________________________________;
