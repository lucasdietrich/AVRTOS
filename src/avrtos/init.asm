#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

/*___________________________________________________________________________*/

#if THREAD_EXPLICIT_MAIN_STACK == 1

.extern _k_main_stack

; this section override the Stack Pointer defined in section .init2 to the defined Main Buffer
; https://www.nongnu.org/avr-libc/user-manual/mem_sections.html
.section .init3, "ax", @progbits
    ldi r28, lo8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
    ldi r29, hi8(_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE))
    out _SFR_IO_ADDR(SPL), r28
    out _SFR_IO_ADDR(SPH), r29
#endif


#if KERNEL_SYSCLOCK_AUTO_INIT
.extern _k_init_sysclock
#endif

.extern _k_scheduler_init

; Kernel final init here
.section .init8, "ax", @progbits

#if KERNEL_SYSCLOCK_AUTO_INIT
    call _k_init_sysclock
#endif

#if KERNEL_DEBUG_PREEMPT_UART
; enable uart RX interrupt
    ldi r17, 1 << RXCIE0
    sts UCSR0B, r17
#endif

    call _k_kernel_init

/*___________________________________________________________________________*/
