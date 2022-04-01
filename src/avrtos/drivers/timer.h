#ifndef _AVRTOS_DRIVER_TIMER_H_
#define _AVRTOS_DRIVER_TIMER_H_

#include <avrtos/avrtos.h>
#include <avrtos/drivers.h>

/* structure change for timer 2 on ATmega328p and for timer 1, 2, 3 on ATmega2560 */
typedef struct {
	__IO uint8_t TCCRnA;
	__IO uint8_t TCCRnB;
	__IO uint8_t TCNTn;
	__IO uint8_t OCRnA;
	__IO uint8_t OCRnB;
} TIMER_Device;
 
// TIMSK0, TIFR0

#endif /* _AVRTOS_DRIVER_TIMER_H_ */