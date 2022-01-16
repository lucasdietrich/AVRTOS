#include <avr/io.h>

#include <avrtos/defines.h>

.global read_sp
.global read_ra
.global set_stack_pointer
.global read_sreg

;_____________________________________________________________________________;

; return the return address of when ther caller calls it
read_ra:
    pop r25
    pop r24
#if __AVR_3_BYTE_PC__
    pop r23
    ldi r22, 0x00
    push r23
#endif
    push r24
    push r25

    ; shift the address (addr_real = addr_cpu << 1)
#if __AVR_3_BYTE_PC__
    add r23, r23
    adc r24, r24
#else
    add r24, r24
#endif
    adc r25, r25

    ret

read_sp:
    lds r20, SREG
    cli
    lds r24, SPL
    lds r25, SPH
    sts SREG, r20
    ret

; structure address in r24, r25
set_stack_pointer:
    push r26
    push r27

    movw r26, r24

    lds r24, SPL
    lds r25, SPH

    st X+, r24
    st X+, r25

    pop r27
    pop r26

    ret

read_sreg:
    lds r24, SREG
    ret

;_____________________________________________________________________________;
