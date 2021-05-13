#include <avr/io.h>

.global semaphore_take
.global semaphore_give

; semaphore struct addr in r24, r25
; mov addr in r30, r31 (Z register)
; return value (uint8_t) in r24
semaphore_take:
    push r30
    push r31

    movw r30, r24

    lds r25, SREG
    cli

    ld r24, Z ; get semaphore count
    tst r24   ; tst
    breq .+4    ; no semaphore left
    dec r24     ; dec
    st Z, r24   ; mutex free, lock it (set FF)

    sts SREG, r25

    pop r31
    pop r30
    ret

; mutex struct addr in r24, r25
semaphore_give:
    push r26
    push r27

    movw r26, r24

    lds r25, SREG
    cli

    ld r24, x
    inc r24
    st X, r24

    sts SREG, r25

    pop r27
    pop r26
    ret