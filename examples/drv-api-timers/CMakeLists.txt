if (NOT QEMU)

	project(sample_drv_api_timers)
	add_executable(${PROJECT_NAME} main.c)

	# AVRTOS Configuration
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		CONFIG_INTERRUPT_POLICY=1
		CONFIG_STDIO_USART=0
		CONFIG_KERNEL_UPTIME=1
		CONFIG_KERNEL_SYSLOCK_HW_TIMER=1
		CONFIG_DRIVERS_TIMER2_API=1
		CONFIG_DRIVERS_TIMER4_API=1
		CONFIG_DRIVERS_TIMER5_API=1

	)

	target_link_avrtos(${PROJECT_NAME})

	target_prepare_env(${PROJECT_NAME})

endif()