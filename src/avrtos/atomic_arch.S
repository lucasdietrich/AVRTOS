#include <avr/io.h>

#include <avrtos/defines.h>

#if KERNEL_ATOMIC_API == 1

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

atomic_get:
        mov     r26, r24
        ld      r24, X
        ret

atomic_blind_clear:
        mov     r26, r24
        ; TODO, find a way to use __zero_reg__ instead of r1
        st      X, r1 ; r1=__zero_reg__ is the 0 register
        ret

atomic_clear:
        mov     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        sts     SREG, r18     ; interrupt are enable 1 instruction later

        ; TODO, find a way to use __zero_reg__ instead of r1
        st      X, r1 ; r1=__zero_reg__ is the 0 register
        ret

atomic_or:
        mov     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        or      r22, r24
        sts     SREG, r18
        st      X, r22
        ret

atomic_xor:
        mov     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        eor     r22, r24
        sts     SREG, r18
        st      X, r22
        ret

atomic_and:
        mov     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        and     r22, r24
        sts     SREG, r18
        st      X, r22
        ret

atomic_inc:
        mov     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        inc     r24
        sts     SREG, r18
        st      X, r24
        ret

atomic_dec:
        mov     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        dec     r24
        sts     SREG, r18
        st      X, r24
        ret

#endif /* KERNEL_ATOMIC_API */