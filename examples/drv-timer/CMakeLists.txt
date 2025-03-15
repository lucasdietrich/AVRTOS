#
# Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0
#

if (${FEATURE_TIMER_COUNT} GREATER 3)

	project(sample_drv_timer)
	add_executable(${PROJECT_NAME} main.c)

	# AVRTOS Configuration
	target_compile_definitions(${PROJECT_NAME} PUBLIC
		CONFIG_KERNEL_UPTIME=1
		CONFIG_THREAD_EXPLICIT_MAIN_STACK=0
		CONFIG_INTERRUPT_POLICY=1
		CONFIG_STDIO_USART=0
		CONFIG_KERNEL_SYSLOCK_HW_TIMER=1
		CONFIG_KERNEL_TIME_SLICE_US=1000
		CONFIG_KERNEL_SYSCLOCK_PERIOD_US=1000
		CONFIG_KERNEL_DELAY_OBJECT_U32=1
	)

	target_link_avrtos(${PROJECT_NAME})

	target_prepare_env(${PROJECT_NAME})

endif()