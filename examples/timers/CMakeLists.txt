project(sample_timers)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_ASSERT=1
	CONFIG_THREAD_STACK_SENTINEL=1
	CONFIG_THREAD_CANARIES=1
	CONFIG_KERNEL_SYSLOCK_HW_TIMER=1
	CONFIG_KERNEL_TIME_SLICE_US=10000
	CONFIG_KERNEL_TIMERS=1
	CONFIG_KERNEL_THREAD_IDLE_ADD_STACK=0x60
	CONFIG_STDIO_USART=0
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
