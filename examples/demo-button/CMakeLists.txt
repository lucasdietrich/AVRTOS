project(sample_demo_button)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_STDIO_PRINTF_TO_USART=0
	CONFIG_KERNEL_UPTIME=1
	CONFIG_KERNEL_SILENT_FAULTS=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
