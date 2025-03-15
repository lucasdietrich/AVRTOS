project(sample_thread_ev_spawn)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
    CONFIG_STDIO_USART=0
    CONFIG_KERNEL_SYSCLOCK_PERIOD_US=1000u
    CONFIG_KERNEL_TIME_SLICE_US=1000u
    CONFIG_THREAD_CANARIES=1u

    CONFIG_THREAD_MAIN_COOPERATIVE=1
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
