#
# Copyright (c) 2018 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
#

cmake_minimum_required(VERSION 3.8.2)

include($ENV{ZEPHYR_BASE}/../nrf/cmake/boilerplate.cmake)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(mqtt-simple)

# NORDIC SDK APP START
target_sources(app PRIVATE
src/https.c
src/main.c
src/uarts.c
src/nvs_flash.c
src/gpio.c
)

# NORDIC SDK APP END
