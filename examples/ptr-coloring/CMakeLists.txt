project(sample_ptr_coloring)
add_executable(${PROJECT_NAME} main.c)

target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_STDIO_USART=0
	CONFIG_SERIAL_AUTO_INIT=1
)


target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
