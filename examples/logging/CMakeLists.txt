project(sample_logging)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_KERNEL_UPTIME=1
	CONFIG_KERNEL_SYSCLOCK_PERIOD_US=1000
	CONFIG_KERNEL_TIME_SLICE_US=1000
	CONFIG_KERNEL_SYSCLOCK_DEBUG=0
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
