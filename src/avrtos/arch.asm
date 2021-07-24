#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

/*___________________________________________________________________________*/

; prempt debug
#if KERNEL_DEBUG_PREEMPT_UART

.global USART_RX_vect

; when calling function, you must push all registers covered by the arguments, even not completely uint8_t require r24 but cover also r25 
; the function will use r25 without pushing on the stack, admitting that it as been saved by ther caller

USART_RX_vect:
    push r17

    lds r17, SREG

    push r0

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

    push r30
    push r31

; read received
    lds r18, UCSR0A    ; UCSR0A =0xC0
;    lds r17, UCSR0B (if 9 bits)
    lds r24, UDR0    ; UDR0 =0xC6

; If error, return -1
    andi r18, (1 << FE0) | (1 << DOR0) | (1 << UPE0)
    breq USART_Continue
    ldi r24, 0x58   ; X show X if error
    
USART_Continue:
    ; call usart_transmit     ; print received character or X

    jmp system_shift

#endif

/*___________________________________________________________________________*/

.global _k_yield
.extern _k_scheduler
.extern k_thread

; push order
; r17 | r0 r18 > r27 r30 r31 | r1 > r16 r28 r29 | SREG

; SREG is saved in r17 during the whole process, DON'T USE r17 in this part without saving it

#if KERNEL_PREEMPTIVE_THREADS

.global TIMER0_OVF_vect

TIMER0_OVF_vect:
    push r17

    ; reset timer counter as soon as possible (to maximize accuracy)
    ldi r17, 0x100 - KERNEL_SYSCLOCK_TIMER0_TCNT0
    sts TCNT0, r17

    lds r17, SREG

    push r0

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

    push r30
    push r31

#if KERNEL_DEBUG
prempt_debug:
    ldi r24, 0x2e           ; '.'
    call usart_transmit
#endif

system_shift:
    call _k_system_shift

check_coop:
    ; to use offset of here IF POSSIBLE
    lds r30, k_thread           ; load current thread addr in X
    lds r31, k_thread + 1

    ldd r18, Z+6      ; read flag

    sbrc r18, 2       ; if coop thread don't preempt
    jmp restore_context1


yield_from_interrupt:
    ori r17, 1 << SREG_I ; Interrupt flag is disabled in interrupt handler, we need to set it manually in SREG

    jmp scheduler_entry
#endif

_k_yield:
    push r17
    lds r17, SREG

save_context1:  ; save call-used registers
    push r0

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

    push r30
    push r31

clear_interrupt_flag:
    ; "brid" I flag pre check would be an oversight
    cli

scheduler_entry:
    ; todo
    ; call scheduler here
    ; check if current thread changed or not
    ; goto cancel_sched of no change

save_context2:
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

    push r28
    push r29

    push r17    ; push SREG on stack

save_sp:
    ; lds r20, SPL
    ; lds r21, SPH

    ; lds r28, k_thread          ; load current thread addr in X
    ; lds r29, k_thread + 1

    ; st Y+, r20       ; write SP in current thread structure
    ; st Y+, r21

    lds r0, SPL
    lds r1, SPH

    lds r26, k_thread            ; load current thread addr in X
    lds r27, k_thread + 1

    st X+, r0   ; write SP in structure
    st X+, r1

scheduler:
    call _k_scheduler 

restore_sp:
    ; movw r28, r24   ; new current thread structure address is in (r24, r25)

    ; ld r20, Y+       ; read sp
    ; ld r21, Y+

    ; sts SPL, r20     ; restore stack pointer
    ; sts SPH, r21

    movw r26, r24

    ld r0, X+
    ld r1, X+

    sts SPL, r0 ; restore stack pointer
    sts SPH, r1

restore_context2:
    pop r17         
    
    ; TODO restore SREG here if k_yield (not interrupt)
    ; we need to know if the call switch happened from an interrupt or not
    ; if not interrupt, set SREG (and I flag potentially) is not a problem here

    pop r29
    pop r28

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

restore_context1:
    pop r31
    pop r30

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

    pop r0

    ; if I flag is set : we need to make sure to set interrupt back at the very last intruction)
    ; else : no problem
    sbrs r17, SREG_I
    jmp no_interrupt_ret

    cbr r17, 1 << SREG_I

    sts SREG, r17
    pop r17

    reti

no_interrupt_ret:
    sts SREG, r17
    pop r17

    ret

/*___________________________________________________________________________*/

#if THREAD_USE_INIT_STACK_ASM == 1

.global _k_thread_stack_create

; thread_t *th         in r24, r25
; thread_entry_t entry in r22, r23
; uint16_t stack_end   in r20, r21
; void* context_p      in r18, r19
_k_thread_stack_create:
    push r26
    push r27
    push r28
    push r29

    ; load stack pointer in X register
    ; mov r22, r23 to X
    movw r26, r20

    ; add 1 to stack pointer in order
    adiw r26, 1

    ; addr of function is already shifted
    ; "return addr" 1 bit shift (>>1)
    ; lsr r23
    ; ror r22

    ; add return addr to stack (with format >> 1)
    st -X, r22 ; SPL
    st -X, r23 ; SPH

    ; push 30 default registers (r1 == 0 for example, ...) + pass (void * context)
    ; r0 > r23 (24 registers)
    ldi r28, 0x00
    ldi r29, 24
    dec r29
    st -X, r28
    brne .-6

    ; void * context ~ push r24 > r25
    st -X, r19
    st -X, r18

    ; push r26 > r31 (6 registers)
    ldi r29, 6
    dec r29
    st -X, r28
    brne .-6
    
    ; push sreg default (0)
    ; lds r29, SREG
    ldi r29, THREAD_DEFAULT_SREG
    st -X, r29

    ; copy stack pointer in thread structure

    ; -1, to first empty stack addr
    sbiw r26, 1

    ; mov r24, r25 (struct thread_t *th) to Y
    movw r28, r24

    ; save SP in thread structure
    st Y+, r26
    st Y, r27

    pop r29
    pop r28
    pop r27
    pop r26

    ret  ; dispath to next thread

#endif

/*___________________________________________________________________________*/
