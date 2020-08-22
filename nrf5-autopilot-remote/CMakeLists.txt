cmake_minimum_required(VERSION 3.2)
project(nrf5-autopilot-remote)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_COMPILER "/usr/local/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "/usr/local/bin/arm-none-eabi-g++")
set(SDK_ROOT "$ENV{HOME}/nonbackupped/nRF5_SDK_17.0.0")

add_definitions(-DNRF5)
add_definitions(-DNRF52)
add_definitions(-DNRF52832)
add_definitions(-DBOARD_CUSTOM)

include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${SDK_ROOT}/modules/nrfx/mdk")
include_directories("${SDK_ROOT}/components/toolchain/cmsis/include")
include_directories("${SDK_ROOT}/components/libraries/util")
include_directories("${SDK_ROOT}/components/libraries/strerror")
include_directories("${SDK_ROOT}/components/libraries/log")
include_directories("${SDK_ROOT}/components/libraries/log/src")
include_directories("${SDK_ROOT}/components/libraries/experimental_section_vars")

file(GLOB_RECURSE SRC_LIST "${PROJECT_SOURCE_DIR}/src/*.*")

add_executable(${PROJECT_NAME} ${SRC_LIST})
