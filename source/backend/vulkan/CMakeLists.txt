cmake_minimum_required(VERSION 3.20)
project(backend_vulkan)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC} ${INC})

# Find Vulkan
find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} 
    ${Vulkan_LIBRARIES}
    backend
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Vulkan_INCLUDE_DIRS}
)

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
