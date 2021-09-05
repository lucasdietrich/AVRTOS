#include <avr/io.h>

.global _k_sem_take
.global _k_sem_give

; mutex struct addr in r24, r25
_k_sem_take:
    movw r30, r24

    ld r24, Z           ; get semaphore count
    tst r24             ; test
    breq nosem          ; no semaphore left
oksem:
    dec r24             ; decrement
    st Z, r24           ; store
    ldi r24, 0x00
    jmp retsemtake
nosem:
    ldi r24, 0xFF
retsemtake:
    ret

; mutex struct addr in r24, r25
_k_sem_give:
    movw r30, r24

    ld r24, Z           ; get semaphore count
    ldd r23, Z+1         ; get semaphore limit

    cpse r23, r24       ; compare
    jmp semgive         ; if count != limit we inc sem count
    jmp retsemgive      ; if count == limit we return
semgive:
    inc r24
    st Z, r24
retsemgive:
    ret