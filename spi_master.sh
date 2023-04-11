#!/usr/bin/bash

set -e

cmake -S . -B build_master \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_TOOLCHAIN_FILE=cmake/avr5-atmega328p-ethernet.cmake \
	-DPROG_DEV=/dev/ttyACM1 \
	-DCMAKE_BUILD_TYPE=Release \
	-DENABLE_SINGLE_SAMPLE=drv-spi-master \

make -C build_master

# parse first argument
# if it is "flash" then flash the device
if [ "$1" == "flash" ]; then
	make -C build_master upload
elif [ "$1" == "monitor" ]; then
	python3 -m serial.tools.miniterm /dev/ttyACM1 500000 --raw --eol LF
fi