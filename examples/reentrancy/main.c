/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>

#include <util/delay.h>

static void thread1_entry(void *context);

K_MUTEX_DEFINE(mutex);
Z_THREAD_DEFINE(thread1, thread1_entry, 512, K_COOPERATIVE, NULL, '1', 0);

int main(void)
{
    serial_init();

    k_mutex_lock(&mutex, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);
    k_mutex_lock(&mutex, K_FOREVER);

    k_thread_start(&thread1);
    k_yield();

    k_mutex_unlock(&mutex);
    k_mutex_unlock(&mutex);
    k_mutex_unlock(&mutex);

    for (;;) {
        k_msleep(1000);
    }
}

static void thread1_entry(void *context)
{
    k_mutex_lock(&mutex, K_FOREVER);

    serial_print("thread1: mutex locked\n");
}