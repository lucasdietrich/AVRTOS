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


// EICRA
// EICRB
// EIMSK
// EIFR

/* TODO Attempt for ATMmega2560 */
// typedef struct {
// 	__IO uint8_t PCICR;
// 	__IO uint8_t EICRA;
// 	__IO uint8_t EICRB;
// 	__IO uint8_t PCMSK0;
// 	__IO uint8_t PCMSK1;
// 	__IO uint8_t PCMSK2;
// } EXTI_Ctrl_Device;

// typedef struct {
// 	__IO uint8_t PCIFR;
// 	__IO uint8_t EIFR;
// } EXTI_Flag_Device;

// typedef struct {
// 	__IO uint8_t EIMSK;
// } EXTI_Ctrl2_Device;

#endif /* _AVRTOS_DRIVER_EXTI_H_ */