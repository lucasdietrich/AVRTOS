#include <avr/io.h>

.global _mutex_read
.global _mutex_lock
.global _mutex_release

; mutex struct addr in r24, r25
; mov addr in r30, r31 (Z register)
; return value (uint8_t) in r24
_mutex_lock:
    movw r30, r24

    ; neither atmega328P nor atmega2560 supports xch instruction
    ; xch Z, r24

    lds r25, SREG
    cli

    ld r24, Z
    com r24     ; if mutex locked (00), com should set flag Z=0
    brne .+2    ; mutex locked
    st Z, r24   ; mutex free, lock it (set FF)

    sts SREG, r25
    ret

; mutex struct addr in r24, r25
_mutex_release:
    movw r26, r24
    ldi r24, 0xFF
    st X, r24
    ret