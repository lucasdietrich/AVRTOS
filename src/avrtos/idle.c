/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "idle.h"

#include <avr/sleep.h>

#define K_MODULE K_MODULE_IDLE

extern uint8_t z_ready_count;
extern struct dnode *z_runqueue;

extern bool z_runqueue_single(void);

#if CONFIG_KERNEL_THREAD_IDLE

static void z_idle_entry(void *context);

#if defined(__QEMU__) && (CONFIG_THREAD_IDLE_COOPERATIVE != 0)
#warning "QEMU doesn't support cooperative IDLE thread for now and I don't know why "
#endif

/*
 * K_THREAD_STACK_MIN_SIZE would be enough
 */
#if CONFIG_THREAD_IDLE_COOPERATIVE

/* TODO, cannot exaplin why it needs 22B instead of 21B ??? */
/* 2B yield+ 19B ctx + ? slight overflow due to scheduler + tqueue/duqueue
 * function ?*/

// K_THREAD_MINIMAL_DEFINE(z_idle, z_idle_entry, K_COOPERATIVE, NULL, 'I');
K_THREAD_DEFINE(z_idle,
		z_idle_entry,
		K_THREAD_STACK_MIN_SIZE + CONFIG_KERNEL_THREAD_IDLE_ADD_STACK,
		K_COOPERATIVE | Z_FLAG_PRIO_LOW,
		NULL,
		'I');
#else

/**
 * @brief If IDLE thread can be preempted, plan additionnal stack
 */
K_THREAD_DEFINE(z_idle,
		z_idle_entry,
		K_THREAD_STACK_MIN_SIZE + Z_INTCTX_SIZE +
			CONFIG_KERNEL_THREAD_IDLE_ADD_STACK,
		K_PREEMPTIVE | Z_FLAG_PRIO_LOW,
		NULL,
		'I');
#endif

/**
 * @brief Idle thread entry function
 *
 * @param context : ignored for now
 */
static void z_idle_entry(void *context)
{
	for (;;) {
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

/*___________________________________________________________________________*/