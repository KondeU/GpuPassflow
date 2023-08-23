cmake_minimum_required(VERSION 3.20)
project(GpuPassflow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
file(GLOB_RECURSE INC include/*)

macro(install_artifact target_artifact)
  install(TARGETS ${target_artifact}
    ARCHIVE DESTINATION library
    LIBRARY DESTINATION library
    RUNTIME DESTINATION runtime
    PUBLIC_HEADER DESTINATION external)
endmacro()

macro(install_external_directory external_directory)
  install(DIRECTORY ${external_directory} DESTINATION external)
endmacro()

add_subdirectory(3rd)
add_subdirectory(source)

install(FILES README.md DESTINATION .)
install(FILES LICENSE.txt DESTINATION .)
install(DIRECTORY include/ DESTINATION include)
install(FILES User.cmake DESTINATION . RENAME GpuPassflow.cmake)
install_external_directory(3rd/deps/dxmath/output/include/)
