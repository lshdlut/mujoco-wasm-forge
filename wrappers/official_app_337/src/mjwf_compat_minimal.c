#include <stdint.h>
#include <mujoco/mujoco.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

// Forward declarations from helper surface and generated exports (mjwf)
extern int       mjwf_helper_make_from_xml(const char* path);
extern void      mjwf_helper_free(int h);
extern int       mjwf_helper_valid(int h);
extern mjModel*  mjwf_helper_model_ptr(int h);
extern mjData*   mjwf_helper_data_ptr(int h);
extern double*   mjwf_data_qpos_ptr(int h);
extern double*   mjwf_data_qvel_ptr(int h);

static int g_h = 0;  // compatibility: single global instance for minimal tests
static mjModel* g_m = NULL;
static mjData*  g_d = NULL;

EMSCRIPTEN_KEEPALIVE
int mjwf_init(const char* xmlpath) {
  g_h = mjwf_helper_make_from_xml(xmlpath);
  if (g_h <= 0) {
    g_m = NULL;
    g_d = NULL;
    return 0;
  }
  g_m = mjwf_helper_model_ptr(g_h);
  g_d = mjwf_helper_data_ptr(g_h);
  return (g_m && g_d) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
void mjwf_step_demo(int steps) {
  if (!mjwf_helper_valid(g_h) || !g_m || !g_d || steps <= 0) return;
  for (int i = 0; i < steps; ++i) {
    mj_step(g_m, g_d);
  }
}

EMSCRIPTEN_KEEPALIVE
double mjwf_qpos0(void) {
  if (!mjwf_helper_valid(g_h)) return 0.0;
  double* p = mjwf_data_qpos_ptr(g_h);
  return p ? p[0] : 0.0;
}

EMSCRIPTEN_KEEPALIVE
double mjwf_qvel0(void) {
  if (!mjwf_helper_valid(g_h)) return 0.0;
  double* p = mjwf_data_qvel_ptr(g_h);
  return p ? p[0] : 0.0;
}

