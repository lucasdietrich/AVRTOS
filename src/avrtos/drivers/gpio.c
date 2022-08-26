#include "gpio.h"

void gpio_init(GPIO_Device *gpio, uint8_t mode, uint8_t pullup)
{
	gpio->DDR = (mode == DIR_INPUT) ? 0x00u : 0xFFu;
	gpio->PORT = (pullup == PIN_NO_PULLUP) ? 0x00u : 0xFFu;
	gpio->PIN = 0x00u;
}