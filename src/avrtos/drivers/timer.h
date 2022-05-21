#ifndef _AVRTOS_DRIVER_TIMER_H_
#define _AVRTOS_DRIVER_TIMER_H_

#include <avrtos/avrtos.h>
#include <avrtos/drivers.h>

/**
 * @brief 16 bits timers registers structure
 * - Timer 1, 3, 4, 5 on ATmega2560
 * - Timer 1 on ATmega328p
 */
typedef struct  {
	__IO uint8_t TCCRnA; /* COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10 */
	__IO uint8_t TCCRnB; /* ICNCn ICESn - WGMn3 WGMn2 CSn2 CSn1 CSn0 */
	__IO uint8_t TCCRnC; /* FOCnA FOCnB FOCnC */
	__IO uint8_t _reserved1; /* reserved */

	union {
		struct {
			__IO uint8_t TCNTnL; /* Timer/Counter - Counter Register Low Byte */
			__IO uint8_t TCNTnH; /* Timer/Counter - Counter Register High Byte */
			__IO uint8_t ICRnL;  /* Timer/Counter - Input Capture Register High Byte */
			__IO uint8_t ICRnH;  /* Timer/Counter - Input Capture Register High Byte */
			__IO uint8_t OCRnAL; /* Timer/Counter - Output Compare Register A Low Byte */
			__IO uint8_t OCRnAH; /* Timer/Counter - Output Compare Register A High Byte */
			__IO uint8_t OCRnBL; /* Timer/Counter - Output Compare Register B Low Byte */
			__IO uint8_t OCRnBH; /* Timer/Counter - Output Compare Register B High Byte */
			__IO uint8_t OCRnCL; /* Timer/Counter - Output Compare Register C Low Byte */
			__IO uint8_t OCRnCH; /* Timer/Counter - Output Compare Register C High Byte */
		};
		struct {
			__IO uint16_t TCNTn; /* Timer/Counter - Counter Register */
			__IO uint16_t IRCN; /* Timer/Counter - Input Capture Register */
			__IO uint16_t OCRnA; /* Timer/Counter - Output Compare Register A */
			__IO uint16_t OCRnB; /* Timer/Counter - Output Compare Register B */
			__IO uint16_t OCRnC; /* Timer/Counter - Output Compare Register C */
		};
	};
} TIMER16_Device;

#define TIMER1_DEVICE ((TIMER16_Device *)(AVR_IO_BASE_ADDR + 0x80U))

#if defined(__AVR_ATmega2560__)
#define TIMER3_DEVICE ((TIMER16_Device *)(AVR_IO_BASE_ADDR + 0x90U))
#define TIMER4_DEVICE ((TIMER16_Device *)(AVR_IO_BASE_ADDR + 0xA0U))
#define TIMER5_DEVICE ((TIMER16_Device *)(AVR_IO_BASE_ADDR + 0x120U))
#endif

/**
 * @brief 8 bits timer registers structure
 * - Timer 1 and 2 on ATmega328p and ATmega2560
 */
typedef struct {
	__IO uint8_t TCCRnA; /* COMnA1 COMnA0 COMnB1 COMnB0 – – WGMn1 WGMn0 */
	__IO uint8_t TCCRnB; /* FOCnA FOCnB – – WGMn2 CSn2 CSn1 CSn0 */
	__IO uint8_t TCNTn; /* Timer/Counter (8-bit) */
	__IO uint8_t OCRnA; /* Timer/Counter Output Compare Register A */
	__IO uint8_t OCRnB; /* Timer/Counter Output Compare Register B */
} TIMER8_Device;
 
#define TIMER0_DEVICE ((TIMER8_Device *)(AVR_IO_BASE_ADDR + 0x44U))
#define TIMER2_DEVICE ((TIMER8_Device *)(AVR_IO_BASE_ADDR + 0xB0U))

/**
 * @brief Timer/Counter Interrupt Flag Register
 * 	- TIFR0 to TIFR5 on ATmega2560
 * 	- TIFR0 to TIFR3 on ATmega328P
 */
#define TIFRn ((__IO uint8_t*)(AVR_IO_BASE_ADDR + 0x35U))

/**
 * @brief Timer/Counter Interrupt Mask Register
 * 	- TIMSK0 to TIMSK5 on ATmega2560
 * 	- TIMSK0 to TIMSK3 on ATmega328P
 */
#define TIMSKn ((__IO uint8_t*)(AVR_IO_BASE_ADDR + 0x6EU))

#define TIMER_TIMSK_SET_OCIEA(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(OCIEnA))
#define TIMER_TIMSK_SET_OCIEB(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(OCIEnB))
#define TIMER_TIMSK_SET_OCIEC(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(OCIEnC))
#define TIMER_TIMSK_SET_TOIE(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(TOIEn))
#define TIMER_TIMSK_SET_ICIE(tim_idx) SET_BIT(TIMSKn[tim_idx], BIT(ICIEn))

#define TIMER_TIMSK_CLEAR_OCIEA(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(OCIEnA))
#define TIMER_TIMSK_CLEAR_OCIEB(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(OCIEnB))
#define TIMER_TIMSK_CLEAR_OCIEC(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(OCIEnC))
#define TIMER_TIMSK_CLEAR_TOIE(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(TOIEn))
#define TIMER_TIMSK_CLEAR_ICIE(tim_idx) CLR_BIT(TIMSKn[tim_idx], BIT(ICIEn))

#define TIMER_TIMSK_READ_OCIEA(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(OCIEnA))
#define TIMER_TIMSK_READ_OCIEB(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(OCIEnB))
#define TIMER_TIMSK_READ_OCIEC(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(OCIEnC))
#define TIMER_TIMSK_READ_TOIE(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(TOIEn))
#define TIMER_TIMSK_READ_ICIE(tim_idx) READ_BIT(TIMSKn[tim_idx], BIT(ICIEn))

// #define TIMER_TIMSK_HAS_OCIEC(tim_dev) IS_TIMER16(tim_dev)
// #define TIMER_TIMSK_HAS_ICIE(tim_dev) IS_TIMER16(tim_dev)

/* 8bits and 16 bits timers */
#define FOCnA FOC0A
#define FOCnB FOC0B

#define WGMn0 WGM00
#define WGMn1 WGM01
#define WGMn2 WGM02

#define CSn0 CS00
#define CSn1 CS01
#define CSn2 CS02

#define COMnA1 COM0A0
#define COMnA0 COM0A1
#define COMnB1 COM0B0
#define COMnB0 COM0B1

#define OCIEnA OCIE0A
#define OCIEnB OCIE0B
#define TOIEn TOIE0

/* 16 bits timers only */
#define FOCnC FOC1C

#define ICNCn ICNC1
#define ICESn ICES1

#define WGMn3 WGM13

#define COMnC0 COM1C0
#define COMnC1 COM1C1

#define OCIEnC OCIE1C
#define ICIEn ICIE1

/* macros */
#if defined(__AVR_ATmega2560__)
#	define TIMERS_COUNT 6
#else
#	define TIMERS_COUNT 3
#endif

#define TIMER_INDEX_EXISTS(tim_idx) ((tim_idx) < TIMERS_COUNT)

#define IS_TIMER_INDEX_8BIT(idx) ((idx == 0) || (idx == 2))
#define IS_TIMER_INDEX_16BIT(idx) ((idx == 1) || (idx == 3) || (idx == 4) || (idx == 5))

#define TIMER_GET_MAX_COUNTER(tim_idx) (IS_TIMER_INDEX_8BIT(tim_idx) ? 0xFFU : 0xFFFFU)

#define IS_TIMER0_DEVICE(dev) ((void *)dev == (void *)TIMER0_DEVICE)
#define IS_TIMER1_DEVICE(dev) ((void *)dev == (void *)TIMER1_DEVICE)
#define IS_TIMER2_DEVICE(dev) ((void *)dev == (void *)TIMER2_DEVICE)
#if defined(TIMSK3)
#	define IS_TIMER3_DEVICE(dev) ((void *)dev == (void *)TIMER3_DEVICE)
#else
#	define IS_TIMER3_DEVICE(dev) (0)
#endif 

#if defined(TIMSK4)
#	define IS_TIMER4_DEVICE(dev) ((void *)dev == (void *)TIMER4_DEVICE)
#else
#	define IS_TIMER4_DEVICE(dev) (0)
#endif 

#if defined(TIMSK5)
#	define IS_TIMER5_DEVICE(dev) ((void *)dev == (void *)TIMER5_DEVICE)
#else
#	define IS_TIMER5_DEVICE(dev) (0)
#endif 

#define IS_TIMER_16BITS(dev) \
	(IS_TIMER1_DEVICE(dev) || IS_TIMER3_DEVICE(dev) \
	|| IS_TIMER4_DEVICE(dev) || IS_TIMER5_DEVICE(dev))

#define IS_TIMER_8BITS(dev) (IS_TIMER0_DEVICE(dev) || IS_TIMER2_DEVICE(dev))

#define IS_TIMER_IDX_8BITS(idx) ((idx == 0) || (idx == 2))
#define IS_TIMER_IDX_16BITS(idx) ((idx == 1) || (idx == 3) || (idx == 4) || (idx == 5))

// #define IS_TIMER8(tim_dev) (sizeof(*tim_dev) == sizeof(TIMER8_Device))
// #define IS_TIMER16(tim_dev) (sizeof(*tim_dev) == sizeof(TIMER16_Device))

typedef enum {
	TIMER_MODE_NORMAL = 0x00,
	TIMER_MODE_CTC = 0x01,
	TIMER_MODE_FAST_PWM = 0x02,
	TIMER_MODE_PHASE_CORRECT_PWM = 0x03,
} timer_mode_t;

/* prescalers for timer 2 only */
typedef enum {
	TIMER2_STOPPED = 0x00,
	TIMER2_PRESCALER_1 = 0x01,
	TIMER2_PRESCALER_8 = 0x02,
	TIMER2_PRESCALER_32 = 0x03,
	TIMER2_PRESCALER_64 = 0x04,
	TIMER2_PRESCALER_128 = 0x05,
	TIMER2_PRESCALER_256 = 0x06,
	TIMER2_PRESCALER_1024 = 0x07,
} timer2_prescaler_t;

/* prescalers for timer 0, 1, 3, 4, 5 */
typedef enum {
	TIMER_STOPPED = 0x00,
	TIMER_PRESCALER_1 = 0x01,
	TIMER_PRESCALER_8 = 0x02,
	TIMER_PRESCALER_64 = 0x03,
	TIMER_PRESCALER_256 = 0x04,
	TIMER_PRESCALER_1024 = 0x05,

	/* unsupported for now */
	TIMER_EXT_FALLING = 0x06,
	TIMER_EXT_RISING = 0x07,
} timer_prescaler_t;

struct timer_config
{
	/**
	 * @brief Timer mode
	 */
	timer_mode_t mode : 4;

	/**
	 * @brief Timer prescaler (can be timer_prescaler_t or timer2_prescaler_t)
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

// typedef enum {
// 	TIMER_0 = 0x00,
// 	TIMER_1,
// 	TIMER_2,
// #if defined(TIMSK3)
// 	TIMER_3,
// #endif
// #if defined(TIMSK4)
// 	TIMER_4,
// #endif 
// #if defined(TIMSK5)
// 	TIMER_5,
// #endif
// 	TIMERS_COUNT,
// } timer_index_t;

// typedef enum {
// 	TIMER_INTERRUPT_INPUT_CAPTURE1 = 0x00,
// 	TIMER_INTERRUPT_OUTPUT_COMPARE1A,
// 	TIMER_INTEERUPT_OUTPUT_COMPARE1B,
// 	TIMER_INTERRUPT_OUTPUT_COMPARE1C,
// 	TIMER_INTERRUPT_OVERFLOW1
// } timer_interrupt_type_t;

/**
 * @brief Get the timer index from the device address
 * 
 * This function is declared static inline, because it can be mostly optimized 
 * to a constant value by the compiler when called on a constant timer device.
 * 
 * Example: "timer_get_index(TIMER4_DEVICE)" will give "4" at compile time.
 * 
 * Note: Do not use this function to get the timer index dynamically.
 * 
 * @param dev 
 * @return int 
 */
static inline int timer_get_index(void *dev)
{
	int ret = -EINVAL;

	if (IS_TIMER0_DEVICE(dev)) {
		ret = 0;
	}
	else if (IS_TIMER1_DEVICE(dev)) {
		ret = 1;
	}
	else if (IS_TIMER2_DEVICE(dev)) {
		ret = 2;
	}
#if defined(__AVR_ATmega2560__)
	else if (IS_TIMER3_DEVICE(dev)) {
		ret = 3;
	}
	else if (IS_TIMER4_DEVICE(dev)) {
		ret = 4;
	}
	else if (IS_TIMER5_DEVICE(dev)) {
		ret = 5;
	}
#endif

	return  ret;
}

/**
 * @brief Get the timer device address from its index
 * 
 * This function is declared static inline, because it can be mostly optimized 
 * to a constant value by the compiler when called on a constant timer device.
 * 
 * Example: "timer_get_device(4)" will give the value of TIMER4_DEVICE at compile time.
 * 
 * Note: Do not use this function to get the timer device address dynamically.
 * 
 * @param dev 
 * @return int 
 */
static inline void *timer_get_device(uint8_t idx)
{
	void *dev = NULL;

	switch (idx) {
	case 0:
		dev = TIMER0_DEVICE;
		break;
	case 1:
		dev = TIMER1_DEVICE;
		break;
	case 2:
		dev = TIMER2_DEVICE;
		break;
#if defined(__AVR_ATmega2560__)
	case 3:
		dev = TIMER3_DEVICE;
		break;
	case 4:
		dev = TIMER4_DEVICE;
		break;
	case 5:
		dev = TIMER5_DEVICE;
		break;
#endif
	}

	return dev;
}

static inline void ll_timer_clear_int_mask(uint8_t tim_idx)
{
	TIMSKn[tim_idx] = 0U;
}

static inline void ll_timer_set_int_mask(uint8_t tim_idx, uint8_t mask)
{
	TIMSKn[tim_idx] = mask;
}

static inline void ll_timer_clear_irq_flag(uint8_t tim_idx)
{
	/* For example : OCF1A: 
	 * OCFnA is automatically cleared when the Output Compare Match A 
	 * Interrupt Vector is executed. Alternatively, OCFnA can be cleared by 
	 * writing a logic one to its bit location.
	 */
	TIFRn[tim_idx] = 0xFFU;
}

static inline void ll_timer8_stop(TIMER8_Device *dev)
{
	/* timer stops counting at the timer prescaler is set to zero */
	dev->TCCRnB &= ~(BIT(CSn0) | BIT(CSn1) | BIT(CSn2));
}

static inline void ll_timer8_counter_reset(TIMER8_Device *dev)
{
	dev->TCNTn = 0x00U;
}

static inline void ll_timer8_start(TIMER8_Device *dev,
				   uint8_t prescaler)
{
	/* timer starts counting at the timer prescaler is set*/
	dev->TCCRnB |= prescaler << CSn0;
}

static inline void ll_timer16_set_tcnt(TIMER16_Device *dev,
				       uint16_t val)
{
	/**
	 * To do a 16-bit write, the high byte must be written before the low byte.
	 *  For a 16-bit read, the low byte must be read before the high byte.
	 */
	dev->TCNTnH = val >> 8;
	dev->TCNTnL = val & 0xffU;
}

static inline void ll_timer16_stop(TIMER16_Device *dev)
{
	/* timer stops counting at the timer prescaler is set to zero */
	dev->TCCRnB &= ~(BIT(CSn0) | BIT(CSn1) | BIT(CSn2));
}

static inline void ll_timer16_counter_reset(TIMER16_Device *dev)
{
	ll_timer16_set_tcnt(dev, 0x0000U);
}

static inline void ll_timer16_start(TIMER16_Device *dev,
				    uint8_t prescaler)
{
	/* timer starts counting at the timer prescaler is set*/
	dev->TCCRnB |= prescaler < CSn0;
}

static inline void ll_timer_stop(void *dev)
{
	/* as TCCRnB is at the same address in the timer regisiters, we
	 * can use the same function to stop all timers types */
	ll_timer8_stop((TIMER8_Device *)dev);
}

static inline void ll_timer_start(void *dev, uint8_t prescaler)
{
	/* see ll_timer_stop */
	ll_timer8_start((TIMER8_Device *)dev, prescaler);
}

void ll_timer8_drv_init(TIMER8_Device *dev,
			uint8_t tim_idx,
			const struct timer_config *config);

void ll_timer16_drv_init(TIMER16_Device *dev,
			 uint8_t tim_idx,
			 const struct timer_config *config);

int timer8_drv_init(TIMER8_Device *dev,
		    const struct timer_config *config);

int timer16_drv_init(TIMER16_Device *dev,
		     const struct timer_config *config);

int timer8_drv_deinit(TIMER8_Device *dev);

int timer16_drv_deinit(TIMER16_Device *dev);

#define TIMER_API_FLAG_AUTOSTART		(1 << 0)

typedef void (*timer_callback_t)(void *dev, uint8_t tim_idx, void *user_data);

/* High level API */
int timer_init(uint8_t tim_idx,
	       uint32_t period_us,
	       timer_callback_t cb,
	       void *user_data,
	       uint8_t flags);


void timer_start(uint8_t tim_idx);

void timer_stop(uint8_t tim_idx);

uint8_t timer_get_prescaler(uint8_t tim_idx);

uint32_t timer_get_max_period_us(uint8_t tim_idx);

#endif /* _AVRTOS_DRIVER_TIMER_H_ */