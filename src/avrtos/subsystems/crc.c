/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crc.h"

uint8_t crc8(const uint8_t *buf, size_t len)
{
    uint8_t crc = 0xff;

    while (len--) {
        crc ^= *buf++;
        for (uint8_t j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    return crc;
}