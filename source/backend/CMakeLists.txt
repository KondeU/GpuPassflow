cmake_minimum_required(VERSION 3.20)
project(backend)

file(GLOB SRC *.h *.cpp)
add_definitions(-DBackendModule)
add_library(${PROJECT_NAME} SHARED ${SRC} ${INC})
target_link_libraries(${PROJECT_NAME} framework)

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES ${SRC} ${INC})

# backend implementation
if(WIN32)
  add_subdirectory(dx12)
endif()
add_subdirectory(vulkan)
add_subdirectory(cpu)
