# "Unix Makefiles" or "Ninja"
GENERATOR="Unix Makefiles"

DEVICE=/dev/ttyACM0
SINGLE_SAMPLE=shell
TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake
BAUDRATE=500000
QEMU=OFF

all: single

.PHONY: single cmake

cmake:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DCMAKE_GENERATOR=$(GENERATOR) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU)

multiple:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DCMAKE_GENERATOR=$(GENERATOR) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU)
	make -C build --no-print-directory

single:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DCMAKE_GENERATOR=$(GENERATOR) \
		-DENABLE_SINGLE_SAMPLE=$(SINGLE_SAMPLE) \
		-DPROG_DEV=$(DEVICE) \
		-DQEMU=$(QEMU)
	make -C build --no-print-directory

upload:
	make -C build upload --no-print-directory

monitor:
	echo "Press Ctrl-T + Q to exit"
	python3 -m serial.tools.miniterm $(DEVICE) $(BAUDRATE)

format:
	find src -iname *.h -o -iname *.c -o -iname *.cpp | xargs clang-format -i

clean:
	rm -rf build