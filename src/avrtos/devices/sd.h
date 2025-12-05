/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_SD_H
#define _AVRTOS_DEVICE_SD_H

#include <stddef.h>
#include <stdint.h>

uint8_t crc7(uint8_t *data, size_t len);

typedef struct sd_cmd {
    uint8_t buf[6u];
} sd_cmd_t;

void sd_cmd_prep(sd_cmd_t *cmd, uint8_t index, uint32_t arg);

#endif /* _AVRTOS_DEVICE_SD_H */