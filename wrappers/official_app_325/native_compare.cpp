// Minimal native harness to generate golden vectors for regression tests.
// Loads an XML model, simulates fixed steps, and prints JSON with qpos[0], qvel[0].

#include <mujoco/mujoco.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void die(const char* msg) {
  std::fprintf(stderr, "%s\n", msg);
  std::exit(2);
}

int main(int argc, char** argv) {
  const char* xmlpath = argc > 1 ? argv[1] : nullptr;
  int steps = argc > 2 ? std::atoi(argv[2]) : 200;
  if (!xmlpath || steps <= 0) {
    std::fprintf(stderr, "Usage: %s <model.xml> [steps]\n", argv[0]);
    return 2;
  }

  char error[1024] = {0};
  mjModel* m = mj_loadXML(xmlpath, nullptr, error, sizeof(error));
  if (!m) {
    std::fprintf(stderr, "loadXML failed: %s\n", error);
    return 2;
  }
  mjData* d = mj_makeData(m);
  if (!d) die("makeData failed");

  // JSON header
  std::printf("{\n");
  std::printf("  \"nq\": %d,\n", m->nq);
  std::printf("  \"nv\": %d,\n", m->nv);
  std::printf("  \"qpos0\": [");

  // simulate and collect
  for (int i = 0; i < steps; ++i) {
    mj_step(m, d);
    // print qpos[0]
    std::printf("%.*g", 17, m->nq > 0 ? d->qpos[0] : 0.0);
    if (i + 1 < steps) std::printf(", ");
  }
  std::printf("],\n");

  // qvel0 series
  // reset and re-run to produce aligned series (or continue with last state)
  // Here we re-run from reset for consistency.
  mj_resetData(m, d);
  std::printf("  \"qvel0\": [");
  for (int i = 0; i < steps; ++i) {
    mj_step(m, d);
    std::printf("%.*g", 17, m->nv > 0 ? d->qvel[0] : 0.0);
    if (i + 1 < steps) std::printf(", ");
  }
  std::printf("]\n");

  std::printf("}\n");

  mj_deleteData(d);
  mj_deleteModel(m);
  return 0;
}

