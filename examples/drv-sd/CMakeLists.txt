if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	project(sample_drv_sd)
	add_executable(${PROJECT_NAME} main.c)

	# AVRTOS Configuration
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		CONFIG_STDIO_USART=0
		CONFIG_SERIAL_AUTO_INIT=1
		CONFIG_THREAD_CANARIES=1
		CONFIG_SD_LOG_LEVEL=4
		CONFIG_SD_CSD=0
	)

	target_link_avrtos(${PROJECT_NAME})

	target_prepare_env(${PROJECT_NAME})
else()
	message(WARNING "Sample ${PROJECT_NAME} is not available for Debug build")
endif()