# Developper notes

## clang-format

- Install clang-format, and run `make format` to format all files
- Or install `xaver.clang-format` extension for VS Code, and format on save or
format using `Ctrl + K, Ctrl + F`

## Troubleshooting

- Note that `qemu_*` like targets override the file [.vscode/launch.json](.vscode/launch.json) each time they are run.
- `qemu_*` like targets run QEMU with `-s -S` options, which means QEMU will wait for a debugger to connect on port 1234, while `run_` targets run QEMU directly.
- If your program crashes/restart/gets stuck, just increase all stack sizes, just in case (especially with `CONFIG_KERNEL_TIMERS`, `CONFIG_KERNEL_EVENTS`).
  - Note: Also increase IDLE stack size with `CONFIG_KERNEL_THREAD_IDLE_ADD_STACK`, IDLE stack is tiny by default
  - Note: function call with ATmge2560 for example are stack consuming (because of the 3B return addresses)
- Please note that, when using VS Code with PIO, `c_cpp_properties.json` file is frequently overwritten by PIO extension, which is not convenient when working with `cmake`.
- Disassembly files can be found :
	- in `build/examples/${sample}` folder for `cmake` build
	- in `tmp/${sample}/*` folder for PlatformIO build, after running `python3 ./scripts/pydis.py`
- In case python `miniterm` package is not installed, install python dependencies with `pip3 install -r scripts/requirements.txt`
- Interesting flags for QEMU avr (modify the `QEMU_ARGS` variable in the toolchain file [cmake/avr.cmake](cmake/avr.cmake)):
  - `-d in_asm`
  - `-d exec`
  - `-d cpu`
  - `-d cpu_reset`
  - All timer traces `--trace "avr_timer16_*"`
  - `-d trace:avr_timer16_read,trace:avr_timer16_read_ifr,trace:avr_timer16_read_imsk,trace:avr_timer16_write,trace:avr_timer16_write_imsk,trace:avr_timer16_write_imsk,trace:avr_timer16_clksrc_update`
  - Or a combinations of all: `-d cpu,in_asm,trace:avr_timer16_read`

## Miscelaneous

- Calculate code metrics with `make metrics`, following metrics are calculated:
	- Flash/ram usage for each (cmake) sample: [docs/metrics/exsizes.txt](./docs/metrics/exsizes.txt)
- To update Arduino samples from the *cmake* ones, run `make arduino_gen`
- To generate the `platformio.ini` file from the *cmake* samples, run `make piogen`.
- Run `make arduino_lint` to check the project is compliant with Arduino Library
- To change the default generator to *make* for example, override following variable in the [Makefile](Makefile)
```
GENERATOR?="Unix Makefiles"
GENERATOR_COMMAND?="make"
GENERATOR_ARGS?="--no-print-directory"
```

## Publishing new release

1. Write changelog for the new release in [`changelogs.md`](changelogs.md)
2. Bump version:
    - In [`src/avrtos/defines.h`](src/avrtos/defines.h), update `AVRTOS_VERSION_MAJOR`, `AVRTOS_VERSION_MINOR` and `AVRTOS_VERSION_REVISION`
    - In [`library.properties`](library.properties), update `version`
3. Run `make publish` and make sure
    - No errors/warnings are generated
    - metrics are generated properly
    - Arduino library is valid and can be properly imported in Arduino IDE
    - Examples works
4. Commit your changes, push to main branch then create a new tag with the version number prefixed with `v` (e.g. `v1.1.0`)

## Publishing Arduino Example

Let's suppose you want to publish a new example named `MyExample`.

1. Create a new directory named `ArduinoPIOMyExample` in `examples` directory.
  It must be prefixed with `ArduinoPIO`
1. Create a file `MyExample.c` or `MyExample.cpp` in this directory.
2. Write your example in this file with following structure:

```c
/* <avrtos.h> must be the very first <avrtos/*> include, this
 * activates the library in the Arduino IDE */
#include <avrtos.h>
#include <avrtos/drivers/i2c.h>
#include <avrtos/drivers/spi.h>

/* Define your kernel objects here

 * Don't use macros which requires CONFIG_AVRTOS_LINKER_SCRIPT to be defined
 * (e.g. K_THREAD_DEFINE, K_MSGQ_DEFINE, K_WORKQUEUE_DEFINE ...).
 * 
 * These objects must be properly initialized in setup() function.
 */
static struct k_thread my_thread;
static uint8_t my_thread_stack[256];

// these macros works without CONFIG_AVRTOS_LINKER_SCRIPT
K_MUTEX_DEFINE(my_mutex);
K_SEM_DEFINE(my_sem, 0, 1);
K_MSGQ_DEFINE(my_msgq, 4, 4);
K_WORK_DEFINE(my_work, my_work_handler);
K_WORK_DELAYABLE_DEFINE(my_delayable_work, my_work_handler);
K_FLAGS_DEFINE(my_flags, 0u);
K_SIGNAL_DEFINE(my_signal);
K_ATOMIC_DEFINE(my_atomic, 0);

void setup() {
    /* Your setup code here */
    serial_init();
    led_init();

    k_thread_create(&my_thread, my_thread_task, my_thread_stack,
                        sizeof(my_thread_stack), K_COOPERATIVE, NULL, 'X');
    k_thread_start(&my_thread);

    // k_thread_set_priority(k_thread_get_main(), K_PREEMPTIVE);
}

void loop() {
    /* Your loop code here */

    /* Make sure to call k_yield() at least once per loop or make the main thread
    * preemptive with k_thread_set_priority(k_thread_get_main(), K_PREEMPTIVE);
    */
    k_idle();
}

void my_thread_task(void *arg) {
    /* Your thread code here */
    for (;;) {
        k_sleep(K_SECONDS(1));
        led_toggle();
    }
}
```

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

## Crosstoold-ng

avr-gcc version >= 11.2.0 is recommanded, if you this version of the package `avr-gcc`
is not available in your distribution, you should consider building it from source
with [crosstool-ng](https://crosstool-ng.github.io/).

Steps to build avr-gcc with crosstool-ng:

```bash
git clone https://github.com/crosstool-ng/crosstool-ng
cd crosstool-ng
git checkout crosstool-ng-1.26.0
./bootstrap
./configure --prefix=/opt/crosstool-ng
make
make install
export PATH="${PATH}:/opt/crosstool-ng/bin"
./ct-ng avr
./ct-ng build
```

Use the generated toolchain in `~/x-tools/avr` in your project with:

```bash
export PATH="~/x-tools/avr/bin:${PATH}"
```

## My versions

=== "Fedora"

    ```
    avr-gcc (Fedora 11.2.0-1.fc36) 11.2.0
    avr-gdb: GNU gdb (GDB) 12.1
    QEMU emulator version 7.0.0 (v7.0.0)
    avrdude version 6.4
    cmake version 3.25.1
    ninja 1.10.2
    GNU Make 4.3
    Python 3.10.9
    ```

=== "Debian"

    Requirements:
    
    sudo apt install gcc-avr gdb-avr avr-libc avrdude cmake ninja-build python3 python3-pip
    python3 -m venv .venv
    source .venv/bin/activate
    pip install -r scripts/requirements.txt