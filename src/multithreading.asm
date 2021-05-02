#include <avr/io.h>

.global thread_switch
.global usart_show_addr

/*

                LDI    R16, low(RAMEND)
                OUT    SPL, R16
                LDI    R16, high(RAMEND)
                OUT    SPH, R16
*/

thread_switch:
    // save current thread registers
    // push 32 registers
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
    // push SREG

    // call debug function
    /*
    push r24
    push r25
    lds r24, SPL
    lds r25, SPH
    call usart_show_addr
    pop r24
    pop r25
    */


    // restore 32 registers
    // pop SREG
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

    ret
