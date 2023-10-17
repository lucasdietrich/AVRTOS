if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	project(sample_wait_variants)
	add_executable(${PROJECT_NAME} main.c)

	# AVRTOS Configuration
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		CONFIG_KERNEL_UPTIME=1
		CONFIG_STDIO_PRINTF_TO_USART=0
	)

	target_link_avrtos(${PROJECT_NAME})

	target_prepare_env(${PROJECT_NAME})
else()
	message(WARNING "Sample ${PROJECT_NAME} is not available for Debug build")
endif()