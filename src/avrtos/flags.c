/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "flags.h"

#include "kernel.h"
#include "kernel_private.h"

#define KERNEL_FLAGS_OPT_SET_ALL_ENABLED 0
#define KERNEL_FLAGS_OPT_SET_ANY_ENABLED 1
#define KERNEL_FLAGS_OPT_CLR_ALL_ENABLED 0
#define KERNEL_FLAGS_OPT_CLR_ANY_ENABLED 1

/**
 * @brief Macros to handle swap_data, used for storing both options and the mask.
 * The swap_data is split into an options byte (upper 8 bits) and a mask byte (lower 8
 * bits).
 */
#define Z_SWAP_DATA_GET_OPTIONS(swapd)   ((uint8_t)(((uint16_t)(swapd)) >> 8u))
#define Z_SWAP_DATA_GET_PEND_MASK(swapd) ((uint8_t)(((uint16_t)(swapd)) & 0xFFu))
#define Z_SWAP_DATA_GET_TRIG_MASK(swapd) ((uint8_t)(((uint16_t)(swapd)) & 0xFFu))

/**
 * @brief Initialize swap_data with the trigger mask only.
 */
#define Z_SWAP_DATA_INIT_TRIG_MASK(trig) ((void *)(((uint16_t)(trig)) & 0xFFu))

/**
 * @brief Initialize swap_data with both options and mask.
 */
#define Z_SWAP_DATA_INIT_OPT_N_MASK(opt, mask)                                           \
    ((void *)((((uint16_t)(opt)) << 8u) | ((uint16_t)(mask))))

int8_t k_flags_init(struct k_flags *flags, k_flags_value_t value)
{
    Z_ARGS_CHECK(flags) return -EINVAL;

    dlist_init(&flags->waitqueue);
    flags->flags       = value;
    flags->reset_value = value;

    return 0;
}

int k_flags_poll(struct k_flags *flags,
                 k_flags_value_t *mask,
                 k_flags_options_t options,
                 k_timeout_t timeout)
{
    int ret = 0;

    Z_ARGS_CHECK(flags && mask) return -EINVAL;
    Z_ARGS_CHECK((options & ~(K_FLAGS_SET_ANY | K_FLAGS_CONSUME)) == 0u) return -ENOTSUP;

    k_flags_value_t mask_val = *mask;
    if (mask_val == 0u) {
        goto exit;
    }

    const uint8_t lock = irq_lock();

    k_flags_value_t value = flags->flags;
    k_flags_value_t trig  = value & mask_val;

    if (trig == 0u) {
        /* No matching flags set, prepare to wait
         *
         * This is the limitation for having CONFIG_KERNEL_FLAGS_SIZE greater than
         * 1 byte. The swap_data is used to store the mask (1 byte) and the options
         * (1 byte).
         */
        z_ker.current->swap_data = Z_SWAP_DATA_INIT_OPT_N_MASK(options, mask_val);

        ret = z_pend_current_on(&flags->waitqueue, timeout);
        if (ret == 0) {
            /* Retrieve the flags that caused the wake-up */
            *mask = Z_SWAP_DATA_GET_TRIG_MASK(z_ker.current->swap_data);
        }
    } else {
        if ((options & K_FLAGS_CONSUME) != 0u) {
            value &= ~mask_val; /* Clear the flags if K_FLAGS_CONSUME is set */
            flags->flags = value;
        }
        *mask = trig;
        ret   = 0; /* Successfully matched flags */
    }

    irq_unlock(lock);

exit:
    return ret;
}

int8_t k_flags_notify(struct k_flags *flags,
                      k_flags_value_t notify_value,
                      k_flags_options_t options)
{
    int8_t ret = 0;
    struct dnode *thread_handle;

    Z_ARGS_CHECK(flags) return -EINVAL;
    Z_ARGS_CHECK((options & ~(K_FLAGS_SET | K_FLAGS_SCHED)) == 0u) return -ENOTSUP;

    const uint8_t lock = irq_lock();

    thread_handle = flags->waitqueue.head;
    notify_value  = ~flags->flags & notify_value;

    while (notify_value != 0u) {
        if (!DITEM_VALID(&flags->waitqueue, thread_handle)) {
            /* No more threads waiting; store remaining flags */
            flags->flags |= notify_value;
            break;
        }

        struct k_thread *const thread =
            CONTAINER_OF(thread_handle, struct k_thread, wflags);
        const uint16_t swap_data   = (uint16_t)thread->swap_data;
        const k_flags_value_t mask = Z_SWAP_DATA_GET_PEND_MASK(swap_data);
        const k_flags_value_t trig = notify_value & mask;

        if (trig != 0u) {
            thread->swap_data = Z_SWAP_DATA_INIT_TRIG_MASK(trig);

            /* Unpend thread */
            dlist_remove(thread_handle);
            z_early_wake_up(thread);

            if (Z_SWAP_DATA_GET_OPTIONS(swap_data) & K_FLAGS_CONSUME) {
                notify_value &= ~trig;
            }

            ret++; /* Increment the number of notified threads */
        }

        thread_handle = thread_handle->next;
    }

    if ((ret > 0) && (options & K_FLAGS_SCHED)) {
        z_yield(); /* Yield if any thread was notified and scheduling is requested */
    }

    irq_unlock(lock);

    return ret;
}

int8_t k_flags_reset(struct k_flags *flags)
{
    Z_ARGS_CHECK(flags) return -EINVAL;

    const uint8_t lock = irq_lock();

    int8_t ret   = (int8_t)z_cancel_all_pending(&flags->waitqueue);
    flags->flags = flags->reset_value;

    irq_unlock(lock);

    return ret;
}
