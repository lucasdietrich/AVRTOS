# Changelog

## avrtos v1.2.0

Key features:
- **I2C**: Introduced full I2C driver, including a new TCN75 device driver. 
- **SPI Driver Refactor**: Enhanced SPI driver by introducing the `spi_regs` 
  intermediate structure, holding SPI registers.
- **Work Queue**: Added support for delayable work queue items with a sample 
  demonstrating the feature.

Minor Changes:
- Stored MCUSR at startup with `CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE` option.
- Introduced new configuration options:
  - Handling of reset cause with `CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE` and `CONFIG_KERNEL_CLEAR_WDT_ON_INIT`
  - `CONFIG_IDLE_HOOK`
  - `CONFIG_WORKQUEUE_DELAYABLE`
  - `CONFIG_SERIAL_AUTO_INIT`
  - Banner configuration options `CONFIG_AVRTOS_BANNER_ENABLE` and `CONFIG_AVRTOS_BANNER`
  - Various options for the I2C driver.
- Updated samples, documentation.
- Removed unused USART functions and eradicated unused code.
- Move assembly files to `src/arch`.

Bug Fixes:

Documentation and Samples:
- Enhanced documentation
- added new samples demonstrating various features:
  - Added `MCP3008` and `TCN75` driver samples.

## avrtos v1.1.0

Main features:
- SPI: Added SPI drivers for master and slave with full support for atmega328p
  and atmega2560.
- SPI: Added two samples to demonstrate communication between master and slave
  (with data rate and error measurement).
- DevOps: Added Dockerfile and Jenkinsfile for CI/CD.
  
Bug Fixes:
- Corrected bad characters in a preprocessor #error message.
- Resolved anomalies in the implementation of k_flags_notify() consequently,
  refined the flags sample.
- IRQ are now always enabled when exiting k_thread_stop(), this adjustment
  ensures threads can be safely resumed using k_thread_start().
- Fixed a bug in QEMU preventing to use all timers (patch submitted and attached
  here).

Improvements:
- Improved time functions: introduced `k_wait` allowing to wait for a specific
  time in a particular mode. Added support for millisecond precision in time API
  through CONFIG_KERNEL_TIME_API_MS_PRECISION.
- Updated and improved documentation.
- Optimized headers inclusions.

Minor changes:
- Introduced `K_TICKS` to declare timeouts in ticks.
- Introduced macros: `FEATURE_TIMER_COUNT` and FEATURE_USART_COUNT to ascertain if
  an example can be built for a certain toolchain.
- Enhanced disassembly script for PlatformIO.
- k_work_submittable removed from public API, but k_workqueue_create was added
  to the Arduino keywords list.
- Added sample for testing a single timer.
- Enhanced cmake targets with better ninja support, making ninja the default
  generator.
- Introduced a script for computing code metrics.
- Dead code was purged.
- Help added for thread-ev-spawn example.
- Defined internal macros: `Z_THREAD_MAIN_PRIORITY` and
  `Z_THREAD_MAIN_STACK_END_ADDR`.
- Eradicated unused/unfished file descriptor features (fd.c/fd.h).
- Eliminated `/*______*/` code boundaries.

## avrtos v1.0.0

AVRTOS is a real-time operating system (RTOS) designed specifically for 8-bit
AVR microcontrollers. The project aims to provide an efficient and highly
configurable RTOS solution for AVR-based systems. Fully C/C++ compliant, AVRTOS
is compatible with the AVR-GCC toolchain, Arduino and PlatformIO frameworks.

Key Features:

- Cooperative and preemptive threads
- Naive scheduler without priority support
- Configurable system clock
- Synchronization objects (mutexes, semaphores, workqueues, etc.)
- Drivers for UART, timers, GPIO, and external interrupts
- Thread sleep and scheduler lock/unlock
- Runtime object creation
- Thread canaries and sentinel stack protection
- Events and timers
- Atomic API, logging subsystem, and Uptime API
- Various data structures
- Thread naming, pseudo random number generator, and debug/utils functions

Supported AVR Architectures:

- AVR5: Tested on ATmega328p (Arduino PRO)
- AVR6: Tested on ATmega2560 (Arduino Mega2560)

Full Changelog: https://github.com/lucasdietrich/AVRTOS/commits/v1.0.0