# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(05_teste_unitario C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Executável principal para rodar na Pico
add_executable(05_teste_unitario
    app/main.c
    drivers/adc_temp_sensor.c
    drivers/adc_to_celsius.c
    hal/hal_temp_sensor.c
)

pico_set_program_name(05_teste_unitario "05_teste_unitario")
pico_set_program_version(05_teste_unitario "0.1")

pico_enable_stdio_uart(05_teste_unitario 0)
pico_enable_stdio_usb(05_teste_unitario 1)

target_include_directories(05_teste_unitario PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(05_teste_unitario
    pico_stdlib
    hardware_adc
)

pico_add_extra_outputs(05_teste_unitario)

# ==========================================================
# Alvo para teste unitário (roda no PC, NÃO na Pico)
# ==========================================================
add_executable(test_adc_to_celsius
    tests/test_adc_to_celsius.c
    drivers/adc_to_celsius.c
    tests/unity/unity.c
)

target_include_directories(test_adc_to_celsius PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/tests/unity
)

# Linka com a biblioteca matemática (para float)
target_link_libraries(test_adc_to_celsius m)
