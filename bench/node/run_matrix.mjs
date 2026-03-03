import { spawnSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

function parseArgs(argv) {
  const args = {
    runs: 5,
    append: false,
    official340: '',
    official350: '',
    forge340: 'dist/3.4.0',
    forge350: 'dist/3.5.0',
    forge350Pthreads: 'dist/3.5.0/pthreads',
    out: 'bench/results',
    playRoot: '',
  };
  for (let i = 0; i < argv.length; i += 1) {
    const a = argv[i];
    if (a === '--runs') args.runs = Number(argv[++i] ?? '');
    else if (a === '--append') args.append = true;
    else if (a === '--official340') args.official340 = argv[++i] ?? '';
    else if (a === '--official350') args.official350 = argv[++i] ?? '';
    else if (a === '--forge340') args.forge340 = argv[++i] ?? '';
    else if (a === '--forge350') args.forge350 = argv[++i] ?? '';
    else if (a === '--forge350-pthreads') args.forge350Pthreads = argv[++i] ?? '';
    else if (a === '--out') args.out = argv[++i] ?? '';
    else if (a === '--play-root') args.playRoot = argv[++i] ?? '';
    else throw new Error(`Unknown arg: ${a}`);
  }
  if (!Number.isFinite(args.runs) || args.runs <= 0) throw new Error(`Invalid --runs: ${args.runs}`);
  if (!args.out) throw new Error('--out is required');
  return args;
}

function sanitizeLabel(s) {
  return String(s || '').replace(/[^a-zA-Z0-9_.-]+/g, '_');
}

function writeJson(outDir, label, idx, payload) {
  fs.mkdirSync(outDir, { recursive: true });
  const ts = new Date().toISOString().replace(/[:.]/g, '');
  const name = `${sanitizeLabel(label)}_${ts}_r${String(idx).padStart(2, '0')}.json`;
  const outPath = path.join(outDir, name);
  fs.writeFileSync(outPath, JSON.stringify(payload, null, 2), 'utf8');
  return outPath;
}

function runNode(scriptPath, args) {
  const res = spawnSync(process.execPath, [scriptPath, ...args], {
    encoding: 'utf8',
    env: { ...process.env },
  });
  if (res.error) throw res.error;
  if (res.status !== 0) {
    throw new Error(`bench script failed: ${scriptPath}\n${res.stderr || ''}`.trim());
  }

  const stdout = String(res.stdout || '').trim();
  if (!stdout) throw new Error(`bench script produced no stdout JSON: ${scriptPath}`);
  const lines = stdout.split(/\r?\n/).filter(Boolean);
  for (let i = lines.length - 1; i >= 0; i -= 1) {
    try {
      return JSON.parse(lines[i]);
    } catch {
      // continue
    }
  }
  throw new Error(`bench script stdout did not contain a JSON line: ${scriptPath}\n${stdout}`);
}

function discoverPlayRoot(explicit) {
  if (explicit) return path.resolve(explicit);
  const sibling = path.resolve(process.cwd(), '..', 'mujoco-wasm-play');
  return fs.existsSync(path.join(sibling, 'model')) ? sibling : '';
}

function main() {
  const args = parseArgs(process.argv.slice(2));
  const repoRoot = process.cwd();
  const playRoot = discoverPlayRoot(args.playRoot);
  const outRoot = path.resolve(repoRoot, args.out);
  const rawDir = path.join(outRoot, 'raw');
  if (!args.append) {
    fs.rmSync(rawDir, { recursive: true, force: true });
  }

  const forgeBench = path.resolve(repoRoot, 'bench/node/bench_forge.mjs');
  const officialBench = path.resolve(repoRoot, 'bench/node/bench_official.mjs');
  const report = path.resolve(repoRoot, 'bench/node/report.mjs');

  const targets = [
    { label: 'forge-3.4.0-single', script: forgeBench, dist: args.forge340, extraArgs: [] },
    { label: 'forge-3.5.0-single', script: forgeBench, dist: args.forge350, extraArgs: [] },
    { label: 'forge-3.5.0-pthreads', script: forgeBench, dist: args.forge350Pthreads, extraArgs: [] },
  ];

  if (args.official340) {
    targets.push({ label: 'official-3.4.0', script: officialBench, dist: args.official340, extraArgs: [] });
  }
  if (args.official350) {
    targets.push({
      label: 'official-3.5.0-hc32',
      script: officialBench,
      dist: args.official350,
      extraArgs: ['--hc', '32'],
    });
    targets.push({
      label: 'official-3.5.0-hc4',
      script: officialBench,
      dist: args.official350,
      extraArgs: ['--hc', '4'],
    });
  }

  const info = {
    timestamp: new Date().toISOString(),
    node: process.version,
    platform: process.platform,
    arch: process.arch,
    cpus: os.cpus().length,
    playRoot: playRoot || null,
  };
  fs.mkdirSync(outRoot, { recursive: true });
  fs.writeFileSync(path.join(outRoot, 'run_meta.json'), JSON.stringify(info, null, 2), 'utf8');

  for (const t of targets) {
    for (let i = 1; i <= args.runs; i += 1) {
      // eslint-disable-next-line no-console
      console.log(`[bench] ${t.label} run ${i}/${args.runs}`);
      const payload = runNode(t.script, [
        '--dist',
        t.dist,
        '--label',
        t.label,
        ...(playRoot ? ['--play-root', playRoot] : []),
        ...t.extraArgs,
      ]);
      const outPath = writeJson(rawDir, t.label, i, payload);
      // eslint-disable-next-line no-console
      console.log(`[bench] wrote ${outPath}`);
    }
  }

  const rep = spawnSync(process.execPath, [report], { encoding: 'utf8' });
  if (rep.error) throw rep.error;
  if (rep.status !== 0) throw new Error(`report failed:\n${rep.stderr || ''}`.trim());
  // eslint-disable-next-line no-console
  console.log(rep.stdout.trim());
}

main();
