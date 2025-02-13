/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avrtos/defines.h>

#if CONFIG_KERNEL_ATOMIC_API == 1

.global atomic_get
.global atomic_clear
.global atomic_or
.global atomic_xor
.global atomic_and
.global atomic_inc
.global atomic_dec
.global atomic_clear_bit
.global atomic_set_bit
.global atomic_set_bit_to
.global atomic_blind_clear
.global atomic_cas

atomic_get:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    ld      r24, X         ; Load the value from the atomic variable
    ret                    ; Return the value in r24

atomic_set:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    st      X, r22         ; Store the value from r22 into the atomic variable
    ret                    ; Return

atomic_blind_clear:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    st      X, r1          ; Clear the atomic variable by storing 0 (r1 is always 0 __zero_reg__)
    ret                    ; Return

atomic_clear:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts
    ld      r24, X         ; Load the current value of the atomic variable
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    st      X, r1          ; Clear the atomic variable (r1 is 0 __zero_reg__)
    ret                    ; Return the old value in r24

atomic_or:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts
    ld      r24, X         ; Load the current value of the atomic variable
    or      r22, r24       ; Perform OR operation between r22 and r24
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    st      X, r22         ; Store the result back into the atomic variable
    ret                    ; Return the old value in r24

atomic_xor:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts
    ld      r24, X         ; Load the current value of the atomic variable
    eor     r22, r24       ; Perform XOR operation between r22 and r24
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    st      X, r22         ; Store the result back into the atomic variable
    ret                    ; Return the old value in r24

atomic_and:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts
    ld      r24, X         ; Load the current value of the atomic variable
    and     r22, r24       ; Perform AND operation between r22 and r24
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    st      X, r22         ; Store the result back into the atomic variable
    ret                    ; Return the old value in r24

atomic_inc:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts
    ld      r24, X         ; Load the current value of the atomic variable
    inc     r24            ; Increment the value
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    st      X, r24         ; Store the new value back into the atomic variable
    ret                    ; Return the new value in r24

atomic_dec:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts
    ld      r24, X         ; Load the current value of the atomic variable
    dec     r24            ; Decrement the value
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    st      X, r24         ; Store the new value back into the atomic variable
    ret                    ; Return the new value in r24

; Compare and Swap (CAS) operation
; r24, r25: address of the atomic variable
; r22: value to compare
; r20: value to set
atomic_cas:
    movw    r26, r24       ; Load address of the atomic variable into X (r26:r27)
    lds     r18, SREG      ; Store SREG (interrupt flag register) into r18
    cli                    ; Disable interrupts

    ldi     r24, 0         ; Prepare default return value (false, no change)

    ld      r25, X         ; Load the current value of the atomic variable
    cpse    r24, r22       ; Compare r24 (current value) with r22 (cmd value)

    jmp     __atomic_cas_ret ; If not equal, return false

    ldi     r24, 1         ; If equal, set return value to true
    st      X, r20         ; Store the new value (r20) into the atomic variable

__atomic_cas_ret:
    sts     SREG, r18      ; Restore SREG (re-enable interrupts)
    ret                    ; Return result in r24 (true or false)

#endif /* CONFIG_KERNEL_ATOMIC_API */
