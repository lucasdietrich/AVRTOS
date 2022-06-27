build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake -DCMAKE_GENERATOR="Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

monitor:
	python3 -m serial.tools.miniterm -p /dev/ttyACM0 --baud 500000

clean:
	rm -rf build