/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "idle.h"

#include <avr/sleep.h>

#define K_MODULE K_MODULE_IDLE

extern struct dnode *z_runq;
extern uint8_t z_ready_count;

#if CONFIG_KERNEL_THREAD_IDLE

static void z_thread_idle_entry(void *context);
#if defined(__QEMU__) && (CONFIG_THREAD_IDLE_COOPERATIVE != 0)
#warning "QEMU doesn't work well with CONFIG_THREAD_IDLE_COOPERATIVE, TODO"
#endif

#if CONFIG_THREAD_IDLE_COOPERATIVE
/* Z_THREAD_STACK_MIN_SIZE without interrupts */
#define Z_THREAD_IDLE_STACK_SIZE                                                         \
	(Z_THREAD_STACK_MIN_SIZE + CONFIG_KERNEL_THREAD_IDLE_ADD_STACK)
#define Z_THREAD_IDLE_PRIORITY (Z_THREAD_PRIO_COOP | Z_THREAD_PRIO_LOW)
#else
/* If IDLE thread can be preempted, plan additionnal stack */
#define Z_THREAD_IDLE_STACK_SIZE                                                         \
	(Z_THREAD_STACK_MIN_SIZE + Z_INTCTX_SIZE + CONFIG_KERNEL_THREAD_IDLE_ADD_STACK)
#define Z_THREAD_IDLE_PRIORITY (Z_THREAD_PRIO_PREEMPT | Z_THREAD_PRIO_LOW)
#endif

#if CONFIG_AVRTOS_LINKER_SCRIPT
Z_THREAD_DEFINE(z_thread_idle,
				z_thread_idle_entry,
				Z_THREAD_IDLE_STACK_SIZE,
				Z_THREAD_IDLE_PRIORITY,
				NULL,
				'I',
				0 // don't start IDLE thread at init
);
#else
__noinit uint8_t z_thread_idle_stack[Z_THREAD_IDLE_STACK_SIZE];
__noinit struct k_thread z_thread_idle;
void z_thread_idle_create(void)
{
	k_thread_create(&z_thread_idle, z_thread_idle_entry, z_thread_idle_stack,
					Z_THREAD_IDLE_STACK_SIZE, Z_THREAD_IDLE_PRIORITY, NULL, 'I');
}
#endif

#if CONFIG_IDLE_HOOK
/**
 * @brief Idle thread hook
 */
__attribute__((weak)) void z_idle_hook(void)
{
}
#endif

/**
 * @brief Idle thread entry function
 *
 * @param context : ignored for now
 */
static void z_thread_idle_entry(void *context)
{
	for (;;) {
#if CONFIG_IDLE_HOOK
		z_idle_hook();
#endif

#if CONFIG_THREAD_IDLE_COOPERATIVE
#warning                                                                                 \
	"CONFIG_THREAD_IDLE_COOPERATIVE is deprecated, prefer use of k_yield_from_isr_cond() instead"
		k_yield();
		// z_yield_from_idle_thread();
#endif /* CONFIG_THREAD_IDLE_COOPERATIVE */

		/* A bit buggy on QEMU but normally works fine */
#if !defined(__QEMU__) && (CONFIG_THREAD_IDLE_COOPERATIVE == 0)
		/* only an interrupt can wake up the CPU after this instruction
		 */
		sleep_cpu();
#endif /* __QEMU__ */
	}
}

#endif /* CONFIG_KERNEL_THREAD_IDLE */

bool k_is_cpu_idle(void)
{
#if CONFIG_KERNEL_THREAD_IDLE
	return z_ready_count == 0;
#else
	return false;
#endif /* CONFIG_KERNEL_THREAD_IDLE */
}

void k_idle(void)
{
	/* if others thread are ready, yield the CPU */
	if (z_ready_count != 0u) {
		k_yield();
	} else {
#ifndef __QEMU__
		/* A bit buggy on QEMU but normally works fine */
		sleep_cpu();
#endif /* __QEMU__ */
	}
}
