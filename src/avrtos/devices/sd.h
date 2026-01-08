/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_SD_H
#define _AVRTOS_DEVICE_SD_H

#include <stddef.h>
#include <stdint.h>

#include <avrtos/avrtos_conf.h>
#include <avrtos/drivers/spi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SD_BLOCK_SIZE CONFIG_SD_BLOCK_SIZE

/**
 * @brief SD card type
 */
enum sd_card_type {
    SD_CARD_TYPE_UNKNOWN = 0,
    SD_CARD_TYPE_SDSC_V1 = 1, /* Standard Capacity v1.x */
    SD_CARD_TYPE_SDSC_V2 = 2, /* Standard Capacity v2.x */
    SD_CARD_TYPE_SDHC    = 3, /* High Capacity */
};

/**
 * @brief SD card CSD register (Card Specific Data)
 */
struct sd_csd {
    uint8_t raw[16];           /* Raw CSD register data */
    uint32_t capacity_bytes;   /* Card capacity in bytes */
    uint32_t capacity_blocks;  /* Card capacity in blocks */
    uint8_t csd_structure;     /* CSD structure version */
    uint16_t max_read_bl_len;  /* Maximum read block length */
    uint16_t max_write_bl_len; /* Maximum write block length */
};

/**
 * @brief SD card CID register (Card Identification)
 */
struct sd_cid {
    uint8_t raw[16];           /* Raw CID register data */
    uint8_t manufacturer_id;   /* Manufacturer ID */
    char oem_id[3];            /* OEM/Application ID (2 chars + null) */
    char product_name[6];      /* Product name (5 chars + null) */
    uint8_t product_revision;  /* Product revision */
    uint32_t serial_number;    /* Product serial number */
    uint16_t manufacture_date; /* Manufacturing date (year/month) */
};

/**
 * @brief SD card information
 */
struct sd_card_info {
    enum sd_card_type type;
    uint8_t version;          /* Version (1 or 2) */
    uint32_t ocr;             /* Operating Conditions Register */
    uint8_t voltage_accepted; /* Voltage range accepted */
    struct sd_csd csd;        /* Card Specific Data */
    struct sd_cid cid;        /* Card Identification */
#if CONFIG_SD_CSD
    uint8_t csd_valid;        /* CSD data is valid */
    uint8_t cid_valid;        /* CID data is valid */
#endif
};

/**
 * @brief SD card device structure
 */
struct sd_device {
    const struct spi_slave *slave;
    struct sd_card_info info;
    uint8_t initialized;
};

typedef struct sd_cmd {
    uint8_t buf[6];
} sd_cmd_t;

/**
 * @brief Calculate CRC7 checksum
 *
 * @param data Data buffer
 * @param len Length of data
 * @return CRC7 checksum
 */
uint8_t crc7(uint8_t *data, size_t len);

/**
 * @brief Prepare SD command
 *
 * @param cmd Command structure to fill
 * @param index Command index
 * @param arg Command argument
 */
void sd_cmd_prep(sd_cmd_t *cmd, uint8_t index, uint32_t arg);

/**
 * @brief Initialize SD card
 *
 * @param dev SD device structure
 * @param slave SPI slave configuration
 * @return 0 on success, negative error code on failure
 */
int sd_init(struct sd_device *dev, const struct spi_slave *slave);

/**
 * @brief Read a single block from SD card
 *
 * @param dev SD device structure
 * @param block_addr Block address
 * @param buf Buffer to store read data (must be at least SD_BLOCK_SIZE bytes)
 * @return 0 on success, negative error code on failure
 */
int sd_read_block(struct sd_device *dev, uint32_t block_addr, uint8_t *buf);

/**
 * @brief Write a single block to SD card
 *
 * @param dev SD device structure
 * @param block_addr Block address
 * @param buf Buffer containing data to write (must be at least SD_BLOCK_SIZE bytes)
 * @return 0 on success, negative error code on failure
 */
int sd_write_block(struct sd_device *dev, uint32_t block_addr, const uint8_t *buf);

/**
 * @brief Get SD card information
 *
 * @param dev SD device structure
 * @param info Pointer to structure to store card information
 * @return 0 on success, negative error code on failure
 */
int sd_get_info(struct sd_device *dev, struct sd_card_info *info);

/**
 * @brief Read CSD register (Card Specific Data)
 *
 * @param dev SD device structure
 * @return 0 on success, negative error code on failure
 */
int sd_read_csd(struct sd_device *dev);

/**
 * @brief Read CID register (Card Identification)
 *
 * @param dev SD device structure
 * @return 0 on success, negative error code on failure
 */
int sd_read_cid(struct sd_device *dev);

/**
 * @brief Get card capacity in bytes
 *
 * @param dev SD device structure
 * @return Capacity in bytes, 0 if CSD not read
 */
uint32_t sd_get_capacity_bytes(struct sd_device *dev);

/**
 * @brief Get card capacity in blocks
 *
 * @param dev SD device structure
 * @return Capacity in blocks, 0 if CSD not read
 */
uint32_t sd_get_capacity_blocks(struct sd_device *dev);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_DEVICE_SD_H */