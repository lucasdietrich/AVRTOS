project(sample_multithreading_switching_frequency)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_PREEMPTIVE_THREADS=0

)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
