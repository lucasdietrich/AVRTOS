project(sample_data_structures)
add_executable(${PROJECT_NAME} main.cpp)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_PREEMPTIVE_THREADS=0
	CONFIG_KERNEL_THREAD_IDLE=0
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
