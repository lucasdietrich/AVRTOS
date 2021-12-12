# AVRTOS : RTOS attempt for 8bits AVR microcontrollers

## Introduction

This project is an attempt to create a real time operating system (RTOS) for AVR 8 bits microcontrollers.

Following avr architectures are supported/tested :
- avr5, especially ATmega328p : tested with an Arduino PRO
- avr6, especially ATmega2560 : tested with an Arduino Mega2560

Following features are supported:
- Cooperative threads
- Preemptive threads with time slice between 1 ms and 16 ms (timer 0 and timer 2) up to 4 seconds (timer 1)
- Configurable sysclock timer among hardware timers : timer0 (8bits), timer1 (16 bits), timer2 (8 bits)
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
- Atomic operations on 8bits variables

Minor features:
- thread naming with a symbol, e.g. 'M' for the main thread 'I' for the idle thread 
- thread context passing
- debug/utils functions : RAM_DUMP, CORE_DUMP, read_ra (read return address)
- data structures : 
  - dlist : doubly linked list
    - raw API
    - dlist API : queue
    - cdlist API : circular dlist
  - queue : singly linked list
  - oqueue : optimized singly linked list
  - tqueue : scheduling singly linked list for time-sorted items
- I/O : leds, uart
- Kernel Assertions (__ASSERT)
- Custom errors code: e.g. : EAGAIN, EINVAL, ETIMEOUT
- Efficient pending feature, allowing to pass directly an object (mutex, semaphore, mem slab bloc, fifo item) to a pending thread. No need to do the whole process : unlock/lock for a mutex or free/allocate for a memory block if a thread is pending for the object being available.
- Mutex thread owner
- Fully C/C++ compliant
- Allow thread safe termination (doesn't work for `main` thread yet).
- Project fully PlatformIO compatible for Windows and Linux
- Redirection of stdout to USART0 (e.g. printf)

What paradigms/concepts are not supported:
- Nested interrupts
- Dedicated stack for interrupt handlers
- Thread local storage (removed)

What features will be implemented :
- **Optimized circular buffer** / **Stream** / **Asymetric put/get**
- Statistics CPU use per thread
- Delayed start, suspending/resuming threads
- Stack sentinels
- Task scheduling
- Kernel fault

What enhancements are planned :
- Using makefile to build the project for a target
- Propose this project as a library
- Fix when submitting the same work two time, while it has not yet been executed -> use double linked lists for (tqueue)
- Measure the execution time for thread switch and all kernel functions calls (k_mutex_lock, k_work_schedule, ...)
- Remove RUNNING state which is implicitely represented by the position of the current thread in the runqueue.
  - A thread is running if it is at the top on the runqueue
  - Also optimize the use of READY
- Optimizing `tqueue_remove` function from O(n) to O(1) where `n` is the number of items in the time queue
  - Optimizing corresponding code in the kernel
- Optimizing timers code
- Unifying `k_timeout_t` and `k_delta_ms_t` types
- Move the examples from a directory above
- Allowing `main` thread to return.

What enhancements/features are not planned :
- Prioritization
- Totally removing CPU idle thread
- Stack for interrupts handlers
- Delay the submission of a work in a workqueue
- Saving thread errno
- Cancel submitted item

## Getting started example :

### Description

In this example, we spawn three threads (+ main thread + idle thread) :
- Two preemptive threads : one set the led ON for 100ms and the other to OFF for the same duration
    - A mutex is protecting the LED access, and both threads keeps the mutex locked while waiting for 100ms.
    - both threads use the same reentrant function
- One cooperative threads blocks the execution of all threads for 500ms every 2 seconds.
- The main thread releases the CPU forever when initialization finished

On an Arduino Pro (or Arduino Pro Mini), based on an ATmega328p (avr5) the led should be blinking at the frequency of 5Hz and then block for 500ms every 2 seconds.

Configuration option : `CONFIG_KERNEL_TIME_SLICE=10`

### Code

```cpp
#include <avr/io.h>
#include <util/delay.h>
#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>

void thread_led(void* context);
void thread_coop(void* context);

uint8_t on = 1u;
uint8_t off = 0u;

K_MUTEX_DEFINE(mymutex);  // mutex protecting LED access
K_THREAD_DEFINE(ledon, thread_led, 0x50, K_PREEMPTIVE, (void*)&on, 'O');
K_THREAD_DEFINE(ledoff, thread_led, 0x50, K_PREEMPTIVE, (void*)&off, 'F');
K_THREAD_DEFINE(coop, thread_coop, 0x100, K_PREEMPTIVE, NULL, 'C');

int main(void)
{
  led_init();
  usart_init();
  k_thread_dump_all();
  sei();
  k_sleep(K_FOREVER);
}

void thread_led(void* context)
{
  const uint8_t thread_led_state = *(uint8_t*)context;
  while (1)   {
    k_mutex_lock(&mymutex, K_FOREVER);
    led_set(thread_led_state);
    usart_transmit(thread_led_state ? 'o' : 'f');
    k_sleep(K_MSEC(100));
    k_mutex_unlock(&mymutex);
  }
}

void thread_coop(void* context)
{
  while (1)   {
    k_sleep(K_MSEC(2000));
    usart_transmit('_');
    _delay_ms(500); // blocking all threads for 500ms
  }
}
```

### Logs

```
===== k_thread =====
M 031C [PREE 0] RUNNING : SP 0/512 -| END @056E
C 032C [COOP 1] READY   : SP 35/256 -| END @0207
F 033C [PREE 1] READY   : SP 35/80 -| END @0257
O 034C [PREE 1] READY   : SP 35/80 -| END @02A7
K 035C [PREE 3] READY   : SP 35/62 -| END @02E6
fofofofofofofofofofo_fofofofofofofofofofo_fofofofofofofofofofo_fofof
```

## All Examples

### Demo
- [poc-shell](./src/examples/poc-shell/main.c)

### Main Features
- [mutex-multithreading-demo](./src/examples/mutex-multithreading-demo/main.c)
- [semaphore-multithreading-demo](./src/examples/semaphore-multithreading-demo/main.c)
- [fifo](./src/examples/fifo/main.c)
- [fifo-slab](./src/examples/fifo-slab/main.c)
- [mem_slab](./src/examples/mem_slab/main.c)
- [msgq](./src/examples/msgq/main.c)
- [signal](./src/examples/signal/main.c)
- [timers](./src/examples/timers/main.c)
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

### Peripherals

- This library only needs one timer among following hardware timers :
  - timer0 : allow KERNEL_TIME_SLICE between 1 and 16 milliseconds
  - timer1 : allow KERNEL_TIME_SLICE between 1 and 16 milliseconds and 20ms, 25ms, 50ms, 100ms, 200ms, 250ms, 500ms, 1s, 2s, 2500ms, 3s, 4s
  - timer2 : allow KERNEL_TIME_SLICE between 1 and 16 milliseconds

The hardware timer used can be configured with CONFIG_KERNEL_SYSLOCK_HW_TIMER configuration option.

- If configuration option KERNEL_DEBUG_PREEMPT_UART is enabled, the usart0 peripheral is used (+ RX interrupt vector)

### qemu

As [qemu](https://github.com/qemu/qemu) support [avr architecture](https://github.com/qemu/qemu/tree/master/target/avr), any program built using this RTOS can be emulated in qemu and debugged using GDB.

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
    - timer timeout queue is 2B (enabled via KERNEL_TIMERS)
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

## Configuration options
| Configuration option | Description |
| --- | --- |
| THREAD_MAIN_THREAD_PRIORITY   | Define the main thread type (coop/prempt) and priority |
| THREAD_EXPLICIT_MAIN_STACK    | Tells if the main stack location and size must be defined at compilation time (1), or if the default main stack behaviour (stack at RAMEND) should be kept (0).
| THREAD_MAIN_STACK_SIZE | In the case we defined (EXPLICIT_MAIN_STACK == 1), this configuration option defines the size of the main stack |
| THREAD_USE_INIT_STACK_ASM | Tells if we should use the C or the Assembler function to define our threads at runtime |
| KERNEL_HIGH_RANGE_TIME_OBJECT_U32 | Configure to use uint32_t as k_delta_ms_t ~= 50 days or keep (uint16_t) ~= 65seconds |
| THREAD_DEFAULT_SREG | Default SREG value for other thread on stack creation. Main thread default SREG is always 0 |
| KERNEL_DEBUG | Enable Kernel Debug features |
| KERNEL_API_NOINLINE | Enable Kernel debug for function, that set some of them noinline |
| KERNEL_SCHEDULER_DEBUG | Enable Kernel Debug in scheduler |
| KERNEL_PREEMPTIVE_THREADS | Enable preemtive threads feature |
| KERNEL_TIME_SLICE | Time slice in milliseconds |
| KERNEL_SYSLOCK_HW_TIMER | Select Hardware timer among 8 bits timers : timer0 (0) and timer2 (2) and 16 bit timer : timer1 (1) |
| KERNEL_SYSCLOCK_AUTO_INIT | Auto start kernel sysclock |
| KERNEL_AUTO_INIT | Kernel auto init, if possible (i.e. not in *.a* lib) |
| KERNEL_DEBUG_PREEMPT_UART | Use uart rx interrupt as preempt signal |
| KERNEL_THREAD_IDLE | Tells if the kernel should define a idle thread to permit all user defined threads to be in waiting/pending status |
| KERNEL_THREAD_IDLE_ADD_STACK | Kernel thread idle addtionnal stack |
| KERNEL_ALLOW_INTERRUPT_YIELD |  Allow interrupt yield, this forces to add more stack to idle thread, since it is possible to save the current interrupt context while being in idle thread this happens often. |
| THREAD_CANARIES | Enable thread canaries |
| THREAD_CANARIES_SYMBOL | Define thread canaries symbol |
| SYSTEM_WORKQUEUE_ENABLE | Enable system workqueue |
| SYSTEM_WORKQUEUE_STACK_SIZE | Define system workqueue stack size |
| SYSTEM_WORKQUEUE_PRIORITY | Define system workqueue thread priority |
| KERNEL_ASSERT | Enable kernel assertion test for debug purpose |
| **~~KERNEL_YIELD_ON_UNPEND~~** | Tells if function _k_unpend_first_thread should immediately switch to the first waiting thread when the object become  available. | 
| THREAD_ALLOW_RETURN | Tells if thread can terminate (need additionnal 2 or 3 bytes per stacks) |
| KERNEL_TIMERS | Enables timers |
| KERNEL_SCHED_LOCK_COUNTER | Enable scheduler lock counter for each thread. |
| KERNEL_IRQ_LOCK_COUNTER | Enable interrupt lock counter for each thread. |
| STDIO_PRINTF_TO_USART | Redirect STDIO output to specified USART |
| KERNEL_UPTIME | Enable uptime counter (ms) feature |
| KERNEL_UPTIME_40BITS | Enable 40 bits timer counter (ms), extends maximum uptime to ~35 years instead of ~47days with the 32bits counter. |
| KERNEL_MAX_SYSCLOCK_PERIOD_MS | Define the maximum period of the sysclock in ms. Normally, the period is automatically calculated from KERNEL_TIME_SLICE but if a higher precision is required for the uptime (in ms). The syslock period can be adjusted independently from thread switch period (KERNEL_TIME_SLICE). |

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
  
2. Emulate on qemu : Run command from WSL :
  `~/qemu/qemu/build/avr-softmmu/qemu-system-avr -M mega2560 -bios .pio/build/Sysclock-qemu-ATmega2560/firmware.elf -s -S -nographic -serial tcp::5678,server=on,wait=off`

3. Attach serial :
  - From WSL : `telnet localhost 5678`
  - From Visual Studio Code IDE : Serial Monitor : `Ctrl + Alt + S`

4. Attach debugger in VSC environnement :
  - `Ctrl + F5`

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
    -DCONFIG_KERNEL_TIME_SLICE=16
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
    -DCONFIG_KERNEL_TIME_SLICE=4
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

Enabling configuration option `KERNEL_SCHEDULER_DEBUG` enables following logs :
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

Enabling configuration option `KERNEL_SCHEDULER_DEBUG` enables following logs :
- `.` : Each time the syslock timer expires, we check if the current thread can be preempted and then the scheduler is called in.
    - If configuration option `KERNEL_DEBUG_PREEMPT_UART` is set, syslock timer overflow handler is replaced by the uart rx interrupt handler.

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

# Ideas: 
- Malloc : https://www.nongnu.org/avr-libc/user-manual/malloc.html
- Exceptions

## Some links :
- More information on data structures : https://en.wikipedia.org/wiki/Linked_list
- Memory sections on AVR : https://www.nongnu.org/avr-libc/user-manual/mem_sections.html
- avr-gcc : https://gcc.gnu.org/wiki/avr-gcc
- Pseudo-ops and operators : https://www.nongnu.org/avr-libc/user-manual/assembler.html#ass_pseudoop
- AVR Instruction Set Manual : http://ww1.microchip.com/downloads/en/devicedoc/atmel-0856-avr-instruction-set-manual.pdf
- cross-referencer example : https://elixir.bootlin.com/zephyr/v2.6.1-rc1/source/kernel/sched.c