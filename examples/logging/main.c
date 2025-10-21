/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_INF

int main(void)
{
    for (;;) {
        LOG_DBG("<dbg> Hello world!");
        LOG_INF("<inf> Hello world!");
        LOG_WRN("<wrn> Hello world!");
        LOG_ERR("<err> Hello world!");
        k_sleep(K_SECONDS(1));
    }
}
