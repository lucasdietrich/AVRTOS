project(sample_thread_prio_multiq)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_ASSERT=0
	CONFIG_THREAD_CANARIES=0
	CONFIG_THREAD_STACK_SENTINEL=0
	CONFIG_STDIO_PRINTF_TO_USART=0
	CONFIG_INTERRUPT_POLICY=1
	CONFIG_THREAD_PRIO_MULTIQ=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
