cmake_minimum_required(VERSION 3.22)
project(mujoco_official_325_wasm)

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

find_program(NODE_EXECUTABLE node)
if (NOT NODE_EXECUTABLE)
  message(FATAL_ERROR "node executable not found; required for wrapper generation")
endif()

get_filename_component(MJWF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(MJWF_AUTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../auto")
set(MJWF_AUTO_HEADER "${MJWF_AUTO_DIR}/mjwf_auto_exports.h")
set(MJWF_AUTO_SOURCE "${MJWF_AUTO_DIR}/mjwf_auto_exports.c")

if (NOT TARGET mjwf_auto_wrappers)
  add_custom_command(
    OUTPUT ${MJWF_AUTO_HEADER} ${MJWF_AUTO_SOURCE}
    COMMAND ${NODE_EXECUTABLE} ${MJWF_ROOT}/scripts/mujoco_abi/autogen_wrappers.mjs
            --include ${MJWF_ROOT}/external/mujoco/include
            --header ${MJWF_AUTO_HEADER}
            --source ${MJWF_AUTO_SOURCE}
            --json ${MJWF_AUTO_DIR}/mjwf_auto_exports.json
    DEPENDS ${MJWF_ROOT}/scripts/mujoco_abi/autogen_wrappers.mjs
    COMMENT "Generating auto mjwf_* wrappers"
    VERBATIM
  )
  add_custom_target(mjwf_auto_wrappers DEPENDS ${MJWF_AUTO_HEADER} ${MJWF_AUTO_SOURCE})
endif()

set(MJWF_ABI_DIR "${MJWF_ROOT}/dist/${MJVER}/abi")
set(MJWF_EXPORTS_JSON "${CMAKE_BINARY_DIR}/exports_${MJVER}.json")
set(MJWF_EXPORTS_LIST "${CMAKE_BINARY_DIR}/exports_${MJVER}.lst")

add_custom_command(
  OUTPUT ${MJWF_EXPORTS_JSON} ${MJWF_EXPORTS_LIST}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${MJWF_ABI_DIR}
  COMMAND ${NODE_EXECUTABLE} ${MJWF_ROOT}/scripts/mujoco_abi/gen_exports_from_abi.mjs
          ${MJWF_ABI_DIR}
          --header ${MJWF_AUTO_HEADER}
          --header ${MJWF_ROOT}/wrappers/official_app_337/include/mjwf_exports.h
          --version ${MJVER}
          --out ${CMAKE_BINARY_DIR}
  DEPENDS
          ${MJWF_ROOT}/scripts/mujoco_abi/gen_exports_from_abi.mjs
          ${MJWF_AUTO_HEADER}
          ${MJWF_ROOT}/wrappers/official_app_337/include/mjwf_exports.h
)
add_custom_target(mjwf_exports_${MJVER} DEPENDS ${MJWF_EXPORTS_JSON} ${MJWF_EXPORTS_LIST})
add_dependencies(mjwf_exports_${MJVER} mjwf_auto_wrappers)

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  add_executable(mujoco_wasm325
    "minimal.c"
    ${MJWF_AUTO_SOURCE}
  )
  add_dependencies(mujoco_wasm325 mjwf_exports_${MJVER})
  include_directories(${MJWF_AUTO_DIR})
  # Lean runtime: ES6, modularize, multi-env; keep FS for demo convenience for now
  set(MJWF_LINK_FLAGS "-s WASM=1 -s STACK_SIZE=5MB -s INITIAL_MEMORY=134217728 -s MAXIMUM_MEMORY=536870912 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web,worker,node -s ASSERTIONS=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=['FS','MEMFS','cwrap','ccall'] -s DISABLE_EXCEPTION_CATCHING=1 -s EXPORT_NAME=load_mujoco -Oz -flto -sEXPORTED_FUNCTIONS=@${MJWF_EXPORTS_LIST}")

  set_target_properties(mujoco_wasm325 PROPERTIES LINK_FLAGS "${MJWF_LINK_FLAGS}")
  target_compile_options(mujoco_wasm325 PRIVATE "-fvisibility=hidden")

  target_link_libraries(mujoco_wasm325 PRIVATE mujoco)
endif()

# Native comparison harness (for regression vs. official native)
add_executable(mujoco_compare325 "native_compare.cpp")
target_link_libraries(mujoco_compare325 PRIVATE mujoco)
