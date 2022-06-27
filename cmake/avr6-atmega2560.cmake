set(CMAKE_C_COMPILER "/usr/lib64/ccache/avr-gcc")
set(CMAKE_CXX_COMPILER "/usr/lib64/ccache/avr-g++")

# prevent "is not able to compile a simple test program"
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")
set(CMAKE_GENERATOR "Unix Makefiles")

set(F_CPU 16000000UL)
set(MCU atmega2560)
set(QEMU_MCU mega2560)

set(BAUDRATE 500000)
set(PROG_DEV "/dev/ttyACM0")

set(VERBOSITY "")

set(OBJECT_GEN_FLAGS "${VERBOSITY} -mmcu=${MCU} -Wall -Wno-main -Wstrict-prototypes -Wundef -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -fno-split-wide-types -fno-tree-scev-cprop" )
# -Werror -pedantic -Wfatal-errors

set(DEBUG_FLAGS "-Og -g -gdwarf-2")
set(RELEASE_FLAGS "-Os")

set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_ASM_FLAGS_DEBUG ${DEBUG_FLAGS})

set(CMAKE_C_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_ASM_FLAGS_RELEASE ${RELEASE_FLAGS})

set(CMAKE_C_FLAGS "${OBJECT_GEN_FLAGS} -std=gnu11 " CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} -std=gnu++11 " CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp " CACHE INTERNAL "ASM Compiler options")

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/../arch/avr/avrtos-avr6.xn)

set(CMAKE_EXE_LINKER_FLAGS
	"${VERBOSITY} -mmcu=${MCU} -Wl,--gc-sections -Wl,-Map,map.out -T${LINKER_SCRIPT} -lc -lm -lgcc -Wl,--print-memory-usage"
)

set(PROG_TYPE wiring)
set(PROG_PARTNO m2560)

file(GLOB_RECURSE AVRTOS_C_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/*.c**")
file(GLOB_RECURSE AVRTOS_ASM_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/avrtos/*.S")

set(AVRTOS_SRC ${AVRTOS_C_SRC} ${AVRTOS_ASM_SRC})


function(target_prepare_env target)
	# get target output name
	get_target_property(output_name ${target} OUTPUT_NAME)
	set(ELF_PATH "${CMAKE_CURRENT_BINARY_DIR}/${output_name}")
	message(STATUS ${ELF_PATH})

	# create hex file
	add_custom_target(hex_${target} ALL avr-objcopy -R .eeprom -O ihex ${output_name} ${exe}.hex)

	# add upload command
	add_custom_target(upload_${target} avrdude -c ${PROG_TYPE} -p ${PROG_PARTNO} -P ${PROG_DEV} -U flash:w:${exe}.hex DEPENDS hex_${target})
	
	# monitor command
	# add_custom_target(monitor_${target} python3 -m serial.tools.miniterm "${PROG_DEV}" "${BAUDRATE}")

	# generate custom target for qemu
	add_custom_target(qemu_${target} qemu-system-avr -M ${QEMU_MCU} -bios ${ELF_PATH} -s -S -nographic)

	# generate launch.json file
	configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/qemu-avr-launch.json.in ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../.vscode/launch.json @ONLY)

	# disassembly
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND avr-objdump -S ${output_name} > ${target}.dis.src.asm VERBATIM
	)
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND avr-readelf -a ${output_name} > ${target}.readelf.txt VERBATIM
	)
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND avr-objdump -d ${output_name} > ${target}.dis.asm VERBATIM
	)
endfunction()