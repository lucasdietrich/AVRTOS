/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/misc/serial.h>
#include <avrtos/drivers/usart.h>

struct k_work_delayable work;
K_SEM_DEFINE(sem, 0, 1);

void work_handler(struct k_work *work)
{
	// struct k_work_delayable *dwork =
	// 	CONTAINER_OF(work, struct k_work_delayable, work);

	serial_transmit('h');
	k_sem_give(&sem);
}

ISR(USART0_RX_vect)
{
	int ret;
	uint8_t c = USART0_DEVICE->UDRn;
	if (c == 'c') {
		serial_transmit('c');
		ret = k_work_delayable_cancel(&work);
	} else {
		serial_transmit('!');
		ret = k_system_work_delayable_schedule(&work, K_MSEC(1000));
	}

	if (ret == 0) {
		serial_transmit('0');
	} else {
		serial_transmit('e');
	}
}

int main(void)
{
	serial_init();
	ll_usart_enable_rx_isr(USART0_DEVICE);

	k_work_delayable_init(&work, work_handler);

	for (;;) {
		k_sem_take(&sem, K_FOREVER);
		printf("w\n");
	}
}