project(sample_sentinel)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_UPTIME=1

	CONFIG_STDIO_PRINTF_TO_USART=0
	CONFIG_THREAD_STACK_SENTINEL=1
	CONFIG_THREAD_STACK_CANARY=1
	CONFIG_THREAD_CANARIES=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
