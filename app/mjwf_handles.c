// Handle pool and lifecycle helpers for MuJoCo WASM
// Generated exports cover raw field access; this file only manages handles.

#include <mujoco/mujoco.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

#define MJWF_MAXH 64

// Scene state helpers are defined in the generated mjwf_scene_geom_soa.c.
void mjwf_scene_reset(int h);

typedef struct MjwfHandle {
  mjModel* m;
  mjData*  d;
  int      last_errno;
  char     last_errmsg[256];
} MjwfHandle;

static MjwfHandle g_pool[MJWF_MAXH];
static int        g_last_errno = 0;
static char       g_last_errmsg[256] = {0};
static int        g_hooks_installed = 0;

// Capture MuJoCo error/warning messages into our last-error buffers.
// This runs inside MuJoCo's error path before it throws mjXError, so we
// can surface a useful message to JS even if the call fails.
static void mjwf_err_sink(const char* msg) {
  g_last_errno = 1;
  if (msg && msg[0]) {
    strncpy(g_last_errmsg, msg, sizeof(g_last_errmsg) - 1);
    g_last_errmsg[sizeof(g_last_errmsg) - 1] = '\0';
  } else {
    g_last_errmsg[0] = '\0';
  }
}

static void mjwf_warn_sink(const char* msg) {
  // Record last warning opportunistically; do not change errno.
  if (msg && msg[0]) {
    strncpy(g_last_errmsg, msg, sizeof(g_last_errmsg) - 1);
    g_last_errmsg[sizeof(g_last_errmsg) - 1] = '\0';
  }
}

static void mjwf_install_mju_hooks(void) {
  if (g_hooks_installed) return;
  // These function pointers are provided by MuJoCo.
  // Installing hooks here ensures we capture messages for all subsequent calls.
  mju_user_error = mjwf_err_sink;
  mju_user_warning = mjwf_warn_sink;
  g_hooks_installed = 1;
}

static void mjwf_set_global_error(int code, const char* msg) {
  g_last_errno = code;
  if (msg && msg[0]) {
    strncpy(g_last_errmsg, msg, sizeof(g_last_errmsg) - 1);
    g_last_errmsg[sizeof(g_last_errmsg) - 1] = '\0';
  } else {
    g_last_errmsg[0] = '\0';
  }
}

static void mjwf_set_error(MjwfHandle* h, int code, const char* msg) {
  if (!h) return;
  h->last_errno = code;
  if (msg && msg[0]) {
    strncpy(h->last_errmsg, msg, sizeof(h->last_errmsg) - 1);
    h->last_errmsg[sizeof(h->last_errmsg) - 1] = '\0';
  } else {
    h->last_errmsg[0] = '\0';
  }
}

static int mjwf_alloc_handle(void) {
  for (int i = 1; i < MJWF_MAXH; ++i) {  // start from 1 for nicer ids
    if (!g_pool[i].m && !g_pool[i].d) {
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

static int mjwf_handle_ok(int h) {
  return h > 0 && h < MJWF_MAXH && g_pool[h].m && g_pool[h].d;
}

#if defined(__EMSCRIPTEN__)
static mjtNum mjwf_time_now(void) {
  // emscripten_get_now returns milliseconds; convert to seconds for mjcb_time.
  return (mjtNum)(emscripten_get_now() * 0.001);
}
#else
static mjtNum mjwf_time_now(void) {
  return 0;
}
#endif

EMSCRIPTEN_KEEPALIVE void mjwf_enable_timers(void) {
  // Install a default timer callback so that d->timer[...] accumulates stats.
  mjcb_time = mjwf_time_now;
}

EMSCRIPTEN_KEEPALIVE int mjwf_helper_errno_last_global(void) {
  return g_last_errno;
}

EMSCRIPTEN_KEEPALIVE const char* mjwf_helper_errmsg_last_global(void) {
  return g_last_errmsg;
}

EMSCRIPTEN_KEEPALIVE int mjwf_helper_errno_last(int h) {
  return mjwf_handle_ok(h) ? g_pool[h].last_errno : 0;
}

EMSCRIPTEN_KEEPALIVE const char* mjwf_helper_errmsg_last(int h) {
  return mjwf_handle_ok(h) ? g_pool[h].last_errmsg : "";
}

EMSCRIPTEN_KEEPALIVE int mjwf_helper_make_from_xml(const char* path) {
  mjwf_install_mju_hooks();
  char error[1024] = {0};
  mjModel* m = mj_loadXML(path, NULL, error, sizeof(error));
  if (!m) {
    mjwf_set_global_error(1, error[0] ? error : "mj_loadXML failed");
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
  mjwf_set_error(&g_pool[h], 0, NULL);
  return h;
}

EMSCRIPTEN_KEEPALIVE int mjwf_helper_valid(int h) {
  return mjwf_handle_ok(h) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE void mjwf_helper_free(int h) {
  if (!mjwf_handle_ok(h)) {
    return;
  }
  mjwf_scene_reset(h);
  mj_deleteData(g_pool[h].d);
  mj_deleteModel(g_pool[h].m);
  mjwf_free_slot(h);
}

EMSCRIPTEN_KEEPALIVE mjModel* mjwf_helper_model_ptr(int h) {
  return mjwf_handle_ok(h) ? g_pool[h].m : NULL;
}

EMSCRIPTEN_KEEPALIVE mjData* mjwf_helper_data_ptr(int h) {
  return mjwf_handle_ok(h) ? g_pool[h].d : NULL;
}

// Internal accessors used by generated code.
mjModel* _mjwf_model_of(int h) {
  return mjwf_helper_model_ptr(h);
}

mjData* _mjwf_data_of(int h) {
  return mjwf_helper_data_ptr(h);
}
