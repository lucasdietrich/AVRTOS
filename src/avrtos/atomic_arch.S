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
.global atomic_cas

atomic_get:
        movw     r26, r24
        ld      r24, X
        ret

atomic_blind_clear:
        movw     r26, r24
        ; TODO, find a way to use __zero_reg__ instead of r1
        st      X, r1 ; r1=__zero_reg__ is the 0 register
        ret

atomic_clear:
        movw     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        sts     SREG, r18     ; interrupt are enable 1 instruction later

        ; TODO, find a way to use __zero_reg__ instead of r1
        st      X, r1 ; r1=__zero_reg__ is the 0 register
        ret

atomic_or:
        movw     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        or      r22, r24
        sts     SREG, r18
        st      X, r22
        ret

atomic_xor:
        movw     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        eor     r22, r24
        sts     SREG, r18
        st      X, r22
        ret

atomic_and:
        movw     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        and     r22, r24
        sts     SREG, r18
        st      X, r22
        ret

atomic_inc:
        movw     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        inc     r24
        sts     SREG, r18
        st      X, r24
        ret

atomic_dec:
        movw     r26, r24
        lds     r18, SREG
        cli
        ld      r24, X
        dec     r24
        sts     SREG, r18
        st      X, r24
        ret

; r24, 25 contains the address of the atomic variable
; r22 contains the value to compare
; r20 contains the value to set
atomic_cas:
        movw     r26, r24
        lds     r18, SREG
        cli

	; prepare default return value (unchanged)
	ldi 	r24, 0

	; load atomic variable value and compare with "cmd"
	ld      r25, X
	cpse 	r24, r22

	; if not equal just return false
	jmp	__atomic_cas_ret

	; if equal, set the new value and return true
	ldi 	r24, 1
	st	X, r20

__atomic_cas_ret:
	sts     SREG, r18
	ret

#endif /* KERNEL_ATOMIC_API */
