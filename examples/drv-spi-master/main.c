/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/spi.h>

#include <util/crc16.h>

#define DELAY_US 25u

int main(void)
{
    serial_init();

    /* Advised configuration */
    const struct spi_config cfg = {
        .role        = SPI_ROLE_MASTER,
        .polarity    = SPI_CLOCK_POLARITY_RISING,
        .phase       = SPI_CLOCK_PHASE_SAMPLE,
        .prescaler   = SPI_PRESCALER_4,
        .irq_enabled = 0u,
    };

    gpiol_pin_init(GPIOB, PIN0, GPIO_MODE_OUTPUT, GPIO_HIGH);

    spi_init(cfg);

    k_thread_dump_all();

    char chr = 0u;

    for (;;) {
        gpiol_pin_write_state(GPIOB, PIN0, GPIO_LOW);

        spi_transceive(chr);

        gpiol_pin_write_state(GPIOB, PIN0, GPIO_HIGH);

        __Z_DBG_GPIO_0_TOGGLE();

        z_cpu_block_us(DELAY_US);

        chr++;
    }
}