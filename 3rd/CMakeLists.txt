cmake_minimum_required(VERSION 3.20)
project(dependent_open_source)

set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps)

option(DOWNLOAD_OPEN_SOURCE "Automatically download dependent open-source libraries." ON)
option(REARCHIVE_OPEN_SOURCE "Clear and then rebuild dependent open-source libraries." ON)
option(BUILD_DEBUG_OPEN_SOURCE "Build dependent open-source libraries with Debug mode." OFF)

if(${DOWNLOAD_OPEN_SOURCE})
  if(WIN32)
    execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
      COMMAND curl -LOJ https://github.com/microsoft/DirectX-Headers/archive/refs/tags/v1.608.2.zip)
  endif()
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
    COMMAND curl -LOJ https://github.com/microsoft/DirectXMath/archive/refs/tags/dec2022.zip)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
    COMMAND curl -LOJ https://github.com/KondeU/EventBus/archive/refs/tags/v0.1.1.zip)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
    COMMAND curl -LOJ https://github.com/tinyobjloader/tinyobjloader/archive/refs/tags/v2.0-rc1.zip)
endif()

if(NOT ${REARCHIVE_OPEN_SOURCE})
  return()
endif()

if(WIN32)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
    COMMAND ${CMAKE_COMMAND} -E remove_directory dxheaders)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
    COMMAND ${CMAKE_COMMAND} -E tar xzf DirectX-Headers-1.608.2.zip)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
    COMMAND ${CMAKE_COMMAND} -E rename DirectX-Headers-1.608.2 dxheaders)
endif()

execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E remove_directory dxmath)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E tar xzf DirectXMath-dec2022.zip)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E rename DirectXMath-dec2022 dxmath)

execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E remove_directory ebus)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E tar xzf EventBus-0.1.1.zip)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E rename EventBus-0.1.1 ebus)

execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E remove_directory objloader)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E tar xzf tinyobjloader-2.0-rc1.zip)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}
  COMMAND ${CMAKE_COMMAND} -E rename tinyobjloader-2.0-rc1 objloader)

if(${BUILD_DEBUG_OPEN_SOURCE})
  set(BUILD_OPEN_SOURCE_MODE Debug)
else()
  set(BUILD_OPEN_SOURCE_MODE Release)
endif()

if(MSVC)
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(THIRD_PARTY_ARCHITECTURE Win32)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(THIRD_PARTY_ARCHITECTURE x64)
  endif()
  set(MSVC_ARCH -A ${THIRD_PARTY_ARCHITECTURE})
endif()

if(WIN32)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/dxheaders
    COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build ${MSVC_ARCH}
      -DDXHEADERS_BUILD_TEST=OFF -DDXHEADERS_BUILD_GOOGLE_TEST=OFF)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/dxheaders
    COMMAND ${CMAKE_COMMAND} --build build --config ${BUILD_OPEN_SOURCE_MODE} --parallel)
  execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/dxheaders
    COMMAND ${CMAKE_COMMAND} --install build --prefix output --config ${BUILD_OPEN_SOURCE_MODE})
endif()

execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/dxmath
  COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build ${MSVC_ARCH})
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/dxmath
  COMMAND ${CMAKE_COMMAND} --build build --config ${BUILD_OPEN_SOURCE_MODE} --parallel)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/dxmath
  COMMAND ${CMAKE_COMMAND} --install build --prefix output --config ${BUILD_OPEN_SOURCE_MODE})

execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/ebus
  COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build ${MSVC_ARCH})
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/ebus
  COMMAND ${CMAKE_COMMAND} --build build --config ${BUILD_OPEN_SOURCE_MODE} --parallel)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/ebus
  COMMAND ${CMAKE_COMMAND} --install build --prefix output --config ${BUILD_OPEN_SOURCE_MODE})

execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/objloader
  COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S . -B build ${MSVC_ARCH}
  -DCMAKE_WARN_DEPRECATED=OFF) # Disable all CMake tool warning: Compatibility with CMake < 2.8.12
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/objloader
  COMMAND ${CMAKE_COMMAND} --build build --config ${BUILD_OPEN_SOURCE_MODE} --parallel)
execute_process(WORKING_DIRECTORY ${THIRD_PARTY_PATH}/objloader
  COMMAND ${CMAKE_COMMAND} --install build --prefix output --config ${BUILD_OPEN_SOURCE_MODE})
