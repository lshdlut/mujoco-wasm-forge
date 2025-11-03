import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";
import assert from "node:assert/strict";
import { performance } from "node:perf_hooks";
import { access } from "node:fs/promises";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const distDir = path.resolve(__dirname, "../dist/3.3.8-alpha");
const wasmPath = path.join(distDir, "mujoco.wasm");
const jsPath = path.join(distDir, "mujoco.js");

await ensureFile(jsPath);
await ensureFile(wasmPath);

const modFactory = (await import(pathToFileURL(jsPath).href)).default;
const Module = await modFactory({
  locateFile: (p) => (p.endsWith(".wasm") ? wasmPath : p),
});
if (Module.ready) await Module.ready;

const parseXMLString = Module.cwrap(
  "mjwf_mj_parseXMLString",
  "number",
  ["string", "number", "number", "number"],
);
const compile = bindExport("mjwf_mj_compile", "number", ["number", "number"]);
const deleteSpec = bindExport("mjwf_mj_deleteSpec", null, ["number"]);
const makeData = bindExport("mjwf_mj_makeData", "number", ["number"]);
const deleteData = bindExport("mjwf_mj_deleteData", null, ["number"]);
const deleteModel = bindExport("mjwf_mj_deleteModel", null, ["number"]);
const resetData = bindExport("mjwf_mj_resetData", null, ["number", "number"]);
const step = bindExport("mjwf_mj_step", null, ["number", "number"]);
const forward = bindExport("mjwf_mj_forward", null, ["number", "number"]);

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

const stackTop = Module.stackSave();

const specPtr = parseXMLString(xml, 0, 0, 0);
assert.notStrictEqual(specPtr, 0, "mj_parseXMLString returned null");

const modelPtr = compile(specPtr, 0);
assert.notStrictEqual(modelPtr, 0, "mj_compile returned null");

const dataPtr = makeData(modelPtr);
assert.notStrictEqual(dataPtr, 0, "mj_makeData returned null");

const results = [];

results.push(
  runBenchmark({
    label: "mj_step",
    iterations: 2000,
    warmups: 500,
    setup: () => resetData(modelPtr, dataPtr),
    fn: () => step(modelPtr, dataPtr),
  }),
);

results.push(
  runBenchmark({
    label: "mj_forward",
    iterations: 4000,
    warmups: 1000,
    setup: () => {
      resetData(modelPtr, dataPtr);
      step(modelPtr, dataPtr); // move slightly off origin
    },
    fn: () => forward(modelPtr, dataPtr),
  }),
);

for (const r of results) {
  console.log(
    `benchmark(${r.label}): iterations=${r.iterations}, total=${r.totalMs.toFixed(
      3,
    )} ms, avg=${r.avgMs.toFixed(6)} ms`,
  );
}
console.log("benchmark summary (json):");
console.log(JSON.stringify(results, null, 2));

deleteData(dataPtr);
deleteModel(modelPtr);
deleteSpec(specPtr);
Module.stackRestore(stackTop);

function runBenchmark({ label, iterations, warmups = 0, setup, fn }) {
  if (setup) setup();
  for (let i = 0; i < warmups; i += 1) fn();
  if (setup) setup();
  const start = performance.now();
  for (let i = 0; i < iterations; i += 1) fn();
  const total = performance.now() - start;
  return { label, iterations, warmups, totalMs: total, avgMs: total / iterations };
}

async function ensureFile(p) {
  try {
    await access(p);
  } catch {
    throw new Error(`Required file missing: ${p}`);
  }
}

function bindExport(name, returnType, argTypes) {
  const direct = Module.wasmExports?.[`_${name}`];
  if (typeof direct === "function") {
    return direct;
  }
  return Module.cwrap(name, returnType, argTypes);
}
