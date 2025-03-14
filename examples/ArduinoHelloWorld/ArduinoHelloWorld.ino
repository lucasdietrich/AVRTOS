/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos.h>
#include <avrtos/avrtos.h>

static uint16_t counter = 0;

void setup(void)
{
    printf("Hello, World!\n");
}

void loop(void)
{
    printf("Counter: %u\n", counter++);
    k_sleep(K_MSEC(1000));
}