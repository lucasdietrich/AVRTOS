#include "idle.h"

#include <avr/sleep.h>
/*___________________________________________________________________________*/

#if KERNEL_THREAD_IDLE

static void _k_idle_entry(void *context);

/*
 * 1U is just a to make sure there is no stack overflow.
 * K_THREAD_STACK_MIN_SIZE would be enough
 */
#if THREAD_IDLE_COOPERATIVE
K_THREAD_DEFINE(_k_idle, _k_idle_entry, K_THREAD_STACK_MIN_SIZE + 1U, K_COOPERATIVE, NULL, 'I');
#else

/**
 * @brief If IDLE thread can be preempted, plan additionnal stack
 */
K_THREAD_DEFINE(_k_idle, _k_idle_entry, K_THREAD_STACK_MIN_SIZE + 1U + 17U, K_PREEMPTIVE, NULL, 'I');
#endif

/**
 * @brief Idle thread entry function
 * 
 * @param context : ignored for now
 */
static void _k_idle_entry(void *context)
{
        for (;;) {

#if THREAD_IDLE_COOPERATIVE
		k_yield();
#endif /* THREAD_IDLE_COOPERATIVE */

		/* A bit buggy on QEMU but normally works fine */
#ifndef __QEMU__
		/* only an interrupt can wake up the CPU after this instruction */
                sleep_cpu();
#endif /* __QEMU__ */
        }
}

#endif /* KERNEL_THREAD_IDLE */

extern struct ditem *_k_runqueue;

bool k_is_cpu_idle(void)
{
#if KERNEL_THREAD_IDLE
	return _k_runqueue == &_k_idle.tie.runqueue;
#else 
	return false;
#endif /* KERNEL_THREAD_IDLE */
}

static inline bool _k_runqueue_single(void)
{
        return _k_runqueue->next == _k_runqueue;
}

void k_idle(void)
{
	/* if others thread are ready, yield the CPU */
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		if (_k_runqueue_single() == false) {
			return _k_yield();
		}
	}

#ifndef __QEMU__
	/* A bit buggy on QEMU but normally works fine */
	sleep_cpu();
#endif /* __QEMU__ */
}

/*___________________________________________________________________________*/