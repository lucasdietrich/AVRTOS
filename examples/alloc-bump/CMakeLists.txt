project(sample_alloc_global)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_THREAD_CANARIES=1
	
	CONFIG_STDIO_PRINTF_TO_USART=0
	CONFIG_SERIAL_AUTO_INIT=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})