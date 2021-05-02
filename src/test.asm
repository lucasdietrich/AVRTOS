#include <avr/io.h>

.global return_sp_assembler
.global push_things_in_stack
.global usart_show_addr

.global read_return_addr


return_sp_assembler:
    lds r24, SPL
    lds r25, SPH
    ret

// return something to tell the compiler to not use r24 (& r25)
push_things_in_stack:
    ldi r24, 0xBB
    ldi r25, 0xBB
    ret

read_return_addr:
    // read addr of "return addr"
    push r26
    push r27
    ldi r26, SPL
    ldi r27, SPH
    // inc 2 times SP
    ld r24, X+
    ld r24, X+

    ld r24, X+
    ld r25, X+
    call usart_show_addr
    pop r27
    pop r26
    ret