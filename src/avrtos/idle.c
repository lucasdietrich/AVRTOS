/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "idle.h"

#include <avr/sleep.h>


#define K_MODULE K_MODULE_IDLE

extern uint8_t _k_ready_count;
extern struct ditem *_k_runqueue;

extern bool _k_runqueue_single(void);

#if CONFIG_KERNEL_THREAD_IDLE

static void _k_idle_entry(void *context);

#if defined(__QEMU__) && (CONFIG_THREAD_IDLE_COOPERATIVE != 0)
#	warning "QEMU doesn't support cooperative IDLE thread for now and I don't know why "
#endif

/*
 * K_THREAD_STACK_MIN_SIZE would be enough
 */
#if CONFIG_THREAD_IDLE_COOPERATIVE

/* TODO, cannot exaplin why it needs 22B instead of 21B ??? */
/* 2B yield+ 19B ctx + ? slight overflow due to scheduler + tqueue/duqueue function ?*/

// K_THREAD_MINIMAL_DEFINE(_k_idle, _k_idle_entry, K_COOPERATIVE, NULL, 'I');
K_THREAD_DEFINE(_k_idle,
		_k_idle_entry,
		K_THREAD_STACK_MIN_SIZE + CONFIG_KERNEL_THREAD_IDLE_ADD_STACK,
		K_COOPERATIVE | K_FLAG_PRIO_LOW,
		NULL,
		'I');
#else

/**
 * @brief If IDLE thread can be preempted, plan additionnal stack
 */
K_THREAD_DEFINE(_k_idle,
		_k_idle_entry,
		K_THREAD_STACK_MIN_SIZE + _K_INTCTX_SIZE + CONFIG_KERNEL_THREAD_IDLE_ADD_STACK,
		K_PREEMPTIVE | K_FLAG_PRIO_LOW,
		NULL,
		'I');
#endif

/**
 * @brief Idle thread entry function
 * 
 * @param context : ignored for now
 */
static void _k_idle_entry(void *context)
{
	for (;;) {
#if CONFIG_THREAD_IDLE_COOPERATIVE
#warning "CONFIG_THREAD_IDLE_COOPERATIVE is deprecated, prefer use of k_yield_from_isr_cond() instead"
		k_yield();
		// _k_yield_from_idle_thread();
#endif /* CONFIG_THREAD_IDLE_COOPERATIVE */

		/* A bit buggy on QEMU but normally works fine */
#if !defined(__QEMU__) && (CONFIG_THREAD_IDLE_COOPERATIVE == 0)
		/* only an interrupt can wake up the CPU after this instruction */
		sleep_cpu();
#endif /* __QEMU__ */
		
	}
}

#endif /* CONFIG_KERNEL_THREAD_IDLE */

bool k_is_cpu_idle(void)
{
#if CONFIG_KERNEL_THREAD_IDLE
	return _k_ready_count == 0;
#else 
	return false;
#endif /* CONFIG_KERNEL_THREAD_IDLE */
}

void k_idle(void)
{
	/* if others thread are ready, yield the CPU */
	if (_k_ready_count != 0u) {
		k_yield();
	} else {
#ifndef __QEMU__
		/* A bit buggy on QEMU but normally works fine */
		sleep_cpu();
#endif /* __QEMU__ */
	}
}

/*___________________________________________________________________________*/