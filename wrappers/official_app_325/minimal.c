#include <mujoco/mujoco.h>
#include <emscripten/emscripten.h>
#include <string.h>

static mjModel* g_m = NULL;
static mjData*  g_d = NULL;

EMSCRIPTEN_KEEPALIVE
int mjw_init(const char* xmlpath) {
  if (g_d) { mj_deleteData(g_d); g_d = NULL; }
  if (g_m) { mj_deleteModel(g_m); g_m = NULL; }
  char error[1024] = {0};
  g_m = mj_loadXML(xmlpath, NULL, error, sizeof(error));
  if (!g_m) { return 0; }
  g_d = mj_makeData(g_m);
  return g_d != NULL;
}

EMSCRIPTEN_KEEPALIVE
void mjw_step_demo(int steps) {
  if (!g_m || !g_d) return;
  for (int i = 0; i < steps; ++i) {
    mj_step(g_m, g_d);
  }
}

EMSCRIPTEN_KEEPALIVE
void mjw_term(void) {
  if (g_d) { mj_deleteData(g_d); g_d = NULL; }
  if (g_m) { mj_deleteModel(g_m); g_m = NULL; }
}

EMSCRIPTEN_KEEPALIVE
int mjw_nq(void) {
  return g_m ? g_m->nq : 0;
}

EMSCRIPTEN_KEEPALIVE
double mjw_qpos0(void) {
  if (!g_d || !g_m || g_m->nq <= 0) return 0.0;
  return g_d->qpos[0];
}

// Return current qvel[0]
EMSCRIPTEN_KEEPALIVE
double mjw_qvel0(void) {
  if (!g_d || !g_m || g_m->nv <= 0) return 0.0;
  return g_d->qvel[0];
}
