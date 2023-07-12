cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS ../esp-box/components)
# set(EXTRA_COMPONENT_DIRS ../../esp/esp-skainet/components)

# set(CMAKE_C_COMPILER xtensa-esp32s3-elf-gcc)
# set(CMAKE_CXX_COMPILER xtensa-esp32s3-elf-g++)
add_compile_options(-fdiagnostics-color=always)

project(demo)