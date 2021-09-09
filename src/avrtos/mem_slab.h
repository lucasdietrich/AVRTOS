#ifndef _AVRTOS_MEM_SLAB_H
#define _AVRTOS_MEM_SLAB_H

#include <avrtos/multithreading.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Implement a pool of memory blocks in RAM.
 * 
 * This implementation is greatly inspired from zephyr rtos : 
 *  https://github.com/zephyrproject-rtos/zephyr/blob/main/kernel/mem_slab.c
 * 
 * Allocation is constant time. We're using a singly-linked list to list all
 * free memory blocks. The list items are stored at the first bytes of free
 * memory blocks :
 * - when a block is allocated, the item is removed from the free_list
 * - when a block is freed, the item is queue to the free_list
 * 
 * Maximum number of blocks is 255
 * Maximum block size is 65535 (but technically impossible on AVR architectures)
 */

/*___________________________________________________________________________*/

/* 11B */
struct k_mem_slab
{
    void *buffer;
    uint8_t count;
    uint16_t block_size;
    struct qitem *free_list;
    struct ditem waitqueue;
};

/*___________________________________________________________________________*/

void _k_mem_slab_init_module(void);

K_NOINLINE int8_t k_mem_slab_init(struct k_mem_slab* slab, void* buffer,
    size_t block_size, uint8_t num_blocks);

K_NOINLINE int8_t _k_mem_slab_alloc(struct k_mem_slab* slab, void** mem);

K_NOINLINE int8_t _k_mem_slab_free(struct k_mem_slab* slab, void** mem);

K_NOINLINE int8_t k_mem_slab_alloc(struct k_mem_slab* slab, void** mem,
    k_timeout_t timeout);

K_NOINLINE void k_mem_slab_free(struct k_mem_slab* slab, void** mem);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif