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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(DonsGraphics C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(DonsGraphics DonsGraphics.c )

# Add sources incrementally 
# must match with executable name and source file names
target_sources(DonsGraphics PRIVATE 
	vga16_graphics.c
    glcdfont.c
)

pico_set_program_name(DonsGraphics "DonsGraphics")
pico_set_program_version(DonsGraphics "0.1")

# must match with pio filename and executable name from above
pico_generate_pio_header(DonsGraphics ${CMAKE_CURRENT_LIST_DIR}/hsync.pio)
pico_generate_pio_header(DonsGraphics ${CMAKE_CURRENT_LIST_DIR}/vsync.pio)
pico_generate_pio_header(DonsGraphics ${CMAKE_CURRENT_LIST_DIR}/rgb.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(DonsGraphics 1)
pico_enable_stdio_usb(DonsGraphics 1)

# Add the standard library to the build
target_link_libraries(DonsGraphics
        pico_stdlib
        hardware_pio
        hardware_dma
        pico_multicore)

# Add the standard include files to the build
target_include_directories(DonsGraphics PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(DonsGraphics)

