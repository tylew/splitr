# Install script for directory: /opt/nordic/ncs/v3.0.0/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/nordic/ncs/toolchains/b8efef2ad5/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/acpica/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/adi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/altera/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_ambiq/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/atmel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_espressif/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_ethos_u/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_gigadevice/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_infineon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_intel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/microchip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/nuvoton/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_nxp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/openisa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/quicklogic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_renesas/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_rpi_pico/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_silabs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_stm32/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_telink/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/ti/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_wch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/xtensa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/tinycrypt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/trusted-firmware-a/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/opt/nordic/ncs/v3.0.0/zephyr/samples/basic/rgb_led/build/rgb_led/zephyr/cmake/reports/cmake_install.cmake")
endif()

