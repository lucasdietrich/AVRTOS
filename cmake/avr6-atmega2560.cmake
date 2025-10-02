#
# Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
#
# SPDX-License-Identifier: Apache-2.0
#

set(F_CPU 16000000UL)
set(MCU atmega2560)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/../architecture/avr/avrtos-avr6.xn)
set(QEMU_MCU mega2560)
set(PROG_TYPE wiring)
set(PROG_PARTNO m2560)

set(FEATURE_TIMER_COUNT 6)
set(FEATURE_USART_COUNT 4)

include(${CMAKE_CURRENT_LIST_DIR}/avr.cmake)