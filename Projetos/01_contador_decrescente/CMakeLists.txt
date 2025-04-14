# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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

project(01_contador_decrescente C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(01_contador_decrescente 
    01_contador_decrescente.c 
    libs/display_oled/inc/ssd1306_i2c.c # Adiciona o arquivo fonte da biblioteca SSD1306
)

pico_set_program_name(01_contador_decrescente "01_contador_decrescente")
pico_set_program_version(01_contador_decrescente "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(01_contador_decrescente 0)
pico_enable_stdio_usb(01_contador_decrescente 1)

# Add the standard library to the build
target_link_libraries(01_contador_decrescente
        pico_stdlib
        hardware_timer
        hardware_gpio
        hardware_i2c # Adiciona suporte ao I2C
)

# Add include directories for the project
target_include_directories(01_contador_decrescente PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        libs/display_oled/inc # Adiciona o diretório de cabeçalhos da biblioteca SSD1306
)

# Add any user requested libraries
target_link_libraries(01_contador_decrescente 
        
        )

pico_add_extra_outputs(01_contador_decrescente)
