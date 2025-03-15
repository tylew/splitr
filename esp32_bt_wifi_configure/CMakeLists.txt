cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_bt_wifi_config)

set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

idf_component_register(SRCS ${COMPONENT_SRCS} INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS})