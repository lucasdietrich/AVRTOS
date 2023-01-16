# AVRTOS : RTOS attempt for 8bits AVR microcontrollers

## Introduction

This project is an attempt to create a real time operating system (RTOS) for AVR 8 bits microcontrollers.

Following avr architectures are supported/tested :
- avr5, especially ATmega328p : tested with an Arduino PRO
- avr6, especially ATmega2560 : tested with an Arduino Mega2560

## Features

Following features are supported:
- Cooperative threads
- Highly configurable sysclock, all hardware timers can be used (from 0 to 2 for the ATmega328p and 0 to 5 for the ATmega2560)
- Preemptive threads with highly configure time slice
- Mutex and Semaphores
- Thread sleep up to 65 seconds (or more if using high precision time objects)
- Scheduler lock/unlock to temporarily set a preemptive thread as cooperative
- Waking up threads waiting on events (mutex unlock, semaphore give)
- Scheduler calls (thread switching) from interrupt handler
- Canaries
- Runtime stack/thread creation
- avr5 and avr6 architectures
- Workqueues and system workqueue 
- Fifo
- Memory slabs
- Pseudo random number generator : [LFSR](https://es.wikipedia.org/wiki/LFSR)
- Timers
- Signals (poll on a single signal)
- Messages Queues (msgq)
- System uptime in ms and seconds (32bits or 40bits ms counter)
- Atomic API for 8bits variables
- Events scheduling 
- Drivers
  - UART
  - Timers
  - GPIO
  - External interrupts
- Highly configurable

Minor features:
- thread naming with a symbol, e.g. 'M' for the main thread 'I' for the idle thread 
- thread context passing
- debug/utils functions : RAM_DUMP, CORE_DUMP, z_read_ra (read return address)
- data structures : 
  - dlist : doubly linked list
    - raw API
    - dlist API : queue
    - cdlist API : circular dlist
  - queue : singly linked list
  - slist_append : optimized singly linked list
  - tqueue : scheduling singly linked list for time-sorted items
- I/O : leds, uart
- Kernel Assertions (__ASSERT)
- Custom errors code: e.g. : EAGAIN, EINVAL, ETIMEDOUT
- Efficient pending feature, allowing to pass directly an object (mutex, semaphore, mem slab bloc, fifo item) to a pending thread. No need to do the whole process : unlock/lock for a mutex or free/allocate for a memory block if a thread is pending for the object being available.
- Mutex thread owner
- Fully C/C++ compliant
- Allow thread safe termination (doesn't work for `main` thread yet).
- Project fully PlatformIO compatible for Windows and Linux
- Redirection of stdout to USART0 (e.g. printf)
- Stack sentinels

What paradigms/concepts are not supported:
- Nested interrupts
- Dedicated stack for interrupt handlers
- Thread local storage (removed)

What features will be implemented :
- **Optimized circular buffer** / **Stream** / **Asymetric put/get**
- Statistics CPU use per thread
- Delayed start, suspending/resuming threads
- Kernel fault
- Threads priority

AVRTOS2 (version 2) main improvements:
- Enhanced clock precision
- Optimized "thread_switch" function
- Cooperative threads needs less stacks
- Improve sysclock accuracy

What enhancements are planned :
- Optimize thread switch function by pushing only call-clobbered registers (https://github.com/greiman/NilRTOS-Arduino/blob/master/libraries/NilRTOS/nilcore.c#L67). Handle switch from interrupt handler (could this help https://github.com/greiman/NilRTOS-Arduino/blob/master/libraries/NilRTOS/nilcore.h#L193 ?).
- Making timers and events process period configurable.
- Using makefile to build the project for a target
- Propose this project as a library
- Fix when submitting the same work two time, while it has not yet been executed -> use double linked lists for (tqueue)
- Measure the execution time for thread switch and all kernel functions calls (k_mutex_lock, k_work_schedule, ...)
- Remove RUNNING state which is implicitely represented by the position of the current thread in the runqueue.
  - A thread is running if it is at the top on the runqueue
  - Also optimize the use of Z_READY
- Optimizing `tqueue_remove` function from O(n) to O(1) where `n` is the number of items in the time queue
  - Optimizing corresponding code in the kernel
- Optimizing timers code
- ~~Unifying `k_timeout_t` and `k_delta_t` types~~
- Move the examples from a directory above
- Allowing `main` thread to return.
- Improve sysclock precision by not resetting TCNTx (check Arduino millis() function for more details)
- Implement Arduino `yield()` function as a wrapper for `k_yield()`
- Read this : https://www.avrfreaks.net/forum/memory-barrier-what-it-does-and-what-it-does-not-do?page=all
- Optimize thread switch for cooperative threads (flags saying if preemtion caused the thread to be switched)

What enhancements/features are not planned :
- Prioritization
- Totally removing CPU idle thread
- Stack for interrupts handlers
- Delay the submission of a work in a workqueue
- Saving thread errno
- Cancel submitted item

## Configuration

This RTOS is highly configurable, see [src/avrtos/avrtos_conf.h](./src/avrtos/avrtos_conf.h) file.

## Getting started example :

### Description

Following code configures an usart, and blink a led at a frequency of 1Hz.
Morover, typing a character on the serial console will wake up a thread.

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
```cpp
/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <avrtos/drivers/usart.h>
#include <avrtos/drivers/gpio.h>

#include <avrtos/logging.h>
#define LOG_LEVEL 	LOG_LEVEL_DBG

#define BUILTIN_LED_PORT 	GPIOB_DEVICE
#define BUILTIN_LED_PIN 	(5u)

static char usart_msgq_buf[16u];
K_MSGQ_DEFINE(usart_msgq, usart_msgq_buf, 1u, 16u);

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
		.baudrate = USART_BAUD_115200,
		.receiver = 1u,
		.transmitter = 1u,
		.mode = USART_MODE_ASYNCHRONOUS,
		.parity = USART_PARITY_NONE,
		.stopbits = USART_STOP_BITS_1,
		.databits = USART_DATA_BITS_8,
		.speed_mode = USART_SPEED_MODE_NORMAL
	};
	ll_usart_init(USART0_DEVICE, &usart_config);
	ll_usart_enable_rx_isr(USART0_DEVICE);

	gpio_pin_init(BUILTIN_LED_PORT, BUILTIN_LED_PIN, GPIO_OUTPUT, 0u);

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
		gpio_pin_toggle(BUILTIN_LED_PORT, BUILTIN_LED_PIN);
		LOG_DBG("<dbg> toggled LED");
		k_sleep(K_MSEC(500u));
	}
}
```

### Logs

```
Application started
===== k_thread =====
L 0x0200 READY   C 1 ____ : SP 22/164:0x0363
X 0x0212 READY   C 1 ____ : SP 22/164:0x0407
M 0x0224 READY   C 1 ____ : SP 0/512:0x21FF
I 0x0236 IDLE    P 1 ____ : SP 22/120:0x02BF
00:00:00 [0.002 s] : <dbg> toggled LED
00:00:00 [0.240 s] : <inf> Received: h
00:00:00 [0.332 s] : <inf> Received: e
00:00:00 [0.431 s] : <inf> Received: l
00:00:00 [0.503 s] : <dbg> toggled LED
00:00:00 [0.532 s] : <inf> Received: l
00:00:00 [0.657 s] : <inf> Received: o
00:00:00 [0.822 s] : <inf> Received:  
00:00:00 [0.915 s] : <inf> Received: w
00:00:01 [1.003 s] : <dbg> toggled LED
00:00:01 [1.055 s] : <inf> Received: o
00:00:01 [1.144 s] : <inf> Received: r
00:00:01 [1.229 s] : <inf> Received: l
00:00:01 [1.312 s] : <inf> Received: d
00:00:01 [1.468 s] : <inf> Received:  
00:00:01 [1.503 s] : <dbg> toggled LED
00:00:01 [1.574 s] : <inf> Received: !
00:00:01 [1.664 s] : <inf> Received: !
00:00:01 [1.756 s] : <inf> Received: !
00:00:02 [2.003 s] : <dbg> toggled LED
00:00:02 [2.503 s] : <dbg> toggled LED
```

Footprint on ATmega2560

```
Memory region         Used Size  Region Size  %age Used
            text:        7574 B       256 KB      2.89%
            data:         591 B         8 KB      7.21%
          eeprom:          0 GB         4 KB      0.00%
```

## All Examples

### Demo
- [shell](./src/examples/shell/main.c)

### Main Features
- [mutex-multithreading-demo](./src/examples/mutex-multithreading-demo/main.c)
- [semaphore-multithreading-demo](./src/examples/semaphore-multithreading-demo/main.c)
- [fifo](./src/examples/fifo/main.c)
- [fifo-slab](./src/examples/fifo-slab/main.c)
- [mem_slab](./src/examples/mem_slab/main.c)
- [msgq](./src/examples/msgq/main.c)
- [signal](./src/examples/signal/main.c)
- [timers](./src/examples/timers/main.c)
- [events](./src/examples/events/main.c)
- [workqueue](./src/examples/workqueue/main.c)
- [prng](./src/examples/prng/main.c)
- [data-structures](./src/examples/data-structures/main.cpp)
- [atomic](./src/examples/atomic/main.c)
- [sched-lock](./src/examples/sched-lock/main.c)
- [sysclock](./src/examples/sysclock/main.c)
- [system-workqueue](./src/examples/system-workqueue/main.c)
- [uptime](./src/examples/uptime/main.c)
- [canaries](./src/examples/canaries/main.c)

### Minor features
- [minimal-example](./src/examples/minimal-example/main.c)
- [prempt-multithreading-demo](./src/examples/prempt-multithreading-demo/main.c)
- [coop-multithreading-demo](./src/examples/coop-multithreading-demo/main.c)
- [multithreading-switching-frequency](./src/examples/multithreading-switching-frequency/main.c)
- [mutex-multithreading-test](./src/examples/mutex-multithreading-test/main.c)
- [object-reservation](./src/examples/object-reservation/main.c)
- [cpp](./src/examples/cpp/main.cpp)
- [thread-termination](./src/examples/thread-termination/main.c)
- [interrupt-yield](./src/examples/interrupt-yield/main.c)
- [idle](./src/examples/idle/main.c)

## Note

### qemu

As [qemu](https://github.com/qemu/qemu) support [avr architecture](https://github.com/qemu/qemu/tree/master/target/avr), any program built using this RTOS can be emulated in qemu and debugged using GDB.

Use of `_delay_ms`, `sleep_cpu` is deprecated in QEMU.

### Overhead
- In term of flash, the overhead is approximately 3KB
- In term of RAM :
  - kernel objects
    - runqueue : 2B
    - events queue : 2B
    - thread idle stack is at least 35/36 byte + 18B thread structure (configurable)
  - a thread structure is 18B + stack size which is at least 35/36byte
        - + 1B if CONFIG_KERNEL_SCHED_LOCK_COUNTER is enabled
        - + 1B if CONFIG_KERNEL_IRQ_LOCK_COUNTER is enabled
  - a mutex is 7B
  - a signal is 6B
  - a semaphore is 6B
  - a workqueue is 7B (system workqueue enabled via SYSTEM_WORKQUEUE)
    - a k_work item is 4B
  - a fifo is 8B
    - a fifo item is at least 2B
  - a memory slab is 11B + allocated buffer
  - a timer is 8B (or 12B with high precision)
    - timer timeout queue is 2B (enabled via CONFIG_KERNEL_TIMERS)
  - a msgq is 15B + allocated buffer
- In term of time, thread switch is between 26µs and 30µs on an 16MHz AVR (will be measured more precisely)
- Plan additionnal stack for every thread that have their interrupt flag set, whose the stack could be used during interrupt handlers calls.
  
### Zephyr RTOS

Inspiration in the naming comes greatly from the project [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr), 
as well as some paradigms and concepts regarding multithreading : [Zephyr : Threads](https://docs.zephyrproject.org/latest/reference/kernel/threads/index.html).
However be carefull, many behavior are different from the ones from Zephyr RTOS ! 

### Features

Some paradigms appear in the code but are actually not implemented for now, e.g. prioritization

### Arduino

This library should be compatible with the Arduino framework but was not tested on it:
- As the `millis()` function from arduino uses timer0 you need to configure the kernel sysclock to use another hardware timer among timer1 (16 bits) and timer2 (8 bits). This should be the only limitation.

- https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring.c#L65-L77

### Git

Git history of the project has been reworked in order to remove big files (as screenshots or disassembly files). Some links can be invalid when working on an old commit.

## PlatformIO

- This project was developed using PlatformIO environnement in Visual Studio Code IDE.
- Each example can be easily built using the *pio* configuration file :
    - Board, e.g. : pro16MHzatmega328, megaatmega2560
    - Choose the linker script depending on the AVR architecture
    - Choose *upload* and *monitor* ports

```ini
[env]
platform = atmelavr
board = pro16MHzatmega328
; board = megaatmega2560

src_filter =
    +<avrtos/>

build_flags = 
    -Wl,-T./avrtos-avr5.xn
    ; -Wl,-T./avrtos-avr6.xn

upload_port = COM3

monitor_port = COM3
monitor_speed = 500000
```

## Known issues

- Set stack pointer only one time, remove instructions `b4` to `ba` (.init2):
  - See linker script
```s
000000b0 <__ctors_end>:
  b0:	11 24       	eor	r1, r1    ; clear r1
  b2:	1f be       	out	0x3f, r1	; clear SREG

  b4:	cf ef       	ldi	r28, 0xFF	; SPL
  b6:	d8 e0       	ldi	r29, 0x08	; SPH
  b8:	de bf       	out	0x3e, r29	; set SPH
  ba:	cd bf       	out	0x3d, r28	; set SPL

  bc:	ce ea       	ldi	r28, 0xAE	; SPL
  be:	d7 e0       	ldi	r29, 0x07	; SPH
  c0:	cd bf       	out	0x3d, r28	; set SPL
  c2:	de bf       	out	0x3e, r29	; set SPH
```

- In order to have a reliable uptime value, interrupts should be enabled in all threads.
  - Disabling interrupt for too long, breaks the uptime accuracy.

## Debugging

### Emulate with QEMU and debug

[qemu](https://www.qemu.org/) implements an emulator for AVR architectures.

My install :
- Environnement : Windows 10
- Ubuntu 20.04 (WSL)
- [qemu v6.1.0](https://github.com/qemu/qemu/tree/v6.1.0)

Steps : 
1. Building project in debug mode : 
  - `build_type = debug`
  - Make sure to use timer1 as syslock as it's the only hardware timer supported for now !

2. Change program location in `launch.json`

3. Emulate on qemu : Run command from WSL :
  `~/qemu/qemu/build/avr-softmmu/qemu-system-avr -M mega2560 -bios .pio/build/Sysclock-qemu-ATmega2560/firmware.elf -s -S -nographic -serial tcp::5678,server=on,wait=off`

4. Attach serial :
  - From WSL : `telnet localhost 5678`
  - From Visual Studio Code IDE : Serial Monitor : `Ctrl + Alt + S`

5. Attach debugger in VSC environnement :
  - `Ctrl + F5`

6. Exit qemu with `Ctrl + A` then `X`

Note that only few peripherals are supported on avr architecture (https://qemu-project.gitlab.io/qemu/system/target-avr.html) :
  - `timer1` : 
    - https://github.com/qemu/qemu/blob/master/hw/timer/avr_timer16.c
    - https://github.com/qemu/qemu/blob/master/include/hw/timer/avr_timer16.h
  - `usart0` :
    - https://github.com/qemu/qemu/blob/master/hw/char/avr_usart.c
    - https://github.com/qemu/qemu/blob/master/include/hw/char/avr_usart.h

But these two peripherals are enough to fully run/debug the kernel with output in console.

Debugging in QEMU : `Sysclock-qemu-ATmega328p` : [sysclock/main.cpp](./src/examples/sysclock/main.cpp) :
![avrtos_qemu1.png](./pics/avrtos_qemu1.png)

Debugging in QEMU : `MutexMultithreadingDemo-qemu-ATmega328p` : [mutex-multithreading-demo/main.cpp](./src/examples/mutex-multithreading-demo/main.cpp) :
![avrtos_qemu2.png](./pics/avrtos_qemu2.png)

If you want to create your own application I advice to define two environnements in `platformio.ini` :

Release env for real board Arduino Pro (ATmega328p): 
```ini
[env:MutexMultithreadingDemo]
src_filter =
    ${env.src_filter}
    +<examples/mutex-multithreading-demo/>

build_flags = 
    ${env.build_flags}
    -DCONFIG_KERNEL_PREEMPTIVE_THREADS=1
    -DCONFIG_KERNEL_TIME_SLICE_US=16000
    -DCONFIG_KERNEL_DEBUG=0
    -DCONFIG_KERNEL_SCHEDULER_DEBUG=0
    -DCONFIG_KERNEL_THREAD_IDLE=1
```

Debug mode for QEMU (ATmega328p)

```ini
[env:MutexMultithreadingDemo-qemu-ATmega328p]
build_type = debug

src_filter =
    ${env.src_filter}
    +<examples/mutex-multithreading-demo/>

build_flags = 
    ${env.build_flags}
    -DCONFIG_KERNEL_SYSLOCK_HW_TIMER=1
    -DCONFIG_KERNEL_TIME_SLICE_US=40000
    -DCONFIG_KERNEL_PREEMPTIVE_THREADS=1
    -DCONFIG_KERNEL_DEBUG=0
    -DCONFIG_KERNEL_SCHEDULER_DEBUG=0
    -DCONFIG_KERNEL_THREAD_IDLE=1

debug_tool = custom
debug_port = :1234
debug_load_mode = manual
monitor_port = socket://localhost:5678
debug_extra_cmds =
    tbreak main
```

## QEMU installation :

- https://www.qemu.org/download/#source

Prerequisites :
```
sudo apt install make, ninja-build, pkg-config, libglib2.0-dev
sudo apt-get install build-essential libcairo2-dev libpango1.0-dev libjpeg-dev libgif-dev librsvg2-dev
```

Install
```
sudo apt install gdb-avr
```


### Console debug

Enabling configuration option `CONFIG_KERNEL_SCHEDULER_DEBUG` enables following logs :
- Before scheduler call :
    - `c` : Meaning that an interrupt planned to call the scheduler but the current thread is in cooperative mode. We restore the current thread same context that was only partially saved.
    - `s` : Afther the scheduler get called, there is still only one thread that is ready to be executed, and we restore the context that was partially saved.
    - `@T` : thread `T` was awakened
- During scheduler call :
    - `!` : The timer of a waiting thread expired and the scheduler poped it off in order to execute it.
    - `~` : Current thread has just been set in WAITING mode and has been removed from the runqueue. The scheduler get the next thread to be executed.
    - `>` : The scheduler simply requeue the current thread and get the next thread to be executed.
    - `p` : Next thread to be executed is the IDLE thread, since there are other threads to be executed we skip the IDLE thread.
    - `M`, `K`, `A`, *Any* : After one of the symbols described above, the next thread symbol is printed in the console.
- On mutex handling:
    - `#T` : current thread `T` is waiting on a mutex
    - `}T`: current thread `T` locked a mutex and is now the owner of it
    - `T{`: current thread `T` unlocked a mutex
- On sempahore handling:
    - `$T` : current thread `T` is waiting on a semaphore
    - `)T` : current thread `T` took a semaphore
    - `T(` : current thread `T` gave a semaphore

Enabling configuration option `CONFIG_KERNEL_SCHEDULER_DEBUG` enables following logs :
- `.` : Each time the syslock timer expires, we check if the current thread can be preempted and then the scheduler is called in.

#### Example with the getting-started example 

```
===== k_thread =====
M 030E [PREE 0] RUNNING : SP 0/512 -| END @0560
C 031E [COOP 1] READY   : SP 35/256 -| END @0207
F 032E [PREE 1] READY   : SP 35/80 -| END @0257
O 033E [PREE 1] READY   : SP 35/80 -| END @02A7
K 034E [PREE 3] READY   : SP 35/62 -| END @02E6
~C~F}Ff~O#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!FF{@O>O}Oo~pF#F~K.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.>Ks.!C_.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c.c!OO{@F>F}Ff~pO#O~K.>Ks.>Ks.>Ks.>K
```

## Optimization

Stacks sizes are tricky to size. Depending when the threads are interrupted, the stack usage can be different and reach high usage very quickly. However following things can help to prevent stack overflow or identify leaks :
- Enable stack canaries (`CONFIG_THREAD_CANARIES`) to have a precise idea of stack usage.
- Enable stack sentinel(`CONFIG_THREAD_STACK_SENTINEL`) to detect stack overflow.
- Using k_sched_lock() arround stack consuming functions.
- Declare thread as COOPERATIVE instead of PREEMPTIVE.
- Use static variables
- Use dynamic allocation (`k_memslab`) for big memory needs.

Example with `MemSlab` :

Priority *PREEMPTIVE* :

```
[0] CANARIES until @0166 [found 39], MAX usage = 72 / 111 + 1 (sentinel)
[1] CANARIES until @01D6 [found 39], MAX usage = 72 / 111 + 1 (sentinel)
[2] CANARIES until @0246 [found 39], MAX usage = 72 / 111 + 1 (sentinel)
[3] CANARIES until @02B6 [found 39], MAX usage = 72 / 111 + 1 (sentinel)
[4] CANARIES until @0304 [found 5], MAX usage = 106 / 111 + 1 (sentinel)
[5] CANARIES until @0396 [found 39], MAX usage = 72 / 111 + 1 (sentinel)
[6] CANARIES until @03EE [found 15], MAX usage = 96 / 111 + 1 (sentinel)
[7] CANARIES until @0458 [found 9], MAX usage = 102 / 111 + 1 (sentinel)
[8] CANARIES until @04C8 [found 9], MAX usage = 102 / 111 + 1 (sentinel)
[9] CANARIES until @0556 [found 39], MAX usage = 72 / 111 + 1 (sentinel)
[M] CANARIES until @079D [found 13], MAX usage = 86 / 99 + 1 (sentinel)
[I] CANARIES until @011B [found 26], MAX usage = 35 / 61 + 1 (sentinel)
```

Priority *COOPERATIVE* :

```
[0] CANARIES until @015E [found 31], MAX usage = 72 / 103 + 1 (sentinel)
[1] CANARIES until @01AC [found 5], MAX usage = 98 / 103 + 1 (sentinel)
[2] CANARIES until @021E [found 15], MAX usage = 88 / 103 + 1 (sentinel)
[3] CANARIES until @027C [found 5], MAX usage = 98 / 103 + 1 (sentinel)
[4] CANARIES until @02E4 [found 5], MAX usage = 98 / 103 + 1 (sentinel)
[5] CANARIES until @034C [found 5], MAX usage = 98 / 103 + 1 (sentinel)
[6] CANARIES until @03CE [found 31], MAX usage = 72 / 103 + 1 (sentinel)
[7] CANARIES until @0426 [found 15], MAX usage = 88 / 103 + 1 (sentinel)
[8] CANARIES until @0484 [found 5], MAX usage = 98 / 103 + 1 (sentinel)
[9] CANARIES until @04EC [found 5], MAX usage = 98 / 103 + 1 (sentinel)
[M] CANARIES until @074D [found 13], MAX usage = 86 / 99 + 1 (sentinel)
[I] CANARIES until @011B [found 26], MAX usage = 35 / 61 + 1 (sentinel)
```

# Todo / ideas :
- Heap, malloc : https://www.nongnu.org/avr-libc/user-manual/malloc.html
- Building using cmake
  - https://github.com/ptrks/CMake-avr-example/blob/master/basic_example/CMakeLists.txt
  - Remove unused code
- Extend drivers support
- Exceptions
- Add a logging system !
- Find a way to not use the serial_print*() function anymore and to use the UART drivers instead
- Get rid of the AVR startup code : [AVR Freaks : Custom Startup Code](https://www.avrfreaks.net/forum/custom-startup-code)
  - Small overhead
- Merge `CONFIG_KERNEL_UPTIME` and `KERNEL_TIME`

## Some links :
- More information on data structures : https://en.wikipedia.org/wiki/Linked_list
- Memory sections on AVR : https://www.nongnu.org/avr-libc/user-manual/mem_sections.html
- avr-gcc : https://gcc.gnu.org/wiki/avr-gcc
- Pseudo-ops and operators : https://www.nongnu.org/avr-libc/user-manual/assembler.html#ass_pseudoop
- AVR Instruction Set Manual : http://ww1.microchip.com/downloads/en/devicedoc/atmel-0856-avr-instruction-set-manual.pdf
- cross-referencer example : https://elixir.bootlin.com/zephyr/v2.6.1-rc1/source/kernel/sched.c
- cmake-avr github repository : https://github.com/mkleemann/cmake-avr

## Troubleshooting

- If your program crashes/restart/gets stuck, just increase all stack sizes, just in case (especially with `CONFIG_KERNEL_TIMERS`, `CONFIG_KERNEL_EVENTS`).
  - Note: Also increase IDLE stack size with `CONFIG_KERNEL_THREAD_IDLE_ADD_STACK`, IDLE stack is tiny by default
  - Note: function call with ATmge2560 for example are stack consuming (because of the 3B return addresses)

## clang-format

- Install clang-format, and run `make format` to format all files
- Or install `xaver.clang-format` extension for VS Code, and format on save or
format using `Ctrl + K, Ctrl + F`

## CMake

It's now possible to build the project with CMake.
- Install `CMake Tools` extension for VS Code, or simply execute `make build && cd build && make`

- When all samples are compiled for the given target, you can either :
  - flash the program using `upload_sample_xxx`
  - or run the program in `QEMU` with `run_sample_xxx`
  - or even debug the program in `QEMU` with `qemu_sample_xxx` and then `Ctrl + F5` (VS Code)
- Then `Ctrl-A`+ `X` to exit `QEMU`

**Note: When running on QEMU, make sure to have compiled the sample with `__QEMU__` defined.**

### TODO
- Move ARCH, MCU, PORT, F_CPU, QEMU out of the general .cmake toolchain file
- Synchronize platformio.ini and CMakeLists.txt flags -> enhance python script `pio_export_defflags.

## Version

```
avr-gcc (Fedora 11.2.0-1.fc36) 11.2.0
avr-gdb: GNU gdb (GDB) 12.1
QEMU emulator version 7.0.0 (v7.0.0)
avrdude version 6.4
cmake version 3.22.2
GNU Make 4.3
Python 3.10.5
```