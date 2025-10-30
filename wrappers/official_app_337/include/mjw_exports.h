// Unified exports for MuJoCo WASM 3.3.7 (forge)
// Keep declarations minimal and stable; all functions are C ABI compatible.

#pragma once

#include <stdint.h>

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

// ----- Global error (for creation failures) -----
EMSCRIPTEN_KEEPALIVE int         mjwf_errno_last_global(void);
EMSCRIPTEN_KEEPALIVE const char* mjwf_errmsg_last_global(void);

// ----- Handle and lifecycle -----
EMSCRIPTEN_KEEPALIVE int  mjwf_make_from_xml(const char* path);
EMSCRIPTEN_KEEPALIVE void mjwf_free(int h);
EMSCRIPTEN_KEEPALIVE int  mjwf_valid(int h);
EMSCRIPTEN_KEEPALIVE int  mjwf_step(int h, int n);
EMSCRIPTEN_KEEPALIVE int  mjwf_forward(int h);
EMSCRIPTEN_KEEPALIVE int  mjwf_reset(int h);

// ----- Per-handle error -----
EMSCRIPTEN_KEEPALIVE int         mjwf_errno_last(int h);
EMSCRIPTEN_KEEPALIVE const char* mjwf_errmsg_last(int h);

// ----- Dimensions (partial; codegen may add more) -----
EMSCRIPTEN_KEEPALIVE int mjwf_nq(int h);
EMSCRIPTEN_KEEPALIVE int mjwf_nv(int h);
EMSCRIPTEN_KEEPALIVE int mjwf_nu(int h);
EMSCRIPTEN_KEEPALIVE int mjwf_nsensordata(int h);

// ----- Time -----
EMSCRIPTEN_KEEPALIVE double mjwf_timestep(int h);
EMSCRIPTEN_KEEPALIVE double mjwf_time(int h);

// ----- Views (pointers) -----
EMSCRIPTEN_KEEPALIVE double* mjwf_qpos_ptr(int h);
EMSCRIPTEN_KEEPALIVE double* mjwf_qvel_ptr(int h);
EMSCRIPTEN_KEEPALIVE double* mjwf_ctrl_ptr(int h);
EMSCRIPTEN_KEEPALIVE double* mjwf_sensordata_ptr(int h);

// ----- Writers (rw views) -----
EMSCRIPTEN_KEEPALIVE void mjwf_set_qpos(int h, const double* buf, int n);
EMSCRIPTEN_KEEPALIVE void mjwf_set_qvel(int h, const double* buf, int n);
EMSCRIPTEN_KEEPALIVE void mjwf_set_ctrl(int h, const double* buf, int n);

// ----- Names / indices -----
// type uses mjOBJ_* enums from MuJoCo
EMSCRIPTEN_KEEPALIVE const char* mjwf_name_at(int h, int type, int id);
EMSCRIPTEN_KEEPALIVE int         mjwf_name2id(int h, int type, const char* name);

#ifdef __cplusplus
}
#endif

