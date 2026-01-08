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

/**
 * @brief Calculate CRC7 checksum using polynomial x^7 + x^3 + 1 (0b10001001)
 *
 * @param data Data buffer
 * @param len Length of data
 * @return CRC7 checksum
 */
uint8_t crc7(uint8_t *data, size_t len);

#endif /* _CRC_H_ */