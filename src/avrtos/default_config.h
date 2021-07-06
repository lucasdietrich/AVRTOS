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
#define DEFAULT_THREAD_MAIN_STACK_SIZE          0x400

//
// Tells if we should use the C or the Assembler function to define our threads at runtiime
//
#define DEFAULT_THREAD_USE_INIT_STACK_ASM       1

/*___________________________________________________________________________*/

#endif