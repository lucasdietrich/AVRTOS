project(sample_arduino)
add_executable(${PROJECT_NAME} main.cpp)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_AVRTOS_LINKER_SCRIPT=0
	CONFIG_STDIO_PRINTF_TO_USART=0
	CONFIG_KERNEL_AUTO_INIT=0
	CONFIG_KERNEL_TIMERS=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
