/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/serial.h>

#include <avrtos/drivers/i2c.h>

#define TCN75A_ADDR 0x49
#define UNKNOW_ADDR 0x77
#define ADDR TCN75A_ADDR

int main(void)
{
	serial_init();
	k_thread_dump_all();

	struct i2c_config config = {
		.prescaler = I2C_PRESCALER_1,
	};

	int8_t ret = i2c_init(I2C0_DEVICE, config);
	k_msleep(100);
	printf("i2c_init: %d\n", ret);

	uint8_t buf[2] = { 0 };
	ret = i2c_master_receive(I2C0_DEVICE, ADDR, buf, 2u);
	k_wait(K_MSEC(2u), K_WAIT_MODE_BLOCK);
	printf("i2c_master_receive: %d (%d)\n", ret, i2c_last_error(I2C0_DEVICE));
	printf("buf: %02x %02x\n", buf[0], buf[1]);

	for (;;) {
		k_sleep(K_MSEC(5000));
		k_dump_stack_canaries();
	}
}