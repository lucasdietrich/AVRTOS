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
#include <avrtos/subsystems/crc.h>
#include <avrtos/sys.h>

#include "avrtos/defines.h"
#include "sd_priv.h"

#define LOG_LEVEL CONFIG_SD_LOG_LEVEL

/* Retries and timing constants */
#define SD_R1_RESPONSE_RETRIES   8
#define SD_POWERUP_CLOCKS        10
#define SD_ACMD41_RETRY_DELAY_MS 100u /* ACMD41 retry delay in ms */

#define STUFF_BITS 0x00000000u

// clang-format off
#define SD_CMD0_BYTES  {0x40, 0x0, 0x0, 0x0, 0x0, 0x95}
#define SD_CMD58_BYTES {0x7a, 0x00, 0x00, 0x00, 0x00, 0xfd}
#define SD_CMD55_BYTES {0x77, 0x00, 0x00, 0x00, 0x00, 0x65}
#define SD_CMD41_BYTES {0x69, 0x40, 0x00, 0x00, 0x00, 0xe5}
#define SD_CMD9_BYTES  {0x49, 0x00, 0x00, 0x00, 0x00, 0xaf}
#define SD_CMD10_BYTES {0x4a, 0x00, 0x00, 0x00, 0x00, 0x1b}

#define SD_CMD_FROM_BYTES(_buf) (sd_cmd_t) { .buf = _buf }
// clang-format on

#define SD_GO_IDLE_STATE SD_CMD_FROM_BYTES(SD_CMD0_BYTES)
#define SD_READ_OCR      SD_CMD_FROM_BYTES(SD_CMD58_BYTES)
#define SD_APP_CMD       SD_CMD_FROM_BYTES(SD_CMD55_BYTES)
#define SD_SEND_OP_COND  SD_CMD_FROM_BYTES(SD_CMD41_BYTES)
#define SD_SEND_CSD      SD_CMD_FROM_BYTES(SD_CMD9_BYTES)
#define SD_SEND_CID      SD_CMD_FROM_BYTES(SD_CMD10_BYTES)

typedef struct sd_cmd {
    uint8_t buf[6u];
} sd_cmd_t;

/**
 * @brief Prepare SD command
 *
 * @param cmd Command structure to fill
 * @param index Command index
 * @param arg Command argument
 */
static void sd_cmd_prep(sd_cmd_t *cmd, uint8_t index, uint32_t arg)
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

static uint8_t sd_send_cmd_read_r1(struct sd_device *dev, sd_cmd_t *cmd)
{
    uint8_t res, i;

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd->buf); i++)
        spi_transceive(cmd->buf[i]);

    res = sd_read_r1();
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
 * @brief Wait for data token from SD card
 *
 * @return 0 if valid data token received, -EIO for invalid token, -ETIMEDOUT on timeout
 */
static int sd_wait_data_token(void)
{
    uint16_t i;
    uint8_t res;

    for (i = 0; i < CONFIG_SD_READ_TIMEOUT; i++) {
        res = spi_transceive(0xFF);
        if (res != 0xFF) {
            if (res == SD_DATA_TOKEN)
                return 0;
            else
                return -EIO;
        }
    }
    return -ETIMEDOUT;
}

/**
 * @brief Read data block with token validation and CRC
 *
 * @param buf Buffer to store read data
 * @param size Size of data to read
 * @return 0 on success, negative error code on failure
 */
static int sd_read_data_block(uint8_t *buf, size_t size)
{
    uint16_t i;
    int ret;
    uint8_t crc_hi, crc_lo;

    ret = sd_wait_data_token();
    if (ret != 0)
        return ret;

    for (i = 0; i < size; i++)
        buf[i] = spi_transceive(0xFF);

    /* Read CRC (not validated) */
    crc_hi = spi_transceive(0xFF);
    crc_lo = spi_transceive(0xFF);
    (void)crc_hi;
    (void)crc_lo;

    return 0;
}

/**
 * @brief Read R3/R7 response (R1 + 4 bytes)
 *
 * @param slave SPI slave
 * @param cmd Command to send
 * @param resp Buffer for 4-byte response
 * @return R1 response byte
 */
static uint8_t
sd_send_cmd_r3(const struct spi_slave *slave, sd_cmd_t *cmd, uint8_t resp[4])
{
    uint8_t res, i;

    spi_slave_select(slave);

    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd->buf); i++)
        spi_transceive(cmd->buf[i]);
    res = sd_read_r1();
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
    sd_cmd_t cmd;

    if (!z_user(dev && slave))
        return -EINVAL;

    dev->slave                 = slave;
    dev->info.type             = SD_CARD_TYPE_UNKNOWN;
    dev->info.ocr              = 0;
    dev->info.voltage_accepted = 0;

    /* Power up sequence
     * Initialization delay is the maximum of:
     * - 1 msec
     * - 74 clock cycles
     * - supply ramp up time
     */
    spi_slave_unselect(slave);
    k_msleep(1);
    for (uint8_t i = 0; i < SD_POWERUP_CLOCKS; i++)
        spi_transceive(0xFF);

    /* CMD0: Reset card */
    cmd = SD_GO_IDLE_STATE;
    res = sd_send_cmd_read_r1(dev, &cmd);
    if (!(res & R1_IDLE_STATE))
        return -EIO;

    /* CMD8: Check voltage range */
    arg = (SD_CMD8_VHS_27_36V << SD_CMD8_VHS_SHIFT) | SD_CMD8_CHECK_PATTERN;
    sd_cmd_prep(&cmd, SD_CMD8, arg);
    res = sd_send_cmd_r3(slave, &cmd, resp);
    if (!(res & R1_IDLE_STATE))
        return -EIO;
    if (!(res & R1_ILLEGAL_COMMAND)) {
        LOG_ERR("Ver2.X SD cards not supported");
        return -ENOTSUP;
    }
    dev->info.type             = SD_CARD_TYPE_SDSC_V1;
    dev->info.voltage_accepted = resp[2] & SD_CMD8_VOLTAGE_MASK;

    /* CMD58: Read OCR */
    cmd = SD_READ_OCR;
    res = sd_send_cmd_r3(slave, &cmd, resp);
    if (res > 1)
        return -EIO;
    dev->info.ocr = sys_read_be32(resp);

    /* ACMD41: Initialize card */
    timeout = CONFIG_SD_INIT_TIMEOUT_MS / SD_ACMD41_RETRY_DELAY_MS;
    while (timeout--) {
        /* CMD55: App command follows */
        cmd = SD_APP_CMD;
        res = sd_send_cmd_read_r1(dev, &cmd);
        if (res > 1)
            return -EIO;

        /* ACMD41: Initialize */
        cmd = SD_SEND_OP_COND;
        res = sd_send_cmd_read_r1(dev, &cmd);
        if (res == 0)
            break;
        if (res > 1)
            return -EIO;

        k_msleep(SD_ACMD41_RETRY_DELAY_MS);
    }
    if (timeout == 0)
        return -ETIMEDOUT;

    /* CMD16: Set block length */
    sd_cmd_prep(&cmd, SD_CMD16, SD_BLOCK_SIZE);
    res = sd_send_cmd_read_r1(dev, &cmd);
    if (res != 0)
        return -EIO;

    return 0;
}

int sd_read_block(struct sd_device *dev, uint32_t block_addr, uint8_t *buf)
{
    int ret;
    sd_cmd_t cmd;
    uint16_t i;
    uint8_t res;

    if (!z_user(!dev || !dev->info.type || !buf))
        return -EINVAL;

    /* CMD17: Read single block */
    sd_cmd_prep(&cmd, SD_CMD17, block_addr * SD_BLOCK_SIZE);

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();
    if (res != 0) {
        ret = -EIO;
        goto exit;
    }

    ret = sd_read_data_block(buf, SD_BLOCK_SIZE);
    if (ret != 0)
        goto exit;

exit:
    spi_slave_unselect(dev->slave);
    return ret;
}

int sd_write_block(struct sd_device *dev, uint32_t block_addr, const uint8_t *buf)
{
    sd_cmd_t cmd;
    uint8_t res;
    uint16_t i;
    int ret;

    if (!z_user(dev && buf))
        return -EINVAL;

    if (!z_user(dev->info.type))
        return -ENODEV;

    /* CMD24: Write single block */
    sd_cmd_prep(&cmd, SD_CMD24, block_addr * SD_BLOCK_SIZE);

    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);
    res = sd_read_r1();
    if (res != 0) {
        ret = -EIO;
        goto exit;
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
        ret = -ETIMEDOUT;
        goto exit;
    }

    /* Check data response */
    if ((res & SD_DATA_RESPONSE_MASK) != SD_DATA_ACCEPTED) {
        ret = -EIO;
        LOG_ERR("Write rejected: 0x%02X", res);
        goto exit;
    }

    /* Wait for write completion */
    ret = sd_wait_ready();

exit:
    spi_slave_unselect(dev->slave);

    return ret;
}

/**
 * @brief Parse CSD v1.0 register
 */
static inline void sd_parse_csd_v1(uint8_t *data, struct sd_csd *csd)
{
    uint16_t c_size;
    uint8_t c_size_mult;
    uint8_t read_bl_len, write_bl_len;

    /* READ_BL_LEN (4 bits [83:80])*/
    read_bl_len          = data[5] & 0x0F;
    csd->max_read_bl_len = 1 << read_bl_len;

    /* WRITE_BL_LEN (4 bits [25:22]) */
    write_bl_len          = ((data[12] & 0x03) << 2) | ((data[13u] >> 6) & 0x03);
    csd->max_write_bl_len = 1 << write_bl_len;

    /* C_SIZE (12 bits [73:62]) */
    c_size = ((uint16_t)(data[6] & 0x03) << 10) | ((uint16_t)data[7] << 2) |
             ((data[8] >> 6) & 0x03);

    /* C_SIZE_MULT (3 bits [49:47]) */
    c_size_mult = ((data[9] & 0x03) << 1) | ((data[10] >> 7) & 0x01);

    /* Calculate capacity: BLOCKNR * BLOCK_LEN
     * BLOCK_LEN = 2^READ_BL_LEN
     * MULT = 2^(C_SIZE_MULT+2)
     * BLOCKNR = (C_SIZE+1) * MULT
     */
    csd->capacity_blocks = ((uint32_t)c_size + 1u) << (c_size_mult + 2);
    csd->capacity_bytes  = csd->capacity_blocks * (uint32_t)csd->max_read_bl_len;
}

/**
 * @brief Parse CID register
 */
static void sd_parse_cid(uint8_t *data, struct sd_cid *cid)
{
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
    cid->manufacturing_date = ((uint16_t)data[13] << 4) | ((data[14] >> 4) & 0x0F);
}

int sd_read_csd(struct sd_device *dev, struct sd_csd *csd)
{
    int ret;
    sd_cmd_t cmd;
    uint16_t i;
    uint8_t res;
    uint8_t buf[SD_CSD_SIZE];

    if (!z_user(dev))
        return -EINVAL;

    if (!z_user(dev->info.type))
        return -ENODEV;

    cmd = SD_SEND_CSD;
    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);
    res = sd_read_r1();
    if (res != 0) {
        ret = -EIO;
        goto exit;
    }

    ret = sd_read_data_block(buf, SD_CSD_SIZE);
    if (ret != 0)
        goto exit;

    LOG_DBG("CSD:");
    LOG_HEXDUMP_DBG(buf, SD_CSD_SIZE);

    /* Parse CSD based on structure version */
    uint8_t csd_structure = (buf[0] >> 6) & 0x03;
    if (csd_structure != SD_CSD_VERSION_1)
        return -ENOTSUP;
    csd->csd_structure = csd_structure;

    sd_parse_csd_v1(buf, csd);

    ret = 0;

exit:
    spi_slave_unselect(dev->slave);

    return ret;
}

int sd_read_cid(struct sd_device *dev, struct sd_cid *cid)
{
    int ret;
    sd_cmd_t cmd;
    uint16_t i;
    uint8_t buf[SD_CID_SIZE];
    uint8_t res;

    if (!z_user(dev))
        return -EINVAL;

    if (!z_user(dev->info.type))
        return -ENODEV;

    cmd = SD_SEND_CID;
    spi_slave_select(dev->slave);
    spi_transceive(0xFF);
    for (i = 0; i < sizeof(cmd.buf); i++)
        spi_transceive(cmd.buf[i]);

    res = sd_read_r1();
    if (res != 0) {
        ret = -EIO;
        goto exit;
    }

    ret = sd_read_data_block(buf, SD_CID_SIZE);
    if (ret != 0)
        goto exit;

    LOG_DBG("CID raw data:");
    LOG_HEXDUMP_DBG(buf, SD_CID_SIZE);

    sd_parse_cid(buf, cid);

    ret = 0;

exit:
    spi_slave_unselect(dev->slave);

    return ret;
}
