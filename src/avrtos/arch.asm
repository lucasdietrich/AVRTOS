#include <avr/io.h>

#include "defines.h"

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

.global _k_thread_switch

; thread_t *from   in r24, r25
; thread_t *to     in r22, r23
_k_thread_switch:
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

    ; push SREG onto stack;
    lds r0, SREG
    push r0

    ; disable interrupts
    ; TODO can be done later
    
    ; save stack pointer in structure (struct thread_t *from)
    cli
    lds r0, SPL
    sei
    lds r1, SPH

    ; set structure pointer to X
    movw r26, r24

    st X+, r0    ; write SPL in uint16_t low byte (?)
    st X+, r1    ; write SPH in uint16_t high byte (?)

    ; restore the other thread

    ; restore stack pointer from  structure (struct thread_t *from)

    ; set structure pointer to X
    movw r26, r22

    ld r0, X+  ; low byte
    ld r1, X+  ; high byte

    ; set stack pointer
    cli
    sts SPL, r0
    sei ; set interrupt mask
    sts SPH, r1

    ; restore SREG from stack;
    pop r0
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

    ret ; 2 following Bytes in stack are return address
    
/*___________________________________________________________________________*/

.global k_yield
.global _k_scheduler
.extern k_thread

; see _k_thread_switch (th* a -> th* b)
k_yield:
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

    lds r0, SREG    ; save flags
    push r0

    cli
    lds r0, SPL
    sei
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

    cli
    sts SPL, r0 ; restore stack pointer
    sei
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

    ret  ; dispath to next thread

/*___________________________________________________________________________*/

#if THREAD_EXPLICIT_MAIN_STACK == 1

.extern _k_main_stack

; this section override the Stack Pointer defined in section .init2 to the defined Main Buffer
; https://www.nongnu.org/avr-libc/user-manual/mem_sections.html
.section .init3,"ax", @progbits
    ldi r28, lo8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
    ldi r29, hi8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
    out _SFR_IO_ADDR(SPL), r28
    out _SFR_IO_ADDR(SPH), r29

#endif

/*___________________________________________________________________________*/