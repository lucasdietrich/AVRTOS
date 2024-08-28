project(sample_hello-world)
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
    CONFIG_SERIAL_AUTO_INIT=1
    CONFIG_SERIAL_USART_BAUDRATE=115200
    CONFIG_STDIO_PRINTF_TO_USART=0 # redirect printf to USART0
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
