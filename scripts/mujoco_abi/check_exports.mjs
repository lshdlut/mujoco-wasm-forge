#!/usr/bin/env node

/**
 * Check that the final WASM exports match the generated whitelist.
 *
 * Required flags:
 *   --abi <dir>
 *   --expected <dist/.../abi/wrapper_exports.json>
 *   --wasm <dist/.../mujoco.wasm>
 */

import { readFileSync, writeFileSync, mkdirSync } from 'node:fs';
import { resolve as pathResolve, join as pathJoin, dirname } from 'node:path';

const ALLOWED_RUNTIME = new Set([
  '__wasm_call_ctors',
  '__wasm_apply_data_relocations',
  '__wasm_init_memory_flag',
  '__heap_base',
  '__data_end',
  '__global_base',
  '__memory_base',
  '__table_base',
  '__stack_pointer',
  '__indirect_function_table',
  '__cxa_increment_exception_refcount',
  '__cxa_is_pointer_type',
  'memory',
  'table',
  'stackSave',
  'stackRestore',
  'stackAlloc',
  'setThrew',
  'emscripten_stack_get_current',
  'emscripten_stack_get_end',
  'emscripten_stack_get_base',
  'emscripten_stack_get_free',
  'emscripten_stack_init',
  'emscripten_stack_set_limits',
  'fflush',
]);

function parseArgs(argv) {
  const opts = {
    abiDir: null,
    expectedJson: null,
    wasmPath: null,
  };
  for (let i = 2; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--abi') opts.abiDir = pathResolve(argv[++i]);
    else if (arg === '--expected') opts.expectedJson = pathResolve(argv[++i]);
    else if (arg === '--wasm') opts.wasmPath = pathResolve(argv[++i]);
    else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  const missing = [];
  if (!opts.abiDir) missing.push('--abi');
  if (!opts.expectedJson) missing.push('--expected');
  if (!opts.wasmPath) missing.push('--wasm');
  if (missing.length) {
    console.error(`Missing required option(s): ${missing.join(', ')}`);
    process.exit(2);
  }
  return opts;
}

function loadJson(path) {
  return JSON.parse(readFileSync(path, 'utf8'));
}

function wasmExports(path) {
  const bytes = readFileSync(path);
  const module = new WebAssembly.Module(bytes);
  return WebAssembly.Module.exports(module).map((e) => e.name);
}

function normalize(name) {
  return name.startsWith('_') ? name : `_${name}`;
}

function ensureDir(dir) {
  mkdirSync(dir, { recursive: true });
}

function main() {
  const opts = parseArgs(process.argv);
  const manifest = loadJson(opts.expectedJson);
  const requiredSet = new Set((manifest.required || []).map((n) => normalize(n)));
  const runtimeKeep = new Set((manifest.runtime_keep || []).map((n) => normalize(n)));

  const exportsRaw = wasmExports(opts.wasmPath);
  const actualWrappers = new Set();
  const unexpectedNonMjwf = new Set();
  const forbiddenPrefixExports = new Set();

  for (const rawName of exportsRaw) {
    const norm = normalize(rawName);
    if (norm.startsWith('_mjwf_')) {
      actualWrappers.add(norm);
      continue;
    }
    if (runtimeKeep.has(norm) || runtimeKeep.has(rawName)) continue;
    if (ALLOWED_RUNTIME.has(norm) || ALLOWED_RUNTIME.has(rawName)) continue;
    if (/^_?mj(v|r|ui|p|c)_/.test(rawName)) {
      forbiddenPrefixExports.add(rawName);
      continue;
    }
    unexpectedNonMjwf.add(rawName);
  }

  const missingRequired = Array.from(requiredSet).filter((name) => !actualWrappers.has(name)).sort();
  const unexpectedList = Array.from(unexpectedNonMjwf).sort();
  const forbiddenList = Array.from(forbiddenPrefixExports).sort();

  const report = {
    wasm: opts.wasmPath,
    expected: opts.expectedJson,
    counts: {
      required: requiredSet.size,
      actualMjwf: actualWrappers.size,
      missingRequired: missingRequired.length,
      unexpectedNonMjwf: unexpectedList.length,
      forbiddenPrefix: forbiddenList.length,
    },
    missingRequired,
    unexpectedNonMjwf: unexpectedList,
    forbiddenPrefixExports: forbiddenList,
    ok: missingRequired.length === 0 && unexpectedList.length === 0 && forbiddenList.length === 0,
  };

  ensureDir(opts.abiDir);
  const outPath = pathJoin(opts.abiDir, 'exports_check.json');
  writeFileSync(outPath, JSON.stringify(report, null, 2));
  console.log(`[exports-check] wrote ${outPath}`);
  console.log(`[exports-check] status ok=${report.ok}`);

  if (!report.ok) {
    if (missingRequired.length) {
      console.error('[exports-check] missing:', missingRequired.slice(0, 20));
      if (missingRequired.length > 20) console.error('...');
    }
    if (unexpectedList.length) {
      console.error('[exports-check] unexpected non-mjwf exports:', unexpectedList.slice(0, 20));
      if (unexpectedList.length > 20) console.error('...');
    }
    if (forbiddenList.length) {
      console.error('[exports-check] forbidden prefix exports:', forbiddenList.slice(0, 20));
      if (forbiddenList.length > 20) console.error('...');
    }
    process.exit(1);
  }
}

main();
