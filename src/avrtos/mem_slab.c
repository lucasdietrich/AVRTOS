#include "mem_slab.h"

#include <avrtos/kernel.h>
#include <avrtos/dstruct/queue.h>

#define K_MODULE K_MODULE_MEMSLAB

static void create_free_list(struct k_mem_slab* slab)
{
    slab->free_list = NULL;
    uint8_t* p = slab->buffer;

    for (uint8_t i = 0u; i < slab->count; i++) {
        /* equivalent to "*(void**) p = slab->free_list;" */
        ((struct qitem*)p)->next = slab->free_list;

        slab->free_list = (struct qitem*)p;
        p += slab->block_size;
    }
}

void _k_mem_slab_init_module(void)
{
    /* __attribute__((used, section(".k_mem_slabs"))) */
}

int8_t k_mem_slab_init(struct k_mem_slab* slab, void* buffer,
    size_t block_size, uint8_t num_blocks)
{
    if (buffer == NULL) {
        return -EINVAL;
    }

    /* we need a list 2 bytes to store the next
     * free block address if not allocated
     */
    if ((block_size < 2) || (num_blocks == 0)) {
        return -EINVAL;
    }

    slab->block_size = block_size;
    slab->count = num_blocks;
    slab->buffer = buffer;
    create_free_list(slab);

    dlist_init(&slab->waitqueue);

    return 0;
}

int8_t _k_mem_slab_alloc(struct k_mem_slab* slab, void** mem)
{
    __ASSERT_NOTNULL(slab);
    __ASSERT_NOTNULL(mem);
    __ASSERT_NOTNULL(slab->free_list);

    *mem = (uint8_t*)slab->free_list;
    slab->free_list = slab->free_list->next;

    return 0;
}

int8_t k_mem_slab_alloc(struct k_mem_slab* slab, void** mem, k_timeout_t timeout)
{
    __ASSERT_NOTNULL(slab);
    __ASSERT_NOTNULL(mem);

    int8_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (slab->free_list != NULL) {
            ret = _k_mem_slab_alloc(slab, mem);
        } else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
            ret = -ENOMEM;
        } else {
            ret = _k_pend_current(&slab->waitqueue, timeout);
            if (ret == 0) {
                ret = _k_mem_slab_alloc(slab, mem);
            }
        }
    }
    return ret;
}

int8_t _k_mem_slab_free(struct k_mem_slab* slab, void** mem)
{
    __ASSERT_NOTNULL(slab);
    __ASSERT_NOTNULL(mem);

    /* eq to "**(struct qitem***)mem = slab->free_list;" */

    (*(struct qitem**)mem)->next = slab->free_list;
    slab->free_list = *(struct qitem**)mem;

    return 0;
}

void k_mem_slab_free(struct k_mem_slab* slab, void** mem)
{
    __ASSERT_NOTNULL(slab);
    __ASSERT_NOTNULL(mem);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (slab->free_list == NULL) {
            _k_mem_slab_free(slab, mem);

            _k_unpend_first_thread(&slab->waitqueue);
        }
    }
}