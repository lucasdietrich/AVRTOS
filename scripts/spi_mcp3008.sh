#!/usr/bin/bash

set -e

cmake -S . -B build_mcp3008 \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_TOOLCHAIN_FILE=cmake/avr5-board-caniot-tiny.cmake \
	-DPROG_DEV=/dev/ttyACM0 \
	-DCMAKE_BUILD_TYPE=Release \
	-DENABLE_SINGLE_SAMPLE=drv-spi-mcp3008 \

make -C build_slave

# parse first argument
# if it is "flash" then flash the device
if [ "$1" == "flash" ]; then
	make -C build_mcp3008 upload
elif [ "$1" == "monitor" ]; then
	python3 -m serial.tools.miniterm /dev/ttyACM0 500000 --raw --eol LF
elif [ "$1" == "clean" ]; then
	rm -rf build_mcp3008
fi