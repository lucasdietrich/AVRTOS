;#define __SFR_OFFSET 0
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

.global USART_RX_vect
.global usart_transmit
.global TIMER2_OVF_vect

;_____________________________________________________________________________;

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
    breq USART_ReceiveNoError
    ldi r24, 0x58 ; X show X if error
    
USART_ReceiveNoError:
    call usart_transmit     ; return received character

    out _SFR_IO_ADDR(SREG), r17

    pop r17
    pop r18
    pop r25
    pop r24

    jmp _interrupt_k_yield

; KERNEL_DEBUG
TIMER2_OVF_vect:
    push r24
    ldi r24, 256 - 156
    sts TCNT2, r24

#if KERNEL_DEBUG
    push r25
    in r25, _SFR_IO_ADDR(SREG)
    ldi r24, 0x2e           ; '.'
    call usart_transmit
    out _SFR_IO_ADDR(SREG), r25
    pop r25
#endif

    pop r24

    reti
    ; jmp _interrupt_k_yield    

;_____________________________________________________________________________;

; switch thread here : stack is +2B (return address)

; see _k_thread_switch (th* a -> th* b)
_interrupt_k_yield:
    ; save current thread registers
    ; push 32 registers
    push r0
    push r1
    push r2
    push r3
    push r4
    push r5
    push r6
    push r7
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push r16
    push r17
    push r18
    push r19
    push r20
    push r21
    push r22
    push r23
    push r25    ; r24, r25 inverted
    push r24    ; r24, r25 inverted
    push r26
    push r27
    push r28
    push r29
    push r30
    push r31

    lds r0, SREG    ; save flags
    push r0

    lds r0, SPL
    lds r1, SPH

    lds r26, k_thread           ; load current thread structure addr in X
    lds r27, k_thread + 1

    st X+, r0   ; save SP
    st X+, r1

    ; determine which is the next thread
    call _k_scheduler

    ; next thread structure addr is in X
    movw r26, r24

    ld r0, X+
    ld r1, X+

    sts SPL, r0 ; restore stack pointer
    sts SPH, r1

    pop r0  ; restore flags
    sts SREG, r0

    ; restore 32 registers
    pop r31
    pop r30
    pop r29
    pop r28
    pop r27
    pop r26
    pop r24    ; r24, r25 inverted
    pop r25    ; r24, r25 inverted
    pop r23
    pop r22
    pop r21
    pop r20
    pop r19
    pop r18
    pop r17
    pop r16
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    pop r3
    pop r2
    pop r1
    pop r0

    ; ret/reti
    reti  ; dispath to next thread

    
;_____________________________________________________________________________;