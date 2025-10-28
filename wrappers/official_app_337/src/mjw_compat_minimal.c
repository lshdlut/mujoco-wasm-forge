#include <stdint.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif

// Forward declarations from handle-based surface and generated exports
extern int      mjw_make_from_xml(const char* path);
extern int      mjw_step(int h, int n);
extern int      mjw_valid(int h);
extern double*  mjw_qpos_ptr(int h);
extern double*  mjw_qvel_ptr(int h);

static int g_h = 0;  // compatibility: single global instance for minimal tests

EMSCRIPTEN_KEEPALIVE
int mjw_init(const char* xmlpath) {
  g_h = mjw_make_from_xml(xmlpath);
  return g_h > 0 ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
void mjw_step_demo(int steps) {
  if (!mjw_valid(g_h) || steps <= 0) return;
  (void)mjw_step(g_h, steps);
}

EMSCRIPTEN_KEEPALIVE
double mjw_qpos0(void) {
  if (!mjw_valid(g_h)) return 0.0;
  double* p = mjw_qpos_ptr(g_h);
  return p ? p[0] : 0.0;
}

EMSCRIPTEN_KEEPALIVE
double mjw_qvel0(void) {
  if (!mjw_valid(g_h)) return 0.0;
  double* p = mjw_qvel_ptr(g_h);
  return p ? p[0] : 0.0;
}

