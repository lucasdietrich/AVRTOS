#
# Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
#
# SPDX-License-Identifier: Apache-2.0
#

# Ignore this toolchain file as it is specific to the following personal project:
# https://github.com/lucasdietrich/caniot-device

include(${CMAKE_CURRENT_LIST_DIR}/avr5-atmega328p.cmake)

set(PROG_TYPE arduino)

include(${CMAKE_CURRENT_LIST_DIR}/avr.cmake)