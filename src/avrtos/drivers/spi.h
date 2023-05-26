#ifndef SPI_H_
#define SPI_H_

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>

/**
 * SPI driver
 *
 * If CONFIG_SPI_ASYNC is disabled, it is possible for the user to define
 * a custom SPI interrupt handler by defining the function:
 *
 * ISR(SPI_STC_vect) {
 *	// Do something with SPI->SPDRn
 * }
 *
 * Advised configuration:
 * - SPI_CLOCK_POLARITY_FALLING with SPI_CLOCK_PHASE_SAMPLE
 * - prescaler >= SPI_PRESCALER_4
 *
 * Discouraged configuration:
 * - SPI_CLOCK_POLARITY_RISING and SPI_CLOCK_PHASE_SAMPLE
 * - prescaler <= SPI_PRESCALER_X2
 *
 * Use convenient defines SPI_CONFIG_MASTER_DEFAULTS() and
 * SPI_CONFIG_SLAVE_DEFAULTS() to set up a default configuration.
 */

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief SPI device registers.
 */
typedef struct {
	__IO uint8_t SPCRn; /* Control register */
	__IO uint8_t SPSRn; /* Status register */
	__IO uint8_t SPDRn; /* Data register */
} SPI_Device;

/* SPI base address */
#define SPI_BASE_ADDR (AVR_IO_BASE_ADDR + 0x004Cu)

/* SPI device */
#define SPI_DEVICE ((SPI_Device *)SPI_BASE_ADDR)

/* SPI device shorthand */
#define SPI SPI_DEVICE

typedef enum {
	SPI_PRESCALER_4	  = 0u,
	SPI_PRESCALER_16  = 1u,
	SPI_PRESCALER_64  = 2u,
	SPI_PRESCALER_128 = 3u,
	SPI_PRESCALER_X2  = 4u,
	SPI_PRESCALER_X8  = 5u,
	SPI_PRESCALER_X32 = 6u,
	SPI_PRESCALER_X64 = 7u
} spi_prescaler_t;

typedef enum {
	SPI_MODE_MASTER = 0u,
	SPI_MODE_SLAVE	= 1u
} spi_mode_t;

/* Leading edge */
typedef enum {
	SPI_CLOCK_POLARITY_RISING  = 0u, /* CPOL = 0, SCK is low when idle */
	SPI_CLOCK_POLARITY_FALLING = 1u /* CPOL = 1, SCK is high when idle */
} spi_clock_polarity_t;

/* Leading edge */
typedef enum {
	SPI_CLOCK_PHASE_SAMPLE = 0u, /* CPHA = 0, sample on leading edge */
	SPI_CLOCK_PHASE_SETUP  = 1u /* CPHA = 1, sample on trailing edge */
} spi_clock_phase_t;

struct spi_config {
	/* Master or slave */
	spi_mode_t mode : 1u;

	/* Clock polarity */
	spi_clock_polarity_t polarity : 1u;

	/* Sample on leading or trailing edge */
	spi_clock_phase_t phase : 1u;

	/* Enable SPI interrupt (ignored if CONFIG_SPI_ASYNC support is enabled) */
	uint8_t irq_enabled : 1u;

	/* Clock prescaler (master only)*/
	spi_prescaler_t prescaler : 3u;

	// /* Force SS pin to output mode if the SPI is in master mode.
	//  * (master only, in slave mode, the SS pin is always input) */
	// uint8_t force_ss_output : 1u;
};

#define SPI_CONFIG_MASTER_DEFAULTS                                               \
	{                                                                        \
		.mode = SPI_MODE_MASTER, .polarity = SPI_CLOCK_POLARITY_FALLING, \
		.phase = SPI_CLOCK_PHASE_SAMPLE, .irq_enabled = 0u,              \
		.prescaler = SPI_PRESCALER_4,                                    \
	}

#define SPI_CONFIG_SLAVE_DEFAULTS                                               \
	{                                                                       \
		.mode = SPI_MODE_SLAVE, .polarity = SPI_CLOCK_POLARITY_FALLING, \
		.phase = SPI_CLOCK_PHASE_SAMPLE, .irq_enabled = 0u,             \
	}

/**
 * @brief Initialize SPI peripheral.
 *
 * @param config Configuration to use.
 * @return int8_t 0 on success, negative on error.
 */
int8_t spi_init(const struct spi_config *config);

/**
 * @brief Deinitialize SPI peripheral.
 *
 * @return int8_t 0 on success, negative on error.
 */
int8_t spi_deinit(void);

/**
 * @brief Transceive a byte over SPI.
 *
 * Assume that the slave is selected.
 *
 * @param tx
 * @return char
 */
char spi_transceive(char tx);

/**
 * @brief Transceive a buffer over SPI.
 *
 * Assume that the slave is selected.
 *
 * @param rxtx
 * @param len
 */
void spi_transceive_buf(char *rxtx, uint8_t len);

struct spi_slave {
	/* Slave chip select port */
	GPIO_Device *cs_port;
	/* Slave chip select pin */
	uint8_t cs_pin : 3u;
	/* Slave chip select active low */
	uint8_t active_state : 1u;
};

/**
 * @brief Initialize a SPI slave structure
 *
 * @param slave Pointer to the slave structure.
 * @param cs_port Pointer to the GPIO port of the slave chip select pin.
 * @param cs_pin Slave chip select pin.
 * @param active_state Slave chip select active low.
 * @return int8_t 0 on success, negative on error.
 */
int8_t spi_slave_init(struct spi_slave *slave,
		      GPIO_Device *cs_port,
		      uint8_t cs_pin,
		      uint8_t active_state);

/**
 * @brief Initialize a SPI chip select pin for the slave.
 *
 * @param slave Pointer to the slave structure.
 * @return int8_t 0 on success, negative on error.
 */
int8_t spi_slave_ss_init(const struct spi_slave *slave);

/**
 * @brief Transceive a byte over SPI with a slave.
 *
 * The slave is selected before the transmission and deselected after.
 *
 * @param slave
 * @param tx
 * @return char
 */
char spi_slave_transceive(const struct spi_slave *slave, char tx);

/**
 * @brief Transceive a buffer over SPI with a slave.
 *
 * The slave is selected before the transmission and deselected after.
 *
 * @param slave
 * @param rxtx
 * @param len
 */
void spi_slave_transceive_buf(const struct spi_slave *slave, char *rxtx, uint8_t len);

/**
 * @brief SPI callback function type for asynchronous SPI tranceive.
 *
 * @param rxtx Pointer to the byte to transmit. The received byte is also returned in this
 * pointer. If NULL, we are notified that the asynchronous transmission has been canceled.
 * @return true Continue the transmission, false stop the transmission.
 */
typedef bool (*spi_callback_t)(char *rxtx);

/**
 * @brief Transceive a byte over SPI asynchronously in master and slave mode.
 *
 * Assume that the slave is selected beforce the transmission and deselected after
 * (or deselected on the callback function).
 *
 * This function is non-blocking and returns immediately.
 *
 * The function enables the SPI interrupt and disables it when the transmission is
 * complete.
 *
 * In master mode, the transmission is started immediately. When a byte transmission
 * is complete, the callback function is called in the interrupt context.
 * The received byte is passed as an argument to the callback function.
 * The next byte to transmit is returned by the callback using the argument pointer.
 * If the callback returns true, the transmission is continued, otherwise it is stopped.
 *
 * If it is decided to abort the asynchronous transmission, the byte prepared for
 * transmission in the callback is ignored.
 *
 * In slave mode, the behavior stays the same, except that the transmission is started
 * when the master starts to transmit. The callback function is called each time a byte
 * is received from the master.
 *
 * In slave mode:
 * In case you want to cancel a transmission, call spi_cancel_async().
 *
 * @param first_tx First byte to transmit.
 * @param callback Callback function to call when the transmission of the first byte is
 * complete.
 * @return int number of bytes sent on success, negative value on error.
 */
int8_t spi_tranceive_async(char first_tx, spi_callback_t callback);

/**
 * @brief Check if an asynchronous SPI transmission is running.
 *
 * As synchronous functions should not be called while an asynchronous transmission is
 * running, this function can be used for this purpose.
 *
 * @return true
 * @return false
 */
bool spi_async_inprogress(void);

/**
 * @brief Cancel an asynchronous SPI transmission.
 *
 * @return int
 */
int8_t spi_cancel_async(void);

#if defined(__cplusplus)
}
#endif

#endif /* SPI_H_ */