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

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})

set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")
set(CMAKE_GENERATOR "Unix Makefiles")

set(VERBOSITY "") # --verbose

# -Werror 
set(OBJECT_GEN_FLAGS "${VERBOSITY} -mmcu=${MCU} -Wall -fno-fat-lto-objects -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -fno-split-wide-types -fno-tree-scev-cprop -flto")
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

set(CMAKE_EXE_LINKER_FLAGS
	"${VERBOSITY} -mmcu=${MCU} -Wl,--gc-sections -Wl,-Map,map.out -T${LINKER_SCRIPT} -lc -lm -lgcc -Wl,--print-memory-usage"
)

file(GLOB_RECURSE AVRTOS_C_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/*.c**")
file(GLOB_RECURSE AVRTOS_ASM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/*.S")

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
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/avrtos/subsys/ipc
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/avrtos/misc
		${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/
	)
endfunction()

function(target_prepare_env target)
	# get target output name
	get_target_property(output_name ${target} OUTPUT_NAME)
	if (output_name STREQUAL "output_name-NOTFOUND")
		set(output_name ${target})
	endif()

	set(ELF_PATH "${CMAKE_CURRENT_BINARY_DIR}/${output_name}")

	# create hex file
	add_custom_target(hex_${target} ALL avr-objcopy -R .eeprom -O ihex ${output_name} ${output_name}.hex 
		DEPENDS ${target}
	)

	# add upload command
	add_custom_target(upload_${target} avrdude -c ${PROG_TYPE} -p ${PROG_PARTNO} -P ${PROG_DEV} -U flash:w:${output_name}.hex 
		DEPENDS hex_${target}
	)
	
	if (DEFINED ENABLE_SINGLE_SAMPLE)
		add_custom_target(upload avrdude -c ${PROG_TYPE} -p ${PROG_PARTNO} -P ${PROG_DEV} -U flash:w:${output_name}.hex 
			DEPENDS hex_${target}
		)
	endif()
	
	# monitor command
	# add_custom_target(monitor_${target} python3 -m serial.tools.miniterm "${PROG_DEV}" "${BAUDRATE}")

	# generate launch.json file
	configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/qemu-avr-launch.json.in ${CMAKE_CURRENT_BINARY_DIR}/launch.${target}.json @ONLY)

	if (QEMU)
		# generate custom target for debug in qemu
		add_custom_target(qemu_${target} 
			COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/launch.${target}.json ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../.vscode/launch.json
			COMMAND qemu-system-avr -M ${QEMU_MCU} -bios ${ELF_PATH} -s -S -nographic
			DEPENDS ${target})

		# generate custom target for run in qemu
		add_custom_target(run_${target} 
			COMMAND qemu-system-avr -M ${QEMU_MCU} -bios ${ELF_PATH} -nographic
			DEPENDS ${target}
		)
	endif()

	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND avr-objdump -S ${output_name} > objdump_src.s VERBATIM
		COMMAND avr-objdump -d ${output_name} > objdump.s VERBATIM
		COMMAND avr-objdump -D ${output_name} > objdump_all.s VERBATIM
		COMMAND avr-objdump -h ${output_name} > objdump_sections.s VERBATIM
		COMMAND avr-readelf -a ${output_name} > readelf.txt VERBATIM
		COMMAND avr-readelf -x .data ${output_name} > section_data.txt VERBATIM
		COMMAND avr-readelf -x .bss ${output_name} > section_bss.txt VERBATIM
		COMMAND avr-readelf -x .noinit ${output_name} > section_noinit.txt VERBATIM
		COMMAND avr-readelf -x .text ${output_name} > section_text.txt VERBATIM
		COMMAND avr-nm --print-size --size-sort --radix=x ${output_name} > nm.txt VERBATIM
		DEPENDS ${output_name}
	)
endfunction()