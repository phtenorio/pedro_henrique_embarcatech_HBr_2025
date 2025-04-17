# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == SEÇÃO AUTOMÁTICA (não alterar) ==
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
# =====================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o SDK Pico antes de tudo
include(pico_sdk_import.cmake)

project(02_leitura_joystick C CXX ASM)

# Inicializa o SDK Pico
pico_sdk_init()

# Cria o executável principal
add_executable(02_leitura_joystick
    02_leitura_joystick.c
    libs/ssd1306.c
)

# Define nome e versão do programa (opcional)
pico_set_program_name(02_leitura_joystick "02_leitura_joystick")
pico_set_program_version(02_leitura_joystick "0.1")

# Ativa saída USB (desativa UART)
pico_enable_stdio_uart(02_leitura_joystick 0)
pico_enable_stdio_usb(02_leitura_joystick 1)

# Linka bibliotecas necessárias do SDK
target_link_libraries(02_leitura_joystick
    pico_stdlib
    hardware_adc
    hardware_i2c
)

# Diretórios de include (inclui a pasta libs)
target_include_directories(02_leitura_joystick PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/libs
)

# Gera UF2, bin, etc.
pico_add_extra_outputs(02_leitura_joystick)
