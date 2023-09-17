/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVER_GPIO_H_
#define _AVRTOS_DRIVER_GPIO_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
	__IO uint8_t PIN;
	__IO uint8_t DDR;
	__IO uint8_t PORT;
} GPIO_Device;

#define DIR_INPUT  0u
#define DIR_OUTPUT 1u

#define PIN_NO_PULLUP	   0u
#define PIN_PULLUP	   1u
#define OUTPUT_DRIVEN_LOW  0u
#define OUTPUT_DRIVEN_HIGH 1u

#define GPIO_INPUT  0u
#define GPIO_OUTPUT 1u

#define GPIO_MODE_INPUT	 0u
#define GPIO_MODE_OUTPUT 1u

#define GPIO_INPUT_NO_PULLUP	0u
#define GPIO_INPUT_PULLUP	1u
#define GPIO_OUTPUT_DRIVEN_LOW	0u
#define GPIO_OUTPUT_DRIVEN_HIGH 1u

#define STATE_LOW  0u
#define STATE_HIGH 1u

#define GPIO_LOW  0u
#define GPIO_HIGH 1u

#define PINn0 PIN0
#define PINn1 PIN1
#define PINn2 PIN2
#define PINn3 PIN3
#define PINn4 PIN4
#define PINn5 PIN5
#define PINn6 PIN6
#define PINn7 PIN7

#define DDn0 DDA0
#define DDn1 DDA1
#define DDn2 DDA2
#define DDn3 DDA3
#define DDn4 DDA4
#define DDn5 DDA5
#define DDn6 DDA6
#define DDn7 DDA7

#define PORTn0 PORTA0
#define PORTn1 PORTA1
#define PORTn2 PORTA2
#define PORTn3 PORTA3
#define PORTn4 PORTA4
#define PORTn5 PORTA5
#define PORTn6 PORTA6
#define PORTn7 PORTA7

#define AVR_GPIO_BASE_ADDR (AVR_IO_BASE_ADDR + 0x20u)

#define AVR_GPIO_ABCDEFG_BASE_ADDR (AVR_IO_BASE_ADDR + 0x20u)
#define AVR_GPIO_HIKL_BASE_ADDR	   (AVR_IO_BASE_ADDR + 0x100u)

#define GPIOA_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x20u))
#define GPIOB_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x23u))
#define GPIOC_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x26u))
#define GPIOD_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x29u))

#define GPIOA GPIOA_DEVICE
#define GPIOB GPIOB_DEVICE
#define GPIOC GPIOC_DEVICE
#define GPIOD GPIOD_DEVICE

#define GPIOA_INDEX 0u
#define GPIOB_INDEX 1u
#define GPIOC_INDEX 2u
#define GPIOD_INDEX 3u

#if defined(PORTE)
#define GPIOE_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x2Cu))
#define GPIOE	     GPIOE_DEVICE
#define GPIOE_INDEX  4u
#endif

#if defined(PORTF)
#define GPIOF_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x2Fu))
#define GPIOF	     GPIOF_DEVICE
#define GPIOF_INDEX  5u
#endif

#if defined(PORTG)
#define GPIOG_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x32u))
#define GPIOG	     GPIOG_DEVICE
#define GPIOG_INDEX  6u
#endif

#if defined(PORTH)
#define GPIOH_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x100u))
#define GPIOH	     GPIOH_DEVICE
#define GPIOH_INDEX  7u
#endif

#if defined(PORTJ)
#define GPIOJ_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x103u))
#define GPIOJ	     GPIOJ_DEVICE
#define GPIOJ_INDEX  8u
#endif

#if defined(PORTK)
#define GPIOK_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x106u))
#define GPIOK	     GPIOK_DEVICE
#define GPIOK_INDEX  9u
#endif

#if defined(PORTL)
#define GPIOL_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x109u))
#define GPIOL	     GPIOL_DEVICE
#define GPIOL_INDEX  10u
#endif

/* Get the GPIO device from the port letter index (0: A, 1: B, ...) */
#define GPIO_DEVICE_ABCDEFG(_idx) \
	((GPIO_Device *)(AVR_GPIO_BASE_ADDR + ((_idx) * sizeof(GPIO_Device))))

#if defined(PORTH)
#define GPIO_DEVICE_HIKL(_idx) \
	((GPIO_Device *)(AVR_GPIO_HIKL_BASE_ADDR + ((_idx - 6u) * sizeof(GPIO_Device))))
#define GPIO_DEVICE(_idx) \
	(((_idx) <= 5) ? GPIO_DEVICE_ABCDEFG(_idx) : GPIO_DEVICE_HIKL(_idx))
#else
#define GPIO_DEVICE GPIO_DEVICE_ABCDEFG
#endif

/* Inline API */

static inline void gpiol_init(GPIO_Device *gpio, uint8_t dir_mask, uint8_t pullup_mask)
{
	gpio->DDR  = dir_mask;
	gpio->PORT = pullup_mask;
}

static inline void gpiol_pin_init(GPIO_Device *gpio,
				  uint8_t pin,
				  uint8_t dir,
				  uint8_t pullup)
{
	/* represent either pullup (if input) or output level (if output)*/
	if (pullup == GPIO_INPUT_NO_PULLUP) {
		gpio->PORT = gpio->PORT & ~BIT(pin);
	} else {
		gpio->PORT = gpio->PORT | BIT(pin);
	}

	if (dir == GPIO_INPUT) {
		gpio->DDR = gpio->DDR & ~BIT(pin);
	} else {
		gpio->DDR = gpio->DDR | BIT(pin);
	}
}

static inline void gpiol_pin_set_direction(GPIO_Device *gpio,
					   uint8_t pin,
					   uint8_t direction)
{
	gpio->DDR = (gpio->DDR & ~BIT(pin)) | ((direction & 1u) << pin);
}

static inline uint8_t gpiol_pin_get_direction(GPIO_Device *gpio, uint8_t pin)
{
	return (gpio->DDR >> pin) & 1u;
}

static inline void gpiol_pin_set_pullup(GPIO_Device *gpio, uint8_t pin, uint8_t pullup)
{
	gpio->PORT = (gpio->PORT & ~BIT(pin)) | ((pullup & 1u) << pin);
}

static inline void gpiol_pin_write_state(GPIO_Device *gpio, uint8_t pin, uint8_t state)
{
	gpio->PORT = (gpio->PORT & ~BIT(pin)) | ((state & 1u) << pin);
}

static inline void gpiol_pin_toggle(GPIO_Device *gpio, uint8_t pin)
{
	gpio->PIN = BIT(pin);
}

static inline uint8_t gpiol_pin_read_state(GPIO_Device *gpio, uint8_t pin)
{
	return (gpio->PIN >> pin) & 1u;
}

/* Non-inline API */

void gpio_init(GPIO_Device *gpio, uint8_t dir_mask, uint8_t pullup_mask);

void gpio_pin_init(GPIO_Device *gpio, uint8_t pin, uint8_t dir, uint8_t pullup);

void gpio_pin_set_direction(GPIO_Device *gpio, uint8_t pin, uint8_t direction);

void gpio_pin_set_pullup(GPIO_Device *gpio, uint8_t pin, uint8_t pullup);

void gpio_pin_write_state(GPIO_Device *gpio, uint8_t pin, uint8_t state);

void gpio_pin_toggle(GPIO_Device *gpio, uint8_t pin);

uint8_t gpio_pin_read_state(GPIO_Device *gpio, uint8_t pin);

#if defined(__cplusplus)
}
#endif

#endif /* _GPIO_H_ */