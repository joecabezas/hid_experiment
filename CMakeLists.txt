# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(dev_hid_composite C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

cmake_minimum_required(VERSION 3.13)

# Add executable. Default name is the project name, version 0.1
add_executable(dev_hid_composite)

target_sources(dev_hid_composite PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        ${CMAKE_CURRENT_LIST_DIR}/interpolators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/job.cpp
        ${CMAKE_CURRENT_LIST_DIR}/linear_job.cpp
        ${CMAKE_CURRENT_LIST_DIR}/point.cpp
        ${CMAKE_CURRENT_LIST_DIR}/schedule.cpp

        ${CMAKE_CURRENT_LIST_DIR}/proto/message.pb.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/nanopb/pb_common.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/nanopb/pb_encode.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/nanopb/pb_decode.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(dev_hid_composite PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/proto
        ${CMAKE_CURRENT_LIST_DIR}/lib/nanopb
        )

# Enable TinyUSB for CDC (Serial) and HID (Mouse + Keyboard)
pico_enable_stdio_usb(dev_hid_composite 1)
pico_enable_stdio_uart(dev_hid_composite 0)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(dev_hid_composite
    PUBLIC
    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(dev_hid_composite PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(dev_hid_composite)

# add url via pico_set_program_url
