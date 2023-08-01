cmake_minimum_required(VERSION 3.20)
project(backend_dx12)

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(${PROJECT_NAME} SHARED ${SRC} ${INC})
target_link_libraries(${PROJECT_NAME} d3dcompiler d3d12 dxgi dxguid)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rd/deps/dxheaders/output/include)
#[[target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rd/deps/dxmath/output/lib)
target_link_libraries(${PROJECT_NAME} DirectX-Headers DirectX-Guids)]]#

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. FILES ${SRC} ${INC})
