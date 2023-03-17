project(sample_prempt_multithreading_demo)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_COOPERATIVE_THREADS=1
	CONFIG_KERNEL_TIME_SLICE_US=250000
	CONFIG_KERNEL_DEBUG=1
	CONFIG_KERNEL_SCHEDULER_DEBUG=0
	CONFIG_KERNEL_DEBUG_PREEMPT_UART=1

)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
