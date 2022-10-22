# Unix Makefiles or Ninja
generator="Unix Makefiles"
sample="isr-unpend"

all: mega2560 build_all

mega2560:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake \
		-DCMAKE_GENERATOR=$(generator) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DENABLE_SINGLE_SAMPLE=$(sample)

build_all:
	make -C build --no-print-directory

atmega328p:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_TOOLCHAIN_FILE=cmake/avr5-atmega328p.cmake \
	-DCMAKE_GENERATOR=$(generator) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

upload:
	make -C build upload --no-print-directory

monitor:
	python3 -m serial.tools.miniterm /dev/ttyACM0 500000

clean:
	rm -rf build