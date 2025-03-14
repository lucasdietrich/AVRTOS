/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mem_slab.h"

#include "alloc/slab_private.h"
#include "dstruct/dlist.h"
#include "dstruct/slist.h"
#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_MEMSLAB

#if CONFIG_AVRTOS_LINKER_SCRIPT
extern struct k_mem_slab __k_mem_slabs_start;
extern struct k_mem_slab __k_mem_slabs_end;

/**
 * This function is called during avrtos initialization to initialize all known memory
 * slabs that are defined using the linker section.
 */
void z_mem_slab_init_module(void)
{
    /* Calculate the number of memory slabs defined in the linker script */
    const uint8_t mem_slabs_count = &__k_mem_slabs_end - &__k_mem_slabs_start;
    for (uint8_t i = 0; i < mem_slabs_count; i++) {
        /* Initialize each slab's free list */
        z_slab_alloc_finalize_init(&(&(&__k_mem_slabs_start)[i])->allocator);
    }
}
#endif

int8_t k_mem_slab_init(struct k_mem_slab *slab,
                       void *buffer,
                       size_t block_size,
                       uint8_t num_blocks)
{
    int8_t ret = slab_init(&slab->allocator, buffer, block_size, num_blocks);

    if (ret == 0) {
        /* Initialize the wait queue for threads waiting on this slab */
        dlist_init(&slab->waitqueue);
    }

    return ret;
}

int8_t k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout)
{
    Z_ARGS_CHECK(slab && mem) return -EINVAL;

    int8_t ret;

    const uint8_t key = irq_lock();

    /* Try to allocate a block directly */
    void *ptr = slab_alloc(&slab->allocator);
    if (ptr != NULL) {
        *mem = ptr;
        ret  = 0;
    } else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
        /* No blocks available and the thread doesn't want to wait */
        ret = -ENOMEM;
    } else {
        /* Wait for a block to become available */
        ret = z_pend_current_on(&slab->waitqueue, timeout);
        if (ret == 0) {
            /* Retrieve the memory block available */
            *mem = z_ker.current->swap_data;
        }
    }

    irq_unlock(key);

    return ret;
}

struct k_thread *k_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
    Z_ARGS_CHECK(slab && mem) return NULL;

    struct k_thread *thread = NULL;

    if (mem == NULL) {
        /* If the memory block is NULL, there's nothing to free */
        goto ret;
    }

    const uint8_t key = irq_lock();

    /* If a thread is pending on the memory slab, give the block directly to the thread */
    thread = z_unpend_first_and_swap(&slab->waitqueue, mem);

    if (thread == NULL) {
        /* Otherwise, free the block */
        slab_free(&slab->allocator, mem);
    }

    irq_unlock(key);

ret:
    return thread;
}
