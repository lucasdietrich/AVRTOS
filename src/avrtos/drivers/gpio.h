#ifndef _AVRTOS_DRIVER_GPIO_H_
#define _AVRTOS_DRIVER_GPIO_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

typedef struct {
	__IO uint8_t PIN;
	__IO uint8_t DDR;
	__IO uint8_t PORT;
} GPIO_Device;

#define INPUT 0u
#define OUTPUT 1u

#define INPUT_NO_PULLUP 0u
#define INPUT_PULLUP 1u

#define OUTPUT_DRIVEN_LOW 0u
#define OUTPUT_DRIVEN_HIGH 1u

#define PIN_LOW 0u
#define PIN_HIGH 1u

#define PINn0 PINA0
#define PINn1 PINA1
#define PINn2 PINA2
#define PINn3 PINA3
#define PINn4 PINA4
#define PINn5 PINA5
#define PINn6 PINA6
#define PINn7 PINA7

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

void gpio_init(GPIO_Device *gpio, uint8_t mode, uint8_t pullup);

static inline void gpio_set_pin_direction(GPIO_Device *gpio, uint8_t pin, uint8_t direction) {
	gpio->DDR = (gpio->DDR & ~BIT(pin)) | (direction << pin);
}

static inline void gpio_set_pin_pullup(GPIO_Device *gpio, uint8_t pin, uint8_t pullup) {
	gpio->PORT = (gpio->PORT & ~BIT(pin)) | (pullup << pin);
}

static inline void gpio_set_pin_output_state(GPIO_Device *gpio, uint8_t pin, uint8_t state) {
	gpio->PORT = (gpio->PORT & ~BIT(pin)) | (state << pin);
}

static inline void gpio_toggle_pin(GPIO_Device *gpio, uint8_t pin) {
	gpio->PIN = BIT(pin);
}


#define GPIOA_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x20u))
#define GPIOB_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x23u))
#define GPIOC_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x26u))
#define GPIOD_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x29u))

#if defined(__AVR_ATmega2560__)
#define GPIOE_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x2Cu))
#define GPIOF_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x2Fu))
#define GPIOG_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x100u))
#define GPIOH_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x103u))
#define GPIOJ_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x106u))
#define GPIOL_DEVICE ((GPIO_Device *)(AVR_IO_BASE_ADDR + 0x109u))
#endif /* __AVR_ATmega2560__ */

#endif /* _GPIO_H_ */