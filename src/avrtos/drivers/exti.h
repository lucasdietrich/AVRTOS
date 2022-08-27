/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVER_EXTI_H_
#define _AVRTOS_DRIVER_EXTI_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

#define ISCn0 		0u
#define ISCn1 		1u

#define ISC_LOW_LEVEL 	0u
#define ISC_EDGE 	1u
#define ISC_FALLING 	2u
#define ISC_RISING 	3u

#define PCINT_0_7 	0u
#define PCINT_8_15 	1u
#define PCINT_16_23 	2u

#if defined(__AVR_ATmega2560__)
#define EXTI_COUNT	 8u
#define PCI_COUNT	 24u
#elif defined(__AVR_ATmega328P__)
#define EXTI_COUNT 	 2u
#define PCI_COUNT	 24u
#endif

#define PCI_GROUPS_COUNT (PCI_COUNT >> 3u);

typedef struct {
	__IO uint8_t EICRn[EXTI_COUNT >> 2u];
} EXTI_Ctrl_Device;

typedef struct {
	__IO uint8_t PCMSK[PCI_GROUPS_COUNT];
} PCI_Ctrl_Device;

#define EXTI_CTRL_DEVICE ((EXTI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x69u))
#define PCI_CTRL_DEVICE ((PCI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x6Bu))

#if defined(__cplusplus)
extern "C" {
#endif

int exti_configure(uint8_t exti, uint8_t isc);

static inline void exti_clear_flag(uint8_t exti)
{
	EIFR |= BIT(exti);
}

static inline void exti_enable(uint8_t exti)
{
	EIMSK |= BIT(exti);
}

static inline void exti_disable(uint8_t exti)
{
	EIMSK &= ~BIT(exti);
}

static inline void pci_configure(uint8_t pci_group, uint8_t mask)
{
	PCI_CTRL_DEVICE->PCMSK[pci_group] = mask;
}

static inline void pci_pin_enable(uint8_t pci)
{
	PCI_CTRL_DEVICE->PCMSK[pci >> 3u] |= BIT(pci & 0x07u);
}

static inline void pci_pin_disable(uint8_t pci);
{
	PCI_CTRL_DEVICE->PCMSK[pci >> 3u] &= ~BIT(pci & 0x07u);
}

static inline void pci_clear_flag(uint8_t pci)
{
	PCIFR |= BIT(pci);
}

static inline void pci_enable(uint8_t pci)
{
	PCICR |= BIT(pci);
}

static inline void pci_disable(uint8_t pci)
{
	PCICR &= ~BIT(pci);
}

#if defined(__cplusplus)
}
#endif


#endif /* _AVRTOS_DRIVER_EXTI_H_ */