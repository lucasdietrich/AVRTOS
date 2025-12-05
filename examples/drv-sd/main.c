// TODO when to check CRC and when not to? CRC_ON_OFF ??

#include <stdint.h>
#include <avrtos/logging.h>

#include "avrtos/defines.h"
#include "avrtos/devices/sd.h"
#include "avrtos/devices/sd_priv.h"
#include "avrtos/fault.h"
#include "avrtos/kernel.h"
#include "spi.h"

#define LOG_LEVEL LOG_LEVEL_INFO

#define BLOCK_LENGTH 512u

uint8_t SD_readRes1(void)
{
    uint8_t i = 0, res1;

    // keep polling until actual data received
    while ((res1 = spi_transceive(0xFF)) == 0xFF) {
        i++;

        // if no data received for 8 bytes, break
        if (i > 8) break;
    }

    return res1;
}

static struct sd_config {
    uint8_t version;
} sd_cfg;

int main(void)
{
    LOG_INF("SD Driver demo started");

    sd_cmd_t cmd;
    uint8_t resp[4u];
    uint8_t res;
    uint8_t block[BLOCK_LENGTH];

    const struct spi_config spi_cfg = {
        .role     = SPI_ROLE_MASTER,
        .polarity = SPI_CLOCK_POLARITY_RISING,
        .phase    = SPI_CLOCK_PHASE_SAMPLE,
        .prescaler =
            SPI_PRESCALER_128, // TODO increase this value to SPI_PRESCALER_4 at least
        .irq_enabled = 0u,
    };

    const struct spi_slave spi_slave = {
        .cs_port      = GPIOB_DEVICE,
        .cs_pin       = 0u,
        .active_state = GPIO_LOW,
        .regs         = spi_config_into_regs(spi_cfg),
    };

    spi_init(spi_cfg);

    spi_slave_ss_init(&spi_slave);

    // CRC CHECKS
    uint8_t cmd0[] = {0x40u, 0u, 0u, 0u, 0u, 1u};
    cmd0[5] |= crc7(cmd0, 5u) << 1;
    LOG_HEXDUMP_INF(cmd0, sizeof(cmd0)); // Expected 40 00 00 00 00 95

    uint8_t cmd17[] = {0x51u, 0u, 0u, 0u, 0u, 1u};
    cmd17[5] |= crc7(cmd17, 5u) << 1;
    LOG_HEXDUMP_INF(cmd17, sizeof(cmd17)); // Expected 51 00 00 00 00 55

    sd_cmd_prep(&cmd, 0u, 0u);
    LOG_HEXDUMP_INF(&cmd, sizeof(cmd)); // Expected 40 00 00 00 00 95

    /* Power up */
    spi_slave_unselect(&spi_slave);
    k_msleep(1);
    for (uint8_t i = 0u; i < 10u; i++) {
        (void)spi_transceive(0xFFu); // 80 clock cycles
    }

    /* CMD0 */
    sd_cmd_prep(&cmd, SD_CMD0, 0u);

    spi_slave_select(&spi_slave);
    spi_transceive(0xFFu);
    for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
        spi_transceive(cmd.buf[i]);
    }
    res = SD_readRes1();
    LOG_INF("CMD0 response: 0x%02X", res);
    spi_slave_unselect(&spi_slave);
    if (res & R1_IDLE_STATE) {
        LOG_INF("\tSD card is in IDLE state");
    } else if (res > 1u) {
        LOG_ERR("\tSD card CMD0 failed");
        panic();
    }

    /* CMD8 */
    uint32_t arg = ((0b0001lu) << 8u) | 0xAAu; // VHS = 1 (2.7-3.6V), check pattern = 0xAA
    sd_cmd_prep(&cmd, SD_CMD8, arg);
    LOG_INF("CMD8 prepared:");
    LOG_HEXDUMP_INF(&cmd, sizeof(cmd)); // Expected 48 00 01 AA 87

    spi_slave_select(&spi_slave);
    spi_transceive(0xFFu);
    for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
        spi_transceive(cmd.buf[i]);
    }
    res = SD_readRes1();
    if (res > 1u) {
        spi_slave_unselect(&spi_slave);
        if (res & R1_ILLEGAL_COMMAND) {
            LOG_ERR("\tSD card CMD8 illegal command - Ver1.X card");
            sd_cfg.version = 1u;
        } else {
            LOG_ERR("\tSD card CMD8 failed err: 0x%02X", res);
            panic();
        }
    } else {
        for (uint8_t i = 0u; i < 4u; i++) {
            resp[i] = spi_transceive(0xFFu);
        }
        spi_slave_unselect(&spi_slave);
        LOG_INF("CMD8 response: 0x%02X", res);
        LOG_HEXDUMP_INF(resp, sizeof(resp));
    }

    if (sd_cfg.version != 1u) {
        LOG_ERR("\tunsupported Ver2.X SD card");
        panic();
    }

    /* CMD 58 */
    sd_cmd_prep(&cmd, SD_CMD58, 0u);
    LOG_INF("CMD58 prepared:");
    LOG_HEXDUMP_INF(&cmd, sizeof(cmd));
    spi_slave_select(&spi_slave);
    spi_transceive(0xFFu);
    for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
        spi_transceive(cmd.buf[i]);
    }
    res = SD_readRes1();
    if (res > 1u) {
        spi_slave_unselect(&spi_slave);
        if (res & R1_ILLEGAL_COMMAND) {
            LOG_ERR("\tSD card CMD58 illegal command");
        } else {
            LOG_ERR("\tSD card CMD58 failed err: 0x%02X", res);
        }
        panic();
    } else {
        for (uint8_t i = 0u; i < 4u; i++) {
            resp[i] = spi_transceive(0xFFu);
        }
        spi_slave_unselect(&spi_slave);
        LOG_INF("CMD58 response OCR register: 0x%02X", res);
        LOG_HEXDUMP_INF(resp, sizeof(resp));
    }

    while (true) {
        /* CMD55, as every application specific command (ACMD) should be preceded by CMD55
         */
        sd_cmd_prep(&cmd, SD_CMD55, 0x0);
        LOG_INF("CMD55 prepared:");
        LOG_HEXDUMP_INF(&cmd, sizeof(cmd));
        spi_slave_select(&spi_slave);
        spi_transceive(0xFFu);
        for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
            spi_transceive(cmd.buf[i]);
        }
        res = SD_readRes1();
        spi_slave_unselect(&spi_slave);
        if (res > 1u) {
            LOG_ERR("\tSD card CMD55 failed err: 0x%02X", res);
            if (res & R1_ILLEGAL_COMMAND) {
                LOG_ERR("\tSD card CMD55 illegal command");
            }
            panic();
        }
        LOG_INF("CMD55 response: 0x%02X", res);
        if (res != 0x01u) { // ???
            LOG_ERR("\tSD card CMD55 unexpected response");
            panic();
        }

        /* ACMD 41 */
        sd_cmd_prep(&cmd, SD_ACMD41, 0x0);
        LOG_INF("ACMD41 prepared:");
        LOG_HEXDUMP_INF(&cmd, sizeof(cmd));
        spi_slave_select(&spi_slave);
        spi_transceive(0xFFu);
        for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
            spi_transceive(cmd.buf[i]);
        }
        res = SD_readRes1();
        if (res > 1u) {
            spi_slave_unselect(&spi_slave);
            LOG_ERR("\tSD card CMD41 failed err: 0x%02X", res);
            if (res & R1_ILLEGAL_COMMAND) {
                LOG_ERR("\tSD card CMD41 illegal command");
            }
            panic();
        } else {
            for (uint8_t i = 0u; i < 4u; i++) {
                resp[i] = spi_transceive(0xFFu);
            }
            spi_slave_unselect(&spi_slave);
            LOG_INF("CMD41 response OCR register: 0x%02X", res);
            LOG_HEXDUMP_INF(resp, sizeof(resp));
        }

        k_msleep(100);
        if (res == 0x00u) {
            LOG_INF("SD card initialized successfully");
            break;
        }
    }

    /* SD card is initialized, proceed with normal operation */

    /* Set block length, CMD16 */

    sd_cmd_prep(&cmd, SD_CMD16, BLOCK_LENGTH);
    LOG_INF("CMD16 prepared:");
    LOG_HEXDUMP_INF(&cmd, sizeof(cmd));
    spi_slave_select(&spi_slave);
    spi_transceive(0xFFu);
    for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
        spi_transceive(cmd.buf[i]);
    }
    res = SD_readRes1();
    spi_slave_unselect(&spi_slave);
    if (res > 1u) {
        LOG_ERR("\tSD card CMD16 failed err: 0x%02X", res);
        if (res & R1_ILLEGAL_COMMAND) {
            LOG_ERR("\tSD card CMD16 illegal command");
        }
        panic();
    }
    LOG_INF("CMD16 response: 0x%02X", res);
    if (res != 0x0) {
        LOG_ERR("\tSD card CMD16 unexpected response");
        panic();
    }

    /* Read a single block, CMD17 */
    uint32_t block_addr = 0*BLOCK_LENGTH; // byte address for SDSC, block address for SDHC
    sd_cmd_prep(&cmd, SD_CMD17, block_addr);
    LOG_INF("CMD17 prepared:");
    LOG_HEXDUMP_INF(&cmd, sizeof(cmd));
    spi_slave_select(&spi_slave);
    spi_transceive(0xFFu);
    for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
        spi_transceive(cmd.buf[i]);
    }
    res = SD_readRes1();
    if (res > 1u) {
        spi_slave_unselect(&spi_slave);
        LOG_ERR("\tSD card CMD17 failed err: 0x%02X", res);
        if (res & R1_ADDRESS_ERROR) {
            LOG_ERR("\tSD card CMD17 address error");
        }
        panic();
    } else {
        LOG_INF("CMD17 response: 0x%02X", res);
        if (res != 0x00u) {
            spi_slave_unselect(&spi_slave);
            LOG_ERR("\tSD card CMD17 unexpected response");
            panic();
        }

        /* Receive data token */
        for (uint16_t i = 0u; i < 1000u; i++) {
            res = spi_transceive(0xFFu);
            if (res != 0xFFu) {
                if (res != SD_DATA_TOKEN) {
                    spi_slave_unselect(&spi_slave);
                    LOG_ERR("\tSD card CMD17 unexpected data token: 0x%02X != 0xFE", res);
                    panic();
                }
                LOG_INF("Data token received: 0x%02X", res);
                break;
            }
        }
        LOG_INF("Receiving block data...");
        for (uint16_t i = 0u; i < BLOCK_LENGTH; i++) {
            block[i] = spi_transceive(0xFFu);
        }
        uint8_t crc_hi = spi_transceive(0xFFu);
        uint8_t crc_lo = spi_transceive(0xFFu);
        LOG_INF("Received CRC: 0x%02X%02X", crc_hi, crc_lo);
        LOG_INF("Block data received:");
        LOG_HEXDUMP_INF(block, BLOCK_LENGTH);
    }
    spi_slave_unselect(&spi_slave);
    
    /* Write a single block, CMD24 */
    sd_cmd_prep(&cmd, SD_CMD24, block_addr);
    LOG_INF("CMD24 prepared:");
    for (uint16_t i = 0u; i < BLOCK_LENGTH; i++) {
        block[i] = 0xEEu; // dummy data to write
    }
    LOG_HEXDUMP_INF(&cmd, sizeof(cmd));
    spi_slave_select(&spi_slave);
    spi_transceive(0xFFu);
    for (uint8_t i = 0u; i < sizeof(cmd.buf); i++) {
        spi_transceive(cmd.buf[i]);
    }
    res = SD_readRes1();
    if (res > 1u) {
        spi_slave_unselect(&spi_slave);
        LOG_ERR("\tSD card CMD24 failed err: 0x%02X", res);
        if (res & R1_ADDRESS_ERROR) {
            LOG_ERR("\tSD card CMD24 address error");
        }
        panic();
    } else {
        spi_transceive(SD_DATA_TOKEN); // data token
        for (uint16_t i = 0u; i < BLOCK_LENGTH; i++) {
            spi_transceive(block[i]);
        }
        
        LOG_INF("Data block sent, waiting for data response...");
        /* Wait for write completion */
        while (1) {
            res = spi_transceive(0xFFu);
            if (res != 0xFFu) {
                LOG_INF("Write completed successfully response: 0x%02X", res);
                break;
            }
        }

        if ((res & 0x07) == 0x05) {
            LOG_INF("Data accepted by SD card");
        } else {
            spi_slave_unselect(&spi_slave);
            LOG_ERR("\tSD card CMD24 data rejected: 0x%02X", res);
            panic();
        }
    }
    spi_slave_unselect(&spi_slave);

    while (1) {
        k_sleep(K_FOREVER);
    }
}