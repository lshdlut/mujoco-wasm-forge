cmake_minimum_required(VERSION 3.22)
project(mujoco_official_338_wasm)

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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

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
set(MJWF_HEADERS_JSON "${CMAKE_CURRENT_BINARY_DIR}/mjapi_${MJVER}.json")
set(MJWF_IMPL_JSON "${CMAKE_CURRENT_BINARY_DIR}/nm_${MJVER}.json")
set(MJWF_ABI_DIR "${MJWF_ROOT}/dist/${MJVER}/abi")
set(MJWF_EXPORTS_JSON "${CMAKE_BINARY_DIR}/exports_${MJVER}.json")
set(MJWF_EXPORTS_LIST "${CMAKE_BINARY_DIR}/exports_${MJVER}.lst")
set(MJWF_TYPES_DTS "${CMAKE_CURRENT_BINARY_DIR}/types_${MJVER}.d.ts")
set(MJWF_IMPL_ARTIFACT "${CMAKE_CURRENT_BINARY_DIR}/lib/libmujoco.a")

add_custom_command(
  OUTPUT ${MJWF_HEADERS_JSON}
  BYPRODUCTS ${MJWF_HEADERS_JSON}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${MJWF_AUTO_DIR}
  COMMAND ${NODE_EXECUTABLE} ${MJWF_ROOT}/scripts/mujoco_abi/autogen_wrappers.mjs
          --include ${MJWF_ROOT}/external/mujoco/include
          --out ${MJWF_HEADERS_JSON}
  DEPENDS mujoco ${MJWF_ROOT}/scripts/mujoco_abi/autogen_wrappers.mjs
  COMMENT "Scanning MuJoCo headers (${MJVER})"
  VERBATIM
)
add_custom_target(mjwf_scan_${MJVER} DEPENDS ${MJWF_HEADERS_JSON})

add_custom_command(
  OUTPUT ${MJWF_IMPL_JSON}
  BYPRODUCTS ${MJWF_IMPL_JSON}
  COMMAND ${NODE_EXECUTABLE} ${MJWF_ROOT}/scripts/mujoco_abi/nm_coverage.mjs
          ${MJWF_IMPL_ARTIFACT}
          --out ${MJWF_IMPL_JSON}
  DEPENDS mujoco ${MJWF_ROOT}/scripts/mujoco_abi/nm_coverage.mjs
  COMMENT "Collecting implemented MuJoCo symbols (${MJVER})"
  VERBATIM
)
add_custom_target(mjwf_impl_${MJVER} DEPENDS ${MJWF_IMPL_JSON})
add_custom_command(
  OUTPUT
    ${MJWF_AUTO_HEADER}
    ${MJWF_AUTO_SOURCE}
    ${MJWF_EXPORTS_JSON}
    ${MJWF_EXPORTS_LIST}
    ${MJWF_TYPES_DTS}
    ${MJWF_ABI_DIR}/wrapper_exports.json
    ${MJWF_ABI_DIR}/exports_report.md
  BYPRODUCTS
    ${MJWF_AUTO_HEADER}
    ${MJWF_AUTO_SOURCE}
    ${MJWF_EXPORTS_JSON}
    ${MJWF_EXPORTS_LIST}
    ${MJWF_TYPES_DTS}
    ${MJWF_ABI_DIR}/wrapper_exports.json
    ${MJWF_ABI_DIR}/exports_report.md
  COMMAND ${NODE_EXECUTABLE} ${MJWF_ROOT}/scripts/mujoco_abi/gen_exports_from_abi.mjs
          --names-json ${MJWF_HEADERS_JSON}
          --impl-json ${MJWF_IMPL_JSON}
          --header ${MJWF_AUTO_HEADER}
          --source ${MJWF_AUTO_SOURCE}
          --version ${MJVER}
          --out ${CMAKE_CURRENT_BINARY_DIR}
          --abi ${MJWF_ABI_DIR}
  DEPENDS
          ${MJWF_HEADERS_JSON}
          ${MJWF_IMPL_JSON}
          ${MJWF_ROOT}/scripts/mujoco_abi/gen_exports_from_abi.mjs
  COMMENT "Generating wrapper aliases and export manifests (${MJVER})"
  VERBATIM
)
add_custom_target(mjwf_exports_${MJVER}
  DEPENDS
    ${MJWF_AUTO_HEADER}
    ${MJWF_AUTO_SOURCE}
    ${MJWF_EXPORTS_JSON}
    ${MJWF_EXPORTS_LIST}
    ${MJWF_TYPES_DTS}
    ${MJWF_ABI_DIR}/wrapper_exports.json
    ${MJWF_ABI_DIR}/exports_report.md
)
add_dependencies(mjwf_exports_${MJVER} mjwf_scan_${MJVER} mjwf_impl_${MJVER})
set_source_files_properties(${MJWF_AUTO_HEADER} ${MJWF_AUTO_SOURCE} PROPERTIES GENERATED TRUE)

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  add_executable(mujoco_wasm338
    ${MJWF_AUTO_SOURCE}
    ${MJWF_AUTO_DIR}/mjwf_stubs.c
  )

  add_dependencies(mujoco_wasm338 mujoco)
  add_dependencies(mujoco_wasm338 mjwf_exports_${MJVER})
  target_include_directories(mujoco_wasm338 PRIVATE ${MJWF_AUTO_DIR})
  target_link_libraries(mujoco_wasm338 PRIVATE mujoco)
  target_compile_options(mujoco_wasm338 PRIVATE "-fvisibility=hidden")
  target_link_options(mujoco_wasm338 PRIVATE
    "-sWASM=1"
    "-sSTACK_SIZE=5242880"
    "-sINITIAL_MEMORY=134217728"
    "-sMAXIMUM_MEMORY=536870912"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sENVIRONMENT=web,worker,node"
    "-sASSERTIONS=1"
    "-sEXPORT_ES6=1"
    "-sMODULARIZE=1"
    "-sFORCE_FILESYSTEM=0"
    "-sWASM_BIGINT=1"
    "-sDISABLE_EXCEPTION_CATCHING=1"
    "-sEXPORT_NAME=load_mujoco"
    "-Oz"
    "-flto"
    "-sEXPORTED_RUNTIME_METHODS=['cwrap','ccall','UTF8ToString','stringToUTF8','lengthBytesUTF8','FS','wasmExports','stackSave','stackRestore','stackAlloc','HEAP8']"
    "-sEXPORTED_FUNCTIONS=@${MJWF_EXPORTS_LIST}"
  )
endif()

# Native comparison harness (for regression vs. official native)
add_executable(mujoco_compare338 "native_compare.cpp")
target_link_libraries(mujoco_compare338 PRIVATE mujoco)

