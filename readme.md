# AVRTOS: An RTOS for 8-bit AVR Microcontrollers

## Introduction
AVRTOS is a real-time operating system (RTOS) designed specifically for 8-bit AVR microcontrollers. The project aims to provide an efficient and highly configurable RTOS solution for AVR-based systems. Fully C/C++ compliant, AVRTOS is compatible with the AVR-GCC toolchain, Arduino and PlatformIO frameworks.

Currently, AVRTOS supports and has been tested on the following AVR architectures:

- AVR5, particularly ATmega328p: Tested on Arduino PRO
- AVR6, particularly ATmega2560: Tested on Arduino Mega2560

## Features

AVRTOS offers an extensive list of features, including:

- Cooperative and preemptive threads
- Round-robin scheduling without priority support
- Configurable system clock with support for all hardware timers (e.g. 0-2 for ATmega328p and 0-5 for ATmega2560)
- Synchronization objects like mutexes, semaphores, workqueues, FIFOs, message queues, memory slabs, flags, signals
- Drivers for UART, timers, GPIO, and external interrupts
- Thread sleep with up to 65-second duration in simple mode (extendable using high-precision time objects)
- Scheduler lock/unlock to temporarily prevent preemption for preemptive threads
- Thread switching from interrupts
- Runtime creation for many kernel objects (threads, mutexes, semaphores, workqueues, fifos, memory slabs, ...)
- Thread canaries and sentinel stack protection
- Events and timers
- Atomic API for 8-bit variables
- Macro-based logging subsystem
- Uptime API
- Data structures: singly and doubly linked lists, queues, ring buffers, timeout queue

Additional Features:
- Thread naming with symbols (e.g., 'M' for the main thread, 'I' for the idle thread)
- Pseudorandom number generator (LFSR)
- Debug/util functions (RAM_DUMP, CORE_DUMP, z_read_ra)
- Kernel assertions (__ASSERT)
- Custom error codes (e.g., EAGAIN, EINVAL, ETIMEDOUT)
- Thread safe termination (excluding main thread)
- stdout redirection to USART0, and stack sentinels

We plan (TODO) to implement several new features and improvements, including:

- **SPI**
	- Check reliability
- CRC
- Low duration sleep (e.g., 18us) with the use a dedicated timer counter.
	- APi would be `uscounter_init()`, `uscounter_get()`, `uscounter_set`
- sys_le32_read/write, etc ...
- Use sysclock as granularity for threads wake up (k_sleep) instead of the timeslice.
	- Option to choose the SYSCLOCK or TIMESLICE as scheduling point: `KERNEL_SCHEDULING_EVENT`
	- Explain that if timeslice is used, it's still possible to wait for small duration with `k_busy_wait(K_USEC(20u));`.
- Add functions:
	- k_msleep (k_sleep(K_MSEC(1000u));
	- k_usleep (which uses k_msleep and k_busy_wait depending on the duration)
	- k_wait which polls uptime, yield() or sleep but doesn't lock the scheduler
	- k_busy_wait which polls uptime and locks the scheduler (k_sched_lock)
	- k_block_us which locks IRQ and use _delay_us (uptime not incremented)
- Threads priority
- Per-thread CPU usage statistics
- Polling
- Drivers for SPI, I2C and ADC
- Memory heaps

## Getting Started

### Description

Example [minimal-example](src/examples/minimal-example/main.c) configures an usart, 
and blink a led at a frequency of 1Hz.
Morover, typing a character on the serial console will wake up a thread which
will print the received character.

### Code

Configuration:

```
CONFIG_KERNEL_COOPERATIVE_THREADS=1
CONFIG_KERNEL_TIME_SLICE_US=1000
CONFIG_INTERRUPT_POLICY=1
CONFIG_KERNEL_THREAD_TERMINATION_TYPE=-1
CONFIG_THREAD_MAIN_COOPERATIVE=1
CONFIG_STDIO_PRINTF_TO_USART=0
CONFIG_KERNEL_UPTIME=1
```

Code:
```c
/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/kernel.h>
#include <avrtos/logging.h>
#include <avrtos/misc/led.h>
#define LOG_LEVEL LOG_LEVEL_DBG

K_MSGQ_DEFINE(usart_msgq, 1u, 16u);

static void thread_usart(void *arg);
static void thread_led(void *arg);

K_THREAD_DEFINE(th_usart, thread_usart, 164u, K_COOPERATIVE, NULL, 'X');
K_THREAD_DEFINE(th_led, thread_led, 164u, K_COOPERATIVE, NULL, 'L');

ISR(USART0_RX_vect)
{
	const char c = USART0_DEVICE->UDRn;

	k_msgq_put(&usart_msgq, &c, K_NO_WAIT);
}

int main(void)
{
	const struct usart_config usart_config = {
		.baudrate    = USART_BAUD_500000,
		.receiver    = 1u,
		.transmitter = 1u,
		.mode	     = USART_MODE_ASYNCHRONOUS,
		.parity	     = USART_PARITY_NONE,
		.stopbits    = USART_STOP_BITS_1,
		.databits    = USART_DATA_BITS_8,
		.speed_mode  = USART_SPEED_MODE_NORMAL,
	};
	ll_usart_init(USART0_DEVICE, &usart_config);
	ll_usart_enable_rx_isr(USART0_DEVICE);

	led_init();

	LOG_INF("Application started");

	k_thread_dump_all();

	k_stop();
}

static void thread_usart(void *arg)
{
	char c;
	for (;;) {
		if (k_msgq_get(&usart_msgq, &c, K_FOREVER) >= 0) {
			k_show_uptime();
			LOG_INF("<inf> Received: %c", c);
		}
	}
}

static void thread_led(void *arg)
{
	for (;;) {
		k_show_uptime();
		led_toggle();
		LOG_DBG("<dbg> toggled LED");
		k_sleep(K_MSEC(500u));
	}
}
```

Build and flash using PlatformIO. Expected output on the serial console: 

![](./pics/pio_minimal_sample_monitor.png)

## Configuration

AVRTOS is highly configurable, file [src/avrtos/avrtos_conf.h](./src/avrtos/avrtos_conf.h) lists all the default configuration options.

For arduino framework, a dedicated configuration file is provided in [src/avrtos/avrtos_conf_arduino.h](./src/avrtos/avrtos_arduinoide_conf.h).

## Build

Several build systems are supported:
- Cmake
- Arduino IDE
- PlatformIO (with and without Arduino Framework)

### Arduino IDE

Install the Arduino IDE (1 or 2) from [arduino.cc](https://www.arduino.cc/en/software).

Copy complete `AVRTOS` folder to your Arduino libraries folder.

Open the Arduino IDE and select your board/port.

Select the sample :

![](./pics/arduino_ide2_open_example.png)

Build and upload the sample.

### PlatformIO

### Build, flash and monitor samples

PlatformIO extension for VSCode is recommended (`platformio.platformio-ide`).
Simply select your sample and build it.

### Use AVRTOS as a library dependency in your project

You can clone this repository in your project's `lib` folder, a typical `platformio.ini` file would look like this:

```ini
[env]
platform = atmelavr
board = pro16MHzatmega328
; board = megaatmega2560

upload_port = COM3
monitor_port = COM3
monitor_speed = 500000

build_src_filter = 
	+<AVRTOS/src/>

build_flags = 
    -Wl,-T./avrtos-avr5.xn
    ; -Wl,-T./avrtos-avr6.xn

	-DCONFIG_THREAD_MAIN_STACK_SIZE=256
	-DCONFIG_THREAD_EXPLICIT_MAIN_STACK=0
	-DCONFIG_THREAD_MAIN_COOPERATIVE=1
    -DCONFIG_KERNEL_SYSCLOCK_PERIOD_US=1000
    -DCONFIG_KERNEL_TIME_SLICE_US=1000
	; ...
```

An example project which uses AVRTOS as a dependency can be found here: [github.com/lucasdietrich/caniot-device](https://github.com/lucasdietrich/caniot-device)

### Cmake

To build the samples with cmake, you'll need to have `avr-gcc` and `cmake` installed.
`avr-gdb` and `qemu` are optional

To configure your environnement for an Arduino Mega 2560, run the following commands:

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="cmake/avr6-atmega2560.cmake"
```

You can also specify the generator :

```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="cmake/avr6-atmega2560.cmake" \
  -DCMAKE_GENERATOR="Ninja"
```

You can also provide the device where the program will be flashed, this enables 
make commands like `make upload_sample_program`:
```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="cmake/avr6-atmega2560.cmake" \
  -DPROG_DEV=/dev/ttyACM0
```

To build the `minimal_example` program, run the following command:

```bash
make -C build sample_minimal_example
```

To flash the binary to your device, run the following command:

```bash
make -C build upload_sample_minimal_example
```

Monitor the serial console with `miniterm`:
```bash
BAUDRATE=500000 make monitor
```

### Debug with QEMU

In case you want to emulate you program with QEMU:
```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="cmake/avr6-atmega2560.cmake" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DQEMU=ON
```

This enables targets like `make run_sample_*` and `make qemu_sample_*`.
For example, to run the `minimal-example` program in QEMU, run the following command:

```bash
make -C build run_sample_minimal_example
```

To exit QEMU, press `Ctrl+A` and then `x`.

If you want to debug your program with QEMU, run the following command:

```bash
make -C build qemu_sample_minimal_example
```

With VS Code, select `QEMU (avr)` configuration, then press `F5` to start debugging (`launch.json` configuration is automatically generated during build).

![](./pics/vscode_qemu_minimal_example.png)

---

## clang-format

- Install clang-format, and run `make format` to format all files
- Or install `xaver.clang-format` extension for VS Code, and format on save or
format using `Ctrl + K, Ctrl + F`

## Troubleshooting

- If your program crashes/restart/gets stuck, just increase all stack sizes, just in case (especially with `CONFIG_KERNEL_TIMERS`, `CONFIG_KERNEL_EVENTS`).
  - Note: Also increase IDLE stack size with `CONFIG_KERNEL_THREAD_IDLE_ADD_STACK`, IDLE stack is tiny by default
  - Note: function call with ATmge2560 for example are stack consuming (because of the 3B return addresses)
- Please note that, when using VS Code with PIO, `c_cpp_properties.json` file is frequently overwritten by PIO extension, which is not convenient when working with `cmake`.
- Disassembly files can be found :
	- in `build/examples/${sample}` folder for `cmake` build
	- in `tmp/${sample}/*` folder for PlatformIO build, after running `python3 ./scripts/pydis.py`
- In case python `miniterm` package is not installed, install python dependencies with `pip3 install -r scripts/requirements.txt`

## Some links :
- More information on data structures : https://en.wikipedia.org/wiki/Linked_list
- Memory sections on AVR : https://www.nongnu.org/avr-libc/user-manual/mem_sections.html
- avr-gcc : https://gcc.gnu.org/wiki/avr-gcc
- Pseudo-ops and operators : https://www.nongnu.org/avr-libc/user-manual/assembler.html#ass_pseudoop
- AVR Instruction Set Manual : http://ww1.microchip.com/downloads/en/devicedoc/atmel-0856-avr-instruction-set-manual.pdf
- cmake-avr github repository : https://github.com/mkleemann/cmake-avr
- [`arduino-lint`](https://github.com/arduino/arduino-lint) tool to check Arduino libraries
- Arduino library specification: https://arduino.github.io/arduino-cli/0.31/library-specification/
- Arduino library manager FAQ: https://github.com/arduino/library-registry/blob/main/FAQ.md#submission-requirements

## My versions

```
avr-gcc (Fedora 11.2.0-1.fc36) 11.2.0
avr-gdb: GNU gdb (GDB) 12.1
QEMU emulator version 7.0.0 (v7.0.0)
avrdude version 6.4
cmake version 3.25.1
GNU Make 4.3
Python 3.10.9
```