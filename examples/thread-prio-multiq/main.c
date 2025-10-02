/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

int main(void)
{
    const struct usart_config cfg = USART_CONFIG_DEFAULT_115200();
    usart_init(USART0_DEVICE, &cfg);

    uint32_t u = 0u;
    for (;;) {
        printf_P(PSTR("Hello ! %u\n"), u++);

        k_sleep(K_SECONDS(1));
    }
}