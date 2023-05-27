/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/drivers/usart.h>
#include <avrtos/avrtos.h>

int main(void)
{
	const struct usart_config cfg = USART_CONFIG_DEFAULT_500000();
	usart_init(USART0_DEVICE, &cfg);

	uint32_t u = 0u;
	for (;;) {
		printf_P(PSTR("Hello ! %u\n"), u++);

		k_sleep(K_SECONDS(1));
	}
}