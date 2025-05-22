#
# Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#

find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_CXX avr-g++ REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_SIZE_TOOL avr-size REQUIRED)
find_program(AVR_OBJDUMP avr-objdump REQUIRED)
find_program(AVR_READELF avr-readelf REQUIRED)
find_program(AVR_NM avr-nm REQUIRED)
find_program(AVR_SIZE avr-size REQUIRED)
find_program(QEMU_SYSTEM_AVR qemu-system-avr REQUIRED)

# get avr-gcc version
execute_process(
	COMMAND ${AVR_CC} -dumpversion
	OUTPUT_VARIABLE AVR_GCC_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (AVR_GCC_VERSION VERSION_LESS 14)
	set(AVR_SIZE_ARGS "-B")
	message(WARNING "AVR-GCC version ${AVR_GCC_VERSION} is not deprecated, please consider using version 14 or higher.")
else()
	set(AVR_SIZE_ARGS "--mcu=${MCU} -C")
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})

set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")
set(CMAKE_GENERATOR "Unix Makefiles")

set(VERBOSITY "") # --verbose

# -Werror 
set(OBJECT_GEN_FLAGS "${VERBOSITY} -mmcu=${MCU} -Wall -Wextra -fno-fat-lto-objects \
    -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
    -fdata-sections -ffunction-sections -fno-split-wide-types \
    -fno-tree-scev-cprop -flto" 
)
# -Wall -Wno-main -Wundef -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -fno-split-wide-types -fno-tree-scev-cprop

set(DEBUG_FLAGS "-Og -g -gdwarf-3 -gstrict-dwarf")
set(RELEASE_FLAGS "-Os")

set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_ASM_FLAGS_DEBUG ${DEBUG_FLAGS})

set(CMAKE_C_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_ASM_FLAGS_RELEASE ${RELEASE_FLAGS})

set(CMAKE_C_FLAGS "${OBJECT_GEN_FLAGS} -Wstrict-prototypes -std=gnu11 " CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} -std=gnu++11 " CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp " CACHE INTERNAL "ASM Compiler options")

file(GLOB_RECURSE AVRTOS_C_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/*.c**")
file(GLOB_RECURSE AVRTOS_ASM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/*.S")

if(${FEATURE_TIMER_COUNT} STREQUAL "")
	set(FEATURE_TIMER_COUNT 0)
endif()

if(${FEATURE_USART_COUNT} STREQUAL 0)
	set(FEATURE_USART_COUNT 0)
endif()

set(AVRTOS_SRC ${AVRTOS_C_SRC} ${AVRTOS_ASM_SRC})

add_definitions(-DF_CPU=${F_CPU})

function(target_link_avrtos target)
	if (QEMU)
		target_compile_definitions(${target} PUBLIC -D__QEMU__)
	endif()

	target_sources(${target} PUBLIC ${AVRTOS_SRC})

	target_include_directories(${target} PUBLIC
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/avrtos/drivers
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/avrtos/dstruct
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/avrtos/misc
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/
	)
endfunction()

function(target_prepare_env target)
	target_link_options(
		${target} PUBLIC
		${VERBOSITY}
		-mmcu=${MCU}
		-Wl,--gc-sections
		-Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/map.out
		-T${LINKER_SCRIPT}
		-lc
		-lm
		-lgcc
		-Wl,--print-memory-usage
	)

	# get target output name
	get_target_property(output_name ${target} OUTPUT_NAME)
	if (output_name STREQUAL "output_name-NOTFOUND")
		set(output_name ${target})
	endif()

	set(ELF_PATH "${CMAKE_CURRENT_BINARY_DIR}/${output_name}")

	# generate launch.json file
	configure_file(
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/qemu-avr-launch.json.in
		${CMAKE_CURRENT_BINARY_DIR}/launch.${target}.json 
		@ONLY
	)

	# create hex file
	add_custom_target(
		hex_${target} 
		ALL
		avr-objcopy -R .eeprom -O ihex ${output_name} ${output_name}.hex 
		DEPENDS ${target}
	)
		
	# add upload command
	add_custom_target(
		upload_${target} 
		avrdude -c ${PROG_TYPE} -p ${PROG_PARTNO} -P ${PROG_DEV} -U flash:w:${output_name}.hex 
		DEPENDS hex_${target}
		USES_TERMINAL
	)

	# add monitor command
	add_custom_target(
		monitor_${target} 
		COMMAND echo "Press Ctrl-T + Q to exit" && python3 -m serial.tools.miniterm "${PROG_DEV}" "${BAUDRATE}" --raw --eol LF
		USES_TERMINAL
	)

	if (QEMU)
		set(QEMU_ARGS "")

		# generate custom target for debug in qemu:
		# - Use console: https://stackoverflow.com/questions/76005036/how-can-i-make-custom-commands-and-targets-flush-their-output-immediately-instea/76005037#76005037
		# - Regenerate and copy launch.json each time we run the target
		add_custom_target(qemu_${target} 
			COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/launch.${target}.json ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../.vscode/launch.json
			COMMAND ${QEMU_SYSTEM_AVR} -M ${QEMU_MCU} -bios ${ELF_PATH} -s -S -nographic ${QEMU_ARGS} -icount auto
			USES_TERMINAL 
			DEPENDS 
				${target} 
				${CMAKE_CURRENT_BINARY_DIR}/launch.${target}.json
		)

		# generate custom target for run in qemu
		add_custom_target(run_${target} 
			COMMAND ${QEMU_SYSTEM_AVR} -M ${QEMU_MCU} -bios ${ELF_PATH} -nographic ${QEMU_ARGS} -icount auto
			USES_TERMINAL 
			DEPENDS ${target}
		)
	endif()

	if (DEFINED ENABLE_SINGLE_SAMPLE)
		add_custom_target(upload DEPENDS upload_${target})
		add_custom_target(monitor  DEPENDS monitor_${target})

		if (QEMU)
			add_custom_target(qemu DEPENDS qemu_${target})
			add_custom_target(run_qemu DEPENDS run_${target})
		endif()
	endif()

	# disassembly + debug info
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${AVR_OBJDUMP} -S ${output_name} > objdump_src.s VERBATIM
		COMMAND ${AVR_OBJDUMP} -d ${output_name} > objdump.s VERBATIM
		COMMAND ${AVR_OBJDUMP} -D ${output_name} > objdump_all.s VERBATIM
		COMMAND ${AVR_OBJDUMP} -h ${output_name} > objdump_sections.s VERBATIM
		COMMAND ${AVR_READELF} -a ${output_name} > readelf.txt VERBATIM
		COMMAND ${AVR_READELF} -x .data ${output_name} > section_data.txt VERBATIM
		COMMAND ${AVR_READELF} -x .bss ${output_name} > section_bss.txt VERBATIM
		COMMAND ${AVR_READELF} -x .noinit ${output_name} > section_noinit.txt VERBATIM
		COMMAND ${AVR_READELF} -x .text ${output_name} > section_text.txt VERBATIM
		COMMAND ${AVR_NM} --print-size --size-sort --radix=x ${output_name} > nm.txt VERBATIM
		COMMAND ${AVR_SIZE} -B ${output_name} > size.txt VERBATIM
		DEPENDS ${output_name}
	)
endfunction()