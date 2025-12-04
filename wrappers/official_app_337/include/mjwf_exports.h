// Unified exports for MuJoCo WASM 3.3.7 (forge, mjwf)
// This header lists helper entrypoints plus the auto-generated exports.

#pragma once

#include <stdint.h>

struct mjModel;
struct mjData;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

// ----- ABI / versioning -----
EMSCRIPTEN_KEEPALIVE int      mjwf_abi_version(void);
EMSCRIPTEN_KEEPALIVE uint32_t mjwf_layout_hash(void);
EMSCRIPTEN_KEEPALIVE const char* mjwf_version_string(void);

// ----- Global error (creation failures) -----
EMSCRIPTEN_KEEPALIVE int         mjwf_helper_errno_last_global(void);
EMSCRIPTEN_KEEPALIVE const char* mjwf_helper_errmsg_last_global(void);

// ----- Handle and lifecycle helpers -----
EMSCRIPTEN_KEEPALIVE int      mjwf_helper_make_from_xml(const char* path);
EMSCRIPTEN_KEEPALIVE void     mjwf_helper_free(int h);
EMSCRIPTEN_KEEPALIVE int      mjwf_helper_valid(int h);
EMSCRIPTEN_KEEPALIVE mjModel* mjwf_helper_model_ptr(int h);
EMSCRIPTEN_KEEPALIVE mjData*  mjwf_helper_data_ptr(int h);

// ----- Per-handle error -----
EMSCRIPTEN_KEEPALIVE int         mjwf_helper_errno_last(int h);
EMSCRIPTEN_KEEPALIVE const char* mjwf_helper_errmsg_last(int h);

// ----- Timing helpers -----
EMSCRIPTEN_KEEPALIVE void mjwf_enable_timers(void);

#include "mjwf_exports_generated.h"

// ----- Benchmark helpers -----
EMSCRIPTEN_KEEPALIVE void mjwf_bench_mj_sensorPos(struct mjModel* m, struct mjData* d, int iterations);
EMSCRIPTEN_KEEPALIVE void mjwf_bench_mj_energyPos(struct mjModel* m, struct mjData* d, int iterations);
EMSCRIPTEN_KEEPALIVE void mjwf_bench_mj_fwdConstraint(struct mjModel* m, struct mjData* d, int iterations);

#ifdef __cplusplus
}
#endif
