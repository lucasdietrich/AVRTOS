#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

/*___________________________________________________________________________*/

#if THREAD_EXPLICIT_MAIN_STACK == 1

.extern _k_main_stack

; this section "overwrite" the Stack Pointer defined in section .init2 to the defined Main Buffer
; https://www.nongnu.org/avr-libc/user-manual/mem_sections.html
.section .init3, "ax", @progbits
    ldi r28, lo8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
    ldi r29, hi8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
    out _SFR_IO_ADDR(SPL), r28
    out _SFR_IO_ADDR(SPH), r29
#endif

/*___________________________________________________________________________*/

; initialize system clock (timer0) in order to preempt preemptive threads
#if KERNEL_SYSCLOCK_AUTO_INIT
.extern _k_init_sysclock
#endif

.extern _k_kernel_init
.extern _k_mem_slab_init_module

; kernel final init here
.section .init8, "ax", @progbits

#if KERNEL_DEBUG_PREEMPT_UART
; enable uart RX interrupt
    ldi r17, 1 << RXCIE0
    sts UCSR0B, r17
#endif

; add READY threads to the runqueue, and adjust addresses endianness in stacks
    call _k_kernel_init

; initialize mem slabs defined at compilation
    call _k_mem_slab_init_module

; initialize canaries in threads stacks
#if THREAD_CANARIES
    call _k_init_thread_canaries
#endif

#if KERNEL_SYSCLOCK_AUTO_INIT
    call _k_init_sysclock
#endif

/*___________________________________________________________________________*/
