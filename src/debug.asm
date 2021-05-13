#include <avr/io.h>

/*___________________________________________________________________________*/

.global read_sp
.global read_ra

; return the return address of when ther callee calls it
read_ra:
    pop r25
    pop r24

    push r24
    push r25

    ; shift the address (addr_real = addr_cpu << 1)
    add r24, r24
    adc r25, r25

    ret

read_sp:
    lds r24, SPL
    lds r25, SPH
    ret

/*___________________________________________________________________________*/

.global set_stack_pointer

; structure address in r24, r25
set_stack_pointer:
    cli

    push r26
    push r27

    mov r26, r24
    mov r27, r25

    lds r24, SPL
    lds r25, SPH

    st X+, r24
    st X+, r25

    pop r27
    pop r26

    sei

    ret

/*___________________________________________________________________________*/

.global testloop_assembler
.global testshift_assembler

testloop_assembler:
    push r16

    ldi r16, 0
    inc r16
    dec r24
    brne .-6

    mov r24, r16

    pop r16

    ret

testshift_assembler:
    lsr r25 ; MSB
    ror r24 ; LSB

    ret