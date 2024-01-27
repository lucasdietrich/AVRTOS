/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_TIMER_DEFS_H_
#define DRIVERS_TIMER_DEFS_H_

#include <avr/io.h>

// Timers

/* 8bits and 16 bits timers */
#define FOCnA FOC0A
#define FOCnB FOC0B

// TCCRnA
#define WGMn0 WGM10
#define WGMn1 WGM11

#define COMnA0 COM1A0
#define COMnA1 COM1A1

#define COMnB0 COM1B0
#define COMnB1 COM1B1

#if defined(COM1C0)
#define COMnC0 COM1C0
#define COMnC1 COM1C1
#endif

#if defined(COMnC0)
#define TIMER_HAS_CHANNEL_C	 1u
#define TIMER_CHANNELS_COUNT 3u
#else
#define TIMER_HAS_CHANNEL_C	 0u
#define TIMER_CHANNELS_COUNT 2u
#endif

// TCCRnB
#define WGMn2 WGM12
#define WGMn3 WGM13

#define CSn0 CS10
#define CSn1 CS11
#define CSn2 CS12

/* Interrupt mask register */
// 8 & 16 bits timers
#define TOIEn  TOIE1
#define OCIEnA OCIE1A
#define OCIEnB OCIE1B
// 16 bits timers
#define OCIEnC OCIE1C
#define ICIEn  ICIE1

#define FOCnC FOC1C

#define ICNCn ICNC1
#define ICESn ICES1

/* macros */
#if defined(TCCR5A)
#define TIMERS_COUNT 6
#elif defined(TCCR4A)
#define TIMERS_COUNT 5
#elif defined(TCCR3A)
#define TIMERS_COUNT 4
#elif defined(TCCR2A)
#define TIMERS_COUNT 3
#elif defined(TCCR1A)
#define TIMERS_COUNT 2
#elif defined(TCCR0A)
#define TIMERS_COUNT 1
#else
#define TIMERS_COUNT 0
#warning "Unsupported MCU"
#endif

#endif /* DRIVERS_TIMER_DEFS_H_ */