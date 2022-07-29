#include "kernel_internals.h"
#include "kernel.h"

#include "idle.h"

#include <util/delay.h>
#include <util/atomic.h>

#include "debug.h"

/*___________________________________________________________________________*/

#define K_MODULE    K_MODULE_KERNEL

/*___________________________________________________________________________*/

void yield(void)
{
	k_yield();
}

/*___________________________________________________________________________*/

/**
 * @brief Runqueue containing the queue of all ready threads.
 * Should never be NULL.
 */
struct ditem *_k_runqueue = &_k_thread_main.tie.runqueue;

struct titem *_k_events_queue = NULL;

static inline bool _k_runqueue_single(void)
{
	return _k_runqueue->next == _k_runqueue;
}

#if KERNEL_SCHEDULER_VARIABLE_FREQUENCY
uint8_t _k_sched_ticks_remaining = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_SCHEDULER_VARIABLE_FREQUENCY */

#if KERNEL_TICKS
// necessary ?
union {
	uint8_t bytes[KERNEL_TICKS_SIZE];
	struct {
		uint32_t u32;

#if KERNEL_TICKS_40BITS
		uint8_t u40_byte;
#endif
	};

} _k_ticks = {
	.bytes = {
		0,
		0,
		0,
		0,
#if KERNEL_TICKS_40BITS == 5
		0,
#endif
	}
};
#endif /* KERNEL_TICKS */

/*___________________________________________________________________________*/

//
// Kernel Private API
//

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

extern struct k_thread _k_idle;

/**
 * @brief Queue the thread in the runqueue. We assume that the thread {th} is K_READY.
 * The thread must not be added to the runqueue already. Keep it's flag unchanged.
 * Assume that the interrupt flag is cleared when called.
 * Assume that the runqueue doesn't contain the IDLE thread
 *
 * @param th : ready thread to queue
 */
static K_NOINLINE void _k_queue(struct k_thread *const th)
{
	__ASSERT_NOINTERRUPT();

	push_back(_k_runqueue, &th->tie.runqueue);
}

/**
 * @brief Schedule the thread to be executed.
 * If the IDLE thread is in the runqueue (it is removed), the scheduled thread become the only thread in the runqueue.
 * Thread is added to the top of the runqueue.
 * - Assume that the thread is K_READY
 * - Assume that the thread is not in the runqueue
 *
 * @param thread_tie thread.tie.runqueue item
 * @return __attribute__((noinline))
 */
static K_NOINLINE void _k_schedule(struct k_thread *thread)
{
	__ASSERT_NOINTERRUPT();

#if THREAD_STACK_SENTINEL && KERNEL_ASSERT
	/* check that stack sentinel is still valid before switching to thread */
	if (k_verify_stack_sentinel(thread) == false) {
		__fault(K_FAULT_SENTINEL);
	}
#endif

	/* Clear flag telling whether the thread is pending */
	thread->wakeup_schd = 0;

	/* Mark this thread as READY */
	thread->state = K_READY;

	if (KERNEL_THREAD_IDLE == 0) {
		/* If there is no thread IDLE,
		*  simply add the thread to running threads
		*/
		push_front(_k_runqueue, &thread->tie.runqueue);
	} else if (0) { /* todo THREAD_IDLE_COOPERATIVE */
		/* if there is a thread IDLE and it's cooperative,
		 * simply add the thread to running threads
		 *
		 * Note: IDLE thread will remove itself from the runqueue.
		 */
		push_front(_k_runqueue, &thread->tie.runqueue);

		/* Mark the IDLE thread as stopped so that it will
		 * remove itself from the runqueue on _k_yield_from_idle_thread() call
		 */
		_k_idle.state = K_STOPPED;
	} else {
		/* if thread IDLE is preemptive, then we need to update
		 * the runqueue here, because thread idle will be preempted
		 * immediately after
		 */
		if (k_is_cpu_idle() == true) {
			dlist_ref(&thread->tie.runqueue);
			_k_runqueue = &thread->tie.runqueue;
		} else {
			push_front(_k_runqueue, &thread->tie.runqueue);
		}
	}
}

/**
 * @brief Do a thread switch (ASM function)
 * 1. save context of the first thread
 * 2. store the SP of the first thread to its structure
 * 3. restore the SP of the second thread from its structure
 * 4. restore context of the second thread
 *
 * @param from
 * @param to
 */
extern void _k_thread_switch(struct k_thread *from,
			     struct k_thread *to);

/**
 * @brief Schedule current thread wake up.
 *
 * Assumptions:
 * - thread is suspended (K_PENDING)
 * - thread is not in the runqueue
 *
 * @param thread
 * @param timeout
 * @return K_NOINLINE
 */
static K_NOINLINE void _k_schedule_wake_up(k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_TRUE(_current->wakeup_schd == 0);

	if (!K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		_current->wakeup_schd = 1;
		_current->tie.event.timeout = K_TIMEOUT_TICKS(timeout);
		_current->tie.event.next = NULL;
		_tqueue_schedule(&_k_events_queue, &_current->tie.event);
	}
}

/**
 * @brief Remove the current thread from the runqueue.
 * Stop the execution of the current thread (until it is scheduled again with function _k_schedule or _k_schedule_wake_up)
 * State flag is changed to K_PENDING.
 *
 * Assumptions:
 * - interrupt flag is cleared when called.
 * - thread is in the runqueue
 */
static K_NOINLINE void _k_suspend(void)
{
	__ASSERT_NOINTERRUPT();

	_current->state = K_PENDING;

	if (KERNEL_THREAD_IDLE) {
		if (_k_runqueue_single() == true) {
			/* if the suspended thread is the only one it the runqueue
			 * then add the idle thread to the runqueue */
			struct ditem *const tie = &_k_idle.tie.runqueue;
			dlist_ref(tie);
			_k_runqueue = tie;
			
			if (0) { /* todo THREAD_IDLE_COOPERATIVE */
				/* if idle thread is cooperative,
				 * we need to mark it as ready, to preserve
				 * threads context integrity */
				_k_idle.state = K_READY;
			}
		} else {
			/* if there are more threads, simply remove the suspended
			 * thread from the runqueue */
			pop_ref(&_k_runqueue);
		}
	} else {
		/* If IDLE thread is not enabled, check that at least one other
		 * thread is running */
		__ASSERT_LEASTTWO_RUNNING();

		pop_ref(&_k_runqueue);
	}
}

/**
 * @brief Wake up a thread that is pending for an event.
 *
 * Assumptions:
 *  - thread is in K_PENDING mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 *  - interrupt flag is cleared when called.
 *
 *
 * @param th thread to wake up
 */
static K_NOINLINE void _k_wake_up(struct k_thread *th)
{
	__ASSERT_NOTNULL(th);
	__ASSERT_NOINTERRUPT();
	__ASSERT_THREAD_STATE(th, K_PENDING);

	__K_DBG_WAKEUP(th); // @

	/* Remove the thread from the events queue */
	if (th->wakeup_schd) {
		tqueue_remove(&_k_events_queue, &th->tie.event);
	}

	_k_schedule(th);
}

/**
 * @brief Suspend the current thread and schedule its awakening for later
 *
 * Assumptions :
 *  - interrupt flag is cleared when called.
 *
 * @param timeout
 */
static K_NOINLINE void _k_reschedule(k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();

	_k_suspend();

	_k_schedule_wake_up(timeout);
}

inline static void swap_endianness(void **addr)
{
	*addr = (void *)HTONS(*addr);
}

void _k_kernel_init(void)
{
	/* main thread is the first running (ready or not),
	 * and it is already in queue */
	for (uint8_t i = 0; i < &__k_threads_end - &__k_threads_start; i++) {
		struct k_thread *const thread = &(&__k_threads_start)[i];

		if (thread == &_k_thread_main) {
			continue;
		}

#if KERNEL_THREAD_IDLE
		const bool is_thread_idle = thread == &_k_idle;
#else
		const bool is_thread_idle = false;
#endif

		/* idle thread must not be added to the
		 * runqueue as the main thread is running */
		if (!is_thread_idle &&
		    (thread->state == K_READY)) {
			push_back(_k_runqueue, &thread->tie.runqueue);
		}

		/* Swap endianness of addresses in compilation-time built stacks.
		* We cannot change the endianness of addresses determined by the
		* linker at compilation time. So we need to do it on system start up
		*/
		struct _k_callsaved_ctx *ctx = thread->stack.end -
			_K_CALLSAVED_CTX_SIZE + 1u;
		swap_endianness(&ctx->thread_context);
		swap_endianness((void *)&ctx->thread_entry);
		swap_endianness(&ctx->pc);

#if __AVR_3_BYTE_PC__
		ctx->pch = 0;
#endif /* __AVR_3_BYTE_PC__ */
	}
}

void _k_system_shift(void)
{
	__ASSERT_NOINTERRUPT();
	__STATIC_ASSERT_AUTOMSG(KERNEL_TIME_SLICE_TICKS != 0);

	tqueue_shift(&_k_events_queue, KERNEL_TIME_SLICE_TICKS);

#if KERNEL_SCHEDULER_VARIABLE_FREQUENCY
	_k_sched_ticks_remaining = KERNEL_TIME_SLICE_TICKS;
#endif /* KERNEL_SCHEDULER_VARIABLE_FREQUENCY */

	struct ditem *const ready = (struct ditem *)tqueue_pop(&_k_events_queue);
	if (ready != NULL) {
		struct k_thread *const thread = THREAD_FROM_EVENTQUEUE(ready);

		__K_DBG_SCHED_EVENT(thread);  // !

		/* set ready thread expired flag */
		thread->timer_expired = 1u;

		_k_schedule(thread);
	}

#if KERNEL_TIMERS
	_k_timers_process();
#endif

#if KERNEL_EVENTS
	_k_event_q_process();
#endif /* KERNEL_EVENTS */
}


/**
 * @brief Choose the next thread to be executed.
 * This function is called during any thread switch in order to determine which
 * one is the following thread to be executed.
 *
 * This function is called in k_yield function
 *
 * @return struct k_thread* : next thread to be executed
 */
struct k_thread *_k_scheduler(void)
{
	__ASSERT_NOINTERRUPT();

#if THREAD_STACK_SENTINEL && KERNEL_ASSERT
	k_assert_registered_stack_sentinel();
#endif

	struct k_thread *const prev = _current;

	/* reset flags */
	prev->pend_canceled = 0;
	prev->timer_expired = 0;

 	if (prev->state == K_PENDING) {
		/* runqueue is positionned to the
		 * normally next thread to be executed */
		__K_DBG_SCHED_PENDING();        // ~
	} else {
		/* next thread is positionned at the top of the runqueue */
		ref_requeue(&_k_runqueue);

		__K_DBG_SCHED_REQUEUE();        // >
	}

	_current = CONTAINER_OF(_k_runqueue, struct k_thread, tie.runqueue);

	__ASSERT_THREAD_STATE(_current, K_READY);

	__K_DBG_SCHED_NEXT(_current);

	return prev;
}

K_NOINLINE int8_t _k_pend_current(struct ditem *waitqueue,
				  k_timeout_t timeout)
{
	__ASSERT_NOINTERRUPT();

	int8_t err = -1;
	if (timeout.value != 0) {
		/* queue thread to pending queue of the object */
		dlist_queue(waitqueue, &_current->wany);

		/* schedule thread wake up if timeout is set */
		_k_reschedule(timeout);

		_k_yield();

		/* if timer expired, we manually remove the thread from
		 * the pending queue
		 */
		if (_current->timer_expired) {
			dlist_remove(&_current->wany);
			err = -ETIMEOUT;
		} else if (_current->pend_canceled) {
			err = -ECANCEL;
		} else {
			err = 0;
		}
	}
	return err;
}

K_NOINLINE struct k_thread *_k_unpend_first_thread(struct ditem *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct ditem *tie = dlist_dequeue(waitqueue);
	if (DITEM_VALID(waitqueue, tie)) {
		struct k_thread *pending_thread = THREAD_FROM_WAITQUEUE(tie);

		/* immediate wake up is not more required because
		 * with the swap model, the object is already reserved for the
		 * first pending thread
		 */
		_k_wake_up(pending_thread);

		/* we return !NULL if a pending thread got the object*/
		return pending_thread;
	}
	/* if no thread is pending on the object, we simply return */
	return NULL;
}

K_NOINLINE struct k_thread *_k_unpend_first_and_swap(struct ditem *waitqueue,
						     void *set_swap_data)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = _k_unpend_first_thread(waitqueue);
	if ((pending_thread != NULL) && (set_swap_data != NULL)) {
		pending_thread->swap_data = set_swap_data;
	}
	return pending_thread;
}

K_NOINLINE void _k_cancel_first_pending(struct ditem *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	struct k_thread *pending_thread = _k_unpend_first_thread(waitqueue);
	if (pending_thread != NULL) {
		/* tells that we canceled the thread pending on the object */
		pending_thread->pend_canceled = 1;
	}
}

K_NOINLINE uint8_t _k_cancel_pending(struct ditem *waitqueue)
{
	__ASSERT_NOINTERRUPT();
	__ASSERT_NOTNULL(waitqueue);

	uint8_t count = 0;
	struct k_thread *pending_thread;
	for (;;) {
		pending_thread = _k_unpend_first_thread(waitqueue);
		if (pending_thread != NULL) {
			pending_thread->pend_canceled = 1;

			count++;
		} else {
			return count;
		}
	}
}

/*___________________________________________________________________________*/

//
// Kernel Public API
//

#if KERNEL_IRQ_LOCK_COUNTER
void irq_disable(void)
{
	cli();

	_current->irq_lock_cnt++;
}

void irq_enable(void)
{
	if (_current->irq_lock_cnt == 0) {
		return;
	} else if (_current->irq_lock_cnt == 1) {
		_current->irq_lock_cnt = 0;
	} else {
		_current->irq_lock_cnt--;
		return;
	}

	sei();
}
#endif

void k_sched_lock(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_current->sched_lock = 1;
	}

#if KERNEL_SCHED_LOCK_COUNTER
	_current->sched_lock_cnt++;
#endif /* KERNEL_SCHED_LOCK_COUNTER */

	__K_DBG_SCHED_LOCK(_current);
}

void k_sched_unlock(void)
{
#if KERNEL_SCHED_LOCK_COUNTER
	if (_current->sched_lock_cnt == 0) {
		return;
	} else if (_current->sched_lock_cnt == 1) {
		_current->sched_lock_cnt = 0;
	} else {
		_current->sched_lock_cnt--;
		return;
	}
#endif /* KERNEL_SCHED_LOCK_COUNTER */

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_current->sched_lock = 0;
	}

	__K_DBG_SCHED_UNLOCK();
}

static uint8_t _cur_get_flags(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return _current->flags;
	}

	__builtin_unreachable();
}

bool k_sched_locked(void)
{
	const uint8_t flags = _cur_get_flags();

	return (flags & (K_FLAG_SCHED_LOCKED | K_FLAG_COOP)) != 0;
}


bool k_cur_is_preempt(void)
{
	return (_cur_get_flags() & K_FLAG_COOP) == 0;
}

bool k_cur_is_coop(void)
{
	return (_cur_get_flags() & K_FLAG_COOP) != 0;
}

void k_sleep(k_timeout_t timeout)
{
	if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			_k_reschedule(timeout);

			_k_yield();
		}
	}
}

#if KERNEL_UPTIME
void k_wait(k_timeout_t timeout)
{
	__ASSERT_INTERRUPT();

	uint64_t ticks = k_ticks_get_64();
	uint64_t now;

	do {
		k_idle(); /* idle the thread until next interrupt */

		now = k_ticks_get_64();
	} while (now - ticks < K_TIMEOUT_TICKS(timeout));
}
#endif /* KERNEL_UPTIME */

void k_block(k_timeout_t timeout)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		k_ticks_t ticks = K_TIMEOUT_TICKS(timeout);
		while (ticks != 0) {
			_delay_us(K_TICKS_US);
			ticks--;
		}
	}
}

void k_suspend(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		_k_suspend();
	}
}

void k_resume(struct k_thread *th)
{
	if (th->state == K_PENDING) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			_k_schedule(th);
		}
	} else {
		/* thread pending, ready of running and then already started */
	}
}

void k_start(struct k_thread *th)
{
	if (th->state == K_STOPPED) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			_k_schedule(th);
		}
	}
}

/**
 * @brief Stop the thread
 *
 * @param th : ready/pending thread to start.
 */
static K_NOINLINE void _k_stop(void)
{
	__ASSERT_NOINTERRUPT();

	_k_suspend();

	_current->state = K_STOPPED;
}

void k_stop()
{
	cli();

	_k_stop();

	_k_yield();
}

/*___________________________________________________________________________*/

#if KERNEL_UPTIME

uint32_t k_uptime_get(void)
{
#if KERNEL_TICKS_40BITS
	return k_ticks_get_64() / K_TICKS_PER_SECOND;
#else
	return k_ticks_get_32() / K_TICKS_PER_SECOND;
	return 0;
#endif /* KERNEL_UPTIME */
}

uint32_t k_uptime_get_ms32(void)
{
#if KERNEL_TICKS
	return k_ticks_get_32() / K_TICKS_PER_MS;
#else
	return 0;
#endif /* KERNEL_UPTIME */
}

uint64_t k_uptime_get_ms64(void)
{
#if KERNEL_TICKS_40BITS
	return k_ticks_get_64() / K_TICKS_PER_MS;
#elif KERNEL_TICKS
	return k_ticks_get_32() / K_TICKS_PER_MS;
#else
	return 0;
#endif /* KERNEL_UPTIME */
}

#endif /* KERNEL_UPTIME */

/*___________________________________________________________________________*/