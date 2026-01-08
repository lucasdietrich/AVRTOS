/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <avrtos/avrtos.h>

#include <util/delay.h>

static void thread1_entry(void *context);

K_MUTEX_DEFINE(mutex);
K_THREAD_DEFINE_STOPPED(thread1, thread1_entry, 512, K_COOPERATIVE, NULL, '1');

int main(void)
{
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

static void thread1_entry(void *arg)
{
    ARG_UNUSED(arg);

    k_mutex_lock(&mutex, K_FOREVER);

    printf_P(PSTR("thread1: mutex locked"));
}