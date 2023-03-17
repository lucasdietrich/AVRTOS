#
# Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#

set(F_CPU 16000000UL)
set(MCU atmega2560)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/../architecture/avr/avrtos-avr6.xn)
set(QEMU_MCU mega2560)
set(PROG_TYPE wiring)
set(PROG_PARTNO m2560)

include(${CMAKE_CURRENT_LIST_DIR}/avr.cmake)