cmake_minimum_required(VERSION 3.22)
project(mjwasm_forge)

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

set(MJWF_PROFILE "fast" CACHE STRING "MuJoCo WASM wrapper profile (default|fast|official|fast_official)")
set_property(CACHE MJWF_PROFILE PROPERTY STRINGS default fast official fast_official)

# Resolve repo root from the app directory and pull in MuJoCo sources.
get_filename_component(MJWF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
add_subdirectory("${MJWF_ROOT}/external/mujoco" official_build EXCLUDE_FROM_ALL)

find_program(NODE_EXECUTABLE node)
if (NOT NODE_EXECUTABLE)
  message(FATAL_ERROR "node executable not found; required for wrapper generation")
endif()
set(MJWF_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MJWF_AUTO_HEADER "${MJWF_APP_DIR}/mjwf_abi_funcs.h")
set(MJWF_AUTO_SOURCE "${MJWF_APP_DIR}/mjwf_abi_funcs.c")
set(MJWF_IMPL_JSON "${CMAKE_CURRENT_BINARY_DIR}/nm_${MJVER}.json")
set(MJWF_ABI_DIR "${MJWF_ROOT}/dist/${MJVER}/abi")
set(MJWF_IMPL_ARTIFACT "${CMAKE_CURRENT_BINARY_DIR}/lib/libmujoco.a")

# Prefer an explicit cache variable (e.g. -DMJWF_EXPORTS_LIST=...) or an environment override,
# otherwise default to the versioned dist export list.
if (NOT MJWF_EXPORTS_LIST)
  if (DEFINED ENV{MJWF_EXPORTS_LIST})
    set(MJWF_EXPORTS_LIST "$ENV{MJWF_EXPORTS_LIST}")
  else()
    set(MJWF_EXPORTS_LIST "${MJWF_ABI_DIR}/exports.lst")
  endif()
endif()

add_custom_command(
  OUTPUT ${MJWF_IMPL_JSON}
  COMMAND ${NODE_EXECUTABLE} ${MJWF_ROOT}/abi_impl/nm_coverage.mjs
          ${MJWF_IMPL_ARTIFACT}
          --out ${MJWF_IMPL_JSON}
  DEPENDS mujoco ${MJWF_ROOT}/abi_impl/nm_coverage.mjs
  COMMENT "Collecting implemented MuJoCo symbols (${MJVER})"
  VERBATIM
)
add_custom_target(mjwf_impl_${MJVER} DEPENDS ${MJWF_IMPL_JSON})

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  option(MJWF_ENABLE_PLUGINS "Link MuJoCo plugin libraries into the wasm build" ON)
  option(MJWF_ENABLE_SIMD "Enable WebAssembly SIMD (-msimd128)" ON)
  set(MJWF_SIMD_FLAG "")
  if (MJWF_ENABLE_SIMD)
    set(MJWF_SIMD_FLAG "-msimd128")
  endif()
  set(MJWF_OPT_FLAG "-Oz")
  set(MJWF_ASSERT_FLAG "-sASSERTIONS=1")
  set(MJWF_FS_FLAG "-sFORCE_FILESYSTEM=0")
  # MuJoCo uses C++ exceptions (mjXError/mjCError) for XML parsing and model
  # compilation error handling. Disabling exception catching turns recoverable
  # load errors (e.g., missing referenced files) into a hard abort.
  set(MJWF_EXC_FLAG "-sDISABLE_EXCEPTION_CATCHING=0")
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

  add_executable(mjwasm_forge
    ${MJWF_AUTO_SOURCE}
    ${MJWF_APP_DIR}/mjwf_stubs.c
    ${MJWF_APP_DIR}/mjwf_handles.c
    ${MJWF_APP_DIR}/mjwf_abi_structs.c
    ${MJWF_APP_DIR}/mjwf_scene_geom_soa.c
  )

  add_dependencies(mjwasm_forge mujoco)
  target_include_directories(mjwasm_forge PRIVATE ${MJWF_AUTO_DIR})
  target_link_libraries(mjwasm_forge PRIVATE mujoco)
  set(_mjwf_plugin_targets "")
  cmake_path(SET _mjwf_plugin_root NORMALIZE "${MJWF_ROOT}/external/mujoco/plugin")
  get_property(_mjwf_mujoco_subdirs DIRECTORY "${MJWF_ROOT}/external/mujoco" PROPERTY SUBDIRECTORIES)
  foreach(_mjwf_subdir IN LISTS _mjwf_mujoco_subdirs)
    cmake_path(IS_PREFIX _mjwf_plugin_root "${_mjwf_subdir}" NORMALIZE _mjwf_is_plugin_dir)
    if (_mjwf_is_plugin_dir)
      get_property(_mjwf_dir_targets DIRECTORY "${_mjwf_subdir}" PROPERTY BUILDSYSTEM_TARGETS)
      foreach(_mjwf_target IN LISTS _mjwf_dir_targets)
        if (TARGET ${_mjwf_target})
          get_target_property(_mjwf_target_type ${_mjwf_target} TYPE)
          if (_mjwf_target_type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY)$")
            list(APPEND _mjwf_plugin_targets ${_mjwf_target})
          endif()
        endif()
      endforeach()
    endif()
  endforeach()
  list(REMOVE_DUPLICATES _mjwf_plugin_targets)
  if (MJWF_ENABLE_PLUGINS)
    # MuJoCo plugins register via static constructors. When the plugins are
    # built as archives (as is the case under Emscripten), the registration
    # object files would otherwise be stripped unless we force-load them.
    if (_mjwf_plugin_targets)
      target_link_libraries(
        mjwasm_forge
        PRIVATE
        "-Wl,--whole-archive"
        ${_mjwf_plugin_targets}
        "-Wl,--no-whole-archive"
      )
    else()
      set(_mjwf_sensor_plugin_dir "${MJWF_ROOT}/external/mujoco/plugin/sensor")
      if (EXISTS "${_mjwf_sensor_plugin_dir}/register.cc")
        message(
          STATUS
            "MJWF_ENABLE_PLUGINS is ON but no MuJoCo plugin targets were discovered under "
            "${_mjwf_plugin_root}; compiling sensor/touch_grid sources directly"
        )
        target_sources(
          mjwasm_forge
          PRIVATE
          "${_mjwf_sensor_plugin_dir}/register.cc"
          "${_mjwf_sensor_plugin_dir}/touch_grid.cc"
        )
        # Some plugins include internal MuJoCo headers from src/.
        target_include_directories(mjwasm_forge PRIVATE "${MJWF_ROOT}/external/mujoco/src")
      else()
        message(
          WARNING
            "MJWF_ENABLE_PLUGINS is ON but no MuJoCo plugin targets were discovered under "
            "${_mjwf_plugin_root}"
        )
      endif()
    endif()
  endif()
  target_compile_options(mjwasm_forge PRIVATE "-fvisibility=hidden" ${MJWF_OPT_FLAG} ${MJWF_SIMD_FLAG})
  # Ensure exception catching is enabled consistently at compile+link time; MuJoCo relies on
  # try/catch for load-time fallbacks (e.g., namespaced refs in <replicate>/<attach>).
  target_compile_options(mjwasm_forge PRIVATE ${MJWF_EXC_FLAG})
  foreach(_target mujoco ${_mjwf_plugin_targets})
    if (TARGET ${_target})
      target_compile_options(${_target} PRIVATE ${MJWF_EXC_FLAG} ${MJWF_SIMD_FLAG})
    endif()
  endforeach()
  target_link_options(mjwasm_forge PRIVATE
    "-sWASM=1"
    "-sSTACK_SIZE=5242880"
    "-sINITIAL_MEMORY=134217728"
    "-sMAXIMUM_MEMORY=2147483648"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sENVIRONMENT=web,worker,node"
    "-sEXPORT_ES6=1"
    "-sMODULARIZE=1"
    "-sWASM_BIGINT=1"
    "-sEXPORT_NAME=load_mujoco"
    "-lexports.js"
    ${MJWF_OPT_FLAG}
    ${MJWF_SIMD_FLAG}
    ${MJWF_ASSERT_FLAG}
    ${MJWF_FS_FLAG}
    ${MJWF_EXC_FLAG}
    ${MJWF_DEBUG_FLAGS}
    ${MJWF_LTO_FLAG}
    "-sEXPORTED_RUNTIME_METHODS=['cwrap','ccall','UTF8ToString','stringToUTF8','lengthBytesUTF8','FS','wasmExports','stackSave','stackRestore','stackAlloc','HEAP8','HEAPU8','HEAP32','HEAPU32','HEAPF32','HEAPF64']"
    "-sEXPORTED_FUNCTIONS=@${MJWF_EXPORTS_LIST}"
  )
endif()

# Native comparison harness (for regression vs. official native).
# The source is optional in this refactored layout; skip the target when
# native_compare.cpp is not present.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/native_compare.cpp")
  add_executable(mujoco_compare "native_compare.cpp")
  target_link_libraries(mujoco_compare PRIVATE mujoco)
endif()
