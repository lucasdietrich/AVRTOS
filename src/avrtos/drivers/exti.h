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

#if defined(__AVR_ATmega2560__)
#define EXTI_COUNT	 8u
#elif defined(__AVR_ATmega328P__)
#define EXTI_COUNT 	 2u
#endif

typedef struct {
	__IO uint8_t EICRn[EXTI_COUNT >> 2u];
} EXTI_Ctrl_Device;

#define EXTI_CTRL_DEVICE ((EXTI_Ctrl_Device *)(AVR_IO_BASE_ADDR + 0x69u))

#if defined(__cplusplus)
extern "C" {
#endif

int exti_configure(uint8_t exti, uint8_t isc);

static inline void exti_clear_flag(uint8_t exti) {
	EIFR |= BIT(exti);
}

static inline void exti_enable(uint8_t exti) {
	EIMSK |= BIT(exti);
}

static inline void exti_disable(uint8_t exti) {
	EIMSK &= ~BIT(exti);
}

#if defined(__cplusplus)
}
#endif


#endif /* _AVRTOS_DRIVER_EXTI_H_ */