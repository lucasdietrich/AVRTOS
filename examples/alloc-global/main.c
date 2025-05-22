/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/alloc/alloc.h>
#include <avrtos/avrtos.h>

#define INIT_ALLOC_SIZE 128u

int main(void)
{
    void *ptr;
    uint16_t total      = 0u;
    uint16_t alloc_size = INIT_ALLOC_SIZE;

    size_t atotal, used, free;

    for (;;) {

        k_global_allocator_stats_get(&atotal, &used, &free);
        printf("[ total: %u used: %u free: %u ]\t", atotal, used, free);

        ptr = k_malloc(alloc_size);
        printf("k_malloc(%u): %p\n", alloc_size, ptr);

        if (ptr) {
            total += alloc_size;
        } else if (alloc_size) {
            alloc_size >>= 1;
        } else {
            break;
        }
    }

    printf("Total allocated: %u\n", total);

    z_global_allocator_reset();
    k_global_allocator_stats_get(&atotal, &used, &free);
    printf("[ total: %u used: %u free: %u ]\tCleared\n", atotal, used, free);

    return 0;
}