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

#include "common.h"

/*___________________________________________________________________________*/

//
// This file contains all the default configuration options of the kernel
//
// You may need to reset the build directory if you modify this configuration file
//
// Few constants defined here may be used in a asm file, avoid the use of the unsigned flag ("u") at the end of numbers
// i.e. prefer : CONFIG_THREAD_MAIN_STACK_SIZE 0x100 to CONFIG_THREAD_MAIN_STACK_SIZE 0x100u
//

/*___________________________________________________________________________*/

//
// Define the main thread type (coop/prempt) and priority
//
#define DEFAULT_THREAD_MAIN_COOPERATIVE         	0

//
// Tells if the main stack location and size must be defined at compilation time (1)
// or if the default main stack behaviour (stack at RAMEND) should be kept (0)
//
#define DEFAULT_THREAD_EXPLICIT_MAIN_STACK          	1

//
// In the case we defined (EXPLICIT_MAIN_STACK == 1), this configuration option
// defines the size of the main stack
//
#define DEFAULT_THREAD_MAIN_STACK_SIZE              	0x200

//
// Tells if we should use the C or the Assembler function to define our threads at runtime
//
#define DEFAULT_THREAD_USE_INIT_STACK_ASM           	1

//
// Default SREG value for other thread on stack creation.
// Main thread default SREG is always 0
//
// From datasheet
//  Bit 7 6 5 4 3 2 1 0
//  0x3F (0x5F) I T H S V N Z C SREG
//  Read/Write R/W R/W R/W R/W R/W R/W R/W R/W
//  Initial Value 0 0 0 0 0 0 0 0
#define DEFAULT_THREAD_DEFAULT_SREG             	(1 << SREG_I)

//
//  Enable Kernel Debug features
//
#define DEFAULT_KERNEL_DEBUG                    	0

//
//  Enable Kernel debug for function, that set some of them noinline 
//
#define DEFAULT_KERNEL_API_NOINLINE             	0

//
// Enable Kernel Debug in scheduler
//
#define DEFAULT_KERNEL_SCHEDULER_DEBUG          	0

//
// Enable preemtive threads feature
//
#define DEFAULT_KERNEL_PREEMPTIVE_THREADS       	1

//
// Sysclock period when precision mode is disabled (LLU is important)
//
#define DEFAULT_KERNEL_SYSCLOCK_PERIOD_US       	1024LLU

//
// Time slice in milliseconds (0 if using SYSCLOCK period)
//
#define DEFAULT_KERNEL_TIME_SLICE               	1024LLU

//
// Select Hardware timer among 8 bits timers : timer0 (0) and timer2 (2) and 16 bit timer : timer1 (1)
//
#define DEFAULT_KERNEL_SYSLOCK_HW_TIMER         	2

//
//  Auto start kernel sysclock
//
#define DEFAULT_KERNEL_SYSCLOCK_AUTO_INIT       	1

//
//  Sysclock precision mode
//
#define DEFAULT_KERNEL_SYSCLOCK_PRECISION_MODE		1

//
// Give the size of the ticks variable
//
#define DEFAULT_KERNEL_TICKS_40BITS   			5

//
//  Kernel auto initialisation
//
#define DEFAULT_KERNEL_AUTO_INIT                	1

//
// Tells if the kernel should define a idle thread to permit all user defined threads to be in waiting/pending status
//
#define DEFAULT_KERNEL_THREAD_IDLE              	1

//
// Kernel thread idle addtionnal stack
//
#define DEFAULT_KERNEL_THREAD_IDLE_ADD_STACK    	0

//
// Enable thread canaries
//
#define DEFAULT_THREAD_CANARIES                 	0

//
// Define thread canaries symbol
//
#define DEFAULT_THREAD_CANARIES_SYMBOL          	0xAA

//
// Define thread sentinel symbol
//
#define DEFAULT_THREAD_STACK_SENTINEL          		0

//
// Define thread sentinel size
//
#define DEFAULT_THREAD_STACK_SENTINEL_SIZE      	1

//
// Define thread sentinel symbol
//
#define DEFAULT_THREAD_STACK_SENTINEL_SYMBOL    	0xAA

//
// Enable system workqueue
//
#define DEFAULT_SYSTEM_WORKQUEUE_ENABLE         	0

//
// Define system workqueue stack size
//
#define DEFAULT_SYSTEM_WORKQUEUE_STACK_SIZE     	0x200

//
// Define system workqueue thread priority
//
#define DEFAULT_SYSTEM_WORKQUEUE_PRIORITY       	K_PREEMPTIVE

//
// Enable kernel assertion test for debug purpose
//
#define DEFAULT_KERNEL_ASSERT                   	0

//
// Enable timers support
//
#define DEFAULT_KERNEL_TIMERS                   	0

//
// Enable events support
//
#define DEFAULT_KERNEL_EVENTS                   	0

//
// Enabled thread errno
//
#define DEFAULT_THREAD_ERRNO                    	0

/***
 * @brief Enable scheduler lock counter for each thread to allow 
 * k_sched_lock() to be called recursively. 
 * 
 * Note: This option leads to performance overhead when locking/unlocking the scheduler,
 * enable it only if needed.
 * 
 */
#define DEFAULT_KERNEL_SCHED_LOCK_COUNTER       	0

/***
 * @brief Enable IRQ lock counter for each thread to allow 
 * irq_enable() to be called recursively. 
 * 
 * If enabled, calling irq_disable() doesn't garantee that interrupts 
 * are actually enabled again.
 * 
 * Note: This option leads to performance overhead when enabling/disabling irqs,
 * enable it only if needed.
 * 
 */
#define DEFAULT_KERNEL_IRQ_LOCK_COUNTER       		0

/**
 * @brief Tells to which USART printf function output should be redirected.
 * Disable the option by setting to -1.
 */
#define DEFAULT_STDIO_PRINTF_TO_USART			-1

/**
 * @brief Enable uptime counter in milliseconds
 */
#define DEFAULT_KERNEL_UPTIME				1

/**
 * @brief Enable performance monitoring 
 */
#define DEFAULT_KERNEL_THREAD_MONITORING		0

/**
 * @brief Enable atomic API
 */
#define DEFAULT_KERNEL_ATOMIC_API			0

/**
 * @brief Enable system time API
 */
#define DEFAULT_KERNEL_TIME				0

/**
 * @brief Use UART RX interrupt as preemptive signal
 */
#define DEFAULT_KERNEL_DEBUG_PREEMPT_UART		0

/*___________________________________________________________________________*/

#endif