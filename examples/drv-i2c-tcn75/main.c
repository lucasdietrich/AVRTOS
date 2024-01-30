/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/devices/tcn75.h>
#include <avrtos/drivers/i2c.h>
#include <avrtos/misc/serial.h>

#define TCN75A_ADDR_LSb 0b001
#define I2C_DEVICE		I2C0_DEVICE

int main(void)
{
	serial_init();

	struct i2c_config config = {
		.prescaler = I2C_PRESCALER_1,
		.twbr	   = I2C_CALC_TWBR(I2C_PRESCALER_1, 400000),
	}; // or use I2C_CONF_400000

	int8_t ret = i2c_init(I2C_DEVICE, config);
	printf("i2c_init: %d last_error: %d\n", ret, i2c_last_error(I2C_DEVICE));
	k_msleep(100);

	struct tcn75_context tcn75;
	ret = tcn75_init_context(&tcn75, TCN75A_ADDR_LSb, TCN75_DEFAULT_CONFIG, I2C_DEVICE);
	printf("tcn75_init_context: %d last_error: %d\n", ret, i2c_last_error(I2C_DEVICE));

	ret = tcn75_configure(&tcn75);
	printf("tcn75_configure: %d last_error: %d\n", ret, i2c_last_error(I2C_DEVICE));

	ret = tcn75_select_data_register(&tcn75);
	printf("tcn75_select_data_register: %d last_error: %d\n", ret,
		   i2c_last_error(I2C_DEVICE));

	for (;;) {
		int16_t temp = tcn75_select_read(&tcn75);
		if (temp != INT16_MAX) {
			printf("tcn75_read: %d\n", temp);
		} else {
			printf("tcn75_read: error %d\n", i2c_last_error(I2C_DEVICE));
		}

		k_sleep(K_MSEC(1000));
	}

	ret = i2c_deinit(I2C_DEVICE);
	printf("i2c_deinit: %d\n", ret);
}