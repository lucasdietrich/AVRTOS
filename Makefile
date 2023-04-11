# "Unix Makefiles" / "make" / "--no-print-directory"
# GENERATOR="Ninja"
# GENERATOR_COMMAND="ninja"
# GENERATOR_ARGS=""
GENERATOR?="Unix Makefiles"
GENERATOR_COMMAND?="make"
GENERATOR_ARGS?="--no-print-directory"


DEVICE?=/dev/ttyACM0
SINGLE_SAMPLE?=drv-spi-master
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

# spi: cmake
# 	make -C build sample_drv_spi_master
# 	make -C build sample_drv_spi_slave
	
# 	avr-objcopy -R .eeprom -O ihex build/examples/drv-spi-master/sample_drv_spi_master build/examples/drv-spi-master/sample_drv_spi_master.hex
# 	avr-objcopy -R .eeprom -O ihex build/examples/drv-spi-slave/sample_drv_spi_slave build/examples/drv-spi-slave/sample_drv_spi_slave.hex

# 	avrdude -c wiring -p m2560 -P /dev/ttyACM0 -U flash:w:build/examples/drv-spi-master/sample_drv_spi_master.hex
# 	avrdude -c wiring -p m328p -P /dev/ttyACM1 -U flash:w:build/examples/drv-spi-slave/sample_drv_spi_slave.hex

gen: format piogen arduinogen