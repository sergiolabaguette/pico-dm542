cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(dm542
    dm542.c
)

pico_enable_stdio_usb(dm542 1)

pico_add_extra_outputs(dm542)

target_link_libraries(dm542 pico_stdlib)