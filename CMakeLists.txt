#
# Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20)

project(avrtos-project)

# Important to build assembly code
enable_language(C CXX ASM)

# set( CMAKE_VERBOSE_MAKEFILE on )

set(CMAKE_BUILD_TYPE Debug)
set(QEMU OFF)
set(PROG_DEV "/dev/ttyACM2")
SET(ENABLE_SINGLE_SAMPLE "drv-exti")
# SET(ENABLE_SINGLE_SAMPLE "drv-gpio")

add_subdirectory(src/examples)