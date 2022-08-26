/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <avr/io.h>
#include <avrtos/defines.h>

.global usart_core_dump
.global usart_transmit

.global usart_hex16
.global usart_hex

usart_core_dump:
    ; save core context on stack
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
    
    lds r0, SREG
    push r0

    ; print in usart
    ldi r24, 0x3c
    call usart_transmit
    call usart_transmit
    call usart_transmit
    call usart_transmit

    cli    
    lds r26, SPL
    lds r27, SPH

    ; show stack pointer before calling the function
    movw r24, r26
    call usart_hex16

    ldi r24, 32 ; "space"
    call usart_transmit

	/* TODO : OBSOLETE*/
    adiw r26, _K_ARCH_PC_SIZE + 33 + 1    ; for return address + registers + sreg

    ; show return address
    ld r24, -X
    ld r25, -X
    lsl r24 ; adjust address
    rol r25
    call usart_hex16

    ldi r16, 33

iter_stack:
    ldi r24, 32 ; "space"
    call usart_transmit

    ld r24, -X
    ; call usart_transmit
    call usart_hex

    dec r16
    brne iter_stack

    ldi r24, 0x3e
    call usart_transmit
    call usart_transmit
    call usart_transmit
    call usart_transmit
    ldi r24, 10
    call usart_transmit

    pop r0
    sts SREG, r0

    ; restore core context
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