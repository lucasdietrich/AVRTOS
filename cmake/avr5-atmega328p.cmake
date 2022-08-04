set(F_CPU 16000000UL)
set(MCU atmega328p)
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/../arch/avr/avrtos-avr5.ld)
set(QEMU_MCU uno)
set(PROG_TYPE wiring)
set(PROG_PARTNO m328p)

include(${CMAKE_CURRENT_LIST_DIR}/avr.cmake)