
project(workqueue)

enable_language(C CXX ASM)

set(EXE ${PROJECT_NAME})

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/main.c")
add_executable(${EXE} ${AVRTOS_SRC} ${SOURCES})

set_target_properties(${EXE} PROPERTIES OUTPUT_NAME ${EXE}.elf)

# AVRTOS Configuration
target_compile_definitions(${EXE} PUBLIC
	"CONFIG_KERNEL_SYSLOCK_HW_TIMER=1"
	"CONFIG_KERNEL_TIME_SLICE_US=1000"
	"CONFIG_KERNEL_DEBUG=0"
	"CONFIG_KERNEL_SCHEDULER_DEBUG=0"
	"CONFIG_KERNEL_UPTIME=1"
)

target_include_directories(${EXE} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/../../avrtos/drivers
	${CMAKE_CURRENT_LIST_DIR}/../../avrtos/dstruct
	${CMAKE_CURRENT_LIST_DIR}/../../avrtos/misc
	${CMAKE_CURRENT_LIST_DIR}/../../
)

qemu_generate_command(${EXE})
qemu_generate_debug_launch_json(${EXE})
generate_hex(${EXE})
generate_upload_command(${EXE})
generate_post_build_command(${EXE})
generate_miniterm_command(${PROG_DEV} ${BAUDRATE})