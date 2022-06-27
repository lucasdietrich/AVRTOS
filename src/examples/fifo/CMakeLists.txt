project(sample_fifo)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_SYSLOCK_HW_TIMER=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
