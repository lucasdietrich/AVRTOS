/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/alloc/alloc.h>
#include <avrtos/alloc/slab.h>
#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

#include <avr/io.h>

struct slab_allocator slabspool;

#define BLOCK_SIZE   16u
#define BLOCKS_COUNT 16u

uint8_t buf[(BLOCK_SIZE) * (BLOCKS_COUNT)];

static void print_stats(void)
{
    struct alloc_stats stats;
    slab_stats(&slabspool, &stats);
    printf("total: %u used: %u free: %u\n", stats.total, stats.used, stats.free);
}

int main(void)
{
    void *ptr;

    int8_t ret = slab_init(&slabspool, buf, BLOCK_SIZE, BLOCKS_COUNT);
    printf("slab_init (buf: %p): %d\n", buf, ret);

    uint8_t ptrs_count       = 0;
    void *ptrs[BLOCKS_COUNT] = {0};

    do {
        print_stats();

        ptr = slab_alloc(&slabspool);
        printf("slab_alloc(): %p\n", ptr);

        if (ptr) {
            // Store the pointer for later deallocation
            ptrs[ptrs_count++] = ptr;
        }
    } while (ptr);

    print_stats();

    // Free all allocated blocks
    for (uint8_t i = 0; i < ptrs_count; i++) {
        slab_free(&slabspool, ptrs[i]);
        printf("slab_free(%p)\n", ptrs[i]);

        ptrs[i] = NULL;

        print_stats();
    }
}