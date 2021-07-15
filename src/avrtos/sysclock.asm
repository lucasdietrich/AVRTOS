#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

#if KERNEL_PREEMPTIVE_THREADS

.global _k_init_sysclock
.global TIMER0_OVF_vect
.global usart_transmit
.extern _k_interrupt_yield
.extern _k_system_shift

TIMER0_OVF_vect:
    push r24

    ldi r24, 0x100 - KERNEL_SYSCLOCK_TIMER0_TCNT0
    sts TCNT0, r24

    ; _k_system_shift
    ; push r25
    ; ldi r24, lo8(KERNEL_TIME_SLICE)
    ; ldi r25, hi8(KERNEL_TIME_SLICE)
    ; call _k_system_shift
    ; pop r25

#if KERNEL_DEBUG
    push r25
    in r25, _SFR_IO_ADDR(SREG)
    ldi r24, 0x2e           ; '.'
    call usart_transmit
    out _SFR_IO_ADDR(SREG), r25
    pop r25
#endif

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
    ldi r16, (0 << WGM02) | (0 << FOC0A) | (0 << FOC0B) | (1 << CS00) | (0 << CS01) |  (1 << CS02)
    sts TCCR0B, r16
    
; TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (0 << TOIE0);
    ldi r16, (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0)
    sts TIMSK0, r16

    pop r16
    ret

#endif