/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_SD_PRIV_H
#define _AVRTOS_DEVICE_SD_PRIV_H

#define SD_CMD0 0u
#define SD_CMD8 8u
#define SD_CMD16 16u
#define SD_CMD17 17u
#define SD_CMD24 24u
#define SD_CMD55 55u
#define SD_CMD58 58u

#define SD_ACMD41 41u

#define SD_DATA_TOKEN 0xFEu

#define R1_IDLE_STATE      0x01
#define R1_ERASE_ERASE     0x02
#define R1_ILLEGAL_COMMAND 0x04
#define R1_COM_CRC_ERROR   0x08
#define R1_ERASE_SEQ_ERROR 0x10
#define R1_ADDRESS_ERROR   0x20
#define R1_PARAMETER_ERROR 0x40

#define OCR_VDD_27_28  (1u << 15u)
#define OCR_VDD_28_29  (1u << 16u)
#define OCR_VDD_29_30  (1u << 17u)
#define OCR_VDD_30_31  (1u << 18u)
#define OCR_VDD_31_32  (1u << 19u)
#define OCR_VDD_32_33  (1u << 20u)
#define OCR_VDD_33_34  (1u << 21u)
#define OCR_VDD_34_35  (1u << 22u)
#define OCR_VDD_35_36  (1u << 23u)

#endif /* _AVRTOS_DEVICE_SD_PRIV_H */