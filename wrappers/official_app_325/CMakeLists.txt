cmake_minimum_required(VERSION 3.22)
project(mujoco_official_325_wasm)

if(NOT DEFINED ENV{EMSDK})
  message(FATAL_ERROR "EMSDK not set; run emsdk_env")
endif()
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/_wasm")

option(MUJOCO_BUILD_EXAMPLES "" OFF)
option(MUJOCO_BUILD_SIMULATE "" OFF)
option(MUJOCO_BUILD_TESTS "" OFF)
option(MUJOCO_BUILD_SAMPLES "" OFF)

# Expect MuJoCo sources cloned to ../../external/mujoco
add_subdirectory("${CMAKE_SOURCE_DIR}/../../external/mujoco" official_build EXCLUDE_FROM_ALL)

add_executable(mujoco_wasm325 "minimal.c")
# Lean runtime: ES6, modularize, multi-env; keep FS for demo convenience for now
set_target_properties(mujoco_wasm325 PROPERTIES
  LINK_FLAGS "-s WASM=1 -s STACK_SIZE=5MB -s INITIAL_MEMORY=134217728 -s MAXIMUM_MEMORY=536870912 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web,worker,node -s ASSERTIONS=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=['FS','MEMFS','cwrap','ccall'] -s DISABLE_EXCEPTION_CATCHING=1 -s EXPORT_NAME=load_mujoco -Oz -flto"
)

target_link_libraries(mujoco_wasm325 PRIVATE mujoco)

# Native comparison harness (for regression vs. official native)
add_executable(mujoco_compare325 "native_compare.cpp")
target_link_libraries(mujoco_compare325 PRIVATE mujoco)
