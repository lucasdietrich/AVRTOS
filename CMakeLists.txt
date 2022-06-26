cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake)
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

###############################################################################
project(blinking-led)

enable_language(C CXX ASM)

add_definitions(
    -DF_CPU=${F_CPU}
)

set(PROG_DEV "/dev/ttyACM0")

###############################################################################

set(EXE ${PROJECT_NAME})
set(SOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_AVRTOS "${SOURCES_PATH}/avrtos/*.c**")
file(GLOB_RECURSE SRC_ASM "${SOURCES_PATH}/avrtos/*.S")
file(GLOB SRC_APP "${SOURCES_PATH}/examples/blinking-led/main.c")

add_executable(${EXE} ${SRC_AVRTOS} ${SRC_ASM} ${SRC_APP})
set_target_properties(${EXE} PROPERTIES OUTPUT_NAME ${EXE}.elf)

# AVRTOS Configuration
target_compile_definitions(${EXE} PUBLIC
	"CONFIG_THREAD_EXPLICIT_MAIN_STACK=0"
	"CONFIG_KERNEL_PREEMPTIVE_THREADS=1"
	"CONFIG_KERNEL_SYSLOCK_HW_TIMER=1"
	"CONFIG_KERNEL_TIME_SLICE_US=1000"
	"CONFIG_KERNEL_TIME_SLICE_US=1000"
	"CONFIG_KERNEL_DEBUG=0"
	"CONFIG_KERNEL_SCHEDULER_DEBUG=0"
	"CONFIG_KERNEL_THREAD_IDLE=1"
	"CONFIG_KERNEL_THREAD_IDLE_ADD_STACK=50"
)

target_include_directories(${EXE} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/drivers
	${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/dstruct
	${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/misc
	${CMAKE_CURRENT_SOURCE_DIR}/src/
)

qemu_generate_command(${EXE})
qemu_generate_debug_launch_json()
generate_hex(${EXE})
generate_upload_command(${EXE})
generate_post_build_command(${EXE})