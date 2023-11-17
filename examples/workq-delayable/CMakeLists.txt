project(sample_workq_delayable)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_TIME_SLICE_US=1000
	CONFIG_SYSTEM_WORKQUEUE_ENABLE=1
	CONFIG_STDIO_PRINTF_TO_USART=0
	CONFIG_WORKQUEUE_DELAYABLE=1
	CONFIG_KERNEL_EVENTS=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
