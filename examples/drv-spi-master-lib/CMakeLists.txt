# project(sample_drv_spi_master_lib)
# add_executable(${PROJECT_NAME} main.cpp)

# # AVRTOS Configuration
# target_compile_definitions(${PROJECT_NAME} PUBLIC
# 	CONFIG_STDIO_PRINTF_TO_USART=0
# 	CONFIG_KERNEL_DEBUG=0
# 	CONFIG_KERNEL_SYSLOCK_HW_TIMER=1
# 	CONFIG_KERNEL_TIME_SLICE_US=1000
# 	CONFIG_KERNEL_UPTIME=1
# 	CONFIG_THREAD_CANARIES=1
# 	CONFIG_KERNEL_DEBUG_GPIO=0
# )

# target_link_avrtos(${PROJECT_NAME})

# target_prepare_env(${PROJECT_NAME})
