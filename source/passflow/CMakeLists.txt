cmake_minimum_required(VERSION 3.20)
project(passflow)

set(GP_OPT_FRAME_RESOURCES_KEY_TYPE "${GP_OPT_FRAME_RESOURCES_KEY_TYPE}")
if(DEFINED GP_OPT_FRAME_RESOURCES_KEY_TYPE)
  #set(GP_OPT_FRAME_RESOURCES_KEY_TYPE_NAME ${GP_OPT_FRAME_RESOURCES_KEY_TYPE})
  add_definitions(-DGP_OPT_FRAME_RESOURCES_KEY_TYPE=${GP_OPT_FRAME_RESOURCES_KEY_TYPE})
  message(STATUS "Custom specified FRsKey type: ${GP_OPT_FRAME_RESOURCES_KEY_TYPE}")
endif()
message(STATUS "@@@@@@@@@@@@FRsKey@@@@@@@@@@@@${GP_OPT_FRAME_RESOURCES_KEY_TYPE}")

file(GLOB_RECURSE SRC *.h *.cpp)
add_library(${PROJECT_NAME} STATIC ${SRC} ${INC})
target_link_libraries(${PROJECT_NAME} backend)

install_artifact(${PROJECT_NAME})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../.. FILES ${SRC} ${INC})
