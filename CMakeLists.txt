cmake_minimum_required(VERSION 3.20)

project(avrtos-project)

enable_language(C CXX ASM)


# include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake")
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake)
# include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake)

set(QEMU ON)

# set(F_CPU 16000000UL)
# set(MCU atmega2560)
set(QEMU_MCU mega2560)

set(BAUDRATE 500000)
set(PROG_DEV "/dev/ttyACM0")

if (QEMU)
	set(QEMU_FLAG "-D__QEMU__")
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

add_definitions(
    -DF_CPU=${F_CPU}
    ${QEMU_FLAG}
)

add_subdirectory(src/examples)