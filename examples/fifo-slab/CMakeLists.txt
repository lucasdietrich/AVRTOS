project(sample_fifo_slab)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_SYSLOCK_HW_TIMER=1
	CONFIG_KERNEL_TIME_SLICE_US=4000
	CONFIG_KERNEL_SYSCLOCK_DEBUG=0
	CONFIG_KERNEL_SCHEDULER_DEBUG=0
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
