/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tcn75.h"

int8_t tcn75_init_context(struct tcn75_context *tcn75,
						  uint8_t addr_lsb,
						  uint8_t config,
						  I2C_Device *i2c)
{
	Z_ARGS_CHECK(tcn75) return -EINVAL;

	tcn75->addr	  = (addr_lsb & 0x07u) | TCN75_ADDR_BASE;
	tcn75->config = config;
	tcn75->i2c	  = i2c;

	return 0;
}

int8_t tcn75_configure(struct tcn75_context *tcn75)
{
	Z_ARGS_CHECK(tcn75) return -EINVAL;

	const uint8_t buf[2u] = {TCN75_CONFIG_REGISTER, tcn75->config};

	return i2c_master_transmit(tcn75->i2c, tcn75->addr, buf, 2u);
}

int8_t tcn75_select_data_register(struct tcn75_context *tcn75)
{
	Z_ARGS_CHECK(tcn75) return -EINVAL;

	const uint8_t buf[1u] = {TCN75_TEMPERATURE_REGISTER};

	return i2c_master_transmit(tcn75->i2c, tcn75->addr, buf, 1u);
}

static int16_t tcn75_temp2int16(uint8_t msb, uint8_t lsb)
{
	int16_t i16_temp;

	const uint8_t neg = msb >> 7u;

	/* Resolution of abs is 2^-4 °C */
	uint16_t abs = (msb << 4u) | (lsb >> 4u);
	if (neg) { /* 2s complement if negative value */
		abs = ~abs + 1u;
	}
	/* cast to 12 bits value */
	abs &= 0x7ffu;

	/* i16_temp resolution is 0.01°C */
	i16_temp = (100.0 / 16) * abs;

	if (neg) {
		i16_temp = -i16_temp;
	}

	return i16_temp;
}

int16_t tcn75_read(struct tcn75_context *tcn75)
{
	Z_ARGS_CHECK(tcn75) return -EINVAL;

	int16_t temperature = INT16_MAX;

	uint8_t buf[2u] = {0};
	int8_t ret		= i2c_master_receive(tcn75->i2c, tcn75->addr, buf, 2u);
	if (ret == 0) {
		temperature = tcn75_temp2int16(buf[0], buf[1]);
	}

	return temperature;
}
