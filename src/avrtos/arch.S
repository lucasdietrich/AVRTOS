#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

.extern _k_timers_process


#if KERNEL_UPTIME
.extern _k_uptime_ms
#if KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS
.extern _k_remaining_sysclock_hits
#endif /* KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS */ 
#endif /* KERNEL_UPTIME */

/*___________________________________________________________________________*/

.global __k_interrupts

__k_interrupts:
    ldi r24, 0x00
    lds r25, SREG
    sbrc r25, SREG_I
    ldi r24, 0x01
    ret

/*___________________________________________________________________________*/

#if KERNEL_DEBUG_PREEMPT_UART

.global _K_USART_RX_vect

_K_USART_RX_vect:
    push r17
    lds r17, SREG

    /* reset r1 
     * @see __zero_reg__ : https://gcc.gnu.org/wiki/avr-gcc
     */
    push r1
    eor r1, r1

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
    ldi r24, 0x58   ; show X if error
    
USART_Continue:
    jmp system_shift

#endif

/*___________________________________________________________________________*/

.global k_yield
.extern _k_scheduler
.extern _current

; push order
; r17 | r0 r18 > r27 r30 r31 | r28 r19 r1 > r16 | SREG

; SREG is saved in r17 during the whole process, 
; DON'T USE r17 in this part without saving it

#if KERNEL_DEBUG_PREEMPT_UART == 0

#include <avrtos/sysclock_config.h>

.global SYSCLOCK_TIMERX_OVF_vect

SYSCLOCK_TIMERX_OVF_vect:
    push r17

    /* push __zero_reg__ and reset it to 0 as instructions may have used it as MUL
     * @see https://gcc.gnu.org/wiki/avr-gcc
     */
    push r1
    eor r1, r1

#if KERNEL_SYSLOCK_HW_TIMER == 1
    ; from ATmega328p documentation : timer1 (16 bits)
    ; > To do a 16-bit write, the high byte must be written before the low byte. 
    ; > For a 16-bit read, the low byte must be read before the high byte.

    ldi r17, SYSCLOCK_TIMER_TCNTH
    sts SYSCLOCK_HW_REG_TCNTXH, r17
#endif

    ldi r17, SYSCLOCK_TIMER_TCNTL
    sts SYSCLOCK_HW_REG_TCNTXL, r17
    ; reset timer counter as soon as possible (to maximize accuracy)

    lds r17, SREG

#if KERNEL_UPTIME
    push r24
    push r25

    lds r24, _k_uptime_ms
    lds r25, _k_uptime_ms+1
    adiw r24, SYSCLOCK_PERIOD_MS
    sts _k_uptime_ms, r24
    sts _k_uptime_ms+1, r25

    /* It took me 5 hours of debugging with QEMU to figure out why my
      _k_uptime_ms counter got randomly crazy ... sad =(
      The reason is I used r0 (__tmp_reg__) instead of r1 (__zero_reg__).

      Actually I think that GitHub copilot advised me to use this r0
      register instead of r1 .. fool
      */
    lds r24, _k_uptime_ms+2
    adc r24, r1
    sts _k_uptime_ms+2, r24

    lds r24, _k_uptime_ms+3
    adc r24, r1
    sts _k_uptime_ms+3, r24

#if KERNEL_UPTIME_40BITS
    lds r24, _k_uptime_ms+4
    adc r24, r1
    sts _k_uptime_ms+4, r24
#endif 


#if KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS
    lds r24, _k_remaining_sysclock_hits
    dec r24
    sts _k_remaining_sysclock_hits, r24

    /* restore registers that were temporarely 
    pushed in a wrong but convenient order */
    pop r25

    breq isr_save_context1
    
    pop r24
    pop r1
    sts SREG, r17
    pop r17
    reti
#else 

    /* restore registers that were temporarely 
    pushed in a wrong but convenient order */

    pop r25
    pop r24

#endif /* KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS */
#endif /* KERNEL_UPTIME */

isr_save_context1:

#if KERNEL_UPTIME && (KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS)
    
    /* reset _k_remaining_sysclock_hits */
    ldi r24, KERNEL_TIME_SLICE / SYSCLOCK_PERIOD_MS
    sts _k_remaining_sysclock_hits, r24

    pop r24

#endif /* KERNEL_UPTIME && (KERNEL_TIME_SLICE != SYSCLOCK_PERIOD_MS) */

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

#endif
; KERNEL_DEBUG_PREEMPT_UART

system_shift:
#if KERNEL_DEBUG
    ldi r24, 0x2e           ; '.'
    call usart_transmit
#endif
    call _k_system_shift

yield_from_interrupt:
    ; Interrupt flag is disabled in interrupt handler, 
    ; we need to set it manually in SREG
    ori r17, 1 << SREG_I 

#if KERNEL_PREEMPTIVE_THREADS
check_coop:
    ; to use offset of here IF POSSIBLE
    lds ZL, _current           ; load current thread addr in X
    lds ZH, _current + 1

    ldd r18, Z+2      ; read flag
    andi r18, K_FLAG_COOP | K_FLAG_SCHED_LOCKED
    breq scheduler_entry

    ; if coop thread don't preempt
#if KERNEL_SCHEDULER_DEBUG == 1
    ldi r24, 0x63 ; 'c'
    call usart_transmit
#endif
#endif

    jmp restore_context1


k_yield:
    push r17
    lds r17, SREG

save_context1:  ;  save __zero_reg__ + call-used registers
    push r1 /* __zero_reg__ is normally 0 here */

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
    ; calling the scheduler here helps to :
    ; - no wasting time to push every registers in the case the same thread 
    ;   will be executed next. That would lead to restore the same whole context
    ; - limit stack need when calling _k_scheduler

    ; Before partial context save/restore
    ; [M] CANARIES until @07B8 [found 464], MAX usage = 48 / 512
    ; [2] CANARIES until @010A [found 5], MAX usage = 507 / 512
    ; [1] CANARIES until @03CE [found 201], MAX usage = 55 / 256
    ; [L] CANARIES until @04DA [found 213], MAX usage = 43 / 256
    ; [K] CANARIES until @050B [found 6], MAX usage = 39 / 45

    ; [M] CANARIES until @07BC [found 468], MAX usage = 44 / 512
    ; [2] CANARIES until @010B [found 6], MAX usage = 506 / 512
    ; [1] CANARIES until @03D2 [found 205], MAX usage = 51 / 256
    ; [L] CANARIES until @04DE [found 217], MAX usage = 39 / 256
    ; [K] CANARIES until @050E [found 9], MAX usage = 36 / 45

    ; After partial context save/restore

    ; push Y address registers, as we need it to store _current
    push r28
    push r29

    ; r28, r29 shouldn't be destroyed when calling _k_scheduler
    ; and then must be caller saved registers
    lds r28, _current          ; load current thread address in Y
    lds r29, _current + 1

    call _k_scheduler 

    ; r24, r25 contains the address of the next thread to be executed

    ; save the remaining context on thread switch
    cp r24, r28
    brne save_context2
    cp r25, r29
    brne save_context2

#if KERNEL_SCHEDULER_DEBUG == 1
    push r24
    ldi r24, 0x73 ; 's'
    call usart_transmit
    pop r24
#endif

    pop r29
    pop r28
    ; restore the partial context saved if there is no thread switch
    jmp restore_context1

save_context2:
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

    push r17    ; push SREG on stack

save_sp:
    lds r20, SPL
    lds r21, SPH

    st Y+, r20       ; write SP in current thread structure
    st Y+, r21

restore_sp:
    movw r28, r24   ; new thread structure address is in (r24, r25)

    ld r20, Y+       ; read sp
    ld r21, Y+

    sts SPL, r20     ; restore stack pointer
    sts SPH, r21

restore_context2:
    pop r17     ; load SREG in r17

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

    pop r29
    pop r28

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

    pop r1

    ; if I flag is set : we need to make sure to set interrupt back 
    ;   at the very last intruction
    sbrs r17, SREG_I
    jmp no_interrupt_ret

    cbr r17, 1 << SREG_I
    sts SREG, r17

    ; we pop the last register
    pop r17

    ; and we set enable interrupt again while returning
    reti

no_interrupt_ret:
    sts SREG, r17
    pop r17

    ret

/*___________________________________________________________________________*/

#if THREAD_USE_INIT_STACK_ASM == 1

.global _k_thread_stack_create
.extern _k_thread_entry

; thread_t *th         in r24, r25
; thread_entry_t entry in r22, r23
; uint16_t stack_end   in r20, r21
; void* context_p      in r18, r19
_k_thread_stack_create:
; TODO can be optimized a lot
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
#if THREAD_ALLOW_RETURN == 1
    ldi r20, lo8(_k_thread_entry)
    ldi r21, hi8(_k_thread_entry)
    lsr r21
    ror r20
    st -X, r20 ; SPL
    st -X, r21 ; SPH
#else
    st -X, r22 ; SPL
    st -X, r23 ; SPH
#endif

    ; push 30 default registers (r1 == 0 for example, ...) + pass (void * context)
    ldi r28, 0x00
#if THREAD_ALLOW_RETURN == 1
    ldi r29, 7 + _K_ARCH_STACK_SIZE_FIXUP
#else
    ldi r29, 9 + _K_ARCH_STACK_SIZE_FIXUP
#endif
    dec r29
    st -X, r28
    brne .-6

#if THREAD_ALLOW_RETURN == 1
    ; entry ~ push r22 > r23
    st -X, r22
    st -X, r23
#endif

    ; void * context ~ push r24 > r25
    st -X, r18
    st -X, r19

    ldi r29, 21
    dec r29
    st -X, r28
    brne .-6
    
    ; push sreg default (0)
    ; lds r29, SREG
    ldi r29, THREAD_DEFAULT_SREG
    st -X, r29

    ; -1, to first empty stack addr
    sbiw r26, 1

    ; mov r24, r25 (struct k_thread *th) to Y
    movw r28, r24

    ; save SP in thread structure
    st Y+, r26
    st Y, r27

    pop r29
    pop r28
    pop r27
    pop r26

    ret  ; dispatch to next thread

#endif

/*___________________________________________________________________________*/
