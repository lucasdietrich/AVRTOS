
AVRTOS is a real-time operating system (RTOS) crafted for 8-bit AVR microcontrollers.
It aims to provide an efficient and highly configurable RTOS solution for AVR-based 
systems. Fully C/C++ compliant, AVRTOS is compatible with the AVR-GCC toolchain, 
Arduino and PlatformIO frameworks, it can even be emulated using QEMU.

AVRTOS has been successfully tested on the following AVR architectures:

- AVR5, particularly ATmega328p
- AVR6, especially ATmega2560

Please bear in mind that as a personal project, it doesn't come with any guarantees.
I hope you find AVRTOS as exciting to use as I found it to develop !

## Features

AVRTOS offers an extensive list of features, including:

- Cooperative and preemptive threads
- Naive scheduler without priority support
- Configurable system clock with support for all hardware timers (e.g. 0-2 for ATmega328p and 0-5 for ATmega2560)
- Synchronization objects like mutexes, semaphores, workqueues (+delayables), FIFOs, message queues, memory slabs, flags, signals
- Drivers for UART, timers, GPIO, SPI, I2C and external interrupts
- Devices drivers for TCN75
- Thread sleep with up to 65-second duration in simple mode (extendable using high-precision time objects)
- Scheduler lock/unlock to temporarily prevent preemption for preemptive threads
- Thread switching from interrupts
- Runtime creation for many kernel objects (threads, mutexes, semaphores, workqueues, fifos, memory slabs, ...)
- Diagnostics: Thread canaries and sentinel stack protection
- Events and timers
- Atomic API for 8-bit variables
- Macro-based logging subsystem
- Uptime API
- Data structures: singly and doubly linked lists, queues, ring buffers, timeout queue

Additional Features:

- Thread naming with symbols (e.g., 'M' for the main thread, 'I' for the idle thread)
- Pseudorandom number generator (LFSR)
- Debugging and utility functions  (RAM_DUMP, CORE_DUMP, z_read_ra)
- Kernel assertions (__ASSERT)
- Custom error codes (e.g., EAGAIN, EINVAL, EIO, ENOMEM, ...)
- Thread safe termination (excluding main thread)
- `stdout` redirection to USART0
- Various wait variants (e.g., k_sleep, k_wait with modes IDLE, ACTIVE, BLOCK and z_cpu_block_us)
- Reset reason detection
- Dockerfile and Jenkinsfile templates for CI/CD
- Full compatibility with QEMU emulation

Planned Features (TODOs):

- Comprehensive tests
- Enhanced documentation based on `mkdocs` (and `doxygen` ??)
- Ultra-low duration sleep (e.g., 18us) using a dedicated timer counter with API: `uscounter_init()`, `uscounter_get()`, `uscounter_set`
- Functions like sys_le32_read/write
- Utilizing sysclock for thread wake-up granularity (k_sleep) instead of timeslice
  - Option to select SYSCLOCK or TIMESLICE as the scheduling point (`KERNEL_SCHEDULING_EVENT`)
  - Note on using `k_busy_wait(K_USEC(20u))` for precise short-duration waits
- Thread priority implementation
- Per-thread CPU usage statistics
- Polling mechanisms
- ADC drivers
- Memory heap management
- Runtime detection of available space for the main thread stack (init)
- Consideration of available heap space
- Implementation of I2C Repeated Start
- Refactoring: Move or remove unused `AVRTOS_VERSION_MAJOR` variables
- Organize MCU specific fixups and board-specific items in a dedicated "board" directory
- Relocation of assembly files to "arch/avr"
- Consolidation of private defines in _private.h headers
- Investigation of high metric readings
- Tickless kernel + timeslicing features
- Renaming "static inline" functions to "__always_inline"
- Implementation of builtin_ctz for 8-bit variables
- Sample for discovering the I2C bus
- Removal of outdated samples
- i2c write_then_read function
- Add support for rentrency
  - irq_lock/irq_unlock
  - k_sched_lock/k_sched_unlock
  - k_mutex_lock/k_mutex_unlock