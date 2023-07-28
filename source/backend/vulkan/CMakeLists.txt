cmake_minimum_required(VERSION 3.20)
project(backend_vulkan)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(backend_vulkan SHARED ${SRC} ${INC})

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
