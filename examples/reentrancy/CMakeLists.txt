project(sample_reentrancy)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_DEBUG=0
	CONFIG_KERNEL_SCHEDULER_DEBUG=0
	CONFIG_KERNEL_API_NOINLINE=1
	CONFIG_KERNEL_THREAD_TERMINATION_TYPE=1
	CONFIG_KERNEL_REENTRANCY=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
