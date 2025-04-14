cmake_minimum_required(VERSION 3.13)

# Nome do projeto
project(bitdoglab_counter_project)

# Inicializar o SDK da Raspberry Pi Pico
include(pico_sdk_import.cmake)
pico_sdk_init()

# Adicionar o executável principal
add_executable(bitdoglab_counter main.c)

# Incluir o diretório onde está o arquivo .h
target_include_directories(bitdoglab_counter PRIVATE ${CMAKE_SOURCE_DIR})

# Linkar bibliotecas necessárias
target_link_libraries(bitdoglab_counter pico_stdlib hardware_gpio hardware_timer)

# Gerar arquivos binários UF2 para a Pico
pico_add_extra_outputs(bitdoglab_counter)
