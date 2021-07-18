#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

#if KERNEL_PREEMPTIVE_THREADS

.global _k_init_sysclock
.global TIMER0_OVF_vect
.global usart_transmit
.extern _k_interrupt_yield
.extern _k_system_shift

; https://gcc.gnu.org/wiki/avr-gcc
; read Call-Used Registers section
TIMER0_OVF_vect:
    push r24

    ; reset timer as soon as possible
    ldi r24, 0x100 - KERNEL_SYSCLOCK_TIMER0_TCNT0
    sts TCNT0, r24

    ; due to these call-clobbered registers, we need to save them when calling a c function
    ; TODO write scheduler in assembly in order to gain performance (no need to push all these registers)
    push r15
    lds r15, SREG

    push r18
    push r19
    push r20
    push r21
    push r22
    push r23

    push r25
    push r26
    push r27
    push r30
    push r31

    ; shift
    call _k_system_shift    ; cause _k_system_shift calls usart_transmit which needs a parameter (r24, r25)

#if KERNEL_DEBUG
    in r25, _SFR_IO_ADDR(SREG)
    ldi r24, 0x2e           ; '.'
    call usart_transmit
    out _SFR_IO_ADDR(SREG), r25
#endif

    pop r31
    pop r30
    pop r27
    pop r26
    pop r25

    pop r23
    pop r22
    pop r21
    pop r20
    pop r19
    pop r18

    sts SREG, r15
    pop r15

    pop r24

    jmp _k_preempt_routine  ; yield current thread and switch to another one


_k_init_sysclock:
    push r16

; TCNT0 = KERNEL_SYSCLOCK_TIMER0_TCNT0 ;
    ldi r16, 0x100 - KERNEL_SYSCLOCK_TIMER0_TCNT0 ;
    sts TCNT0, r16

; TCCR0A = (0 << COM0A0) | (0 << COM0A1) | (0 << COM0B0) | (0 << COM0B1) | (0 << WGM01) | (0 << WGM00);
    ldi r16, (0 << COM0A0) | (0 << COM0A1) | (0 << COM0B0) | (0 << COM0B1) | (0 << WGM01) | (0 << WGM00)
    sts TCCR0A, r16

; TCCR0B = (0 << WGM02) | (0 << FOC0A) | (0 << FOC0B) | (1 << CS00) | (0 << CS01) | (1 << CS02);
    ldi r16, (0 << WGM02) | (0 << FOC0A) | (0 << FOC0B) | KERNEL_SYSCLOCK_TIMER0_PRESCALER
    sts TCCR0B, r16
    
; TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (0 << TOIE0);
    ldi r16, (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0)
    sts TIMSK0, r16

    pop r16
    ret

#endif