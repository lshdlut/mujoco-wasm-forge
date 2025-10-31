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
  set(GEN_H ${CMAKE_CURRENT_SOURCE_DIR}/include/mjwf_exports_generated.h)
  set(GEN_C ${GEN_DIR}/mjwf_exports_generated.c)

  add_custom_command(
    OUTPUT ${GEN_H} ${GEN_C}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/codegen/gen_exports.py ${SPEC_YAML} ${GEN_H} ${GEN_C}
    DEPENDS ${SPEC_YAML} ${CMAKE_CURRENT_SOURCE_DIR}/codegen/gen_exports.py
    VERBATIM
    COMMENT "Generating mjwf exports from spec_337.yaml"
  )

  add_executable(mujoco_wasm337
    src/mjwf_handles.c
    src/mjwf_entrypoints.c
    src/mjwf_compat_minimal.c
    ${GEN_C}
  )

  add_dependencies(mujoco_wasm337 mujoco)

  set(MJWF_LINK_FLAGS "-s WASM=1 -s STACK_SIZE=5MB -s INITIAL_MEMORY=134217728 -s MAXIMUM_MEMORY=536870912 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web,worker,node -s ASSERTIONS=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=['FS','MEMFS','cwrap','ccall'] -s DISABLE_EXCEPTION_CATCHING=1 -s EXPORT_NAME=load_mujoco -Oz -flto")

  if (DEFINED MJWF_EXPORTS_JSON AND EXISTS "${MJWF_EXPORTS_JSON}")
    file(READ "${MJWF_EXPORTS_JSON}" MJWF_EXPORTS_JSON_TEXT)
    string(JSON MJWF_EXPORTS_LEN LENGTH "${MJWF_EXPORTS_JSON_TEXT}" required)
    if (MJWF_EXPORTS_LEN GREATER 0)
      set(MJWF_EXPORTS_LIST)
      math(EXPR MJWF_EXPORTS_LAST "${MJWF_EXPORTS_LEN}-1")
      foreach(idx RANGE 0 ${MJWF_EXPORTS_LAST})
        string(JSON MJWF_EXPORT_NAME GET "${MJWF_EXPORTS_JSON_TEXT}" required ${idx})
        list(APPEND MJWF_EXPORTS_LIST "\"_${MJWF_EXPORT_NAME}\"")
      endforeach()
      string(JSON MJWF_RUNTIME_LEN LENGTH "${MJWF_EXPORTS_JSON_TEXT}" runtime_keep)
      if (MJWF_RUNTIME_LEN GREATER 0)
        math(EXPR MJWF_RUNTIME_LAST "${MJWF_RUNTIME_LEN}-1")
        foreach(ridx RANGE 0 ${MJWF_RUNTIME_LAST})
          string(JSON MJWF_RUNTIME_NAME GET "${MJWF_EXPORTS_JSON_TEXT}" runtime_keep ${ridx})
          list(APPEND MJWF_EXPORTS_LIST "\"${MJWF_RUNTIME_NAME}\"")
        endforeach()
      endif()
      list(REMOVE_DUPLICATES MJWF_EXPORTS_LIST)
      string(JOIN "," MJWF_EXPORTS_JOINED ${MJWF_EXPORTS_LIST})
      set(MJWF_LINK_FLAGS "${MJWF_LINK_FLAGS} -sEXPORTED_FUNCTIONS=[${MJWF_EXPORTS_JOINED}]")
      message(STATUS "mjwf: using EXPORTED_FUNCTIONS from ${MJWF_EXPORTS_JSON} (count=${MJWF_EXPORTS_LEN})")
    else()
      message(WARNING "mjwf: exports list empty in ${MJWF_EXPORTS_JSON}")
    endif()
  else()
    message(STATUS "mjwf: MJWF_EXPORTS_JSON not provided; using default link exports")
  endif()

  set_target_properties(mujoco_wasm337 PROPERTIES LINK_FLAGS "${MJWF_LINK_FLAGS}")
  target_link_libraries(mujoco_wasm337 PRIVATE mujoco)
endif()

# Native comparison harness (for regression vs. official native)
add_executable(mujoco_compare337 "native_compare.cpp")
target_link_libraries(mujoco_compare337 PRIVATE mujoco)
