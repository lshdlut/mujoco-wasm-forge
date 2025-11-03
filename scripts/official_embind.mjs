import path from "node:path";
import { pathToFileURL } from "node:url";
import { performance } from "node:perf_hooks";

const workspace = process.env.EMBIND_WASM_DIST
  ? path.resolve(process.env.EMBIND_WASM_DIST)
  : path.resolve(
      process.env.HOME ?? "",
      "dev/mujoco-wasm-forge/local_tools/embind/workspace/mujoco/wasm/dist",
    );

const wasmPath = path.join(workspace, "mujoco_wasm.wasm");
const jsPath = path.join(workspace, "mujoco_wasm.js");

const modFactory = (await import(pathToFileURL(jsPath).href)).default;
const Module = await modFactory({
  locateFile: (p) => (p.endsWith(".wasm") ? wasmPath : p),
});
if (Module.ready) await Module.ready;

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

const MODEL_PATH = "/tmp/official_benchmark.xml";
Module.FS.writeFile(MODEL_PATH, xml, { encoding: "utf8" });

const model = Module.MjModel.loadFromXML(MODEL_PATH);
const data = new Module.MjData(model);

const results = [];

results.push(
  runBenchmark({
    label: "mj_step (official)",
    iterations: 2000,
    warmups: 500,
    setup: () => Module.mj_resetData(model, data),
    fn: () => Module.mj_step(model, data),
  }),
);

results.push(
  runBenchmark({
    label: "mj_forward (official)",
    iterations: 4000,
    warmups: 1000,
    setup: () => {
      Module.mj_resetData(model, data);
      Module.mj_step(model, data);
    },
    fn: () => Module.mj_forward(model, data),
  }),
);

for (const r of results) {
  console.log(
    `${r.label}: iterations=${r.iterations}, total=${r.totalMs.toFixed(
      3,
    )} ms, avg=${r.avgMs.toFixed(6)} ms`,
  );
}

console.log("summary:", JSON.stringify(results, null, 2));

data.delete();
model.delete();
try {
  Module.FS.unlink(MODEL_PATH);
} catch {
  // ignore
}

function runBenchmark({ label, iterations, warmups = 0, setup, fn }) {
  if (setup) setup();
  for (let i = 0; i < warmups; i += 1) fn();
  if (setup) setup();
  const start = performance.now();
  for (let i = 0; i < iterations; i += 1) fn();
  const total = performance.now() - start;
  return { label, iterations, warmups, totalMs: total, avgMs: total / iterations };
}
