/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_TICKLESS_DEFS_H_
#define _AVRTOS_TICKLESS_DEFS_H_

#define TICKLESS_WRAP_MODE_PERFORMANCE 0
#define TICKLESS_WRAP_MODE_PRECISION   1

// (.venv) lucas@zgw:~/AVRTOS$ python3 scripts/timer_calc.py
// timer0 :  8 bits, prescalers = 1, 8, 64, 256, 1024
// prescaler = 1  freq = 62500.0 Hz (0.0625 us)  period = 16 us
// prescaler = 8  freq = 7812.5 Hz (0.5 us)  period = 128 us
// prescaler = 64  freq = 976.5625 Hz (4.0 us)  period = 1024 us
// prescaler = 256  freq = 244.140625 Hz (16.0 us)  period = 4096 us
// prescaler = 1024  freq = 61.03515625 Hz (64.0 us)  period = 16384 us
// timer1 : 16 bits, prescalers = 1, 8, 64, 256, 1024
// prescaler = 1  freq = 244.140625 Hz (0.0625 us)  period = 4096 us
// prescaler = 8  freq = 30.517578125 Hz (0.5 us)  period = 32768 us
// prescaler = 64  freq = 3.814697265625 Hz (4.0 us)  period = 262144 us
// prescaler = 256  freq = 0.95367431640625 Hz (16.0 us)  period = 1048576 us
// prescaler = 1024  freq = 0.2384185791015625 Hz (64.0 us)  period = 4194304 us
// timer2 :  8 bits, prescalers = 1, 8, 32, 64, 128, 256, 1024
// prescaler = 1  freq = 62500.0 Hz (0.0625 us)  period = 16 us
// prescaler = 8  freq = 7812.5 Hz (0.5 us)  period = 128 us
// prescaler = 32  freq = 1953.125 Hz (2.0 us)  period = 512 us
// prescaler = 64  freq = 976.5625 Hz (4.0 us)  period = 1024 us
// prescaler = 128  freq = 488.28125 Hz (8.0 us)  period = 2048 us
// prescaler = 256  freq = 244.140625 Hz (16.0 us)  period = 4096 us
// prescaler = 1024  freq = 61.03515625 Hz (64.0 us)  period = 16384 us

// TICKLESS CONFIGURATION
#define TICKLESS_WRAP_MODE       TICKLESS_WRAP_MODE_PERFORMANCE
#define TICKLESS_PRESCALER_VALUE 8u

#if TICKLESS_PRESCALER_VALUE == 1024
// prescaler = 1024  freq = 0.2384185791015625 Hz (64.0 us)  period = 4194304 us
// Max schedulable duration on 32-bits counter is (1<<32) * 64us = 3j
// Max schedulable duration on 40-bits counter is (1<<40) * 64us = 814j
#define TICKLESS_PRESCALER_VALUE 1024u
#define TICKLESS_US_PER_TICK     64u
#define TICKLESS_US_PER_PERIOD   4194304u
#define TICKLESS_TOP_PERIOD_US   4000000u
#elif TICKLESS_PRESCALER_VALUE == 256
// prescaler = 256  freq = 0.95367431640625 Hz (16.0 us)  period = 1048576 us
// Max schedulable duration on 32-bits counter is (1<<32) * 16us = 18h
// Max schedulable duration on 40-bits counter is (1<<40) * 16us = 203j
#define TICKLESS_PRESCALER_VALUE 256u
#define TICKLESS_US_PER_TICK     16u
#define TICKLESS_US_PER_PERIOD   1048576u
#define TICKLESS_TOP_PERIOD_US   1000000u
#elif TICKLESS_PRESCALER_VALUE == 64
// prescaler = 64  freq = 3.814697265625 Hz (4.0 us)  period = 262144 us
// Max schedulable duration on 32-bits counter is (1<<32) * 4us = 4.8h
// Max schedulable duration on 40-bits counter is (1<<40) * 4us = 50j
#define TICKLESS_PRESCALER_VALUE 64u
#define TICKLESS_US_PER_TICK     4u
#define TICKLESS_US_PER_PERIOD   262144u
#define TICKLESS_TOP_PERIOD_US   250000u
#elif TICKLESS_PRESCALER_VALUE == 8
// prescaler = 8  freq = 7812.5 Hz (0.5 us)  period = 128 us
// Max schedulable duration on 32-bits counter is (1<<32) * 0.5us = 36m
// Max schedulable duration on 32-bits counter is (1<<40) * 0.5us = 6j
#define TICKLESS_PRESCALER_VALUE 8u
#define TICKLESS_US_PER_TICK     0.5
#define TICKLESS_US_PER_PERIOD   32768u
#define TICKLESS_TOP_PERIOD_US   25000u
// #error "Probable tick calculation error with prescaler 8, check the code"
#elif TICKLESS_PRESCALER_VALUE == 1
// prescaler = 1  freq = 244.140625 Hz (0.0625 us)  period = 4096 us
// Max schedulable duration on 32-bits counter is (1<<32) * 0.0625us = 4m28s
// Max schedulable duration on 32-bits counter is (1<<40) * 0.0625us = 19h
#define TICKLESS_PRESCALER_VALUE 1u
#define TICKLESS_US_PER_TICK     0.0625
#define TICKLESS_US_PER_PERIOD   4096u
#define TICKLESS_TOP_PERIOD_US   4000u
#error "Probable tick calculation error with prescaler 1, check the code"
#endif

#endif /* _AVRTOS_TICKLESS_DEFS_H_ */
