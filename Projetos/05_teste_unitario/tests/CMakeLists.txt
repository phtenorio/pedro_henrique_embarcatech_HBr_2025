cmake_minimum_required(VERSION 3.13)
project(test_adc_to_celsius C)

set(CMAKE_C_STANDARD 11)

# Caminhos absolutos para os arquivos fonte e includes
set(PROJ_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

add_executable(test_adc_to_celsius
    ${CMAKE_CURRENT_LIST_DIR}/test_adc_to_celsius.c
    ${PROJ_ROOT}/drivers/adc_to_celsius.c
    ${CMAKE_CURRENT_LIST_DIR}/unity/unity.c
)

# target_include_directories(test_adc_to_celsius PRIVATE
#     ${PROJ_ROOT}/include
#     ${CMAKE_CURRENT_LIST_DIR}/unity
# )

target_include_directories(test_adc_to_celsius PRIVATE
    ../include
    unity
)

target_link_libraries(test_adc_to_celsius m)
