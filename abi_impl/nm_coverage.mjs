#!/usr/bin/env node

/**
 * Enumerate symbols implemented in libmujoco.a (B-set) using llvm-nm.
 *
 * Usage:
 *   node abi_impl/nm_coverage.mjs <libmujoco.a> --out build/mujoco_impl.json
 *
 * The script never exits with failure; errors are captured in the JSON payload.
 */

import { existsSync, writeFileSync, mkdirSync } from 'node:fs';
import { spawnSync } from 'node:child_process';
import { resolve as pathResolve, dirname, delimiter as pathDelimiter } from 'node:path';

function parseArgs(argv) {
  if (argv.length < 3) {
    console.error('Usage: node abi_impl/nm_coverage.mjs <libmujoco.a> [--out report.json]');
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

function findExecutable(candidates) {
  const pathEnv = process.env.PATH || '';
  const pathEntries = pathEnv.split(pathDelimiter).filter(Boolean);

  for (const cand of candidates) {
    if (!cand) continue;
    // Absolute path candidates.
    if (cand.includes('/') || cand.includes('\\')) {
      if (existsSync(cand)) return cand;
      continue;
    }
    for (const dir of pathEntries) {
      const full = pathResolve(dir, cand);
      if (existsSync(full)) return cand;
      if (process.platform === 'win32' && existsSync(`${full}.exe`)) {
        return `${cand}.exe`;
      }
    }
  }

  return null;
}

function resolveDefaultNm() {
  const candidates = [];
  if (process.env.EMSDK) {
    candidates.push(pathResolve(process.env.EMSDK, 'upstream', 'bin', 'llvm-nm'));
  }
  candidates.push(
    'llvm-nm',
    'llvm-nm-19',
    'llvm-nm-18',
    'llvm-nm-17',
    'llvm-nm-16',
    'llvm-nm-15',
    'llvm-nm-14',
    'llvm-nm-13',
    'nm',
  );

  const found = findExecutable(candidates);
  return found || 'llvm-nm';
}

function ensureDirFor(filePath) {
  if (!filePath) return;
  mkdirSync(dirname(filePath), { recursive: true });
}

function normalizeArtifactPath(artifact) {
  const norm = artifact.replace(/\\/g, '/');
  const idx = norm.indexOf('/build/');
  if (idx !== -1) {
    // Keep the build-relative portion only, so different workspaces agree.
    return norm.slice(idx + 1);
  }
  const parts = norm.split('/').filter(Boolean);
  return parts.length ? parts[parts.length - 1] : artifact;
}

function normalizeNmPath(nmPath) {
  const norm = nmPath.replace(/\\/g, '/');
  const parts = norm.split('/').filter(Boolean);
  return parts.length ? parts[parts.length - 1] : nmPath;
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
    artifact: normalizeArtifactPath(opts.artifact),
    nmPath: normalizeNmPath(opts.nmPath),
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
