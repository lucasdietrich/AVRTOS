# Changelog

## avrtos v1.3.1

Key features:
- Fix Arduino build

## avrtos v1.3.0

Key Features:
- (experimental feature) New MCP2515 CAN driver: Introduced a new CAN driver for 
  the MCP2515 controller, 
  enabling CAN bus communication over SPI. Introduce `CONFIG_DEVICE_MCP2515` 
  configuration option to enable or disable the MCP2515 driver.
- Documentation Overhaul: Improved documentation across multiple modules, including 
  mutexes, semaphores, signals, FIFOs, flags, standard I/O, system time, timers, 
  events, atomic operations, idle thread, and various kernel headers.
- Kernel State Structure: Introduced the `z_kernel` structure to centralize all 
  kernel state variables.
- Thread Sleep Improvement: Reworked the `z_pend_current()` function and modified 
  `k_sleep()` to utilize it.
- Semaphore Enhancement: Added `k_sem_cancel_wait()` function to allow cancellation 
  of threads waiting on a semaphore.
- Renamed macros:
  - Configuration Options Renaming
    - Renamed several configuration macros for better consistency and clarity:
      - `CONFIG_KERNEL_DEBUG` to `CONFIG_KERNEL_SYSCLOCK_DEBUG=0`
      - `CONFIG_KERNEL_TIME_SLICE_TICKS` to `CONFIG_KERNEL_TIME_SLICE_US`
      - `K_EVENTS_PERIOD_TICKS` to `Z_EVENTS_PERIOD_TICKS`
      - `K_TIMERS_PERIOD_TICKS` to `Z_TIMERS_PERIOD_TICKS`
  - SPI Driver Update: Renamed `spi_mode_t` to `spi_role_t` and introduced a new 
    `spi_mode_t` for better clarity in the SPI driver.

Minor changes:
- New Sample: Introduced a `hello-world` sample to help new users 
  get started quickly.
- Function Relocation: Moved uptime functions to the `systime.c` file.
- UART Configuration: Changed the default UART baud rate to 115200 bps for 
  consistency across the platform.
- QEMU Configuration: Defined `icount auto` in QEMU settings.
- Makefile Upload Behavior: Adjusted the Makefile to flash the most recently 
  built sample when uploading

Bug fixes:
- I2C Driver Frequency Calculation Fix: Corrected the `FREQ_CALC` macro to ensure 
  accurate frequency settings in the I2C driver.
- Timer Driver Function Correction: Fixed `ll_timer16_start()` and introduced 
  `ll_timer16_get_tcnt()`.
- Workqueue Module Macro Fix: Fixed the `K_WORK_DELAYABLE_INIT` macro and 
  reintroduced `z_delayable_work_trigger()` to restore delayable workqueue functionality.
- Kernel Synchronization:
  - Scheduler Lock Improvement: Modified the kernel to lock the scheduler only 
    if it is not already locked, preventing potential deadlocks.

## avrtos v1.2.1

Key features:
- **I2C**:
  - Introduce function `i2c_master_write_read()` enabling to write and 
  read data in a single transaction.
  - Rename `i2c_master_transmit())` to `i2c_master_write()`.
  - Rename `i2c_master_receive()` to `i2c_master_read()`.
  - Add option to configure the speed of the I2C bus on initialization.

Minor changes:
- Introduce `CONFIG_KERNEL_REENTRANCY` configuration option to enable/disable
  reentrancy support for kernel objects, such as mutexes and sched lock/unlock.
- Change code style, update `.clang-format` and format the codebase.
- Improve stack sentinel verification by introducing a thread monitoring mecanism.
- Introduce `CONFIG_THREAD_MONITOR` and `CONFIG_THREAD_MAIN_MONITOR` configuration
  options to enable/disable thread monitoring.
- Introduce `CONFIG_THREAD_STACK_SENTINEL_AUTO_VERIFY` configuration option to
  enable/disable automatic stack sentinel verification.
- Improve `timers` subsystem.

Bug fixes:
- Fix returned error code in `i2c_master_write()` and `i2c_master_read()`.
- Fix `K_MSEC` macro with big values of `CONFIG_KERNEL_SYSCLOCK_PERIOD_US`.
- Fix `K_WORK_DELAYABLE_INIT` macro.

## avrtos v1.2.0

Key Features:
- **I2C**: Introduced a comprehensive I2C driver, and a minimal TCN75 device driver.
- **SPI Driver Refactor**: Enhanced the SPI driver by introducing the `spi_regs` 
  intermediate structure, which holds SPI registers.
- **Work Queue**: Added support for delayable work queue items, with a sample 
  demonstrating this feature.

Minor Changes:
- Stored MCUSR at startup using the `CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE` 
  option.
- Introduced new configuration options:
  - Management of reset cause with `CONFIG_KERNEL_MINICORE_SAVE_RESET_CAUSE` and 
    `CONFIG_KERNEL_CLEAR_WDT_ON_INIT`.
  - Addition of an idle hook through `CONFIG_IDLE_HOOK`.
  - Enabling of the delayable work feature with `CONFIG_WORKQUEUE_DELAYABLE`.
  - `CONFIG_SERIAL_AUTO_INIT`.
  - Banner configuration options `CONFIG_AVRTOS_BANNER_ENABLE` and `CONFIG_AVRTOS_BANNER`.
  - Various options for the I2C driver.
- Updated samples and documentation.
- Removed unused USART functions and eradicated obsolete code.
- Moved assembly files to `src/arch`.
- Improved overall error code returns and extended support for `CONFIG_KERNEL_ARGS_CHECK`.

Bug Fixes:
- Fixed `CONFIG_KERNEL_AUTO_INIT` with the Arduino framework by utilizing the 
  `initVariant()` function.

Documentation and Samples:
- Enhanced documentation.
- Added new samples demonstrating various features:
  - `MCP3008` and `TCN75` driver samples.
  - Arduino sample `MinimalExample`.

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
- IRQ are now always enabled when exiting k_thread_abort(), this adjustment
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