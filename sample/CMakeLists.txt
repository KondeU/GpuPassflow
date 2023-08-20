cmake_minimum_required(VERSION 3.20)
project(sample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/../output/GpuPassflow.cmake)

file(GLOB_RECURSE SRC *.h *.cpp)
add_executable(${PROJECT_NAME} ${SRC} ${GPU_PASSFLOW_HEADERS})
target_use_gpu_passflow(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/.. FILES ${SRC} ${INC})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../output/runtime/ DESTINATION .)
