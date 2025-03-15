project(sample_fault_hook)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_STDIO_USART=0
	CONFIG_KERNEL_UPTIME=1
	
	CONFIG_KERNEL_FAULT_VERBOSITY=1

	CONFIG_KERNEL_THREAD_TERMINATION_TYPE=-1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
