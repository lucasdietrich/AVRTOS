# "Unix Makefiles" / "make" / "--no-print-directory"
# GENERATOR="Ninja"
# GENERATOR_COMMAND="ninja"
# GENERATOR_ARGS=""
GENERATOR?="Unix Makefiles"
GENERATOR_COMMAND?="make"
GENERATOR_ARGS?="--no-print-directory"


DEVICE?=/dev/ttyACM0
SINGLE_SAMPLE?=shell
TOOLCHAIN_FILE?=cmake/avr6-atmega2560.cmake
BAUDRATE?=500000
QEMU?=OFF

# if QEMU is enabled, set CMAKE_BUILD_TYPE to Debug
ifeq ($(QEMU),ON)
	CMAKE_BUILD_TYPE?=Debug
else
	CMAKE_BUILD_TYPE?=Release
endif

all: single

.PHONY: single cmake

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
		-DENABLE_SINGLE_SAMPLE=$(SINGLE_SAMPLE) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU) \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	$(GENERATOR_COMMAND) -C build $(GENERATOR_ARGS)

upload:
	$(GENERATOR_COMMAND) -C build upload $(GENERATOR_ARGS)

monitor:
	echo "Press Ctrl-T + Q to exit"
	python3 -m serial.tools.miniterm $(DEVICE) $(BAUDRATE) --raw --eol LF

format:
	find src -iname *.h -o -iname *.c -o -iname *.cpp | xargs clang-format -i

clean:
	rm -rf build

piogen:
	python3 ./scripts/piogen.py

arduinogen:
	python3 ./scripts/arduinogen.py

gen: format piogen arduinogen