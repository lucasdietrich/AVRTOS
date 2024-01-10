#include "flags.h"

#include "kernel.h"
#include "kernel_private.h"

#define KERNEL_FLAGS_OPT_SET_ALL_ENABLED 0
#define KERNEL_FLAGS_OPT_SET_ANY_ENABLED 1
#define KERNEL_FLAGS_OPT_CLR_ALL_ENABLED 0
#define KERNEL_FLAGS_OPT_CLR_ANY_ENABLED 1

#define Z_SWAP_DATA_GET_OPTIONS(swapd)	 ((uint8_t)(((uint16_t)swapd) >> 8u))
#define Z_SWAP_DATA_GET_PEND_MASK(swapd) ((uint8_t)(((uint16_t)swapd) & 0xFFu))
#define Z_SWAP_DATA_GET_TRIG_MASK(swapd) ((uint8_t)(((uint16_t)swapd) & 0xFFu))

#define Z_SWAP_DATA_INIT_TRIG_MASK(trig) ((void *)(((uint16_t)trig) & 0xFFu))
#define Z_SWAP_DATA_INIT_OPT_N_MASK(opt, mask) \
	((void *)((((uint16_t)opt) << 8u) | ((uint16_t)mask)))

int k_flags_init(struct k_flags *flags, uint8_t value)
{
	Z_ARGS_CHECK(flags) return -EINVAL;

	dlist_init(&flags->_waitqueue);
	flags->flags	   = value;
	flags->reset_value = value;

	return 0;
}

int k_flags_poll(struct k_flags *flags,
		 uint8_t mask,
		 k_flags_options options,
		 k_timeout_t timeout)
{
	int ret = -EAGAIN;

	Z_ARGS_CHECK(flags) return -EINVAL;
	Z_ARGS_CHECK((options & ~(K_FLAGS_SET_ANY | K_FLAGS_CONSUME)) == 0u)
	{
		return -ENOTSUP;
	}

	if (mask == 0u) {
		ret = 0;
		goto exit;
	}

	const uint8_t lock = irq_lock();

	uint8_t value	   = flags->flags;
	const uint8_t trig = value & mask;

	if (trig == 0u) {
		/* Set into swap_data flags the thread in pending on and
		 * K_FLAGS_CONSUME option */
		z_current->swap_data = Z_SWAP_DATA_INIT_OPT_N_MASK(options, mask);

		ret = z_pend_current(&flags->_waitqueue, timeout);
		if (ret == 0) {
			/* Bits that made the thread ready are stored in
			 * swap_data */
			ret = Z_SWAP_DATA_GET_TRIG_MASK(z_current->swap_data);
		}
	} else {
		if ((options & K_FLAGS_CONSUME) != 0u) {
			value &= ~mask;
			flags->flags = value;
		}
		ret = trig;
	}

	irq_unlock(lock);

exit:
	return ret;
}

int k_flags_notify(struct k_flags *flags, uint8_t notify_value, k_flags_options options)
{
	int ret = 0;
	struct dnode *thread_handle;

	Z_ARGS_CHECK(flags) return -EINVAL;
	Z_ARGS_CHECK((options & ~(K_FLAGS_SET | K_FLAGS_SCHED)) == 0u) return -ENOTSUP;

	const uint8_t lock = irq_lock();

	thread_handle = flags->_waitqueue.head;
	notify_value  = ~flags->flags & notify_value;

	while (notify_value != 0u) {
		if (!DITEM_VALID(&flags->_waitqueue, thread_handle)) {
			/* No more threads, save remaining value to notify
			 * into k_flags structure */
			flags->flags |= notify_value;
			break;
		}

		struct k_thread *const thread =
			CONTAINER_OF(thread_handle, struct k_thread, wflags);
		const uint16_t swap_data = (uint16_t)thread->swap_data;
		const uint8_t mask	 = Z_SWAP_DATA_GET_PEND_MASK(swap_data);
		const uint8_t trig	 = notify_value & mask;

		if (trig != 0u) {
			thread->swap_data = Z_SWAP_DATA_INIT_TRIG_MASK(trig);

			/* Unpend thread */
			dlist_remove(thread_handle);
			z_wake_up(thread);

			if (Z_SWAP_DATA_GET_OPTIONS(swap_data) & K_FLAGS_CONSUME) {
				notify_value &= ~trig;
			}

			/* Increment number of thread notified */
			ret++;
		}

		thread_handle = thread_handle->next;
	}

	if ((ret > 0) && (options & K_FLAGS_SCHED)) {
		z_yield();
	}

	irq_unlock(lock);

	return ret;
}

int k_flags_reset(struct k_flags *flags)
{
	Z_ARGS_CHECK(flags) return -EINVAL;

	const uint8_t lock = irq_lock();

	int ret	     = z_cancel_all_pending(&flags->_waitqueue);
	flags->flags = flags->reset_value;

	irq_unlock(lock);

	return ret;
}