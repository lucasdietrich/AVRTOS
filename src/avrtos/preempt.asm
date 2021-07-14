;#define __SFR_OFFSET 0
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

.global USART_RX_vect
.global usart_transmit
.global TIMER2_OVF_vect
.global _k_interrupt_yield
.global _k_preempt_routine

;_____________________________________________________________________________;


_k_preempt_routine:

; TODO here

; add time to xtqueue
; todo do upstream work in order to determine if another thread is ready
    

; switch thread here : stack is +2B (return address)
_k_interrupt_yield:
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
    push r24
    push r25
    push r26
    push r27
    push r28
    push r29
    push r30
    push r31

    lds r16, SREG
    ori r16, 1 << SREG_I ; we Interrupt flag is disabled we need to to it manually in SREG

    push r16

    lds r0, SPL
    lds r1, SPH

    lds r26, k_thread           ; load current thread addr in X
    lds r27, k_thread + 1

    st X+, r0   ; write SP in structure
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
    pop r25
    pop r24
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