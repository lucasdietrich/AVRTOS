if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	project(sample_drv_spi_can_mcp2515)
	add_executable(${PROJECT_NAME} main.c)

	# AVRTOS Configuration
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		CONFIG_STDIO_PRINTF_TO_USART=0
		CONFIG_DEVICE_MCP2515=1
		CONFIG_SYSTEM_WORKQUEUE_ENABLE=1
		CONFIG_KERNEL_SYSLOCK_HW_TIMER=2
		CONFIG_KERNEL_SYSCLOCK_PERIOD_US=1000
		CONFIG_KERNEL_TIME_SLICE_US=1000
		CONFIG_KERNEL_UPTIME=1
		CONFIG_THREAD_CANARIES=1
	)

	target_link_avrtos(${PROJECT_NAME})

	target_prepare_env(${PROJECT_NAME})
else()
	message(WARNING "Sample ${PROJECT_NAME} is not available for Debug build")
endif()