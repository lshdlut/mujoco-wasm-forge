// Handle pool and lifecycle for MuJoCo WASM 3.3.7
// This isolates multi-instance management behind integer handles.

#include <mujoco/mujoco.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#define MJWF_MAXH 64

typedef struct MjwfHandle {
  mjModel* m;
  mjData*  d;
  int      last_errno;
  char     last_errmsg[256];
} MjwfHandle;

static MjwfHandle g_pool[MJWF_MAXH];
static int       g_last_errno = 0;
static char      g_last_errmsg[256] = {0};

static void mjwf_set_global_error(int code, const char* msg) {
  g_last_errno = code;
  if (msg) {
    strncpy(g_last_errmsg, msg, sizeof(g_last_errmsg)-1);
    g_last_errmsg[sizeof(g_last_errmsg)-1] = '\0';
  } else {
    g_last_errmsg[0] = '\0';
  }
}

EMSCRIPTEN_KEEPALIVE int mjwf_errno_last_global(void) { return g_last_errno; }
EMSCRIPTEN_KEEPALIVE const char* mjwf_errmsg_last_global(void) { return g_last_errmsg; }

static void mjwf_set_error(MjwfHandle* h, int code, const char* msg) {
  if (!h) return;
  h->last_errno = code;
  if (msg) {
    strncpy(h->last_errmsg, msg, sizeof(h->last_errmsg)-1);
    h->last_errmsg[sizeof(h->last_errmsg)-1] = '\0';
  } else {
    h->last_errmsg[0] = '\0';
  }
}

static int mjwf_alloc_handle(void) {
  for (int i = 1; i < MJWF_MAXH; ++i) { // start from 1 for nicer ids
    if (g_pool[i].m == NULL && g_pool[i].d == NULL) {
      g_pool[i].last_errno = 0;
      g_pool[i].last_errmsg[0] = '\0';
      return i;
    }
  }
  return -1;
}

static void mjwf_free_slot(int h) {
  if (h <= 0 || h >= MJWF_MAXH) return;
  g_pool[h].m = NULL;
  g_pool[h].d = NULL;
  g_pool[h].last_errno = 0;
  g_pool[h].last_errmsg[0] = '\0';
}

EMSCRIPTEN_KEEPALIVE int mjwf_make_from_xml(const char* path) {
  char error[1024] = {0};
  mjModel* m = mj_loadXML(path, NULL, error, sizeof(error));
  if (!m) {
    mjwf_set_global_error(1, error[0] ? error : "loadXML failed");
    return -1;
  }
  mjData* d = mj_makeData(m);
  if (!d) {
    mj_deleteModel(m);
    mjwf_set_global_error(2, "mj_makeData failed");
    return -1;
  }
  int h = mjwf_alloc_handle();
  if (h < 0) {
    mj_deleteData(d);
    mj_deleteModel(m);
    mjwf_set_global_error(3, "no free handle");
    return -1;
  }
  g_pool[h].m = m;
  g_pool[h].d = d;
  g_pool[h].last_errno = 0;
  g_pool[h].last_errmsg[0] = '\0';
  return h;
}

EMSCRIPTEN_KEEPALIVE void mjwf_free(int h) {
  if (h <= 0 || h >= MJWF_MAXH) return;
  if (g_pool[h].d) { mj_deleteData(g_pool[h].d); g_pool[h].d = NULL; }
  if (g_pool[h].m) { mj_deleteModel(g_pool[h].m); g_pool[h].m = NULL; }
  mjwf_free_slot(h);
}

EMSCRIPTEN_KEEPALIVE int mjwf_valid(int h) {
  return (h > 0 && h < MJWF_MAXH && g_pool[h].m && g_pool[h].d) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int mjwf_step(int h, int n) {
  if (!mjwf_valid(h) || n <= 0) return 0;
  MjwfHandle* H = &g_pool[h];
  for (int i = 0; i < n; ++i) {
    mj_step(H->m, H->d);
  }
  return 1;
}

EMSCRIPTEN_KEEPALIVE int mjwf_forward(int h) {
  if (!mjwf_valid(h)) return 0;
  MjwfHandle* H = &g_pool[h];
  mj_forward(H->m, H->d);
  return 1;
}

EMSCRIPTEN_KEEPALIVE int mjwf_reset(int h) {
  if (!mjwf_valid(h)) return 0;
  MjwfHandle* H = &g_pool[h];
  mj_resetData(H->m, H->d);
  return 1;
}

EMSCRIPTEN_KEEPALIVE int mjwf_errno_last(int h) {
  if (!mjwf_valid(h)) return 0;
  return g_pool[h].last_errno;
}

EMSCRIPTEN_KEEPALIVE const char* mjwf_errmsg_last(int h) {
  if (!mjwf_valid(h)) return "";
  return g_pool[h].last_errmsg;
}

// --- Time ---
EMSCRIPTEN_KEEPALIVE double mjwf_timestep(int h) { return mjwf_valid(h) ? g_pool[h].m->opt.timestep : 0.0; }
EMSCRIPTEN_KEEPALIVE double mjwf_time(int h) { return mjwf_valid(h) ? g_pool[h].d->time : 0.0; }

// Dims and pointer getters are generated from spec into mjwf_exports_generated.c.
// This file provides handle lifecycle and helpers only.

static void mjwf_copy_doubles(double* dst, const double* src, int n) {
  if (!dst || !src || n <= 0) return;
  memcpy(dst, src, (size_t)n * sizeof(double));
}

EMSCRIPTEN_KEEPALIVE void mjwf_set_qpos(int h, const double* buf, int n) {
  if (!mjwf_valid(h)) return;
  int N = g_pool[h].m->nq;
  if (n > N) n = N;
  mjwf_copy_doubles(g_pool[h].d->qpos, buf, n);
}

EMSCRIPTEN_KEEPALIVE void mjwf_set_qvel(int h, const double* buf, int n) {
  if (!mjwf_valid(h)) return;
  int N = g_pool[h].m->nv;
  if (n > N) n = N;
  mjwf_copy_doubles(g_pool[h].d->qvel, buf, n);
}

EMSCRIPTEN_KEEPALIVE void mjwf_set_ctrl(int h, const double* buf, int n) {
  if (!mjwf_valid(h)) return;
  int N = g_pool[h].m->nu;
  if (n > N) n = N;
  mjwf_copy_doubles(g_pool[h].d->ctrl, buf, n);
}

// --- Names / indices ---
EMSCRIPTEN_KEEPALIVE const char* mjwf_name_at(int h, int type, int id) {
  if (!mjwf_valid(h)) return NULL;
  const char* nm = mj_id2name(g_pool[h].m, type, id);
  return nm;
}

EMSCRIPTEN_KEEPALIVE int mjwf_name2id(int h, int type, const char* name) {
  if (!mjwf_valid(h)) return -1;
  int id = mj_name2id(g_pool[h].m, type, name);
  return id;
}

// --- Internal accessors for generator (not exported) ---
mjModel* _mjwf_model_of(int h) {
  if (!mjwf_valid(h)) return NULL;
  return g_pool[h].m;
}

mjData* _mjwf_data_of(int h) {
  if (!mjwf_valid(h)) return NULL;
  return g_pool[h].d;
}
