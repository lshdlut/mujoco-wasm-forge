cmake_minimum_required(VERSION 3.22)
project(mujoco_official_337_wasm)

# Only require EMSDK when targeting Emscripten
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "EMSDK not set; run emsdk_env")
  endif()
endif()

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/_wasm")

option(MUJOCO_BUILD_EXAMPLES "" OFF)
option(MUJOCO_BUILD_SIMULATE "" OFF)
option(MUJOCO_BUILD_TESTS "" OFF)
option(MUJOCO_BUILD_SAMPLES "" OFF)

# Expect MuJoCo sources cloned to ../../external/mujoco
add_subdirectory("${CMAKE_SOURCE_DIR}/../../external/mujoco" official_build EXCLUDE_FROM_ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Python3 COMPONENTS Interpreter)

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  set(SPEC_YAML ${CMAKE_CURRENT_SOURCE_DIR}/codegen/spec_337.yaml)
  set(GEN_H ${CMAKE_CURRENT_SOURCE_DIR}/include/mjw_exports_generated.h)
  set(GEN_C ${GEN_DIR}/mjw_exports_generated.c)

  add_custom_command(
    OUTPUT ${GEN_H} ${GEN_C}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/codegen/gen_exports.py ${SPEC_YAML} ${GEN_H} ${GEN_C}
    DEPENDS ${SPEC_YAML} ${CMAKE_CURRENT_SOURCE_DIR}/codegen/gen_exports.py
    VERBATIM
    COMMENT "Generating mjw exports from spec_337.yaml"
  )

  add_executable(mujoco_wasm337
    src/mjw_handles.c
    src/mjw_entrypoints.c
    src/mjw_compat_minimal.c
    ${GEN_C}
  )

  add_dependencies(mujoco_wasm337 mujoco)

  # Lean runtime: ES6, modularize, multi-env; keep FS and runtime helpers for JS side
  set_target_properties(mujoco_wasm337 PROPERTIES
    LINK_FLAGS "-s WASM=1 -s STACK_SIZE=5MB -s INITIAL_MEMORY=134217728 -s MAXIMUM_MEMORY=536870912 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web,worker,node -s ASSERTIONS=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=['FS','MEMFS','cwrap','ccall'] -s DISABLE_EXCEPTION_CATCHING=1 -s EXPORT_NAME=load_mujoco -Oz -flto"
  )
  target_link_libraries(mujoco_wasm337 PRIVATE mujoco)
endif()

# Native comparison harness (for regression vs. official native)
add_executable(mujoco_compare337 "native_compare.cpp")
target_link_libraries(mujoco_compare337 PRIVATE mujoco)
