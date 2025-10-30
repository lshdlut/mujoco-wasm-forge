#include <stdint.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

// Forward declarations from handle-based surface and generated exports (mjwf)
extern int      mjwf_make_from_xml(const char* path);
extern int      mjwf_step(int h, int n);
extern int      mjwf_valid(int h);
extern double*  mjwf_qpos_ptr(int h);
extern double*  mjwf_qvel_ptr(int h);

static int g_h = 0;  // compatibility: single global instance for minimal tests

EMSCRIPTEN_KEEPALIVE
int mjwf_init(const char* xmlpath) {
  g_h = mjwf_make_from_xml(xmlpath);
  return g_h > 0 ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
void mjwf_step_demo(int steps) {
  if (!mjwf_valid(g_h) || steps <= 0) return;
  (void)mjwf_step(g_h, steps);
}

EMSCRIPTEN_KEEPALIVE
double mjwf_qpos0(void) {
  if (!mjwf_valid(g_h)) return 0.0;
  double* p = mjwf_qpos_ptr(g_h);
  return p ? p[0] : 0.0;
}

EMSCRIPTEN_KEEPALIVE
double mjwf_qvel0(void) {
  if (!mjwf_valid(g_h)) return 0.0;
  double* p = mjwf_qvel_ptr(g_h);
  return p ? p[0] : 0.0;
}

