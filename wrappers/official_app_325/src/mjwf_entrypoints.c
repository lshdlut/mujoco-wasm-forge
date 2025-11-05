// Misc entrypoints (mjwf): ABI, version info, layout hash

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

#define MJWF_ABI_VERSION 1

// Adjust when export surface changes in a breaking way
#define MJWF_LAYOUT_HASH 0x3250A1B3u

EMSCRIPTEN_KEEPALIVE int mjwf_abi_version(void) {
  return MJWF_ABI_VERSION;
}

EMSCRIPTEN_KEEPALIVE uint32_t mjwf_layout_hash(void) {
  return MJWF_LAYOUT_HASH;
}

EMSCRIPTEN_KEEPALIVE const char* mjwf_version_string(void) {
  // MuJoCo exposes mj_versionString(). We return that when available.
  static char buf[128];
  const char* mv = mj_versionString();
  // The forge target version is 3.2.5
  snprintf(buf, sizeof(buf), "mujoco %s | forge 3.2.5", mv ? mv : "unknown");
  return buf;
}

EMSCRIPTEN_KEEPALIVE void mjwf_bench_mj_sensorPos(mjModel* m, mjData* d, int iterations) {
  if (!m || !d || iterations <= 0) return;
  for (int i = 0; i < iterations; ++i) {
    mj_sensorPos(m, d);
  }
}

EMSCRIPTEN_KEEPALIVE void mjwf_bench_mj_energyPos(mjModel* m, mjData* d, int iterations) {
  if (!m || !d || iterations <= 0) return;
  for (int i = 0; i < iterations; ++i) {
    mj_energyPos(m, d);
  }
}

EMSCRIPTEN_KEEPALIVE void mjwf_bench_mj_fwdConstraint(mjModel* m, mjData* d, int iterations) {
  if (!m || !d || iterations <= 0) return;
  for (int i = 0; i < iterations; ++i) {
    mj_fwdConstraint(m, d);
  }
}

