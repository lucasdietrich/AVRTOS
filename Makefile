# GENERATOR?="Unix Makefiles"
# GENERATOR_COMMAND?="make"
# GENERATOR_ARGS?="--no-print-directory"

GENERATOR?="Ninja"
GENERATOR_COMMAND?="ninja"
GENERATOR_ARGS?=

DEVICE?=/dev/ttyACM0
SAMPLE?=shell
TOOLCHAIN_FILE?=cmake/avr6-atmega2560.cmake
BAUDRATE?=115200
QEMU?=ON

# if QEMU is enabled, set CMAKE_BUILD_TYPE to Debug by default
ifeq ($(QEMU),ON)
	CMAKE_BUILD_TYPE?=Debug
else
	CMAKE_BUILD_TYPE?=Release
endif

all: single

.PHONY: single cmake multiple upload monitor qemu run_qemu format clean piogen arduino_gen gen

cmake:
	cmake -S . -B build \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DCMAKE_GENERATOR=$(GENERATOR) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU) \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

multiple:
	cmake -S . -B build \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DCMAKE_GENERATOR=$(GENERATOR) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU) \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	$(GENERATOR_COMMAND) -C build $(GENERATOR_ARGS)

single:
	cmake -S . -B build \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DCMAKE_GENERATOR=$(GENERATOR) \
		-DENABLE_SINGLE_SAMPLE=$(SAMPLE) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU) \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	$(GENERATOR_COMMAND) -C build $(GENERATOR_ARGS)

upload:
	$(GENERATOR_COMMAND) -C build upload $(GENERATOR_ARGS)

monitor:
	$(GENERATOR_COMMAND) -C build monitor $(GENERATOR_ARGS)

qemu:
	$(GENERATOR_COMMAND) -C build qemu $(GENERATOR_ARGS)

run_qemu:
	$(GENERATOR_COMMAND) -C build run_qemu $(GENERATOR_ARGS)

format:
	find src -iname *.h -o -iname *.c -o -iname *.cpp | xargs clang-format -i

clean:
	rm -rf build

piogen:
	python3 ./scripts/piogen.py

arduino_gen:
	python3 ./scripts/arduinogen.py

arduino_lint:
	arduino-lint --compliance strict --library-manager update
	
metrics:
	scripts/metrics-example-sizes.sh

gen: format piogen arduino_gen