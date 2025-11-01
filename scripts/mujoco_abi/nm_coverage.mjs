#!/usr/bin/env node

/**
 * Enumerate symbols implemented in libmujoco.a (B-set) using llvm-nm.
 *
 * Usage:
 *   node scripts/mujoco_abi/nm_coverage.mjs <libmujoco.a> --out build/mujoco_impl.json
 *
 * The script never exits with failure; errors are captured in the JSON payload.
 */

import { existsSync, writeFileSync } from 'node:fs';
import { spawnSync } from 'node:child_process';
import { resolve as pathResolve, dirname } from 'node:path';
import { mkdirSync } from 'node:fs';

function parseArgs(argv) {
  if (argv.length < 3) {
    console.error('Usage: node scripts/mujoco_abi/nm_coverage.mjs <libmujoco.a> [--out report.json]');
    process.exit(2);
  }
  const opts = {
    artifact: pathResolve(argv[2]),
    nmPath: process.env.LLVM_NM || process.env.EMNM || resolveDefaultNm(),
    out: null,
  };
  for (let i = 3; i < argv.length; ++i) {
    const arg = argv[i];
    if (arg === '--nm') {
      opts.nmPath = argv[++i];
    } else if (arg === '--out') {
      opts.out = pathResolve(argv[++i]);
    } else {
      console.error(`Unknown argument: ${arg}`);
      process.exit(2);
    }
  }
  return opts;
}

function resolveDefaultNm() {
  if (process.env.EMSDK) {
    return pathResolve(process.env.EMSDK, 'upstream', 'bin', 'llvm-nm');
  }
  return 'llvm-nm';
}

function ensureDirFor(filePath) {
  if (!filePath) return;
  mkdirSync(dirname(filePath), { recursive: true });
}

function runNm(nmPath, artifact) {
  const res = spawnSync(nmPath, ['-g', '--defined-only', '-P', artifact], { encoding: 'utf8' });
  return res;
}

function collectSymbols(stdout) {
  const names = [];
  const lines = stdout.split('\n');
  for (const line of lines) {
    if (!line) continue;
    if (line.includes('no symbols')) continue;
    if (line.endsWith(':')) continue;
    const [symbol] = line.split(/\s+/);
    if (!symbol) continue;
    const normalized = symbol.startsWith('_') ? symbol.slice(1) : symbol;
    if (normalized) names.push(normalized);
  }
  return names;
}

function main() {
  const opts = parseArgs(process.argv);
  const report = {
    artifact: opts.artifact,
    nmPath: opts.nmPath,
    ok: false,
    symbols: [],
    count: 0,
    error: null,
  };

  if (!existsSync(opts.artifact)) {
    report.error = `Artifact not found: ${opts.artifact}`;
  } else {
    const res = runNm(opts.nmPath, opts.artifact);
    if (res.error) {
      report.error = `Failed to execute ${opts.nmPath}: ${res.error.message || String(res.error)}`;
    } else if (res.status !== 0) {
      report.error = `${opts.nmPath} exited with code ${res.status}: ${res.stderr || ''}`.trim();
    } else {
      const symbols = collectSymbols(res.stdout);
      symbols.sort();
      report.symbols = symbols;
      report.count = symbols.length;
      report.ok = true;
    }
  }

  if (opts.out) {
    ensureDirFor(opts.out);
    writeFileSync(opts.out, JSON.stringify(report, null, 2));
    console.log(`[nm-scan] wrote ${opts.out} (symbols=${report.count}, ok=${report.ok})`);
  } else {
    process.stdout.write(JSON.stringify(report, null, 2));
  }
}

main();
