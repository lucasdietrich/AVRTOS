/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_EXTI_H_
#define _AVRTOS_DRIVERS_EXTI_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

/**
 * @brief Definitions for External Interrupt (EXTI) and Pin Change Interrupt (PCI) 
 * configuration and handling.
 * 
 * This header provides macros, inline functions, and control structures for 
 * configuring external interrupts and pin change interrupts on AVR microcontrollers.
 */

/* EXTI control definitions */
#define ISCn0 0u
#define ISCn1 1u

/* EXTI Interrupt Sense Control modes */
#define ISC_LOW_LEVEL 0u /**< Low level trigger */
#define ISC_EDGE      1u /**< Any edge trigger */
#define ISC_FALLING   2u /**< Falling edge trigger */
#define ISC_RISING    3u /**< Rising edge trigger */

/* Pin Change Interrupt (PCI) groups */
#define PCINT_0_7    0u  /**< Group for PCINT0 - PCINT7 */
#define PCINT_8_15   1u  /**< Group for PCINT8 - PCINT15 */
#define PCINT_16_23  2u  /**< Group for PCINT16 - PCINT23 */

/* Pin Change Interrupt vector mappings */
#define PCINT_0_7_vect   PCINT0_vect
#define PCINT_8_15_vect  PCINT1_vect
#define PCINT_16_23_vect PCINT2_vect

/* GPIO to PCI group association macros */
#define GPIO_EXTI_DEV_GROUP_IS_PCINT_0_7(_dev)    ((_dev) == GPIOB)
#define GPIO_EXTI_DEV_GROUP_IS_PCINT_8_15(_dev)   ((_dev) == GPIOC)
#define GPIO_EXTI_DEV_GROUP_IS_PCINT_16_23(_dev)  ((_dev) == GPIOD)

/* Determine the PCI group for a given GPIO device */
#define GPIO_PCINT_GROUP(_dev)                                                         \
    (GPIO_EXTI_DEV_GROUP_IS_PCINT_0_7(_dev)   ? PCINT_0_7 :                             \
     GPIO_EXTI_DEV_GROUP_IS_PCINT_8_15(_dev)  ? PCINT_8_15 :                            \
     GPIO_EXTI_DEV_GROUP_IS_PCINT_16_23(_dev) ? PCINT_16_23 : 0xFFu)

/* Definitions for supported devices */
#if defined(__AVR_ATmega2560__)
#define EXTI_COUNT 8u
#define PCI_COUNT  24u
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
#define EXTI_COUNT 2u
#define PCI_COUNT  24u
#endif

#define PCI_GROUPS_COUNT (PCI_COUNT >> 3u)

/**
 * @brief Structure for controlling external interrupts.
 */
typedef struct {
    __IO uint8_t EICRn[EXTI_COUNT >> 2u]; /**< External Interrupt Control Registers */
} EXTI_Ctrl_Device;

/**
 * @brief Structure for controlling pin change interrupts.
 */
typedef struct {
    __IO uint8_t PCMSK[PCI_GROUPS_COUNT]; /**< Pin Change Mask Registers */
} PCI_Ctrl_Device;

/* Base addresses for the control structures */
#define EXTI_CTRL_DEVICE ((EXTI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x69u))
#define PCI_CTRL_DEVICE  ((PCI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x6Bu))

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Configure an external interrupt.
 * 
 * Configures an external interrupt with the specified interrupt sense control (ISC) mode.
 *
 * @param exti EXTI number (e.g., INT0 or INT1 for ATmega328P).
 * @param isc Interrupt sense control mode (e.g., ISC_FALLING, ISC_RISING).
 * @return int8_t 0 on success, -EINVAL if EXTI number is invalid.
 * 
 * Example for ATmega328P:
 * 
 * ```c
 * #include <avr/io.h>
 * #include <avrtos/drivers/exti.h>
 * exti_configure(INT0, ISC_FALLING);
 * exti_clear_flag(INT0);
 * exti_enable(INT0);
 * ```
 */
int8_t exti_configure(uint8_t exti, uint8_t isc);

/**
 * @brief Clear an external interrupt flag.
 *
 * Clears the interrupt flag for the specified EXTI.
 *
 * @param exti EXTI number.
 */
__always_inline void exti_clear_flag(uint8_t exti)
{
    EIFR |= BIT(exti);
}

/**
 * @brief Get the status of an external interrupt flag.
 *
 * @param exti EXTI number.
 * @return uint8_t 1 if the interrupt flag is set, 0 otherwise.
 */
__always_inline uint8_t exti_get_flag(uint8_t exti)
{
    return (EIFR & BIT(exti)) >> exti;
}

/**
 * @brief Poll the external interrupt flag until it is set.
 *
 * @param exti EXTI number.
 */
__always_inline void exti_poll_flag(uint8_t exti)
{
    while (!exti_get_flag(exti))
        ;
}

/**
 * @brief Enable an external interrupt.
 *
 * Enables the specified EXTI.
 *
 * @param exti EXTI number.
 */
__always_inline void exti_enable(uint8_t exti)
{
    EIMSK |= BIT(exti);
}

/**
 * @brief Disable an external interrupt.
 *
 * Disables the specified EXTI.
 *
 * @param exti EXTI number.
 */
__always_inline void exti_disable(uint8_t exti)
{
    EIMSK &= ~BIT(exti);
}

/**
 * @brief Configure a pin change interrupt for a specific group.
 * 
 * Configures the pin change interrupt for the specified group by setting the mask.
 * 
 * @param pci_group Pin change interrupt group number (e.g., PCINT_16_23).
 * @param mask Bitmask of pins to enable for interrupts.
 * 
 * ```c
 * #include <avr/io.h>
 * #include <avrtos/drivers/exti.h>
 * pci_configure(PCINT_16_23, BIT(PCINT0) | BIT(PCINT1));
 * pci_clear_flag(PCINT_16_23);
 * pci_enable(PCINT_16_23);
 * ```
 */
__always_inline void pci_configure(uint8_t pci_group, uint8_t mask)
{
    PCI_CTRL_DEVICE->PCMSK[pci_group] = mask;
}

/**
 * @brief Enable a pin change interrupt for a specific line in a group.
 *
 * @param group PCI group number.
 * @param line Line number (0-7) within the group.
 */
__always_inline void pci_pin_enable_group_line(uint8_t group, uint8_t line)
{
    PCI_CTRL_DEVICE->PCMSK[group] |= BIT(line);
}

/**
 * @brief Disable a pin change interrupt for a specific line in a group.
 *
 * @param group PCI group number.
 * @param line Line number (0-7) within the group.
 */
__always_inline void pci_pin_disable_group_line(uint8_t group, uint8_t line)
{
    PCI_CTRL_DEVICE->PCMSK[group] &= ~BIT(line);
}

/**
 * @brief Enable a pin change interrupt for a specific pin.
 *
 * Enables the pin change interrupt for the specified PCI pin.
 * 
 * Example for ATmega328P:
 * 
 * `pci_pin_enable(23)` is equivalent to `pci_pin_enable_group_line(2, 7)` for PCINT23.
 * 
 * @param pci Pin change interrupt number.
 */
__always_inline void pci_pin_enable(uint8_t pci)
{
    pci_pin_enable_group_line(pci >> 3u, pci & 0x07u);
}

/**
 * @brief Disable a pin change interrupt for a specific pin.
 *
 * Disables the pin change interrupt for the specified PCI pin.
 * 
 * Example for ATmega328P:
 * 
 * `pci_pin_disable(23)` is equivalent to `pci_pin_disable_group_line(2, 7)` for PCINT23.
 * 
 * @param pci Pin change interrupt number.
 */
__always_inline void pci_pin_disable(uint8_t pci)
{
    pci_pin_disable_group_line(pci >> 3u, pci & 0x07u);
}

/**
 * @brief Clear a pin change interrupt flag for a specific group.
 * 
 * Clears the interrupt flag for the specified PCI group.
 * 
 * Example for ATmega328P:
 * 
 * ```c
 * #include <avr/io.h>
 * #include <avrtos/drivers/exti.h>
 * pci_clear_flag(PCINT_16_23);
 * ```
 * 
 * @param group PCI group number.
 */
__always_inline void pci_clear_flag(uint8_t group)
{
    PCIFR |= BIT(group);
}

/**
 * @brief Enable a pin change interrupt for a specific group.
 * 
 * @param group PCI group number.
 */
__always_inline void pci_enable(uint8_t group)
{
    PCICR |= BIT(group);
}

/**
 * @brief Disable a pin change interrupt for a specific group.
 * 
 * @param group PCI group number.
 */
__always_inline void pci_disable(uint8_t group)
{
    PCICR &= ~BIT(group);
}

#if defined(__cplusplus)
}
#endif

#endif /* _AVRTOS_DRIVERS_EXTI_H_ */
