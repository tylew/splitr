# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf_audio_switch)

# List all source files by module
set(BUTTON_SRCS
    src/modules/button/button.c
)

set(LED_SRCS
    src/modules/led/led.c
)

set(BLE_SRCS
    src/modules/ble/ble.c
)

# Main application sources
set(APP_SRCS
    src/main.c
)

# Add all sources to the application
target_sources(app PRIVATE
    ${APP_SRCS}
    ${BUTTON_SRCS}
    ${LED_SRCS}
    ${BLE_SRCS}
)

# Add include directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modules
)
