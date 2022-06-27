
project(uptime)

set(EXE ${PROJECT_NAME})

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/main.c")
add_executable(${EXE} ${AVRTOS_SRC} ${SOURCES})

set_target_properties(${EXE} PROPERTIES OUTPUT_NAME ${EXE}.elf)

# AVRTOS Configuration
target_compile_definitions(${EXE} PUBLIC
	"CONFIG_KERNEL_ASSERT=0"
	"CONFIG_STDIO_PRINTF_TO_USART=0"
	"CONFIG_KERNEL_DEBUG=0"
	"CONFIG_KERNEL_SYSLOCK_HW_TIMER=1"
	"CONFIG_KERNEL_TIME_SLICE_US=4000"
	"CONFIG_KERNEL_UPTIME=1"
	"CONFIG_KERNEL_API_NOINLINE=1"
	"CONFIG_THREAD_CANARIES=1"
	"CONFIG_KERNEL_TIME=1"
)

target_include_directories(${EXE} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/../../avrtos/drivers
	${CMAKE_CURRENT_LIST_DIR}/../../avrtos/dstruct
	${CMAKE_CURRENT_LIST_DIR}/../../avrtos/misc
	${CMAKE_CURRENT_LIST_DIR}/../../
)

target_prepare_env(${EXE})