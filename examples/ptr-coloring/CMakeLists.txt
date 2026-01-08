project(sample_ptr_coloring)
add_executable(${PROJECT_NAME} main.c)

target_compile_definitions(${PROJECT_NAME} PUBLIC
		)


target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
