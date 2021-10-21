#ifndef _AVRTOS_MEM_SLAB_H
#define _AVRTOS_MEM_SLAB_H

#include <avrtos/multithreading.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

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
 * Complexity :
 * - Initialization is O(n)
 * - Allocation/free is O(1)
 * 
 * Maximum number of blocks is 255
 * Maximum block size is 65535 (but technically impossible on AVR architectures)
 */

/*___________________________________________________________________________*/

/**
 * @brief Structure representing a memory slab.
 * 
 * buffer : buffer from which all blocks will be allocated.
 * count : numbers of blocks
 * block_size : size of a block
 * free_list : list of all free blocks
 * waitqueue : list contains all threads pending on a fifo item.
 * 
 * Note : sizeof(buffer) must equals count * block_size
 */
struct k_mem_slab
{
        void *buffer;
        uint8_t count;
        uint16_t block_size;
        struct qitem *free_list;
        struct ditem waitqueue;
};

/*___________________________________________________________________________*/

#define K_MEM_SLAB_INIT(slab, buf, size, num_blocks) \
    {                                                \
        .buffer = buf,                               \
        .count = num_blocks,                         \
        .block_size = size,                          \
        .free_list = NULL,                           \
        .waitqueue = DLIST_INIT(slab.waitqueue)      \
    }

#define _K_MEM_SLAB_BUF_NAME(slab_name) _k_mem_slab_buf_##slab_name

#define K_MEM_SLAB_DEFINE(slab_name, size, num_blocks)                                  \
    uint8_t _k_mem_slab_buf_##slab_name[(size)*(num_blocks)];                           \
    __attribute__((used, section(".k_mem_slabs"))) static struct k_mem_slab slab_name = \
        K_MEM_SLAB_INIT(slab_name, _k_mem_slab_buf_##slab_name, size, num_blocks)

/*___________________________________________________________________________*/

/**
 * @brief This function is called on start up to initialize all memory
 * slabs defined at runtime using the macro K_MEM_SLAB_DEFINE
 */
void _k_mem_slab_init_module(void);

/**
 * @brief Initialize a memory slab at runtime
 * 
 * @param slab address of the slab structure
 * @param buffer address of the buffer
 * @param block_size 
 * @param num_blocks 
 * @return K_NOINLINE return 0 on success else error code
 */
K_NOINLINE int8_t k_mem_slab_init(struct k_mem_slab *slab, void *buffer,
        size_t block_size, uint8_t num_blocks);

/**
 * @brief Internal function to allocate a block
 * 
 * Assumptions :
 * - slab not null
 * - mem not null
 * - interrupts are disabled
 * 
 * @param slab 
 * @param mem 
 * @return K_NOINLINE 
 */
K_NOINLINE int8_t _k_mem_slab_alloc(struct k_mem_slab *slab, void **mem);

/**
 * @brief Internal function to free a block
 * 
 * Assumptions :
 * - slab not null
 * - mem not null
 * - interrupts are disabled
 * - mem is currently "allocated"
 * 
 * @param slab 
 * @param mem 
 * @return K_NOINLINE 
 */
K_NOINLINE int8_t _k_mem_slab_free(struct k_mem_slab *slab, void *mem);

/**
 * @brief Allocate a memory block. 
 * 
 * If there is no block available timeout is different from K_NO_WAIT,
 * the function is blocking until a block is freed or timeout.
 * 
 * Can be called from an interrupt routine.
 * 
 * Assumptions :
 * - slab not null
 * - mem not null
 * 
 * @param slab 
 * @param mem 
 * @param timeout 
 * @return K_NOINLINE 
 */
K_NOINLINE int8_t k_mem_slab_alloc(struct k_mem_slab *slab, void **mem,
    k_timeout_t timeout);

/**
 * @brief Free a memory block and notify the first pending thread that
 * a mem slab is available.
 * 
 * Switch thread before returning if a thread is waiting on a block.
 * 
 * Cannot be called from an interrupt routine 
 * if timeout is different from K_NO_WAIT.
 * 
 * Assumptions :
 * - slab not null
 * - mem not null
 * - mem is currently "allocated"
 * 
 * @param slab 
 * @param mem 
 * @return K_NOINLINE 
 */
K_NOINLINE void k_mem_slab_free(struct k_mem_slab *slab, void *mem);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif