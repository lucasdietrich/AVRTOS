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

#define LOG_LEVEL LOG_LEVEL_INFO

#define BLOCK_OFFSET 0u /* Block number to read/write */

int main(void)
{
    struct sd_device sd;
    struct sd_card_info info;
    uint8_t block[SD_BLOCK_SIZE];
    struct k_prng prng;
    uint32_t seed;
    int ret;

    LOG_INF("SD card driver demo");

    const struct spi_config spi_cfg = {
        .role        = SPI_ROLE_MASTER,
        .polarity    = SPI_CLOCK_POLARITY_RISING,
        .phase       = SPI_CLOCK_PHASE_SAMPLE,
        .prescaler   = SPI_PRESCALER_128,
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
    ret = sd_get_info(&sd, &info);
    if (ret == 0) {
        LOG_INF("Card type: %d, version: %d", info.type, info.version);
        LOG_INF("OCR: 0x%08lX, voltage: 0x%02X", info.ocr, info.voltage_accepted);
    }

#if CONFIG_SD_CSD

    /* Read CSD (Card Specific Data) */
    ret = sd_read_csd(&sd);
    if (ret == 0) {
        LOG_INF("Capacity: %lu blocks (%lu bytes)", sd_get_capacity_blocks(&sd),
                sd_get_capacity_bytes(&sd));
        LOG_INF("Capacity: %lu KB / %lu MB", sd_get_capacity_bytes(&sd) / 1024,
                sd_get_capacity_bytes(&sd) / (1024UL * 1024UL));
    }

    /* Read CID (Card Identification) */
    ret = sd_read_cid(&sd);
    if (ret == 0) {
        ret = sd_get_info(&sd, &info);
        if (ret == 0 && info.cid_valid) {
            LOG_INF("Manufacturer: 0x%02X", info.cid.manufacturer_id);
            LOG_INF("OEM/App ID: %s", info.cid.oem_id);
            LOG_INF("Product: %s (rev %u.%u)", info.cid.product_name,
                    info.cid.product_revision >> 4, info.cid.product_revision & 0x0F);
            LOG_INF("Serial: 0x%08lX", info.cid.serial_number);
            LOG_INF("Manufactured: %u/%u", 2000 + (info.cid.manufacture_date >> 4),
                    info.cid.manufacture_date & 0x0F);
        }
    }

#endif

    /* Read block from SD card */
    ret = sd_read_block(&sd, BLOCK_OFFSET, block);
    if (ret < 0) {
        LOG_ERR("Read failed: %d", ret);
        goto error;
    }

    LOG_INF("Block %u read:", BLOCK_OFFSET);
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
    ret = sd_write_block(&sd, BLOCK_OFFSET, block);
    if (ret < 0) {
        LOG_ERR("Write failed: %d", ret);
        goto error;
    }

    LOG_INF("Block %u written with random data", BLOCK_OFFSET);

    /* Verify write */
    memset(block, 0, SD_BLOCK_SIZE);
    ret = sd_read_block(&sd, BLOCK_OFFSET, block);
    if (ret < 0) {
        LOG_ERR("Verify read failed: %d", ret);
        goto error;
    }

    LOG_INF("Verify data:");
    LOG_HEXDUMP_INF(block, SD_BLOCK_SIZE);

    LOG_INF("Demo completed successfully");

    while (1)
        k_sleep(K_FOREVER);

error:
    while (1)
        k_sleep(K_FOREVER);
}