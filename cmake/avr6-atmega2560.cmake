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

function(qemu_generate_command PATH)
	add_custom_target(qemu qemu-system-avr -M ${QEMU_MCU} -bios ${PATH}.elf -s -S -nographic)
endfunction()

function(qemu_generate_debug_launch_json)
	configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/qemu-avr-launch.json.in ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../.vscode/launch.json @ONLY)
endfunction()

function(generate_hex PATH)
	add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex ${PATH}.elf ${PATH}.hex)
endfunction()

function(generate_upload_command PATH)
	add_custom_target(upload avrdude -c ${PROG_TYPE} -p ${PROG_PARTNO} -P ${PROG_DEV} -U flash:w:${PATH}.hex DEPENDS hex)
endfunction()

function(generate_post_build_command PATH)
	add_custom_command(TARGET ${PATH} POST_BUILD
		COMMAND avr-objdump -S ${PATH}.elf > ${PATH}.dis.src.asm VERBATIM
	)
	add_custom_command(TARGET ${PATH} POST_BUILD
		COMMAND avr-readelf -a ${PATH}.elf > ${PATH}.readelf.txt VERBATIM
	)
	add_custom_command(TARGET ${PATH} POST_BUILD
		COMMAND avr-objdump -d ${PATH}.elf > ${PATH}.dis.asm VERBATIM
	)
endfunction()