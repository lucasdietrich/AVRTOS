#include "idle.h"

#include <avr/sleep.h>


#define K_MODULE K_MODULE_IDLE

#if KERNEL_THREAD_IDLE

static void _k_idle_entry(void *context);

#if defined(__QEMU__) && (THREAD_IDLE_COOPERATIVE != 0)
#	error "QEMU doesn't support cooperative IDLE thread for now and I don't know why "
#endif

/*
 * K_THREAD_STACK_MIN_SIZE would be enough
 */
#if THREAD_IDLE_COOPERATIVE

/* TODO, cannot exaplin why it needs 22B instead of 21B ??? */
/* 2B yield+ 19B ctx + ? slight overflow due to scheduler + tqueue/duqueue function ?*/

// K_THREAD_MINIMAL_DEFINE(_k_idle, _k_idle_entry, K_COOPERATIVE, NULL, 'I');
K_THREAD_DEFINE(_k_idle,
		_k_idle_entry,
		K_THREAD_STACK_MIN_SIZE + KERNEL_THREAD_IDLE_ADD_STACK,
		K_COOPERATIVE,
		NULL,
		'I');
#else

/**
 * @brief If IDLE thread can be preempted, plan additionnal stack
 */
K_THREAD_DEFINE(_k_idle,
		_k_idle_entry,
		K_THREAD_STACK_MIN_SIZE + _K_INTCTX_SIZE + KERNEL_THREAD_IDLE_ADD_STACK,
		K_PREEMPTIVE,
		NULL,
		'I');
#endif

extern struct ditem *_k_runqueue;

static inline bool _k_runqueue_single(void)
{
        return _k_runqueue->next == _k_runqueue;
}

/* TODO THREAD_IDLE_COOPERATIVE */
#if 0
/**
 * @brief Yield function to be used only in the IDLE thread.
 * 
 * If detects the K_STOPPED is set for it and will remove itself from
 * the runqueue in order to let the ready thread to be executed.
 */
static void _k_yield_from_idle_thread(void)
{
	cli();

	__ASSERT_NOINTERRUPT();
	__ASSERT(K_ASSERT_ISTHREADIDLE, _current == &_k_idle);

	if (_k_idle.state == K_STOPPED) {
		
		/* check that the idle thread is still the first thread in the runqueue */
		__ASSERT(
			K_ASSERT_ISTHREADIDLE,
			CONTAINER_OF(_k_runqueue, struct k_thread, tie.runqueue) == &_k_idle);
		
		/* check that there is another thread ready to run */
		__ASSERT_LEASTTWO_RUNNING();

		/* remove IDLE thread from runqueue */
		pop_ref(&_k_runqueue);

		/* yield to ready thread */
		_k_yield();
	}

	/* interrupts are always enabled in idle thread */
	sei();
}
#endif /* THREAD_IDLE_COOPERATIVE */

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
		// _k_yield_from_idle_thread();
#endif /* THREAD_IDLE_COOPERATIVE */

		/* A bit buggy on QEMU but normally works fine */
#if !defined(__QEMU__) || (THREAD_IDLE_COOPERATIVE == 0)
		/* only an interrupt can wake up the CPU after this instruction */
		sleep_cpu();
#endif /* __QEMU__ */

		
	}
}

#endif /* KERNEL_THREAD_IDLE */

bool k_is_cpu_idle(void)
{
#if KERNEL_THREAD_IDLE
	return _k_runqueue == &_k_idle.tie.runqueue;
#else 
	return false;
#endif /* KERNEL_THREAD_IDLE */
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