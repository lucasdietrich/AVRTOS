/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/serial.h>

#include <avrtos/drivers/i2c.h>
#include <avrtos/devices/tcn75.h>

#define TCN75A_ADDR_LSb 0b001

int main(void)
{
	serial_init();

	struct i2c_config config = {
		.prescaler = I2C_PRESCALER_1,
	};

	int8_t ret = i2c_init(I2C0_DEVICE, config);
	printf("i2c_init: %d\n", ret);
	k_msleep(100);

	struct tcn75_context tcn75;
	ret = tcn75_init_context(&tcn75, TCN75A_ADDR_LSb, TCN75_DEFAULT_CONFIG, I2C0_DEVICE);
	printf("tcn75_init_context: %d\n", ret);

	ret = tcn75_configure(&tcn75);
	printf("tcn75_configure: %d\n", ret);

	ret = tcn75_select_data_register(&tcn75);
	printf("tcn75_select_data_register: %d\n", ret);

	for (;;) {
		int16_t temp = tcn75_read(&tcn75);
		if (temp != INT16_MAX) {
			printf("tcn75_read: %d\n", temp);
		} else {
			printf("tcn75_read: error\n");
		}

		k_sleep(K_MSEC(1000));
	}

	ret = i2c_deinit(I2C0_DEVICE);
	printf("i2c_deinit: %d\n", ret);
}