/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sd.h"

#include <avrtos/avrtos_conf.h>
#include <avrtos/errno.h>
#include <avrtos/kernel.h>
#include <avrtos/logging.h>
#include <avrtos/sys.h>

#include "sd_priv.h"

#define LOG_LEVEL CONFIG_SD_LOG_LEVEL

/* G(x) = x^7 + x^3 + 1 (0b10001001) */
#define CRC7_POLY 0x89

#define SD_R1_RESPONSE_RETRIES 8
#define SD_POWERUP_CLOCKS      10

/**
 * @brief Calculate CRC7 checksum
 */
uint8_t crc7(uint8_t *data, size_t len)
{
    uint8_t crc = 0;

    while (len-- > 0) {
        crc ^= *data++;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ (CRC7_POLY << 1);
            else
                crc <<= 1;
        }
    }

    return crc >> 1;
}

/**
 * @brief Prepare SD command
 */
void sd_cmd_prep(sd_cmd_t *cmd, uint8_t index, uint32_t arg)
{
    /* [byte 0]: Start bit (0) + transmission bit (1) + command index (6)
     * [byte 1-4]: Argument (32 bits)
     * [byte 5]: CRC7 (7 bits) + end bit (1)
     */
    cmd->buf[0] = SD_CMD_START_BIT | (index & SD_CMD_INDEX_MASK);
    sys_write_be32(&cmd->buf[SD_CMD_ARG_OFFSET], arg);
    cmd->buf[SD_CMD_CRC_OFFSET] =
        crc7(&cmd->buf[0], SD_CMD_CRC_OFFSET) << 1 | SD_CMD_END_BIT;
}

/**
 * @brief Read R1 response from SD card
 */
static uint8_t sd_read_r1(void)
{
    uint8_t i, res;

    for (i = 0; i < SD_R1_RESPONSE_RETRIES; i++) {
        res = spi_transceive(0xFF);
        if (res != 0xFF)
            break;
    }

    LOG_DBG("R1: 0x%02X", res);
    return res;
}

/**
 * @brief Send command with inline implementation
 *
 * @param slave SPI slave
 * @param index Command index
 * @param arg Command argument
 * @return R1 response byte
 */
static uint8_t sd_send_cmd_raw(const struct spi_slave *slave, uint8_t index, uint32_t arg)
{
    sd_cmd_t cmd;
    uint8_t res;
    uint8_t i;

    sd_cmd_prep(&cmd, index, arg);

    LOG_DBG("CMD%u arg=0x%08lX", index, arg);

    spi_slave_select(slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();

    return res;
}

/**
 * @brief Send command to SD card
 *
 * @param dev SD device structure
 * @param index Command index
 * @param arg Command argument
 * @return R1 response byte
 */
static uint8_t sd_send_cmd(struct sd_device *dev, uint8_t index, uint32_t arg)
{
    uint8_t res;

    res = sd_send_cmd_raw(dev->slave, index, arg);
    spi_slave_unselect(dev->slave);

    return res;
}

/**
 * @brief Wait for SD card to be ready
 *
 * @return 0 on success, negative error code on timeout
 */
static int sd_wait_ready(void)
{
    uint16_t timeout = CONFIG_SD_WRITE_TIMEOUT_MS;

    while (--timeout) {
        if (spi_transceive(0xFF) == 0xFF)
            return 0;
        k_msleep(1);
    }

    return -ETIMEDOUT;
}

/**
 * @brief Read R3/R7 response (R1 + 4 bytes)
 *
 * @param slave SPI slave
 * @param index Command index
 * @param arg Command argument
 * @param resp Buffer for 4-byte response
 * @return R1 response byte
 */
static uint8_t sd_send_cmd_r3(const struct spi_slave *slave,
                              uint8_t index,
                              uint32_t arg,
                              uint8_t resp[4])
{
    uint8_t res;
    uint8_t i;

    res = sd_send_cmd_raw(slave, index, arg);

    if (res <= 1) {
        for (i = 0; i < 4; i++)
            resp[i] = spi_transceive(0xFF);
        LOG_DBG("R3/R7: %02X %02X %02X %02X", resp[0], resp[1], resp[2], resp[3]);
    }

    spi_slave_unselect(slave);

    return res;
}

int sd_init(struct sd_device *dev, const struct spi_slave *slave)
{
    uint8_t res;
    uint32_t arg;
    uint8_t resp[4];
    uint16_t timeout;

    if (!dev || !slave)
        return -EINVAL;

    dev->slave                 = slave;
    dev->info.type             = SD_CARD_TYPE_UNKNOWN;
    dev->info.version          = 0;
    dev->info.ocr              = 0;
    dev->info.voltage_accepted = 0;
#if CONFIG_SD_CSD
    dev->info.csd_valid = 0;
    dev->info.cid_valid = 0;
#endif
    dev->initialized = 0;

    /* Power up sequence */
    spi_slave_unselect(slave);
    k_msleep(1);
    for (uint8_t i = 0; i < SD_POWERUP_CLOCKS; i++)
        spi_transceive(0xFF);

    /* CMD0: Reset card */
    res = sd_send_cmd(dev, SD_CMD0, 0);
    if (!(res & R1_IDLE_STATE)) {
        LOG_ERR("CMD0 failed: 0x%02X", res);
        return -EIO;
    }

    /* CMD8: Check voltage range */
    arg = (SD_CMD8_VHS_27_36V << SD_CMD8_VHS_SHIFT) | SD_CMD8_CHECK_PATTERN;
    res = sd_send_cmd_r3(slave, SD_CMD8, arg, resp);

    if (res > 1) {
        if (res & R1_ILLEGAL_COMMAND) {
            dev->info.version = 1;
            dev->info.type    = SD_CARD_TYPE_SDSC_V1;
        } else {
            LOG_ERR("CMD8 failed: 0x%02X", res);
            return -EIO;
        }
    } else {
        dev->info.voltage_accepted = resp[2] & SD_CMD8_VOLTAGE_MASK;
    }

    if (dev->info.version != 1) {
        LOG_ERR("Ver2.X SD cards not supported");
        return -ENOTSUP;
    }

    /* CMD58: Read OCR */
    res = sd_send_cmd_r3(slave, SD_CMD58, 0, resp);
    if (res > 1) {
        LOG_ERR("CMD58 failed: 0x%02X", res);
        return -EIO;
    }
    dev->info.ocr = ((uint32_t)resp[0] << 24) | ((uint32_t)resp[1] << 16) |
                    ((uint32_t)resp[2] << 8) | resp[3];

    /* ACMD41: Initialize card */
    timeout = CONFIG_SD_INIT_TIMEOUT_MS / SD_ACMD41_RETRY_DELAY;
    while (timeout--) {
        /* CMD55: App command follows */
        res = sd_send_cmd(dev, SD_CMD55, 0);
        if (res > 1) {
            LOG_ERR("CMD55 failed: 0x%02X", res);
            return -EIO;
        }

        /* ACMD41: Initialize */
        res = sd_send_cmd(dev, SD_ACMD41, 0);
        if (res == 0)
            break;
        if (res > 1) {
            LOG_ERR("ACMD41 failed: 0x%02X", res);
            return -EIO;
        }

        k_msleep(SD_ACMD41_RETRY_DELAY);
    }

    if (timeout == 0) {
        LOG_ERR("SD card init timeout");
        return -ETIMEDOUT;
    }

    /* CMD16: Set block length */
    res = sd_send_cmd(dev, SD_CMD16, SD_BLOCK_SIZE);
    if (res != 0) {
        LOG_ERR("CMD16 failed: 0x%02X", res);
        return -EIO;
    }

    dev->initialized = 1;
    LOG_INF("SD card initialized");

    return 0;
}

int sd_read_block(struct sd_device *dev, uint32_t block_addr, uint8_t *buf)
{
    sd_cmd_t cmd;
    uint8_t res;
    uint16_t i;
    uint8_t crc_hi, crc_lo;

    if (!dev || !dev->initialized || !buf)
        return -EINVAL;

    /* CMD17: Read single block */
    sd_cmd_prep(&cmd, SD_CMD17, block_addr * SD_BLOCK_SIZE);

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();
    if (res != 0) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("CMD17 failed: 0x%02X", res);
        return -EIO;
    }

    /* Wait for data token */
    for (i = 0; i < CONFIG_SD_READ_TIMEOUT; i++) {
        res = spi_transceive(0xFF);
        if (res != 0xFF) {
            if (res != SD_DATA_TOKEN) {
                spi_slave_unselect(dev->slave);
                LOG_ERR("Invalid data token: 0x%02X", res);
                return -EIO;
            }
            break;
        }
    }

    if (i == CONFIG_SD_READ_TIMEOUT) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("Read timeout");
        return -ETIMEDOUT;
    }

    /* Read data block */
    for (i = 0; i < SD_BLOCK_SIZE; i++)
        buf[i] = spi_transceive(0xFF);

    /* Read CRC (not validated) */
    crc_hi = spi_transceive(0xFF);
    crc_lo = spi_transceive(0xFF);
    LOG_DBG("CRC: 0x%02X%02X", crc_hi, crc_lo);
    (void)crc_hi;
    (void)crc_lo;

    spi_slave_unselect(dev->slave);

    return 0;
}

int sd_write_block(struct sd_device *dev, uint32_t block_addr, const uint8_t *buf)
{
    sd_cmd_t cmd;
    uint8_t res;
    uint16_t i;
    int ret;

    if (!dev || !dev->initialized || !buf)
        return -EINVAL;

    /* CMD24: Write single block */
    sd_cmd_prep(&cmd, SD_CMD24, block_addr * SD_BLOCK_SIZE);

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();
    if (res != 0) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("CMD24 failed: 0x%02X", res);
        return -EIO;
    }

    /* Send data token */
    spi_transceive(SD_DATA_TOKEN);

    /* Write data block */
    for (i = 0; i < SD_BLOCK_SIZE; i++)
        spi_transceive(buf[i]);

    /* Dummy CRC */
    spi_transceive(0xFF);
    spi_transceive(0xFF);

    /* Read data response token */
    for (i = 0; i < CONFIG_SD_READ_TIMEOUT; i++) {
        res = spi_transceive(0xFF);
        if (res != 0xFF)
            break;
    }

    if (i == CONFIG_SD_READ_TIMEOUT) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("Write data response timeout");
        return -ETIMEDOUT;
    }

    /* Check data response */
    if ((res & SD_DATA_RESPONSE_MASK) != SD_DATA_ACCEPTED) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("Write rejected: 0x%02X", res);
        return -EIO;
    }

    /* Wait for write completion */
    ret = sd_wait_ready();
    if (ret < 0) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("Write completion timeout");
        return ret;
    }

    spi_slave_unselect(dev->slave);

    return 0;
}

int sd_get_info(struct sd_device *dev, struct sd_card_info *info)
{
    if (!dev || !info)
        return -EINVAL;

    if (!dev->initialized)
        return -ENODEV;

    *info = dev->info;
    return 0;
}

#if CONFIG_SD_CSD

/**
 * @brief Parse CSD v1.0 register
 */
static void sd_parse_csd_v1(struct sd_csd *csd)
{
    uint8_t *data = csd->raw;
    uint16_t c_size;
    uint8_t c_size_mult;
    uint8_t read_bl_len;

    /* CSD structure version */
    csd->csd_structure = (data[0] >> 6) & 0x03;

    /* READ_BL_LEN */
    read_bl_len          = data[5] & 0x0F;
    csd->max_read_bl_len = 1 << read_bl_len;

    /* C_SIZE (12 bits) */
    c_size = ((uint16_t)(data[6] & 0x03) << 10) | ((uint16_t)data[7] << 2) |
             ((data[8] >> 6) & 0x03);

    /* C_SIZE_MULT (3 bits) */
    c_size_mult = ((data[9] & 0x03) << 1) | ((data[10] >> 7) & 0x01);
    LOG_INF("c_size: %u, c_size_mult: %u, read_bl_len: %u", c_size, c_size_mult,
            read_bl_len);

    /* WRITE_BL_LEN */
    csd->max_write_bl_len = 1 << ((data[12] >> 2) & 0x0F);

    /* Calculate capacity: BLOCKNR = (C_SIZE+1) * MULT, MULT = 2^(C_SIZE_MULT+2) */
    csd->capacity_blocks = (c_size + 1) << (c_size_mult + 2);
    csd->capacity_bytes  = csd->capacity_blocks * csd->max_read_bl_len;
}

/**
 * @brief Parse CID register
 */
static void sd_parse_cid(struct sd_cid *cid)
{
    uint8_t *data = cid->raw;

    /* Manufacturer ID */
    cid->manufacturer_id = data[0];

    /* OEM/Application ID */
    cid->oem_id[0] = data[1];
    cid->oem_id[1] = data[2];
    cid->oem_id[2] = '\0';

    /* Product name (5 ASCII characters) */
    cid->product_name[0] = data[3];
    cid->product_name[1] = data[4];
    cid->product_name[2] = data[5];
    cid->product_name[3] = data[6];
    cid->product_name[4] = data[7];
    cid->product_name[5] = '\0';

    /* Product revision */
    cid->product_revision = data[8];

    /* Product serial number */
    cid->serial_number = ((uint32_t)data[9] << 24) | ((uint32_t)data[10] << 16) |
                         ((uint32_t)data[11] << 8) | data[12];

    /* Manufacturing date (12 bits: 8-bit year offset from 2000, 4-bit month) */
    cid->manufacture_date = ((uint16_t)data[13] << 4) | ((data[14] >> 4) & 0x0F);
}

int sd_read_csd(struct sd_device *dev)
{
    sd_cmd_t cmd;
    uint8_t res;
    uint16_t i;
    uint8_t crc_hi, crc_lo;

    if (!dev || !dev->initialized)
        return -EINVAL;

    /* CMD9: Send CSD */
    sd_cmd_prep(&cmd, SD_CMD9, 0);

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();
    if (res != 0) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("CMD9 failed: 0x%02X", res);
        return -EIO;
    }

    /* Wait for data token */
    for (i = 0; i < CONFIG_SD_READ_TIMEOUT; i++) {
        res = spi_transceive(0xFF);
        if (res != 0xFF) {
            if (res != SD_DATA_TOKEN) {
                spi_slave_unselect(dev->slave);
                LOG_ERR("Invalid CSD data token: 0x%02X", res);
                return -EIO;
            }
            break;
        }
    }

    if (i == CONFIG_SD_READ_TIMEOUT) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("CSD read timeout");
        return -ETIMEDOUT;
    }

    /* Read CSD data */
    for (i = 0; i < SD_CSD_SIZE; i++)
        dev->info.csd.raw[i] = spi_transceive(0xFF);

    LOG_INF("CSD raw data:");
    LOG_HEXDUMP_INF(dev->info.csd.raw, SD_CSD_SIZE);

    /* Read CRC (not validated) */
    crc_hi = spi_transceive(0xFF);
    crc_lo = spi_transceive(0xFF);
    (void)crc_hi;
    (void)crc_lo;

    spi_slave_unselect(dev->slave);

    /* Parse CSD based on structure version */
    uint8_t csd_structure = (dev->info.csd.raw[0] >> 6) & 0x03;
    if (csd_structure == 0) {
        sd_parse_csd_v1(&dev->info.csd);
    } else if (csd_structure == 1) {
        return -ENOTSUP;
    } else {
        LOG_ERR("Unknown CSD structure: %u", csd_structure);
        return -ENOTSUP;
    }

    dev->info.csd_valid = 1;
    LOG_INF("CSD: capacity=%lu blocks (%lu bytes)", dev->info.csd.capacity_blocks,
            dev->info.csd.capacity_bytes);

    return 0;
}

int sd_read_cid(struct sd_device *dev)
{
    sd_cmd_t cmd;
    uint8_t res;
    uint16_t i;
    uint8_t crc_hi, crc_lo;

    if (!dev || !dev->initialized)
        return -EINVAL;

    /* CMD10: Send CID */
    sd_cmd_prep(&cmd, SD_CMD10, 0);

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();
    if (res != 0) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("CMD10 failed: 0x%02X", res);
        return -EIO;
    }

    /* Wait for data token */
    for (i = 0; i < CONFIG_SD_READ_TIMEOUT; i++) {
        res = spi_transceive(0xFF);
        if (res != 0xFF) {
            if (res != SD_DATA_TOKEN) {
                spi_slave_unselect(dev->slave);
                LOG_ERR("Invalid CID data token: 0x%02X", res);
                return -EIO;
            }
            break;
        }
    }

    if (i == CONFIG_SD_READ_TIMEOUT) {
        spi_slave_unselect(dev->slave);
        LOG_ERR("CID read timeout");
        return -ETIMEDOUT;
    }

    /* Read CID data */
    for (i = 0; i < SD_CID_SIZE; i++)
        dev->info.cid.raw[i] = spi_transceive(0xFF);

    LOG_INF("CID raw data:");
    LOG_HEXDUMP_INF(dev->info.cid.raw, SD_CID_SIZE);

    /* Read CRC (not validated) */
    crc_hi = spi_transceive(0xFF);
    crc_lo = spi_transceive(0xFF);
    (void)crc_hi;
    (void)crc_lo;

    spi_slave_unselect(dev->slave);

    /* Parse CID */
    sd_parse_cid(&dev->info.cid);

    dev->info.cid_valid = 1;
    LOG_INF("CID: MID=0x%02X OEM=%s Name=%s Rev=%u.%u SN=0x%08lX Date=%u/%u",
            dev->info.cid.manufacturer_id, dev->info.cid.oem_id,
            dev->info.cid.product_name, dev->info.cid.product_revision >> 4,
            dev->info.cid.product_revision & 0x0F, dev->info.cid.serial_number,
            2000 + (dev->info.cid.manufacture_date >> 4),
            dev->info.cid.manufacture_date & 0x0F);

    return 0;
}

uint32_t sd_get_capacity_bytes(struct sd_device *dev)
{
    if (!dev || !dev->info.csd_valid)
        return 0;

    return dev->info.csd.capacity_bytes;
}

uint32_t sd_get_capacity_blocks(struct sd_device *dev)
{
    if (!dev || !dev->info.csd_valid)
        return 0;

    return dev->info.csd.capacity_blocks;
}

#endif