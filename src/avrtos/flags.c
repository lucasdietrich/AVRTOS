#include "flags.h"

#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>

#define KERNEL_FLAGS_OPT_SET_ALL_ENABLED 1
#define KERNEL_FLAGS_OPT_SET_ANY_ENABLED 1
#define KERNEL_FLAGS_OPT_CLR_ALL_ENABLED 1
#define KERNEL_FLAGS_OPT_CLR_ANY_ENABLED 1

#define SWAP_DATA_GET_OPTIONS(swapd) ((uint8_t)(((uint16_t) swapd) >> 8u))
#define SWAP_DATA_GET_PEND_MASK(swapd) ((uint8_t)(((uint16_t) swapd) & 0xFFu))
#define SWAP_DATA_GET_TRIG_MASK(swapd) ((uint8_t)(((uint16_t) swapd) & 0xFFu))

#define SWAP_DATA_INIT_TRIG_MASK(trig) ((void *) (((uint16_t) trig) & 0xFFu))
#define SWAP_DATA_INIT_OPT_N_MASK(opt, mask) ((void *) ((((uint16_t) opt) << 8u) | ((uint16_t) mask)))

int k_flags_init(struct k_flags *flags,
		 uint8_t value)
{
#if KERNEL_CHECKS
	if (flags == NULL) {
		return -EINVAL;
	}
#endif

	dlist_init(&flags->_waitqueue);
	flags->flags = value;
	flags->reset_value = value;

	return 0;
}

int k_flags_poll(struct k_flags *flags,
		 uint8_t mask,
		 k_flags_options options,
		 k_timeout_t timeout)
{
	int ret = -EAGAIN;

#if KERNEL_CHECKS
	if (flags == NULL) {
		return -EINVAL;
	}

	if ((options & ~(K_FLAGS_SET_ANY | K_FLAGS_CONSUME)) != 0u) {
		ret = -ENOTSUP;
		goto exit;
	}
#endif

	if (mask == 0u) {
		ret = 0;
		goto exit;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint8_t value = flags->flags;
		const uint8_t trig = value & mask;

		if (trig == 0u) {
			/* Set into swap_data flags the thread in pending on and
			 * K_FLAGS_CONSUME option */
			_current->swap_data = SWAP_DATA_INIT_OPT_N_MASK(options, mask);
			
			ret = _k_pend_current(&flags->_waitqueue, timeout);
			if (ret == 0) {
				/* Bits that made the thread ready are stored in swap_data */
				ret = SWAP_DATA_GET_TRIG_MASK(_current->swap_data);
			}
		} else {
			if ((options & K_FLAGS_CONSUME) != 0u) {
				value &= ~mask;
				flags->flags = value;
			}
			ret = trig;
		}
	}

exit:
	return ret;
}

int k_flags_notify(struct k_flags *flags,
		   uint8_t mask,
		   k_flags_options options)
{
	int ret = 0;

#if KERNEL_CHECKS
	if (flags == NULL) {
		return -EINVAL;
	}

	if ((options & ~(K_FLAGS_SET | K_FLAGS_SCHED)) != 0u) {
		ret = -ENOTSUP;
		goto exit;
	}
#endif

	if (mask == 0u) {
		goto exit;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		uint8_t value = flags->flags;
		uint8_t notify_value = ~value & mask;

		struct ditem *const wq = &flags->_waitqueue;
		struct ditem *thread_item = wq->head;

		do {
			if (DITEM_VALID(wq, thread_item)) {
				struct k_thread *const thread =
					CONTAINER_OF(thread_item, struct k_thread, wflags);
				const uint16_t swap_data = (uint16_t)thread->swap_data;
				const uint8_t mask = SWAP_DATA_GET_PEND_MASK(swap_data);
				const uint8_t options = SWAP_DATA_GET_OPTIONS(swap_data);
				const uint8_t trig = notify_value & mask;

				if (trig != 0u) {
					thread->swap_data = SWAP_DATA_INIT_TRIG_MASK(trig);

					/* Unpend thread */
					dlist_remove(thread_item);
					_k_wake_up(thread);

					if (options & K_FLAGS_CONSUME) {
						notify_value &= ~trig;
					}

					/* Increment number of thread notified */
					ret++;
				}

				thread_item = thread_item->next;
			} else {
				/* No more threads, save remaining value to notify
				 * into k_flags structure */
				flags->flags |= notify_value;
				notify_value = 0u;
			}
		} while (notify_value != 0u);

		if (options & K_FLAGS_SCHED) {
			if (ret != 0) {
				_k_yield();
			}
		}
	}

exit:
	return ret;
}

int k_flags_reset(struct k_flags *flags)
{
	int ret = -EINVAL;
#if KERNEL_CHECKS
	if (flags == NULL) {
		goto exit;
	}
#endif

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = _k_cancel_all_pending(&flags->_waitqueue);
		flags->flags = flags->reset_value;
	}
#if KERNEL_CHECKS
exit:
#endif
	return ret;
}