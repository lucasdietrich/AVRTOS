/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "avrtos/defines.h"
#include "avrtos/semaphore.h"
#include "avrtos/poll.h"

K_SEM_DEFINE(sem1, 0u, 1u);
K_SEM_DEFINE(sem2, 0u, 1u);

int main(void)
{
    serial_init();

    k_thread_dump_all();

    struct k_pollfd fds[] = {
        {
            .obj.sem = &sem1,
        },
        {
            .obj.sem = &sem2,
        },
    };
    

    while (1) {
        k_poll(fds, 2u, K_MSEC(1000));
    }
}