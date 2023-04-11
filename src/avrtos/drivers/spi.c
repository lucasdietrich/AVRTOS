#include "gpio.h"
#include "spi.h"

#include <avrtos/drivers.h>
#include <avrtos/drivers/gpio.h>

#if defined(__AVR_ATmega328P__)
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

int8_t spi_init(const struct spi_config *config)
{
	spi_deinit();

	if (config->mode == SPI_MODE_MASTER) {
		gpiol_pin_set_direction(GPIOB, SPI_SS_PIN,
					GPIO_MODE_OUTPUT);  // SS

		gpiol_pin_set_direction(GPIOB, SPI_SCK_PIN,
					GPIO_MODE_OUTPUT);  // SCK
		gpiol_pin_set_direction(GPIOB, SPI_MOSI_PIN,
					GPIO_MODE_OUTPUT);  // MOSI
		gpiol_pin_set_direction(GPIOB, SPI_MISO_PIN,
					GPIO_MODE_INPUT);  // MISO

		/* Master mode */
		SPI->SPCRn |= BIT(MSTR);

		/* Set clock rate */
		SPI->SPCRn |= (config->prescaler & 0x3u) << SPR0;
		SPI->SPSRn = (SPI->SPSRn & ~SPI2X_MASK) |
			     ((config->prescaler >> 2u) & SPI2X_MASK) << SPI2X;
	} else {
		gpiol_pin_set_direction(GPIOB, SPI_SS_PIN,
					GPIO_MODE_INPUT);  // SS

		gpiol_pin_set_direction(GPIOB, SPI_SCK_PIN,
					GPIO_MODE_INPUT);  // SCK
		gpiol_pin_set_direction(GPIOB, SPI_MOSI_PIN,
					GPIO_MODE_INPUT);  // MOSI
		gpiol_pin_set_direction(GPIOB, SPI_MISO_PIN,
					GPIO_MODE_OUTPUT);  // MISO
	}

	if (config->polarity == SPI_CLOCK_POLARITY_FALLING) {
		SPI->SPCRn |= BIT(CPOL);
	}

	if (config->phase == SPI_CLOCK_PHASE_SETUP) {
		SPI->SPCRn |= BIT(CPHA);
	}

	/* If async API is used, the interrupt cannot be enabled through
	 * configuration */
#if !CONFIG_SPI_ASYNC
	if (config->irq_enabled) {
		SPI->SPCRn |= BIT(SPIE);
	}
#endif

	// enable SPI
	SPI->SPCRn |= BIT(SPE);

	return 0;
}

int8_t spi_deinit(void)
{
	SPI->SPCRn = 0u;

	return 0u;
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
		      bool active_low)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!slave || !cs_port || cs_pin > PIN7) {
		return -EINVAL;
	}
#endif

	slave->cs_port	  = cs_port;
	slave->cs_pin	  = cs_pin;
	slave->active_low = active_low;

	gpio_pin_init(cs_port, cs_pin, GPIO_MODE_OUTPUT,
		      active_low ? GPIO_HIGH : GPIO_LOW);

	return 0;
}

char spi_slave_transceive(const struct spi_slave *slave, char tx)
{
	char rx;

	gpio_pin_write_state(slave->cs_port, slave->cs_pin,
			     slave->active_low ? GPIO_LOW : GPIO_HIGH);

	rx = spi_transceive(tx);

	gpio_pin_write_state(slave->cs_port, slave->cs_pin,
			     slave->active_low ? GPIO_HIGH : GPIO_LOW);

	return rx;
}

void spi_slave_transceive_buf(const struct spi_slave *slave, char *rxtx, uint8_t len)
{
	gpio_pin_write_state(slave->cs_port, slave->cs_pin,
			     slave->active_low ? GPIO_LOW : GPIO_HIGH);

	spi_transceive_buf(rxtx, len);

	gpio_pin_write_state(slave->cs_port, slave->cs_pin,
			     slave->active_low ? GPIO_HIGH : GPIO_LOW);
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

	rxtx	  = SPI->SPDRn;
	zcontinue = spi_callback(&rxtx);

	if (zcontinue) {
		SPI->SPDRn = rxtx;
	} else {
		SPI->SPCRn &= ~BIT(SPIE);
	}
}

int8_t spi_tranceive_async(char first_tx, spi_callback_t callback)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!callback) return -EINVAL;
#endif

	if (spi_async_inprogress()) return -EBUSY;

	spi_callback = callback;
	SPI->SPDRn = first_tx;
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
		spi_callback(NULL);	  /* Notify cancelation */

		// required ?
		SPI->SPSRn |= BIT(SPIF); /* Clear SPIF flag */
	}

	return 0;
}

#endif /* CONFIG_SPI_ASYNC */