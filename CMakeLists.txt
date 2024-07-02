cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(main
    main.c
)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(main pico_stdlib)

# Enable USB serial output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# Create map/bin/hex file etc.
pico_add_extra_outputs(main)