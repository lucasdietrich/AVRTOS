/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include <avrtos/devices/sd.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/kernel.h>
#include <avrtos/logging.h>
#include <avrtos/prng.h>

#include "avrtos/sys.h"
#include "avrtos/systime.h"

#define LOG_LEVEL LOG_LEVEL_INF

static int test_read(struct sd_device *sd, uint32_t n);
static int test_rw(struct sd_device *sd, uint32_t n);
static int test_update(struct sd_device *sd, uint32_t n, uint8_t pattern);
static int test_readall(struct sd_device *sd, uint32_t max);

int main(void)
{
    struct sd_csd csd;
    struct sd_cid cid;
    struct sd_device sd;
    int ret;

    const struct spi_config spi_cfg = {
        .role        = SPI_ROLE_MASTER,
        .polarity    = SPI_CLOCK_POLARITY_RISING,
        .phase       = SPI_CLOCK_PHASE_SAMPLE,
        .prescaler   = SPI_PRESCALER_4, // SPI_PRESCALER_128
        .irq_enabled = 0,
    };

    const struct spi_slave spi_slave = {
        .cs_port      = GPIOB_DEVICE,
        .cs_pin       = 0,
        .active_state = GPIO_LOW,
        .regs         = spi_config_into_regs(spi_cfg),
    };

    spi_init(spi_cfg);
    spi_slave_ss_init(&spi_slave);

    /* Initialize SD card */
    ret = sd_init(&sd, &spi_slave);
    if (ret < 0) {
        LOG_ERR("SD init failed: %d", ret);
        goto error;
    }

    /* Get and display card information */
    LOG_INF("Card type: %d OCR: 0x%08lX, voltage: 0x%02X", sd.info.type, sd.info.ocr,
            sd.info.voltage_accepted);

    /* Read CSD (Card Specific Data) */
    ret = sd_read_csd(&sd, &csd);
    if (ret == 0) {
        LOG_INF("CSD v1: max_read_bl_len: %u max_write_bl_len: %u capacity: %lu blocks "
                "(%lu B)",
                csd.max_read_bl_len, csd.max_write_bl_len, csd.capacity_blocks,
                csd.capacity_bytes);
    } else {
        LOG_ERR("Read CSD failed: %d", ret);
        goto error;
    }

    /* Read CID (Card Identification) */
    ret = sd_read_cid(&sd, &cid); // FIX THE REFACTORING PROBLEMS !!! 
    if (ret == 0) {
        LOG_INF("Manufacturer: 0x%02X", cid.manufacturer_id);
        LOG_INF("OEM/App ID: %s", cid.oem_id);
        LOG_INF("Product: %s (rev %u.%u)", cid.product_name, cid.product_revision >> 4,
                cid.product_revision & 0x0F);
        LOG_INF("Serial: 0x%08lX", cid.serial_number);
        LOG_INF("Manufactured: %u/%u", 2000 + (cid.manufacturing_date >> 4),
                cid.manufacturing_date & 0x0F);
    } else {
        LOG_ERR("Read CID failed: %d", ret);
        goto error;
    }

    for (uint32_t i = 0; i < 5; i++) {
        ret = test_update(&sd, i, 0xAA);
        if (ret < 0) {
            LOG_ERR("Update test failed: %d", ret);
            goto error;
        }
    }

    for (uint32_t i = 0; i < 5; i++) {
        ret = test_read(&sd, i);
        if (ret < 0) {
            LOG_ERR("Update test failed: %d", ret);
            goto error;
        }
    }

    test_rw(&sd, 0);

    uint32_t max = MIN(2048, csd.capacity_blocks);
    test_readall(&sd, max);

error:
    while (1)
        k_sleep(K_FOREVER);
}

static int test_read(struct sd_device *sd, uint32_t n)
{
    int ret;
    uint8_t block[SD_BLOCK_SIZE];

    ret = sd_read_block(sd, n, block);
    if (ret < 0) {
        LOG_ERR("Read failed: %d", ret);
        return ret;
    }

    LOG_INF("Block %u read:", n);
    LOG_HEXDUMP_INF(block, SD_BLOCK_SIZE);

    return 0;
}

static int test_rw(struct sd_device *sd, uint32_t n)
{
    int ret;
    uint8_t block[SD_BLOCK_SIZE];
    struct k_prng prng;
    uint32_t seed;

    /* Read block from SD card */
    ret = sd_read_block(sd, n, block);
    if (ret < 0) {
        LOG_ERR("Read failed: %d", ret);
        return ret;
    }

    LOG_INF("Block %u read:", n);
    LOG_HEXDUMP_INF(block, SD_BLOCK_SIZE);

    /* Initialize PRNG with seed from first 4 bytes of block */
    seed = ((uint32_t)block[0] << 24) | ((uint32_t)block[1] << 16) |
           ((uint32_t)block[2] << 8) | block[3];

    /* If seed is all zeros or all ones, use default */
    if (seed == 0 || seed == 0xFFFFFFFF)
        seed = K_PRNG_DEFAULT_LFSR32;

    prng = (struct k_prng)K_PRNG_INITIALIZER(seed, K_PRNG_DEFAULT_LFSR31);

    LOG_INF("PRNG seed: 0x%08lX", seed);

    /* Generate random test data */
    k_prng_get_buffer(&prng, block, SD_BLOCK_SIZE);

    /* Write random data to block */
    ret = sd_write_block(sd, n, block);
    if (ret < 0) {
        LOG_ERR("Write failed: %d", ret);
        return ret;
    }

    LOG_INF("Block %u written with random data", n);

    /* Verify write */
    memset(block, 0, SD_BLOCK_SIZE);
    ret = sd_read_block(sd, n, block);
    if (ret < 0) {
        LOG_ERR("Verify read failed: %d", ret);
        return ret;
    }

    LOG_INF("Verify data:");
    LOG_HEXDUMP_INF(block, SD_BLOCK_SIZE);

    return 0;
}

static int test_update(struct sd_device *sd, uint32_t n, uint8_t pattern)
{
    /* Read block and uptdate with pattern */
    int ret;
    uint8_t block[SD_BLOCK_SIZE];
    uint16_t i;
    bool updated = false;

    ret = sd_read_block(sd, n, block);
    if (ret < 0) {
        LOG_ERR("Read failed: %d", ret);
        return ret;
    }

    for (i = 0; i < SD_BLOCK_SIZE; i++) {
        if (block[i] != pattern) {
            block[i] = pattern;
            updated = true;
        }
    }

    if (updated) {
        ret = sd_write_block(sd, n, block);
        if (ret < 0) {
            LOG_ERR("Write failed: %d", ret);
            return ret;
        }
        LOG_INF("Block %u updated with pattern 0x%02x", n, pattern);
    } else {
        LOG_INF("Block %u already has pattern 0x%02x, no update needed", n,
                pattern);
    }

    return 0;
}

static int test_readall(struct sd_device *sd, uint32_t max)
{
    int ret;
    uint8_t block[SD_BLOCK_SIZE];
    uint32_t b;

    LOG_INF("Reading all %lu blocks...", max);

    uint32_t t1, t2;
    uint32_t bytes_read       = 0;
    uint32_t read_duration_ms = 0;

    for (b = 0; b < max; b++) {
        t1  = k_uptime_get_ms32();
        ret = sd_read_block(sd, b, block);
        t2  = k_uptime_get_ms32();
        if (ret < 0) {
            LOG_ERR("Read block %lu failed: %d", b, ret);
            return ret;
        }

        bytes_read += SD_BLOCK_SIZE;
        read_duration_ms += (t2 - t1);

        if ((b + 1) % 128 == 0) {
            LOG_INF("Read %lu/%lu blocks (%lu MB) in %lums (%lu KB/s)", b + 1,
                    max, bytes_read / (1024lu * 1024), read_duration_ms,
                    (bytes_read / 1024) * 1000 / read_duration_ms);
        }

        LOG_DBG("Block %lu: read in %lums", b, (t2 - t1));
        LOG_HEXDUMP_DBG(block, SD_BLOCK_SIZE);
    }

    LOG_INF("All blocks read successfully");

    return 0;
}