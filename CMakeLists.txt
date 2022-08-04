cmake_minimum_required(VERSION 3.20)

project(avrtos-project)

enable_language(C CXX ASM)

set(CMAKE_BUILD_TYPE Debug)
set(QEMU ON)
set(PROG_DEV "/dev/ttyACM0")

# SET(ENABLE_SINGLE_SAMPLE "thread-prio-multiq")
add_subdirectory(src/examples)