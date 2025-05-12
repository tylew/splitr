#
# Copyright (c) 2023 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
+ cmake_minimum_required(VERSION 3.20.0)
+ find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
+ project(broadcast_source)

  # your existing content…
  # e.g. target_sources(app PRIVATE main.c …)

  
target_sources(app PRIVATE
	       ${CMAKE_CURRENT_SOURCE_DIR}/main.c)
