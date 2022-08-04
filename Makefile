generator="Unix Makefiles"

all: mega2560

mega2560:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake -DCMAKE_GENERATOR=$(generator) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

atmega328p:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/avr5-atmega328p.cmake -DCMAKE_GENERATOR=$(generator) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

monitor:
	python3 -m serial.tools.miniterm /dev/ttyACM2 500000

clean:
	rm -rf build