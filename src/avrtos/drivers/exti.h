/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_EXTI_H_
#define _AVRTOS_DRIVERS_EXTI_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

#define ISCn0 0u
#define ISCn1 1u

#define ISC_LOW_LEVEL 0u
#define ISC_EDGE      1u
#define ISC_FALLING   2u
#define ISC_RISING    3u

#define PCINT_0_7   0u
#define PCINT_8_15  1u
#define PCINT_16_23 2u

#define PCINT_0_7_vect	 PCINT0_vect
#define PCINT_8_15_vect	 PCINT1_vect
#define PCINT_16_23_vect PCINT2_vect

#define GPIO_EXTI_DEV_GROUP_IS_PCINT_0_7(_dev)	 ((_dev) == GPIOB)
#define GPIO_EXTI_DEV_GROUP_IS_PCINT_8_15(_dev)	 ((_dev) == GPIOC)
#define GPIO_EXTI_DEV_GROUP_IS_PCINT_16_23(_dev) ((_dev) == GPIOD)

#define GPIO_PCINT_GROUP(_dev)                                    \
	(GPIO_EXTI_DEV_GROUP_IS_PCINT_0_7(_dev)	    ? PCINT_0_7   \
	 : GPIO_EXTI_DEV_GROUP_IS_PCINT_8_15(_dev)  ? PCINT_8_15  \
	 : GPIO_EXTI_DEV_GROUP_IS_PCINT_16_23(_dev) ? PCINT_16_23 \
						    : 0xFFu)

#if defined(__AVR_ATmega2560__)
#define EXTI_COUNT 8u
#define PCI_COUNT  24u
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
#define EXTI_COUNT 2u
#define PCI_COUNT  24u
#endif

#define PCI_GROUPS_COUNT (PCI_COUNT >> 3u)

typedef struct {
	__IO uint8_t EICRn[EXTI_COUNT >> 2u];
} EXTI_Ctrl_Device;

typedef struct {
	__IO uint8_t PCMSK[PCI_GROUPS_COUNT];
} PCI_Ctrl_Device;

#define EXTI_CTRL_DEVICE ((EXTI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x69u))
#define PCI_CTRL_DEVICE	 ((PCI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x6Bu))

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Configure an external interrupt
 *
 * @param exti EXTI number
 * @param isc
 * @return int 0 on success, -EINVAL if EXTI number is invalid
 *
 * Example for ATmega328P :
 *
 * ```c
 * #include <avr/io.h>
 * #include <avrtos/drivers/exti.h>
 * exti_configure(INT0, ISC_FALLING);
 * exti_clear_flag(INT0);
 * exti_enable(INT0);
 * ```
 */
int exti_configure(uint8_t exti, uint8_t isc);

/**
 * @brief Clear an external interrupt flag
 *
 * @param exti EXTI number
 */
__always_inline void exti_clear_flag(uint8_t exti)
{
	EIFR |= BIT(exti);
}

/**
 * @brief Get an external interrupt flag
 *
 * @param exti EXTI number
 * @return uint8_t interrupt flag value
 */
__always_inline uint8_t exti_get_flag(uint8_t exti)
{
	return (EIFR & BIT(exti)) >> exti;
}

/**
 * @brief Poll an external interrupt flag until it is set
 *
 * @param exti EXTI number
 */
__always_inline void exti_poll_flag(uint8_t exti)
{
	while (!exti_get_flag(exti))
		;
}

/**
 * @brief Enable an external interrupt
 *
 * @param exti EXTI number
 */
__always_inline void exti_enable(uint8_t exti)
{
	EIMSK |= BIT(exti);
}

/**
 * @brief Disable an external interrupt
 *
 * @param exti EXTI number
 */
__always_inline void exti_disable(uint8_t exti)
{
	EIMSK &= ~BIT(exti);
}

/**
 * @brief Configure a pin change interrupt
 *
 * @param pci_group Group number
 * @param mask Mask of pins to enable
 *
 * Example for ATmega328P :
 *
 * ```c
 * #include <avr/io.h>
 * #include <avrtos/drivers/exti.h>
 * pci_configure(PCINT_16_23, BIT(PCINT0) | BIT(PCINT1));
 * pci_clear_flag(PCINT_16_23);
 * pci_enable(PCINT_16_23);
 */
__always_inline void pci_configure(uint8_t pci_group, uint8_t mask)
{
	PCI_CTRL_DEVICE->PCMSK[pci_group] = mask;
}

/**
 * @brief Enable a pin change interrupt on a group line
 *
 * @param group Group number
 * @param line Line number (0-7)
 */
__always_inline void pci_pin_enable_group_line(uint8_t group, uint8_t line)
{
	PCI_CTRL_DEVICE->PCMSK[group] |= BIT(line);
}

/**
 * @brief Disable a pin change interrupt on a group line
 *
 * @param group Group number
 * @param line Line number (0-7)
 */
__always_inline void pci_pin_disable_group_line(uint8_t group, uint8_t line)
{
	PCI_CTRL_DEVICE->PCMSK[group] &= ~BIT(line);
}

/**
 * @brief Enable a pin change interrupt on a pin
 *
 * @param pci Pin change interrupt number
 *
 * Example for ATmega328P :
 * `pci_pin_enable(23)` is equivalent to `pci_pin_enable_group_line(2, 7)` # PCINT23
 */
__always_inline void pci_pin_enable(uint8_t pci)
{
	pci_pin_enable_group_line(pci >> 3u, pci & 0x07u);
}

/**
 * @brief Disable a pin change interrupt on a pin
 *
 * @param pci Pin change interrupt number
 *
 * Example for ATmega328P :
 * `pci_pin_disable(23)` is equivalent to `pci_pin_disable_group_line(2, 7)` # PCINT23
 */
__always_inline void pci_pin_disable(uint8_t pci)
{
	pci_pin_disable_group_line(pci >> 3u, pci & 0x07u);
}

/**
 * @brief Clear a pin change interrupt flag
 *
 * @param group Group number
 *
 * Example for ATmega328P :
 *
 * ```c
 * #include <avr/io.h>
 * #include <avrtos/drivers/exti.h>
 * pci_clear_flag(PCINT_16_23);
 * ```
 */
__always_inline void pci_clear_flag(uint8_t group)
{
	PCIFR |= BIT(group);
}

/**
 * @brief Enable a pin change interrupt
 *
 * @param group Group number
 */
__always_inline void pci_enable(uint8_t group)
{
	PCICR |= BIT(group);
}

/**
 * @brief Disable a pin change interrupt
 *
 * @param group Group number
 */
__always_inline void pci_disable(uint8_t group)
{
	PCICR &= ~BIT(group);
}

#if defined(__cplusplus)
}
#endif

#endif /* _AVRTOS_DRIVERS_EXTI_H_ */