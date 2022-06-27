cmake_minimum_required(VERSION 3.20)

project(avrtos-project)

enable_language(C CXX ASM)

# include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake")
# set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake)
# include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/avr6-atmega2560.cmake)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

add_definitions(
    -DF_CPU=${F_CPU}
)

add_subdirectory(src/examples/workqueue)
add_subdirectory(src/examples/blinking-led)
add_subdirectory(src/examples/uptime)