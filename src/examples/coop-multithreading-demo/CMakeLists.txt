project(sample_coop_multithreading_demo)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_THREAD_MAIN_STACK_SIZE=0x100
	CONFIG_KERNEL_PREEMPTIVE_THREADS=0
	CONFIG_THREAD_IDLE_COOPERATIVE=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
