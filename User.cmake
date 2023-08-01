set(GPU_PASSFLOW_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE GPU_PASSFLOW_HEADERS ${GPU_PASSFLOW_DIR}/include/*)

macro(target_use_gpu_passflow target_artifact)
  target_include_directories(
    ${target_artifact} PUBLIC
      ${GPU_PASSFLOW_DIR}/include
  )
  target_link_directories(
    ${target_artifact} PUBLIC
      ${GPU_PASSFLOW_DIR}/library
  )
  target_link_libraries(
    ${target_artifact} PUBLIC
      backend passflow
  )
endmacro()
