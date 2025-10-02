/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "spi.h"

#include <avrtos/drivers.h>
#include <avrtos/drivers/gpio.h>

#include "gpio.h"

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
#define SPI_MOSI_PIN 3u
#define SPI_MISO_PIN 4u
#define SPI_SCK_PIN  5u
#define SPI_SS_PIN   2u
#elif defined(__AVR_ATmega2560__)
#define SPI_MOSI_PIN 2u
#define SPI_MISO_PIN 3u
#define SPI_SCK_PIN  1u
#define SPI_SS_PIN   0u
#endif

#define SPI2X_MASK BIT(SPI2X)

int8_t spi_init(struct spi_config config)
{
#if CONFIG_SPI_ASYNC
    /* If async API is used, the interrupt cannot be enabled through
     * configuration, so disable it */
    config.irq_enabled = 0u;
#endif

    const struct spi_regs stg = spi_config_into_regs(config);

    spi_deinit();

    if (config.role == SPI_ROLE_MASTER) {
        gpiol_pin_set_direction(GPIOB, SPI_SCK_PIN,
                                GPIO_MODE_OUTPUT); // SCK
        gpiol_pin_set_direction(GPIOB, SPI_MOSI_PIN,
                                GPIO_MODE_OUTPUT); // MOSI
        gpiol_pin_set_direction(GPIOB, SPI_MISO_PIN,
                                GPIO_MODE_INPUT); // MISO
    } else {
        gpiol_pin_set_direction(GPIOB, SPI_SS_PIN,
                                GPIO_MODE_INPUT); // SS

        gpiol_pin_set_direction(GPIOB, SPI_SCK_PIN,
                                GPIO_MODE_INPUT); // SCK
        gpiol_pin_set_direction(GPIOB, SPI_MOSI_PIN,
                                GPIO_MODE_INPUT); // MOSI
        gpiol_pin_set_direction(GPIOB, SPI_MISO_PIN,
                                GPIO_MODE_OUTPUT); // MISO
    }

    spi_regs_restore(&stg);

    return 0;
}

struct spi_regs spi_config_into_regs(struct spi_config config)
{
    struct spi_regs regs;

    /* Set clock rate */
    regs.spcr = BIT(SPE) | (config.prescaler & 0x3u) << SPR0;
    regs.spsr = ((config.prescaler >> 2u) & SPI2X_MASK) << SPI2X;

    if (config.role == SPI_ROLE_MASTER) regs.spcr |= BIT(MSTR);
    if (config.polarity == SPI_CLOCK_POLARITY_FALLING) regs.spcr |= BIT(CPOL);
    if (config.phase == SPI_CLOCK_PHASE_SETUP) regs.spcr |= BIT(CPHA);
    if (config.irq_enabled) regs.spcr |= BIT(SPIE);

    return regs;
}

void spi_regs_restore(const struct spi_regs *regs)
{
    SPI->SPCRn = regs->spcr;
    SPI->SPSRn = regs->spsr;
}

void spi_regs_swap(struct spi_regs *regs)
{
    struct spi_regs current_regs;
    spi_regs_save(&current_regs);
    spi_regs_restore(regs);
    *regs = current_regs;
}

void spi_regs_save(struct spi_regs *regs)
{
    regs->spcr = SPI->SPCRn;
    regs->spsr = SPI->SPSRn;
}

void spi_deinit(void)
{
    SPI->SPCRn = 0u;
    SPI->SPSRn = 0u;
}

char spi_transceive(char tx)
{
    SPI->SPDRn = tx;

    while (!(SPI->SPSRn & BIT(SPIF)))
        ;

    return SPI->SPDRn;
}

void spi_transceive_buf(char *rxtx, uint8_t len)
{
    while (len--) {
        *rxtx = spi_transceive(*rxtx);
        rxtx++;
    }
}

int8_t spi_slave_init(struct spi_slave *slave,
                      GPIO_Device *cs_port,
                      uint8_t cs_pin,
                      uint8_t active_state,
                      const struct spi_regs *regs)
{
    Z_ARGS_CHECK(slave && regs && cs_port && cs_pin <= PIN7) return -EINVAL;

    slave->cs_port      = cs_port;
    slave->cs_pin       = cs_pin;
    slave->active_state = active_state;
    slave->regs         = *regs;

    return 0;
}

void spi_slave_select(const struct spi_slave *slave)
{
    gpio_pin_write_state(slave->cs_port, slave->cs_pin, slave->active_state);
}

void spi_slave_unselect(const struct spi_slave *slave)
{
    gpio_pin_write_state(slave->cs_port, slave->cs_pin, 1u - slave->active_state);
}

int8_t spi_slave_ss_init(const struct spi_slave *slave)
{
    Z_ARGS_CHECK(slave) return -EINVAL;

    gpio_pin_init(slave->cs_port, slave->cs_pin, GPIO_MODE_OUTPUT,
                  (slave->active_state == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW);

    return 0;
}

char spi_slave_transceive(const struct spi_slave *slave, char tx)
{
    char rx;

    spi_slave_select(slave);

    rx = spi_transceive(tx);

    spi_slave_unselect(slave);

    return rx;
}

void spi_slave_transceive_buf(const struct spi_slave *slave, char *rxtx, uint8_t len)
{
    gpio_pin_write_state(slave->cs_port, slave->cs_pin,
                         slave->active_state ? GPIO_LOW : GPIO_HIGH);

    spi_transceive_buf(rxtx, len);

    gpio_pin_write_state(slave->cs_port, slave->cs_pin,
                         slave->active_state ? GPIO_HIGH : GPIO_LOW);
}

#if CONFIG_SPI_ASYNC
/* Callback function for async SPI
 * it should remain properly defined when SPI->SPCRn is set
 */
static spi_callback_t spi_callback = NULL;

ISR(SPI_STC_vect)
{
    char rxtx;
    bool zcontinue;

    rxtx      = SPI->SPDRn;
    zcontinue = spi_callback(&rxtx);

    if (zcontinue) {
        SPI->SPDRn = rxtx;
    } else {
        SPI->SPCRn &= ~BIT(SPIE);
    }
}

int8_t spi_transceive_async_start(char first_tx, spi_callback_t callback)
{
    Z_ARGS_CHECK(callback) return -EINVAL;

    if (spi_async_inprogress()) return -EBUSY;

    spi_callback = callback;
    SPI->SPDRn   = first_tx;
    SPI->SPCRn |= BIT(SPIE);

    return 0;
}

bool spi_async_inprogress(void)
{
    return SPI->SPCRn & BIT(SPIE);
}

int8_t spi_cancel_async(void)
{
    if (spi_async_inprogress()) {
        SPI->SPCRn &= ~BIT(SPIE); /* Disable interrupt */
        spi_callback(NULL);       /* Notify cancelation */

        // required ?
        SPI->SPSRn |= BIT(SPIF); /* Clear SPIF flag */
    }

    return 0;
}

#endif /* CONFIG_SPI_ASYNC */