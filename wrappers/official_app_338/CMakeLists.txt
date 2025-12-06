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

set(MJWF_PROFILE "default" CACHE STRING "MuJoCo WASM wrapper profile (default|fast|official|fast_official)")
set_property(CACHE MJWF_PROFILE PROPERTY STRINGS default fast official fast_official)

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

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set(MJWF_OPT_FLAG "-Oz")
  set(MJWF_ASSERT_FLAG "-sASSERTIONS=1")
  set(MJWF_FS_FLAG "-sFORCE_FILESYSTEM=0")
  set(MJWF_EXC_FLAG "-sDISABLE_EXCEPTION_CATCHING=1")
  set(MJWF_DEBUG_FLAGS "")
  set(MJWF_LTO_FLAG "-flto")
  if (MJWF_PROFILE STREQUAL "fast")
    set(MJWF_OPT_FLAG "-O3")
    set(MJWF_ASSERT_FLAG "-sASSERTIONS=0")
  elseif (MJWF_PROFILE STREQUAL "official")
    set(MJWF_OPT_FLAG "-O3")
    set(MJWF_ASSERT_FLAG "-sASSERTIONS=1")
    set(MJWF_FS_FLAG "-sFORCE_FILESYSTEM=1")
    set(MJWF_EXC_FLAG "-sDISABLE_EXCEPTION_CATCHING=0")
    set(MJWF_DEBUG_FLAGS "-gsource-map;-g")
    set(MJWF_LTO_FLAG "")
  elseif (MJWF_PROFILE STREQUAL "fast_official")
    set(MJWF_OPT_FLAG "-O3")
    set(MJWF_ASSERT_FLAG "-sASSERTIONS=1")
    set(MJWF_FS_FLAG "-sFORCE_FILESYSTEM=1")
    set(MJWF_EXC_FLAG "-sDISABLE_EXCEPTION_CATCHING=0")
    set(MJWF_DEBUG_FLAGS "-gsource-map;-g")
    set(MJWF_LTO_FLAG "-flto")
  endif()

  add_executable(mujoco_wasm338
    ${MJWF_AUTO_SOURCE}
    ${MJWF_AUTO_DIR}/mjwf_stubs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mjwf_handles.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mjwf_entrypoints.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mjwf_exports_generated.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mjwf_compat_minimal.c
  )

  add_dependencies(mujoco_wasm338 mujoco)
  add_dependencies(mujoco_wasm338 mjwf_exports_${MJVER})
  target_include_directories(mujoco_wasm338 PRIVATE ${MJWF_AUTO_DIR})
  target_link_libraries(mujoco_wasm338 PRIVATE mujoco)
  target_compile_options(mujoco_wasm338 PRIVATE "-fvisibility=hidden" ${MJWF_OPT_FLAG})
  target_link_options(mujoco_wasm338 PRIVATE
    "-sWASM=1"
    "-sSTACK_SIZE=5242880"
    "-sINITIAL_MEMORY=134217728"
    "-sMAXIMUM_MEMORY=536870912"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sENVIRONMENT=web,worker,node"
    "-sEXPORT_ES6=1"
    "-sMODULARIZE=1"
    "-sWASM_BIGINT=1"
    "-sEXPORT_NAME=load_mujoco"
    ${MJWF_OPT_FLAG}
    ${MJWF_ASSERT_FLAG}
    ${MJWF_FS_FLAG}
    ${MJWF_EXC_FLAG}
    ${MJWF_DEBUG_FLAGS}
    ${MJWF_LTO_FLAG}
    "-sEXPORTED_RUNTIME_METHODS=['cwrap','ccall','UTF8ToString','stringToUTF8','lengthBytesUTF8','FS','wasmExports','stackSave','stackRestore','stackAlloc','HEAP8','HEAPU8','HEAP32','HEAPU32','HEAPF32','HEAPF64']"
    "-sEXPORTED_FUNCTIONS=@${MJWF_EXPORTS_LIST}"
  )
endif()

# Native comparison harness (for regression vs. official native)
add_executable(mujoco_compare338 "native_compare.cpp")
target_link_libraries(mujoco_compare338 PRIVATE mujoco)

