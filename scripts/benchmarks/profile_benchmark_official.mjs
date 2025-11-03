import path from "node:path";
import { pathToFileURL } from "node:url";
import { performance } from "node:perf_hooks";
import { access } from "node:fs/promises";

const ITERATIONS = Number(process.env.BENCH_ITERATIONS ?? 100_000);
const WARMUPS = Number(process.env.BENCH_WARMUPS ?? 1_000);

const distDir = process.env.EMBIND_WASM_DIST
  ? path.resolve(process.env.EMBIND_WASM_DIST)
  : path.resolve(
      process.env.HOME ?? "",
      "dev/mujoco-wasm-forge/local_tools/embind/workspace/mujoco/wasm/dist",
    );

const wasmPath = path.join(distDir, "mujoco_wasm.wasm");
const jsPath = path.join(distDir, "mujoco_wasm.js");

await ensureFile(jsPath);
await ensureFile(wasmPath);

const factory = (await import(pathToFileURL(jsPath).href)).default;
const Module = await factory({
  locateFile: (p) => (p.endsWith(".wasm") ? wasmPath : p),
});
if (Module.ready) await Module.ready;

const readBack = process.env.BENCH_READBACK === "1";
const preForward = process.env.BENCH_PREFORWARD === "1";
const batchMode = process.env.BENCH_BATCH === "1";

const MODEL_PATH = "/tmp/official_bench.xml";
const xml = `<?xml version="1.0"?>
<mujoco model="pendulum">
  <option timestep="0.002" gravity="0 0 -9.81"/>
  <worldbody>
    <body name="link" pos="0 0 0.1">
      <joint name="hinge" type="hinge" axis="0 1 0" damping="0.01"/>
      <geom type="capsule" fromto="0 0 0 0 0 0.2" size="0.02" density="1000"/>
    </body>
  </worldbody>
</mujoco>`;

Module.FS.writeFile(MODEL_PATH, xml, { encoding: "utf8" });

const model = Module.MjModel.loadFromXML(MODEL_PATH);
const data = new Module.MjData(model);

const qposValue = data.qpos;
const qposView =
  typeof qposValue === "function"
    ? qposValue.call(data)
    : qposValue && qposValue.length ? qposValue : null;
const qposCopy = readBack && qposView ? new Float64Array(qposView.length) : null;

if (batchMode) {
  console.warn("Batch mode not supported for official embind benchmark; running loop mode.");
}

const entries = [
  { label: "mj_step", fn: Module.mj_step },
  { label: "mj_forward", fn: Module.mj_forward },
  { label: "mj_sensorPos", fn: Module.mj_sensorPos },
  { label: "mj_energyPos", fn: Module.mj_energyPos },
  { label: "mj_fwdConstraint", fn: Module.mj_fwdConstraint },
];

const results = entries.map(({ label, fn }) =>
  runBenchmark({
    label,
    fn,
    setup: () => Module.mj_resetData(model, data),
  }),
);

for (const r of results) {
  console.log(
    `${r.label}: iterations=${r.iterations}, total=${r.total.toFixed(
      3,
    )} ms, avg=${r.avg.toFixed(6)} ms`,
  );
}
console.log(
  "summary:",
  JSON.stringify(
    results.map(({ label, iterations, warmups, total, avg }) => ({
      label,
      iterations,
      warmups,
      totalMs: total,
      avgMs: avg,
    })),
    null,
    2,
  ),
);

cleanup();

function runBenchmark({ label, fn, setup }) {
  if (typeof fn !== "function") {
    throw new Error(`Missing embind function for ${label}`);
  }

  const needsPreForward = preForward && label !== "mj_step";
  const runSetup = () => {
    if (typeof setup === "function") setup();
    if (needsPreForward) Module.mj_forward(model, data);
  };

  runSetup();
  for (let i = 0; i < WARMUPS; i += 1) fn(model, data);
  runSetup();
  const start = performance.now();
  for (let i = 0; i < ITERATIONS; i += 1) {
    fn(model, data);
    if (readBack && qposView && qposCopy) {
      qposCopy.set(qposView);
    }
  }
  const total = performance.now() - start;
  return { label, iterations: ITERATIONS, warmups: WARMUPS, total, avg: total / ITERATIONS };
}

async function ensureFile(p) {
  try {
    await access(p);
  } catch {
    throw new Error(`Required file missing: ${p}`);
  }
}

function cleanup() {
  data.delete();
  model.delete();
  try {
    Module.FS.unlink(MODEL_PATH);
  } catch {
    // ignore
  }
}

