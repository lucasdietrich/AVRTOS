#
# Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#

set(F_CPU 16000000UL)
set(MCU atmega328p)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/../architecture/avr/avrtos-avr5-atmega328p.xn)
set(QEMU_MCU uno)
set(PROG_TYPE wiring)
set(PROG_PARTNO m328pb)

set(FEATURE_TIMER_COUNT 5)
set(FEATURE_USART_COUNT 2)

include(${CMAKE_CURRENT_LIST_DIR}/avr.cmake)