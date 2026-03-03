import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

import { formatBytes, formatMs, formatNum, median, percentile } from './stats.mjs';

function readJson(filePath) {
  return JSON.parse(fs.readFileSync(filePath, 'utf8'));
}

function listJsonFiles(dir) {
  if (!fs.existsSync(dir)) return [];
  return fs
    .readdirSync(dir)
    .filter((n) => n.endsWith('.json'))
    .map((n) => path.join(dir, n))
    .sort();
}

function groupByLabel(rows) {
  const out = new Map();
  for (const r of rows) {
    const key = String(r?.label ?? '');
    if (!key) continue;
    const arr = out.get(key) ?? [];
    arr.push(r);
    out.set(key, arr);
  }
  return out;
}

function pickMetric(rows, fn) {
  const xs = [];
  for (const r of rows) {
    const v = fn(r);
    if (Number.isFinite(v)) xs.push(Number(v));
  }
  return xs;
}

function mdTable(headers, rows) {
  const head = `| ${headers.join(' | ')} |`;
  const sep = `| ${headers.map(() => '---').join(' | ')} |`;
  const body = rows.map((r) => `| ${r.join(' | ')} |`).join('\n');
  return `${head}\n${sep}\n${body}\n`;
}

function summarizeNodeBench(grouped) {
  const labels = Array.from(grouped.keys()).sort();
  const pkgRows = [];
  const sysRows = [];

  for (const label of labels) {
    const rows = grouped.get(label) ?? [];
    const first = rows[0] ?? null;
    const sizes = first?.distSizes ?? {};
    const js = sizes['mujoco.js'] ?? sizes['mujoco_wasm.js'] ?? null;
    const wasm = sizes['mujoco.wasm'] ?? sizes['mujoco_wasm.wasm'] ?? null;
    pkgRows.push([label, js != null ? formatBytes(js) : 'n/a', wasm != null ? formatBytes(wasm) : 'n/a']);

    const initMs = median(pickMetric(rows, (r) => r?.metrics?.initMs));
    const rssInit = median(pickMetric(rows, (r) => r?.metrics?.rssAfterInit));
    const rssBench = median(pickMetric(rows, (r) => r?.metrics?.rssAfterBench));
    sysRows.push([
      label,
      formatMs(initMs, { digits: 2 }),
      rssInit != null ? formatBytes(rssInit) : 'n/a',
      rssBench != null ? formatBytes(rssBench) : 'n/a',
    ]);
  }

  const modelIds = new Set();
  for (const rows of grouped.values()) {
    for (const r of rows) {
      const models = r?.models ?? {};
      for (const k of Object.keys(models)) modelIds.add(k);
    }
  }

  const modelRows = [];
  for (const id of Array.from(modelIds).sort()) {
    for (const label of labels) {
      const rows = grouped.get(label) ?? [];
      const okRows = rows.filter((r) => r?.models?.[id]?.status === 'ok');
      const fsMs = median(pickMetric(okRows, (r) => r?.models?.[id]?.fsCopyMs ?? r?.models?.[id]?.fsMountMs));
      const compileMs = median(
        pickMetric(okRows, (r) => r?.models?.[id]?.compileMs ?? r?.models?.[id]?.loadMs),
      );
      const stepsPerSec = median(pickMetric(okRows, (r) => r?.models?.[id]?.stepsPerSec));
      const cpuMsPerStep = stepsPerSec ? (1000 / stepsPerSec) : null;

      const anyRow = rows.find((r) => r?.models?.[id]);
      const status = anyRow?.models?.[id]?.status ?? 'n/a';
      modelRows.push([
        id,
        label,
        String(status),
        fsMs != null ? formatMs(fsMs, { digits: 2 }) : 'n/a',
        compileMs != null ? formatMs(compileMs, { digits: 2 }) : 'n/a',
        stepsPerSec != null ? formatNum(stepsPerSec, { digits: 1 }) : 'n/a',
        cpuMsPerStep != null ? formatNum(cpuMsPerStep, { digits: 3 }) : 'n/a',
      ]);
    }
  }

  return {
    pkgTable: mdTable(['Label', 'JS', 'WASM'], pkgRows),
    sysTable: mdTable(['Label', 'Init', 'RSS after init', 'RSS after bench'], sysRows),
    modelTable: mdTable(['Model', 'Label', 'Status', 'FS stage', 'Compile/load', 'Steps/s', 'ms/step'], modelRows),
  };
}

function summarizeBrowserBench(rows) {
  const grouped = groupByLabel(rows);
  const labels = Array.from(grouped.keys()).sort();
  const cases = [];
  for (const label of labels) {
    const rs = grouped.get(label) ?? [];
    const first = rs[0] ?? null;
    const env = first?.env ?? {};
    const model = env?.model ?? 'n/a';
    const forgeBase = env?.forgeBase ?? 'n/a';
    const hasOk = rs.some((r) => r?.status === 'ok');
    const hasSkipped = rs.some((r) => r?.status === 'skipped');
    const hasError = rs.some((r) => r?.status === 'error');
    const status = hasOk ? 'ok' : (hasSkipped ? 'skipped' : (hasError ? 'error' : 'n/a'));
    const cpu = pickMetric(rs, (r) => r?.metrics?.cpuMsPerStep);
    const fps = pickMetric(rs, (r) => r?.metrics?.fps);
    const cpuMed = median(cpu);
    const cpuP90 = percentile(cpu, 90);
    const fpsMed = median(fps);
    cases.push([
      label,
      status,
      model,
      forgeBase,
      cpuMed != null ? formatNum(cpuMed, { digits: 3 }) : 'n/a',
      cpuP90 != null ? formatNum(cpuP90, { digits: 3 }) : 'n/a',
      fpsMed != null ? formatNum(fpsMed, { digits: 1 }) : 'n/a',
    ]);
  }
  return mdTable(['Label', 'Status', 'Model', 'forgeBase', 'CPU median (ms/step)', 'CPU p90', 'FPS median'], cases);
}

function main() {
  const __filename = fileURLToPath(import.meta.url);
  const repoRoot = path.resolve(path.dirname(__filename), '..', '..');
  const resultsRoot = path.resolve(repoRoot, 'bench', 'results');
  const rawDir = path.join(resultsRoot, 'raw');
  const rawBrowserDir = path.join(resultsRoot, 'raw_browser');
  const outPath = path.join(resultsRoot, 'summary.md');

  const nodeRows = listJsonFiles(rawDir).map(readJson).filter((r) => r?.kind === 'node');
  const browserRows = listJsonFiles(rawBrowserDir).map(readJson).filter((r) => r?.kind === 'browser');

  const grouped = groupByLabel(nodeRows);
  const nodeSummary = summarizeNodeBench(grouped);
  const browserTable = summarizeBrowserBench(browserRows);

  const header = [
    '# Bench summary',
    '',
    '> This file is generated by `node bench/node/report.mjs`.',
    '> Numbers vary by machine; see `bench/README.md` for methodology.',
    '',
  ].join('\n');

  const md = [
    header,
    '## Node: package sizes',
    '',
    nodeSummary.pkgTable,
    '## Node: init + memory',
    '',
    nodeSummary.sysTable,
    '## Node: model compile + stepping',
    '',
    nodeSummary.modelTable,
    '## Browser (Play/Simulate HUD)',
    '',
    browserTable,
  ].join('\n');

  fs.mkdirSync(resultsRoot, { recursive: true });
  fs.writeFileSync(outPath, md, 'utf8');
  // eslint-disable-next-line no-console
  console.log(`[bench] wrote ${outPath}`);
}

main();
