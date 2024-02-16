/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_TIMER_H_
#define _AVRTOS_DRIVERS_TIMER_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

#include "timer_defs.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief 16 bits timers registers structure
 * - Timer 1, 3, 4, 5 on ATmega2560
 * - Timer 1 on ATmega328p
 */
typedef struct {
	/* COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10 */
	__IO uint8_t TCCRnA;
	/* ICNCn ICESn - WGMn3 WGMn2 CSn2 CSn1 CSn0 */
	__IO uint8_t TCCRnB;
	/* FOCnA FOCnB FOCnC */
	__IO uint8_t TCCRnC;
	/* reserved */
	__IO uint8_t _reserved1;

	union {
		struct {
			/* Timer/Counter - Counter Register Low Byte */
			__IO uint8_t TCNTnL;
			/* Timer/Counter - Counter Register High Byte */
			__IO uint8_t TCNTnH;
			/* Timer/Counter - Input Capture Register High Byte */
			__IO uint8_t ICRnL;
			/* Timer/Counter - Input Capture Register High Byte */
			__IO uint8_t ICRnH;
			/* Timer/Counter - Output Compare Register A Low Byte */
			__IO uint8_t OCRnAL;
			/* Timer/Counter - Output Compare Register A High Byte */
			__IO uint8_t OCRnAH;
			/* Timer/Counter - Output Compare Register B Low Byte */
			__IO uint8_t OCRnBL;
			/* Timer/Counter - Output Compare Register B High Byte */
			__IO uint8_t OCRnBH;
			/* Timer/Counter - Output Compare Register C Low Byte */
			__IO uint8_t OCRnCL;
			/* Timer/Counter - Output Compare Register C High Byte */
			__IO uint8_t OCRnCH;
		};
		struct {
			/* Timer/Counter - Counter Register */
			__IO uint16_t TCNTn;
			/* Timer/Counter - Input Capture Register */
			__IO uint16_t IRCN;

			union {
				struct {
					/* Timer/Counter - Output Compare Register A */
					__IO uint16_t OCRnA;
					/* Timer/Counter - Output Compare Register B */
					__IO uint16_t OCRnB;
					/* Timer/Counter - Output Compare Register C */
					__IO uint16_t OCRnC;
				};
				/* Timer/Counter - Output Compare Register */
				__IO uint16_t OCRnx[3];
			};
		};
	};
} TIMER16_Device;

#if defined(TCCR1A)
#define TIMER1_DEVICE_ADDR	   (AVR_IO_BASE_ADDR + 0x80u)
#define TIMER1_DEVICE		   ((TIMER16_Device *)(TIMER1_DEVICE_ADDR))
#define TIMER_DEVICE_IS_1(dev) ((void *)dev == (void *)TIMER1_DEVICE)
#define TIMER1_INDEX		   1u
#endif

#if defined(TCCR3A)
#define TIMER3_DEVICE_ADDR	   (AVR_IO_BASE_ADDR + 0x90u)
#define TIMER3_DEVICE		   ((TIMER16_Device *)(TIMER3_DEVICE_ADDR))
#define TIMER_DEVICE_IS_3(dev) ((void *)dev == (void *)TIMER3_DEVICE)
#define TIMER3_INDEX		   3u
#endif

#if defined(TCCR4A)
#define TIMER4_DEVICE_ADDR	   (AVR_IO_BASE_ADDR + 0xA0u)
#define TIMER4_DEVICE		   ((TIMER16_Device *)(TIMER4_DEVICE_ADDR))
#define TIMER_DEVICE_IS_4(dev) ((void *)dev == (void *)TIMER4_DEVICE)
#define TIMER4_INDEX		   4u
#endif

#if defined(TCCR5A)
#define TIMER5_DEVICE_ADDR	   (AVR_IO_BASE_ADDR + 0x120u)
#define TIMER5_DEVICE		   ((TIMER16_Device *)(TIMER5_DEVICE_ADDR))
#define TIMER_DEVICE_IS_5(dev) ((void *)dev == (void *)TIMER5_DEVICE)
#define TIMER5_INDEX		   5u
#endif

/**
 * @brief 8 bits timer registers structure
 * - Timer 1 and 2 on ATmega328p and ATmega2560
 */
typedef struct {
	__IO uint8_t TCCRnA; /* COMnA1 COMnA0 COMnB1 COMnB0 – – WGMn1 WGMn0 */
	__IO uint8_t TCCRnB; /* FOCnA FOCnB – – WGMn2 CSn2 CSn1 CSn0 */
	__IO uint8_t TCNTn;	 /* Timer/Counter (8-bit) */
	__IO uint8_t OCRnA;	 /* Timer/Counter Output Compare Register A */
	__IO uint8_t OCRnB;	 /* Timer/Counter Output Compare Register B */
} TIMER8_Device;

#if defined(TCCR0A)
#define TIMER0_DEVICE_ADDR	   (AVR_IO_BASE_ADDR + 0x44U)
#define TIMER0_DEVICE		   ((TIMER8_Device *)(TIMER0_DEVICE_ADDR))
#define TIMER_DEVICE_IS_0(dev) ((void *)dev == (void *)TIMER0_DEVICE)
#define TIMER0_INDEX		   0u
#endif

#if defined(TCCR2A)
#define TIMER2_DEVICE_ADDR	   (AVR_IO_BASE_ADDR + 0xB0U)
#define TIMER2_DEVICE		   ((TIMER8_Device *)(TIMER2_DEVICE_ADDR))
#define TIMER_DEVICE_IS_2(dev) ((void *)dev == (void *)TIMER2_DEVICE)
#define TIMER2_INDEX		   2u
#endif

/**
 * @brief Timer/Counter Interrupt Flag Register
 * 	- TIFR0 to TIFR5 on ATmega2560
 * 	- TIFR0 to TIFR3 on ATmega328P
 */
#define TIFRn ((__IO uint8_t *)(AVR_IO_BASE_ADDR + 0x35U))

/**
 * @brief Timer/Counter Interrupt Mask Register
 * 	- TIMSK0 to TIMSK5 on ATmega2560
 * 	- TIMSK0 to TIMSK3 on ATmega328P
 */
#define TIMSKn ((__IO uint8_t *)(AVR_IO_BASE_ADDR + 0x6EU))

#define TIMER_TIMSK_SET_OCIEA(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(OCIEnA))
#define TIMER_TIMSK_SET_OCIEB(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(OCIEnB))
#define TIMER_TIMSK_SET_OCIEC(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(OCIEnC))
#define TIMER_TIMSK_SET_TOIE(tim_idx)  SET_BIT(TIMSKn[tim_idx], BIT(TOIEn))
#define TIMER_TIMSK_SET_ICIE(tim_idx)  SET_BIT(TIMSKn[tim_idx], BIT(ICIEn))

#define TIMER_TIMSK_CLEAR_OCIEA(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(OCIEnA))
#define TIMER_TIMSK_CLEAR_OCIEB(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(OCIEnB))
#define TIMER_TIMSK_CLEAR_OCIEC(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(OCIEnC))
#define TIMER_TIMSK_CLEAR_TOIE(tim_idx)	 CLR_BIT(TIMSKn[tim_idx], BIT(TOIEn))
#define TIMER_TIMSK_CLEAR_ICIE(tim_idx)	 CLR_BIT(TIMSKn[tim_idx], BIT(ICIEn))

#define TIMER_TIMSK_READ_OCIEA(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(OCIEnA))
#define TIMER_TIMSK_READ_OCIEB(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(OCIEnB))
#define TIMER_TIMSK_READ_OCIEC(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(OCIEnC))
#define TIMER_TIMSK_READ_TOIE(tim_idx)	READ_BIT(TIMSKn[tim_idx], BIT(TOIEn))
#define TIMER_TIMSK_READ_ICIE(tim_idx)	READ_BIT(TIMSKn[tim_idx], BIT(ICIEn))

#define TIMER_INDEX_EXISTS(tim_idx) ((tim_idx) < TIMERS_COUNT)

#define TIMER_INDEX_IS_8BIT(idx)  ((idx == 0) || (idx == 2))
#define TIMER_INDEX_IS_16BIT(idx) ((idx == 1) || (idx == 3) || (idx == 4) || (idx == 5))

#define TIMER_MAX_COUNTER_8BIT	0xFFu
#define TIMER_MAX_COUNTER_16BIT 0xFFFFu

#define TIMER_GET_MAX_COUNTER(tim_idx)                                                   \
	(TIMER_INDEX_IS_8BIT(tim_idx) ? TIMER_MAX_COUNTER_8BIT : TIMER_MAX_COUNTER_16BIT)
#define TIMER_CALC_COUNTER_VALUE(period_us, prescaler)                                   \
	((((F_CPU / 1000000lu) * period_us) / prescaler - 1lu))
#define TIMER_COUNTER_VALUE_FIT(period_us, prescaler, max)                               \
	(TIMER_CALC_COUNTER_VALUE(period_us, prescaler) <= max)

#define TIMER_DEVICE_IS_0(dev) ((void *)dev == (void *)TIMER0_DEVICE)
#define TIMER_DEVICE_IS_1(dev) ((void *)dev == (void *)TIMER1_DEVICE)
#define TIMER_DEVICE_IS_2(dev) ((void *)dev == (void *)TIMER2_DEVICE)

#define TIMER_DEVICE_IS_16BITS(dev)                                                      \
	(TIMER_DEVICE_IS_1(dev) || TIMER_DEVICE_IS_3(dev) || TIMER_DEVICE_IS_4(dev) ||       \
	 TIMER_DEVICE_IS_5(dev))

#define TIMER_DEVICE_IS_8BITS(dev) (TIMER_DEVICE_IS_0(dev) || TIMER_DEVICE_IS_2(dev))

#define TIMER_INDEX_IS_8BITS(idx)  ((idx == 0) || (idx == 2))
#define TIMER_INDEX_IS_16BITS(idx) ((idx == 1) || (idx == 3) || (idx == 4) || (idx == 5))

typedef enum {
	TIMER_MODE_NORMAL = 0x00, /* 8bit timer */
	TIMER_MODE_PWM_PC_8bit,
	TIMER_MODE_PWM_PC_9bit, /* 8bit timer */
	TIMER_MODE_PWM_PC_10bit,
	TIMER_MODE_CTC, /* OCR1A - timer */
	TIMER_MODE_FAST_PWM_8bit,
	TIMER_MODE_FAST_PWM_9bit, /* 8bit timer */
	TIMER_MODE_FAST_PWM_10bit,
	TIMER_MODE_PWM_PHASE_FREQUENCY_CORRECT_ICRn,
	TIMER_MODE_PWM_PHASE_FREQUENCY_CORRECT_OCRnA, /* 8bit timer */
	TIMER_MODE_PWM_PHASE_CORRECT_ICRn,
	TIMER_MODE_PWM_PHASE_CORRECT_OCRnA,
	TIMER_MODE_CTC_ICRn,
	TIMER_MODE_RESERVED,
	TIMER_MODE_FAST_PWM_ICR1,
	TIMER_MODE_FAST_PWM_OCR1A, /* 8bit timer */
} timer_mode_t;

/* 8 bits */
#define TIMER_MODE_PWM_PC	   TIMER_MODE_PWM_PC_9bit
#define TIMER_MODE_FAST_PWM_PC TIMER_MODE_FAST_PWM_9bit

/* prescalers for timer 2 only */
typedef enum {
	TIMER2_STOPPED		  = 0x00,
	TIMER2_PRESCALER_1	  = 0x01,
	TIMER2_PRESCALER_8	  = 0x02,
	TIMER2_PRESCALER_32	  = 0x03,
	TIMER2_PRESCALER_64	  = 0x04,
	TIMER2_PRESCALER_128  = 0x05,
	TIMER2_PRESCALER_256  = 0x06,
	TIMER2_PRESCALER_1024 = 0x07,
} timer2_prescaler_t;

/* prescalers for timer 0, 1, 3, 4, 5 */
typedef enum {
	TIMER_STOPPED		 = 0x00,
	TIMER_PRESCALER_1	 = 0x01,
	TIMER_PRESCALER_8	 = 0x02,
	TIMER_PRESCALER_64	 = 0x03,
	TIMER_PRESCALER_256	 = 0x04,
	TIMER_PRESCALER_1024 = 0x05,

	/* unsupported for now */
	TIMER_EXT_FALLING = 0x06,
	TIMER_EXT_RISING  = 0x07,
} timer_prescaler_t;

typedef enum {
	TIMER16_OVERFLOW_INTERRUPT				 = 0u,
	TIMER16_OUTPUT_COMPARE_MATCH_A_INTERRUPT = 1u,
	TIMER16_OUTPUT_COMPARE_MATCH_B_INTERRUPT = 2u,
	TIMER16_OUTPUT_COMPARE_MATCH_C_INTERRUPT = 3u,
	TIMER16_INPUT_CAPTURE_INTERRUPT			 = 5u,
} timer16_interrupt_t;

/* Timer Channel Compare Output Mode */
typedef enum {
	/* Normal port operation, OCnA/OCnB/OCnC disconnected */
	TIMER_CHANNEL_COMP_MODE_NORMAL = 0u,
	/* Toggle OCnA/OCnB/OCnC on compare match */
	TIMER_CHANNEL_COMP_MODE_TOGGLE = 1u,
	/* Clear OCnA/OCnB/OCnC on compare match (set output to low  level) */
	TIMER_CHANNEL_COMP_MODE_CLEAR = 2u,
	/* Set OCnA/OCnB/OCnC on compare match (set output to high level */
	TIMER_CHANNEL_COMP_MODE_SET = 3u,
} timer_channel_com_t;

typedef enum {
	TIMER_CHANNEL_A = 0u,
	TIMER_CHANNEL_B = 1u,
	TIMER_CHANNEL_C = 2u,
} timer_channel_t;

struct timer_channel_compare_config {
	timer_channel_com_t mode;
	uint16_t value;
};

struct timer_config {
	/**
	 * @brief Timer mode
	 */
	timer_mode_t mode : 4;

	/**
	 * @brief Timer prescaler (can be timer_prescaler_t or
	 * timer2_prescaler_t)
	 */
	uint8_t prescaler : 3;

	/**
	 * @brief Timer counter value
	 *
	 * Note: Cast to uint8_t for 8 bits timers
	 */
	uint16_t counter;

	/**
	 * @brief Enable interrupts handlers
	 */
	uint8_t timsk; // TOIEn, OCIEnA, OCIEnB, OCIEnC, ICIEn
};

/**
 * @brief Get the timer index from the device address
 *
 * This function is declared __always_inline, because it can be mostly optimized
 * to a constant value by the compiler when called on a constant timer device.
 *
 * Example: "timer_get_index(TIMER4_DEVICE)" will give "4" at compile time.
 *
 * Note: Do not use this function to get the timer index dynamically.
 *
 * @param dev
 * @return int
 */
__always_inline int8_t timer_get_index(void *dev)
{
	switch ((uint16_t)dev) {
#if defined(TIMER0_DEVICE)
	case TIMER0_DEVICE_ADDR:
		return 0;
#endif
#if defined(TIMER1_DEVICE)
	case TIMER1_DEVICE_ADDR:
		return 1;
#endif
#if defined(TIMER2_DEVICE)
	case TIMER2_DEVICE_ADDR:
		return 2;
#endif
#if defined(TIMER3_DEVICE)
	case TIMER3_DEVICE_ADDR:
		return 3;
#endif
#if defined(TIMER4_DEVICE)
	case TIMER4_DEVICE_ADDR:
		return 4;
#endif
#if defined(TIMER5_DEVICE)
	case TIMER5_DEVICE_ADDR:
		return 5;
#endif
	default:
		return -EBADF;
	}
}

/**
 * @brief Get the timer device address from its index
 *
 * This function is declared __always_inline, because it can be mostly optimized
 * to a constant value by the compiler when called on a constant timer device.
 *
 * Example: "timer_get_device(4)" will give the value of TIMER4_DEVICE at
 * compile time.
 *
 * Note: Do not use this function to get the timer device address dynamically.
 *
 * @param dev
 * @return int
 */
__always_inline void *timer_get_device(uint8_t idx)
{
	switch (idx) {
#if defined(TIMER0_DEVICE)
	case 0:
		return TIMER0_DEVICE;
#endif
#if defined(TIMER1_DEVICE)
	case 1:
		return TIMER1_DEVICE;
#endif
#if defined(TIMER2_DEVICE)
	case 2:
		return TIMER2_DEVICE;
#endif
#if defined(TIMER3_DEVICE)
	case 3:
		return TIMER3_DEVICE;
#endif
#if defined(TIMER4_DEVICE)
	case 4:
		return TIMER4_DEVICE;
#endif
#if defined(TIMER5_DEVICE)
	case 5:
		return TIMER5_DEVICE;
#endif
	default:
		return NULL;
	}
}

__always_inline void ll_timer_clear_enable_int_mask(uint8_t tim_idx)
{
	TIMSKn[tim_idx] = 0U;
}

__always_inline void ll_timer_set_enable_int_mask(uint8_t tim_idx, uint8_t mask)
{
	TIMSKn[tim_idx] = mask;
}

__always_inline void ll_timer_clear_irq_flags(uint8_t tim_idx)
{
	/* For example : OCF1A:
	 * OCFnA is automatically cleared when the Output Compare Match A
	 * Interrupt Vector is executed. Alternatively, OCFnA can be cleared by
	 * writing a logic one to its bit location.
	 */
	TIFRn[tim_idx] = 0xFFU;
}

__always_inline uint8_t ll_timer_get_irq_flags(uint8_t tim_idx)
{
	return TIFRn[tim_idx];
}

__always_inline void ll_timer8_stop(TIMER8_Device *dev)
{
	/* timer stops counting at the timer prescaler is set to zero */
	dev->TCCRnB &= ~(BIT(CSn0) | BIT(CSn1) | BIT(CSn2));
}

__always_inline void ll_timer8_counter_reset(TIMER8_Device *dev)
{
	dev->TCNTn = 0x00U;
}

__always_inline void ll_timer8_start(TIMER8_Device *dev, uint8_t prescaler)
{
	/* timer starts counting at the timer prescaler is set*/
	dev->TCCRnB |= prescaler << CSn0;
}

__always_inline void ll_timer16_write_reg16(__IO uint16_t *reg, uint16_t val)
{
	/**
	 * To do a 16-bit write, the high byte must be written before the low
	 * byte. For a 16-bit read, the low byte must be read before the high
	 * byte.
	 */
	*((__IO uint8_t *)reg + 1u) = val >> 8u;
	*((__IO uint8_t *)reg)		= val & 0xffu;
}

__always_inline void ll_timer16_set_tcnt(TIMER16_Device *dev, uint16_t val)
{
	/**
	 * To do a 16-bit write, the high byte must be written before the low
	 * byte. For a 16-bit read, the low byte must be read before the high
	 * byte.
	 */
	dev->TCNTnH = val >> 8;
	dev->TCNTnL = val & 0xffU;
}

__always_inline void ll_timer16_stop(TIMER16_Device *dev)
{
	/* timer stops counting at the timer prescaler is set to zero */
	dev->TCCRnB &= ~(BIT(CSn0) | BIT(CSn1) | BIT(CSn2));
}

__always_inline void ll_timer16_counter_reset(TIMER16_Device *dev)
{
	ll_timer16_set_tcnt(dev, 0x0000U);
}

__always_inline void ll_timer16_start(TIMER16_Device *dev, uint8_t prescaler)
{
	/* timer starts counting at the timer prescaler is set*/
	dev->TCCRnB |= prescaler < CSn0;
}

__always_inline void ll_timer_stop(void *dev)
{
	/* as TCCRnB is at the same address in the timer regisiters, we
	 * can use the same function to stop all timers types */
	ll_timer8_stop((TIMER8_Device *)dev);
}

__always_inline void ll_timer_start(void *dev, uint8_t prescaler)
{
	/* see ll_timer_stop */
	ll_timer8_start((TIMER8_Device *)dev, prescaler);
}

__always_inline void ll_timer16_enable_interrupt(uint8_t tim_idx, timer16_interrupt_t n)
{
	TIMSKn[tim_idx] |= BIT(n);
}

__always_inline void ll_timer16_disable_interrupt(uint8_t tim_idx, timer16_interrupt_t n)
{
	TIMSKn[tim_idx] &= ~BIT(n);
}

void ll_timer8_init(TIMER8_Device *dev,
					uint8_t tim_idx,
					const struct timer_config *config);

void ll_timer16_init(TIMER16_Device *dev,
					 uint8_t tim_idx,
					 const struct timer_config *config);

void ll_timer16_channel_configure(TIMER16_Device *dev,
								  timer_channel_t channel,
								  const struct timer_channel_compare_config *config);

void ll_timer8_deinit(TIMER8_Device *dev, uint8_t tim_idx);

void ll_timer16_deinit(TIMER16_Device *dev, uint8_t tim_idx);

int8_t timer_calc_prescaler(uint8_t timer_index, uint32_t period_us, uint16_t *counter);

int8_t timer8_init(TIMER8_Device *dev, const struct timer_config *config);

int8_t timer16_init(TIMER16_Device *dev, const struct timer_config *config);

int8_t timer8_deinit(TIMER8_Device *dev);

int8_t timer16_deinit(TIMER16_Device *dev);

#define TIMER_API_FLAG_AUTOSTART (1 << 0)

typedef void (*timer_callback_t)(void *dev, uint8_t tim_idx, void *user_data);

/* High level API */
int8_t timer_init(uint8_t tim_idx,
				  uint32_t period_us,
				  timer_callback_t cb,
				  void *user_data,
				  uint8_t flags);

void timer_start(uint8_t tim_idx);

void timer_stop(uint8_t tim_idx);

uint8_t timer_get_prescaler(uint8_t tim_idx);

uint32_t timer_get_max_period_us(uint8_t tim_idx);

#if defined(__cplusplus)
}
#endif

#endif /* _AVRTOS_DRIVERS_TIMER_H_ */