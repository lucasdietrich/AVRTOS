/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/spi.h>

#define VREF           4.886f
#define ADC_RESOLUTION 10u
#define ADC_MAX_VALUE  ((1u << ADC_RESOLUTION) - 1u)

int main(void)
{
    serial_init();

    const struct spi_config cfg = {
        .role        = SPI_ROLE_MASTER,
        .polarity    = SPI_CLOCK_POLARITY_RISING,
        .phase       = SPI_CLOCK_PHASE_SAMPLE,
        .prescaler   = SPI_PRESCALER_X32,
        .irq_enabled = 0u,
    };

    // initialize SPI
    spi_init(cfg);

    // initialize PB0 as CS
    gpiol_pin_init(GPIOB, PIN0, GPIO_MODE_OUTPUT, GPIO_HIGH);

    const uint8_t channel  = 0u;
    const uint8_t sgl_diff = 1u; /* Single-ended */

    for (;;) {
        gpiol_pin_write_state(GPIOB, PIN0, GPIO_LOW);

        spi_transceive(0x1); /* 7 leading zeros + start bit */

        const uint8_t msb = spi_transceive((sgl_diff << 7u) | (channel << 4u));
        const uint8_t lsb = spi_transceive(0x0);

        gpiol_pin_write_state(GPIOB, PIN0, GPIO_HIGH);

        uint16_t value = ((msb & 0x7u) << 8u) | lsb;
        float voltage  = (value * VREF) / (1 << ADC_RESOLUTION);

        printf_P(PSTR("ADC value: %u,\tvoltage: %.3f V (%.2f %%)\n"), value, voltage,
                 (100.0f * value / ADC_MAX_VALUE));

        k_sleep(K_MSEC(100u));
    }
}