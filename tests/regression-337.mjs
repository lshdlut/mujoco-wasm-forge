import path from "node:path";
import { fileURLToPath } from "node:url";
import fs from "node:fs";
import assert from "node:assert/strict";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootDir = path.resolve(__dirname, "..");
const distDir = path.resolve(rootDir, "dist", "3.3.7");
const wasmPath = path.join(distDir, "mujoco.wasm");
const manifestPath = path.join(distDir, "abi", "wrapper_exports.json");

assert.ok(fs.existsSync(wasmPath), "dist/3.3.7/mujoco.wasm missing");
assert.ok(fs.existsSync(manifestPath), "dist/3.3.7/abi/wrapper_exports.json missing");

const manifest = JSON.parse(fs.readFileSync(manifestPath, "utf8"));
const wasmBytes = fs.readFileSync(wasmPath);
const wasmModule = new WebAssembly.Module(wasmBytes);
const exportsList = WebAssembly.Module.exports(wasmModule).map((e) => e.name);

const required = new Set(manifest.required || []);
const optional = new Set(manifest.optional || []);
const runtimeKeep = new Set(manifest.runtime_keep || []);

const allowedRuntime = new Set([
  "__wasm_call_ctors",
  "__wasm_apply_data_relocations",
  "__wasm_init_memory_flag",
  "__heap_base",
  "__data_end",
  "__global_base",
  "__memory_base",
  "__table_base",
  "__stack_pointer",
  "__indirect_function_table",
  "__cxa_increment_exception_refcount",
  "__cxa_is_pointer_type",
  "memory",
  "table",
  "stackSave",
  "stackRestore",
  "stackAlloc",
  "setThrew",
  "emscripten_stack_get_current",
  "emscripten_stack_get_end",
  "emscripten_stack_get_base",
  "emscripten_stack_get_free",
  "emscripten_stack_init",
  "emscripten_stack_set_limits",
  "fflush",
]);

const actual = new Set();
for (const name of exportsList) {
  actual.add(name);
  if (!name.startsWith("_")) {
    actual.add(`_${name}`);
  }
}

const missingRequired = [...required].filter((name) => !actual.has(name));
const missingOptional = [...optional].filter((name) => !actual.has(name));

const unexpected = exportsList.filter((name) => {
  const normalized = name.startsWith("_") ? name : `_${name}`;
  if (required.has(normalized) || optional.has(normalized)) return false;
  if (runtimeKeep.has(name) || runtimeKeep.has(normalized)) return false;
  if (allowedRuntime.has(name)) return false;
  return true;
});

const forbiddenPrefixes = exportsList.filter((name) => {
  const normalized = name.startsWith("_") ? name : `_${name}`;
  return /^_?mj(v|r|ui)_/.test(normalized) && !normalized.startsWith("_mjwf_");
});

assert.strictEqual(
  missingRequired.length,
  0,
  `Missing required exports: ${missingRequired.slice(0, 10).join(", ")}`,
);
assert.strictEqual(
  forbiddenPrefixes.length,
  0,
  `Forbidden prefix exports detected: ${forbiddenPrefixes.join(", ")}`,
);
assert.deepStrictEqual(unexpected, [], `Unexpected exports: ${unexpected.join(", ")}`);

console.log(
  `regression(3.3.7): exports ok (${required.size} required, optional miss=${missingOptional.length})`,
);
