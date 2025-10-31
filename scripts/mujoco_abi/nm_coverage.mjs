#!/usr/bin/env node

// Verify that every mj_/mju_/mjs_* implementation has a corresponding mjwf_* wrapper.
// Usage: node scripts/mujoco_abi/nm_coverage.mjs <artifact> <wrapper_exports.json> [--nm path] [--out report.json]

import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { spawnSync } from 'node:child_process';
import { resolve as pathResolve } from 'node:path';

const ALLOW_PREFIX = [
  /^mj(?![vru]_)/,
  /^mju_/,
  /^mjs_/,
];

const DENY_PREFIX = [
  /^mjv_/,
  /^mjr_/,
  /^mjui_/,
];

function parseArgs(argv) {
  if (argv.length < 4) {
    console.error('Usage: node scripts/mujoco_abi/nm_coverage.mjs <artifact> <wrapper_exports.json> [--nm path] [--out report.json]');
    process.exit(2);
  }
  const opts = {
    artifact: pathResolve(argv[2]),
    manifest: pathResolve(argv[3]),
    nm: process.env.LLVM_NM || 'llvm-nm',
    out: null,
  };
  for (let i = 4; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--nm') opts.nm = argv[++i];
    else if (arg === '--out') opts.out = pathResolve(argv[++i]);
    else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  return opts;
}

function normalizeSymbol(name) {
  return name.startsWith('_') ? name.slice(1) : name;
}

function allowSymbol(name) {
  return ALLOW_PREFIX.some((re) => re.test(name));
}

function denySymbol(name) {
  return DENY_PREFIX.some((re) => re.test(name));
}

function collectImplementationSymbols(nmPath, target) {
  const res = spawnSync(nmPath, ['--format=posix', '--defined-only', target], { encoding: 'utf8' });
  if (res.error) {
    console.error(`[nm-coverage] Failed to execute ${nmPath}:`, res.error);
    process.exit(2);
  }
  if (res.status !== 0) {
    console.error(`[nm-coverage] ${nmPath} exited with code ${res.status}:`, res.stderr);
    process.exit(res.status);
  }
  const symbols = new Set();
  const lines = res.stdout.split('\n');
  for (const line of lines) {
    const trimmed = line.trim();
    if (!trimmed) continue;
    const parts = trimmed.split(/\s+/);
    const name = normalizeSymbol(parts[parts.length - 1]);
    if (!name) continue;
    if (!allowSymbol(name)) continue;
    if (denySymbol(name)) continue;
    symbols.add(name);
  }
  return symbols;
}

function collectGeneratedSymbols(manifestPath) {
  const data = JSON.parse(readFileSync(manifestPath, 'utf8'));
  const wrappers = new Set([
    ...(data.required || []),
    ...(data.optional || []),
  ]);
  const originals = new Set();
  for (const wrapper of wrappers) {
    if (!wrapper.startsWith('mjwf_')) continue;
    const candidate = wrapper.replace(/^mjwf_/, '');
    if (allowSymbol(candidate) && !denySymbol(candidate)) {
      originals.add(candidate);
    }
  }
  return { wrappers, originals };
}

function main() {
  const opts = parseArgs(process.argv);
  if (!existsSync(opts.artifact)) {
    console.error(`[nm-coverage] Artifact not found: ${opts.artifact}`);
    process.exit(1);
  }
  if (!existsSync(opts.manifest)) {
    console.error(`[nm-coverage] Manifest not found: ${opts.manifest}`);
    process.exit(1);
  }

  const impl = collectImplementationSymbols(opts.nm, opts.artifact);
  const { wrappers, originals } = collectGeneratedSymbols(opts.manifest);
  const missing = Array.from(impl).filter((sym) => !originals.has(sym)).sort();
  const unusedWrappers = Array.from(originals).filter((sym) => !impl.has(sym)).sort();

  const report = {
    artifact: opts.artifact,
    manifest: opts.manifest,
    implCount: impl.size,
    generatedCount: originals.size,
    wrappersCount: wrappers.size,
    missing,
    unusedWrappers,
    ok: missing.length === 0,
  };

  if (opts.out) {
    writeFileSync(opts.out, JSON.stringify(report, null, 2));
    console.log(`[nm-coverage] wrote ${opts.out}`);
  }

  if (missing.length) {
    console.error('[nm-coverage] Missing wrappers for:', missing.slice(0, 20));
    if (missing.length > 20) console.error('...');
    process.exit(1);
  }

  console.log(`[nm-coverage] OK (impl=${impl.size}, wrappers=${originals.size})`);
}

main();
