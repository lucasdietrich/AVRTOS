project(sample_canaries)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CONFIG_THREAD_CANARIES=1
	CONFIG_THREAD_EXPLICIT_MAIN_STACK=1

)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
