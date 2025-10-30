// Misc entrypoints: ABI, version info, layout hash

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <mujoco/mujoco.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#define MJW_ABI_VERSION 1

// Adjust when export surface changes in a breaking way
#define MJW_LAYOUT_HASH 0x3370A1B3u

EMSCRIPTEN_KEEPALIVE int mjwf_abi_version(void) {
  return MJW_ABI_VERSION;
}

EMSCRIPTEN_KEEPALIVE uint32_t mjwf_layout_hash(void) {
  return MJW_LAYOUT_HASH;
}

EMSCRIPTEN_KEEPALIVE const char* mjwf_version_string(void) {
  // MuJoCo exposes mj_versionString(). We return that when available.
  static char buf[128];
  const char* mv = mj_versionString();
  // The forge target version is 3.3.7
  snprintf(buf, sizeof(buf), "mujoco %s | forge 3.3.7", mv ? mv : "unknown");
  return buf;
}

