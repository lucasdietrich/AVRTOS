/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CRC_H_
#define _CRC_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Compute CRC8 of a buffer.
 *
 * @param buf Buffer to compute CRC8.
 * @param len Length of the buffer.
 * @return uint8_t Computed CRC8 of the buffer.
 */
uint8_t crc8(const uint8_t *buf, size_t len);

#endif /* _CRC_H_ */