#!/usr/bin/bash

set -e

cmake -S . -B build_slave \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake \
	-DPROG_DEV=/dev/ttyACM0 \
	-DCMAKE_BUILD_TYPE=Release \
	-DENABLE_SINGLE_SAMPLE=drv-spi-slave \

make -C build_slave

# parse first argument
# if it is "flash" then flash the device
if [ "$1" == "flash" ]; then
	make -C build_slave upload
elif [ "$1" == "monitor" ]; then
	python3 -m serial.tools.miniterm /dev/ttyACM0 115200 --raw --eol LF
fi