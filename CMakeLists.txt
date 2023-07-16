cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS ../esp-box/components)
add_compile_options(-fdiagnostics-color=always)

project(esp-box-lite-bfans-tts)