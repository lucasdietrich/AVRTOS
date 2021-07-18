/**
 * @file config.h
 * @author Lucas Dietrich (ldietrich.fr)
 * @brief 
 * @version 0.1
 * @date 2021-05-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _AVRTOS_DEFAULT_CONFIG_H
#define _AVRTOS_DEFAULT_CONFIG_H

#include <avr/io.h>

/*___________________________________________________________________________*/

//
// This file contains all the default configuration options of the kernel
//
// You may need to reset the build directory if you modify this configuration file
//
// Few constants defined here may be used in a asm file, avoid the use of the unsigned flag ("u") at the end of numbers
// prefer : CONFIG_THREAD_MAIN_STACK_SIZE 0x100 to CONFIG_THREAD_MAIN_STACK_SIZE 0x100u
//

/*___________________________________________________________________________*/

//
// Define the maximum number of threads supported
//
#define DEFAULT_THREAD_MAX                      4

//
// Define the main thread type (coop/prempt) and priority
//
#define DEFAULT_THREAD_MAIN_THREAD_PRIORITY     K_PRIO_PREEMPT(8)

//
// Tells if the main stack location and size must be defined at compilation time (1)
// or if the default main stack behaviour (stack at RAMEND) should be kept (0)
//
#define DEFAULT_THREAD_EXPLICIT_MAIN_STACK      1

//
// In the case we defined (EXPLICIT_MAIN_STACK == 1), this configuration option
// defines the size of the main stack
//
#define DEFAULT_THREAD_MAIN_STACK_SIZE          0x200

//
// Tells if we should use the C or the Assembler function to define our threads at runtiime
//
#define DEFAULT_THREAD_USE_INIT_STACK_ASM              1

//
// Tell if we can name threads using a 2 letters symbol (e.g. M, T1, T2, ...)
//
#define DEFAULT_THREAD_SYMBOL                          1

//
// Configure to use uint32_t as k_delta_ms_t ~= 50 days or keep (uint16_t) ~= 65seconds
//
#define DEFAULT_KERNEL_HIGH_RANGE_TIME_OBJECT_U32      0

//
// Default SREG value for other thread on stack creation
//  Main thread default SREG is still 0
//
// From datasheet
//  Bit 7 6 5 4 3 2 1 0
//  0x3F (0x5F) I T H S V N Z C SREG
//  Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
//  Initial Value 0 0 0 0 0 0 0 0
#define DEFAULT_THREAD_DEFAULT_SREG             1 << SREG_I

//
//  Enable Kernel Debug features
//
#define DEFAULT_KERNEL_DEBUG                    1

//
// enable preemtive threads feature
//
#define DEFAULT_KERNEL_PREEMPTIVE_THREADS       1

//
// Time slice in milliseconds
// TODO, use 100µs as base unit
//
#define DEFAULT_KERNEL_TIME_SLICE               16

//
//  auto start kernel sysclock
//
#define DEFAULT_KERNEL_SYSCLOCK_AUTO_INIT       1

//
//  Use uart rx interrupt as preempt signal
//
#define DEFAULT_KERNEL_DEBUG_PREEMPT_UART       0

/*___________________________________________________________________________*/

#endif