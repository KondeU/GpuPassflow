cmake_minimum_required(VERSION 3.20)
project(backend_cpu)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(backend_cpu SHARED ${SRC} ${INC})
target_link_libraries(${PROJECT_NAME} backend)

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
