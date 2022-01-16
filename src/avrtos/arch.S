#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>
#include <avrtos/sysclock_config.h>

.global _k_thread_switch
.global _k_thread_entry
.global K_SYSCLOCK_TIMERX_OVF_vect

.extern _current
.extern runqueue

.extern _k_ticks 			; u32/u40
.extern _k_sched_ticks_remaining 	; u8

.extern _k_system_shift

K_SYSCLOCK_TIMERX_OVF_vect:
	push	r1
	push	r0
	lds	r0, SREG
	push	r0
	eor	r1, r1
	push	r18

#if K_SYSCLOCK_TCNT_ISNULL == 0
#if KERNEL_SYSLOCK_HW_TIMER == 1
	; from ATmega328p documentation : timer1 (16 bits)
	; > To do a 16-bit write, the high byte must be written before the low byte. 
	; > For a 16-bit read, the low byte must be read before the high byte.

	ldi r18, K_SYSCLOCK_TIMER_TCNTH
	sts K_SYSCLOCK_HW_REG_TCNTXH, r18
#endif
; KERNEL_SYSLOCK_HW_TIMER

	ldi r18, K_SYSCLOCK_TIMER_TCNTL
	sts K_SYSCLOCK_HW_REG_TCNTXL, r18
	; reset timer counter as soon as possible (to maximize accuracy)
#endif
; KERNEL_SYSCLOCK_PRECISION_MODE

#if KERNEL_TICKS
	ldi r18, 0x01			; 1 cycle

	lds r0, _k_ticks		; 2 cycles
	add r0, r18			; 1 cycle
	sts _k_ticks, r0		; 2 cycles

	lds r0, _k_ticks + 1		; 2 cycles
	adc r0, r1			; 1 cycle
	sts _k_ticks + 1, r0		; 2 cycles
	
	lds r0, _k_ticks + 2		; 2 cycles
	adc r0, r1			; 1 cycle
	sts _k_ticks + 2, r0		; 2 cycles

	lds r0, _k_ticks + 3		; 2 cycles
	adc r0, r1			; 1 cycle
	sts _k_ticks + 3, r0		; 2 cycles

#if KERNEL_TICKS_40BITS
	lds r0, _k_ticks + 4		; 2 cycles
	adc r0, r1			; 1 cycle
	sts _k_ticks + 4, r0		; 2 cycles
#endif
; KERNEL_TICKS_40BITS

#if KERNEL_SCHEDULER_VARIABLE_FREQUENCY
	lds r18, _k_sched_ticks_remaining
	subi r18, 0x01
	sts _k_sched_ticks_remaining, r18
	brne __int_restore_minimal_context
#endif 
#endif
; KERNEL_TICKS
	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
	push	r24
	push	r25
	push	r26
	push	r27
	push	r30
	push	r31

#if KERNEL_DEBUG
	ldi r24, 0x2e           ; '.'
	call usart_transmit
#endif
; KERNEL_DEBUG

	call _k_system_shift

#if KERNEL_PREEMPTIVE_THREADS && (KERNEL_DEBUG_PREEMPT_UART == 0)
	lds ZL, _current           ; load current thread addr in Z
	lds ZH, _current + 1

	ldd r18, Z + 2      ; read flag
	andi r18, K_FLAG_COOP | K_FLAG_SCHED_LOCKED
	brne __int_restore

__scheduler:
	call _k_scheduler
#endif
; KERNEL_PREEMPTIVE_THREADS

__int_restore:
	pop	r31
	pop	r30
	pop	r27
	pop	r26
	pop	r25
	pop	r24
	pop	r23
	pop	r22
	pop	r21
	pop	r20
	pop	r19
__int_restore_minimal_context:
	pop	r18
	pop	r0
	sts	SREG, r0
	pop	r0
	pop	r1
	reti


; _k_scheduler2:
; ; clear flags
; 	lds	r30, _current
; 	lds	r31, _current + 1
; 	ldd	r25, Z+2
; 	andi	r25, 0xBF	; MASK
; 	andi	r25, 0xDF 	; MASK
; 	std	Z+2, r25

; ; PENDING ?
; 	andi	r25, 0x03	; MASK
; 	cpi	r25, 0x02	; PENDING

; ; go to refrequeue
; 	brne	refrequeue

; ; _current = runqueue
; update_current:
; 	lds	r22, runqueue		; 0x80020e <runqueue>
; 	lds	r23, runqueue + 1	; 0x80020f <runqueue+0x1>
; 	subi	r22, 0x03		; get thread from ditem addr
; 	sbc	r23, r1
; 	sts	_current + 1, r23	; 0x800211 <_current+0x1>
; 	sts	_current, r22		; 0x800210 <_current>
 
;  ; prev == _current ?
;  	cp	r22, r30
;  	cpc	r23, r31

;  ; go to return
;  	breq	.+6

;  ; thread switch
;  	movw	r24, r30
;  	call	_k_thread_switch
;  	ret

; ; ref requeue
; refrequeue:
; 	lds	r26, runqueue	; 0x80020e <runqueue>
; 	lds	r27, runqueue + 1	; 0x80020f <runqueue+0x1>
; 	ld	r24, X+
; 	ld	r25, X
; 	sts	runqueue + 1, r25	; 0x80020f <runqueue+0x1>
; 	sts	runqueue, r24	; 0x80020e <runqueue>
; 	jmp 	update_current

; "from" thread addr is in r24, r25
; "to" thread  addr in in r22, r23
_k_thread_switch:
	push r2
	push r3
	push r4
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
	lds r17, SREG
	push r17

	; save context and write SP in thread structure
	movw r30, r24
	lds r20, SPL
	lds r21, SPH
	st Y+, r20
	st Y+, r21

	; restore SP and restore context
	movw r30, r22
	ld r20, Z+
	ld r21, Z+
	sts SPL, r20
	sts SPH, r21

	pop r17
	sts SREG, r17
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	pop r3
	pop r2

	ret

.extern k_stop
.extern __fault
_k_thread_entry:
	movw r24, r2
	movw r30, r4
	icall

; TODO remove or add configuration to forbid thread termination
	ldi r24, 0x03 ; K_THREAD_TERMINATED
	call __fault

	call k_stop


.global __k_interrupts
__k_interrupts:
    ldi r24, 0x00
    lds r25, SREG
    sbrc r25, SREG_I
    ldi r24, 0x01
    ret


#if KERNEL_TICKS
.global k_ticks_get_32
.global k_ticks_get_64
k_ticks_get_32:
	lds	r26, SREG
	cli
	lds 	r22, _k_ticks
	lds 	r23, _k_ticks + 1
	lds 	r24, _k_ticks + 2
	sts	SREG, r26
	lds 	r25, _k_ticks + 3
	ret

k_ticks_get_64:
	lds	r26, SREG
	cli
	lds 	r18, _k_ticks
	lds 	r19, _k_ticks + 1
	lds 	r20, _k_ticks + 2
	lds 	r21, _k_ticks + 3
	sts	SREG, r26
#if KERNEL_TICKS_40BITS
	lds	r22, _k_ticks + 4
#else
	ldi	r22, 0x00
#endif /* KERNEL_TICKS_40BITS */
	ldi	r23, 0x00
	ldi	r24, 0x00
	ldi	r25, 0x00
	ret
#endif /* KERNEL_UPTIME */

.global __debug
__debug:
	ret


#if KERNEL_DEBUG_PREEMPT_UART
.global _K_USART_RX_vect
_K_USART_RX_vect:
	push	r1
	push	r0
	lds	r0, SREG
	push	r0
	eor	r1, r1
	push	r18
	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
	push	r24
	push	r25
	push	r26
	push	r27
	push	r30
	push	r31

	lds r24, UDR0
	call usart_transmit

	call _k_scheduler

	jmp __int_restore
#endif
; KERNEL_DEBUG_PREEMPT_UART