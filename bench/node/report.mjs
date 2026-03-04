import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

import { formatBytes, formatMs, formatNum, median, percentile } from './stats.mjs';

function escapeCsvCell(v) {
  if (v === null || v === undefined) return '';
  const s = String(v);
  if (s.includes('"') || s.includes(',') || s.includes('\n') || s.includes('\r')) {
    return `"${s.replaceAll('"', '""')}"`;
  }
  return s;
}

function writeCsv(outPath, headers, rows) {
  const lines = [];
  lines.push(headers.map(escapeCsvCell).join(','));
  for (const r of rows) {
    lines.push(r.map(escapeCsvCell).join(','));
  }
  fs.mkdirSync(path.dirname(outPath), { recursive: true });
  fs.writeFileSync(outPath, `${lines.join('\n')}\n`, 'utf8');
}

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
  const pkgRowsMd = [];
  const sysRowsMd = [];
  const pkgRowsRaw = [];
  const sysRowsRaw = [];

  for (const label of labels) {
    const rows = grouped.get(label) ?? [];
    const first = rows[0] ?? null;
    const sizes = first?.distSizes ?? {};
    const js = sizes['mujoco.js'] ?? sizes['mujoco_wasm.js'] ?? null;
    const wasm = sizes['mujoco.wasm'] ?? sizes['mujoco_wasm.wasm'] ?? null;
    pkgRowsMd.push([label, js != null ? formatBytes(js) : 'n/a', wasm != null ? formatBytes(wasm) : 'n/a']);
    pkgRowsRaw.push([label, js ?? null, wasm ?? null]);

    const initMs = median(pickMetric(rows, (r) => r?.metrics?.initMs));
    const rssInit = median(pickMetric(rows, (r) => r?.metrics?.rssAfterInit));
    const rssPeak = median(pickMetric(rows, (r) => r?.metrics?.rssPeakBytes));
    const rssBench = median(pickMetric(rows, (r) => r?.metrics?.rssAfterBench));
    sysRowsMd.push([
      label,
      formatMs(initMs, { digits: 2 }),
      rssInit != null ? formatBytes(rssInit) : 'n/a',
      rssPeak != null ? formatBytes(rssPeak) : 'n/a',
      rssBench != null ? formatBytes(rssBench) : 'n/a',
    ]);
    sysRowsRaw.push([label, initMs ?? null, rssInit ?? null, rssPeak ?? null, rssBench ?? null]);
  }

  const modelIds = new Set();
  for (const rows of grouped.values()) {
    for (const r of rows) {
      const models = r?.models ?? {};
      for (const k of Object.keys(models)) modelIds.add(k);
    }
  }

  const modelRowsMd = [];
  const modelRowsRaw = [];
  for (const id of Array.from(modelIds).sort()) {
    for (const label of labels) {
      const rows = grouped.get(label) ?? [];
      const okRows = rows.filter((r) => r?.models?.[id]?.status === 'ok');
      const fsMs = median(pickMetric(okRows, (r) => r?.models?.[id]?.fsCopyMs ?? r?.models?.[id]?.fsMountMs));
      const compileMs = median(
        pickMetric(okRows, (r) => r?.models?.[id]?.compileMs ?? r?.models?.[id]?.loadMs),
      );
      const firstStepMs = median(pickMetric(okRows, (r) => r?.models?.[id]?.firstStepMs));
      const ttfsMs = median(pickMetric(okRows, (r) => r?.models?.[id]?.ttfsMs));
      const stepsPerSec = median(pickMetric(okRows, (r) => r?.models?.[id]?.stepsPerSec));
      const cpuMsPerStep = stepsPerSec ? (1000 / stepsPerSec) : null;

      const anyRow = rows.find((r) => r?.models?.[id]);
      const status = anyRow?.models?.[id]?.status ?? 'n/a';
      modelRowsMd.push([
        id,
        label,
        String(status),
        fsMs != null ? formatMs(fsMs, { digits: 2 }) : 'n/a',
        compileMs != null ? formatMs(compileMs, { digits: 2 }) : 'n/a',
        firstStepMs != null ? formatMs(firstStepMs, { digits: 2 }) : 'n/a',
        ttfsMs != null ? formatMs(ttfsMs, { digits: 2 }) : 'n/a',
        stepsPerSec != null ? formatNum(stepsPerSec, { digits: 1 }) : 'n/a',
        cpuMsPerStep != null ? formatNum(cpuMsPerStep, { digits: 3 }) : 'n/a',
      ]);
      modelRowsRaw.push([
        id,
        label,
        String(status),
        fsMs ?? null,
        compileMs ?? null,
        firstStepMs ?? null,
        ttfsMs ?? null,
        stepsPerSec ?? null,
        cpuMsPerStep ?? null,
      ]);
    }
  }

  return {
    pkgTable: mdTable(['Label', 'JS', 'WASM'], pkgRowsMd),
    sysTable: mdTable(['Label', 'Init', 'RSS after init', 'RSS peak', 'RSS after bench'], sysRowsMd),
    modelTable: mdTable(['Model', 'Label', 'Status', 'FS stage', 'Compile/load', 'First step', 'TTFS', 'Steps/s', 'ms/step'], modelRowsMd),
    labels,
    modelIds: Array.from(modelIds).sort(),
    pkgRowsRaw,
    sysRowsRaw,
    modelRowsRaw,
  };
}

function summarizeNodeFfi(grouped) {
  const labels = Array.from(grouped.keys()).sort();
  const metricNames = new Set();
  for (const rows of grouped.values()) {
    for (const r of rows) {
      const ffi = r?.metrics?.ffi ?? null;
      if (!ffi || typeof ffi !== 'object') continue;
      for (const k of Object.keys(ffi)) metricNames.add(k);
    }
  }

  const names = Array.from(metricNames).sort();
  const rowsMd = [];
  const rowsRaw = [];
  for (const name of names) {
    for (const label of labels) {
      const rows = grouped.get(label) ?? [];
      const ns = pickMetric(rows, (r) => r?.metrics?.ffi?.[name]?.nsPerCall);
      const ms = pickMetric(rows, (r) => r?.metrics?.ffi?.[name]?.totalMs);
      const calls = median(pickMetric(rows, (r) => r?.metrics?.ffi?.[name]?.calls));
      const nsMed = median(ns);
      const nsP90 = percentile(ns, 90);
      const msMed = median(ms);
      if (nsMed == null && msMed == null) continue;
      rowsMd.push([
        name,
        label,
        calls != null ? formatNum(calls, { digits: 0 }) : 'n/a',
        nsMed != null ? formatNum(nsMed, { digits: 1 }) : 'n/a',
        nsP90 != null ? formatNum(nsP90, { digits: 1 }) : 'n/a',
        msMed != null ? formatMs(msMed, { digits: 2 }) : 'n/a',
      ]);
      rowsRaw.push([name, label, calls ?? null, nsMed ?? null, nsP90 ?? null, msMed ?? null]);
    }
  }
  return {
    md: rowsMd.length
      ? mdTable(['Metric', 'Label', 'Calls', 'ns/call (median)', 'ns/call (p90)', 'totalMs (median)'], rowsMd)
      : '',
    rowsRaw,
  };
}

function summarizeNodeReload(grouped) {
  const labels = Array.from(grouped.keys()).sort();
  const rowsMd = [];
  const rowsRaw = [];
  for (const label of labels) {
    const rows = grouped.get(label) ?? [];
    const iters = median(pickMetric(rows, (r) => r?.metrics?.reload?.pendulum?.iterations));
    const totalMs = median(pickMetric(rows, (r) => r?.metrics?.reload?.pendulum?.totalMs));
    const msPerIter = median(pickMetric(rows, (r) => r?.metrics?.reload?.pendulum?.msPerIter));
    const rssBefore = median(pickMetric(rows, (r) => r?.metrics?.reload?.pendulum?.rssBefore));
    const rssAfter = median(pickMetric(rows, (r) => r?.metrics?.reload?.pendulum?.rssAfter));
    const rssPeak = median(pickMetric(rows, (r) => r?.metrics?.reload?.pendulum?.rssPeakBytes));
    if (iters == null && totalMs == null && rssPeak == null) continue;
    const drift = (rssAfter != null && rssBefore != null) ? (rssAfter - rssBefore) : null;
    rowsMd.push([
      label,
      iters != null ? formatNum(iters, { digits: 0 }) : 'n/a',
      totalMs != null ? formatMs(totalMs, { digits: 2 }) : 'n/a',
      msPerIter != null ? formatNum(msPerIter, { digits: 3 }) : 'n/a',
      rssBefore != null ? formatBytes(rssBefore) : 'n/a',
      rssAfter != null ? formatBytes(rssAfter) : 'n/a',
      drift != null ? formatBytes(drift) : 'n/a',
      rssPeak != null ? formatBytes(rssPeak) : 'n/a',
    ]);
    rowsRaw.push([label, iters ?? null, totalMs ?? null, msPerIter ?? null, rssBefore ?? null, rssAfter ?? null, drift ?? null, rssPeak ?? null]);
  }
  return {
    md: rowsMd.length
      ? mdTable(['Label', 'Iters', 'totalMs (median)', 'ms/iter (median)', 'RSS before', 'RSS after', 'RSS drift', 'RSS peak'], rowsMd)
      : '',
    rowsRaw,
  };
}

function modelMetricsFor(grouped, label, modelId) {
  const rows = grouped.get(label) ?? [];
  const okRows = rows.filter((r) => r?.models?.[modelId]?.status === 'ok');
  const fsMs = median(pickMetric(okRows, (r) => r?.models?.[modelId]?.fsCopyMs ?? r?.models?.[modelId]?.fsMountMs));
  const compileMs = median(pickMetric(okRows, (r) => r?.models?.[modelId]?.compileMs ?? r?.models?.[modelId]?.loadMs));
  const stepsPerSec = median(pickMetric(okRows, (r) => r?.models?.[modelId]?.stepsPerSec));
  const msPerStep = stepsPerSec ? (1000 / stepsPerSec) : null;
  const any = rows.find((r) => r?.models?.[modelId]);
  const status = any?.models?.[modelId]?.status ?? 'n/a';
  return { status, fsMs, compileMs, stepsPerSec, msPerStep };
}

function summarizeNodeComparisons(grouped, modelIds, pairs) {
  const sections = [];
  const rowsOut = [];
  for (const p of pairs) {
    const { id: pairId, title, leftLabel, rightLabel } = p;
    if (!grouped.has(leftLabel) || !grouped.has(rightLabel)) continue;

    const rows = [];
    for (const id of modelIds) {
      const left = modelMetricsFor(grouped, leftLabel, id);
      const right = modelMetricsFor(grouped, rightLabel, id);
      if (left.status !== 'ok' || right.status !== 'ok') continue;

      const compileSpeedup = (left.compileMs != null && right.compileMs != null && left.compileMs > 0)
        ? (right.compileMs / left.compileMs)
        : null;
      const stepSpeedup = (left.msPerStep != null && right.msPerStep != null && left.msPerStep > 0)
        ? (right.msPerStep / left.msPerStep)
        : null;

      rowsOut.push([
        pairId,
        title,
        id,
        leftLabel,
        rightLabel,
        left.compileMs ?? null,
        right.compileMs ?? null,
        compileSpeedup,
        left.msPerStep ?? null,
        right.msPerStep ?? null,
        stepSpeedup,
      ]);

      rows.push([
        id,
        left.compileMs != null ? formatMs(left.compileMs, { digits: 2 }) : 'n/a',
        right.compileMs != null ? formatMs(right.compileMs, { digits: 2 }) : 'n/a',
        compileSpeedup != null ? formatNum(compileSpeedup, { digits: 2 }) : 'n/a',
        left.msPerStep != null ? formatNum(left.msPerStep, { digits: 3 }) : 'n/a',
        right.msPerStep != null ? formatNum(right.msPerStep, { digits: 3 }) : 'n/a',
        stepSpeedup != null ? formatNum(stepSpeedup, { digits: 2 }) : 'n/a',
      ]);
    }

    if (!rows.length) continue;
    sections.push(`### ${title}`);
    sections.push('');
    sections.push(
      mdTable(
        [
          'Model',
          `${leftLabel} compile`,
          `${rightLabel} compile`,
          'Compile speedup (right/left)',
          `${leftLabel} ms/step`,
          `${rightLabel} ms/step`,
          'Step speedup (right/left)',
        ],
        rows,
      ),
    );
  }

  if (!sections.length) {
    return { md: '', rowsRaw: [] };
  }
  return {
    md: ['## Node: paired comparisons (common OK subset)', '', ...sections].join('\n'),
    rowsRaw: rowsOut,
  };
}

function summarizeBrowserBench(rows) {
  const grouped = groupByLabel(rows);
  const labels = Array.from(grouped.keys()).sort();
  const cases = [];
  const casesRaw = [];
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

    const ok = rs.filter((r) => r?.status === 'ok');
    const readyToSnap = pickMetric(ok, (r) => r?.metrics?.ttfsReadyToSnapshotMs);
    const navToSnap = pickMetric(ok, (r) => r?.metrics?.ttfsNavToSnapshotMs);
    const cpu = pickMetric(ok, (r) => r?.metrics?.cpuMsPerStep);
    const fps = pickMetric(ok, (r) => r?.metrics?.fps);

    const readyToSnapMed = median(readyToSnap);
    const readyToSnapP90 = percentile(readyToSnap, 90);
    const navToSnapMed = median(navToSnap);
    const navToSnapP90 = percentile(navToSnap, 90);
    const cpuMed = median(cpu);
    const cpuP90 = percentile(cpu, 90);
    const fpsMed = median(fps);

    cases.push([
      label,
      status,
      model,
      forgeBase,
      readyToSnapMed != null ? formatMs(readyToSnapMed, { digits: 1 }) : 'n/a',
      readyToSnapP90 != null ? formatMs(readyToSnapP90, { digits: 1 }) : 'n/a',
      navToSnapMed != null ? formatMs(navToSnapMed, { digits: 1 }) : 'n/a',
      navToSnapP90 != null ? formatMs(navToSnapP90, { digits: 1 }) : 'n/a',
      cpuMed != null ? formatNum(cpuMed, { digits: 3 }) : 'n/a',
      cpuP90 != null ? formatNum(cpuP90, { digits: 3 }) : 'n/a',
      fpsMed != null ? formatNum(fpsMed, { digits: 1 }) : 'n/a',
    ]);
    casesRaw.push([
      label,
      status,
      model,
      forgeBase,
      readyToSnapMed ?? null,
      readyToSnapP90 ?? null,
      navToSnapMed ?? null,
      navToSnapP90 ?? null,
      cpuMed ?? null,
      cpuP90 ?? null,
      fpsMed ?? null,
    ]);
  }
  return {
    md: mdTable(
      [
        'Label',
        'Status',
        'Model',
        'forgeBase',
        'TTFS ready→snap (median)',
        'TTFS ready→snap (p90)',
        'TTFS nav→snap (median)',
        'TTFS nav→snap (p90)',
        'CPU median (ms/step)',
        'CPU p90',
        'FPS median',
      ],
      cases,
    ),
    rowsRaw: casesRaw,
  };
}

function main() {
  const __filename = fileURLToPath(import.meta.url);
  const repoRoot = path.resolve(path.dirname(__filename), '..', '..');
  const resultsRoot = path.resolve(repoRoot, 'bench', 'results');
  const rawDir = path.join(resultsRoot, 'raw');
  const rawBrowserDir = path.join(resultsRoot, 'raw_browser');
  const outPath = path.join(resultsRoot, 'summary.md');

  const nodeFiles = listJsonFiles(rawDir);
  const browserFiles = listJsonFiles(rawBrowserDir);
  const nodeRows = nodeFiles.map(readJson).filter((r) => r?.kind === 'node');
  const browserRows = browserFiles.map(readJson).filter((r) => r?.kind === 'browser');

  const grouped = groupByLabel(nodeRows);
  const nodeSummary = summarizeNodeBench(grouped);
  const browserSummary = summarizeBrowserBench(browserRows);
  const ffiSummary = summarizeNodeFfi(grouped);
  const reloadSummary = summarizeNodeReload(grouped);
  const comparePairs = [
    { id: '340_single_vs_official', title: '3.4.0 (single build): forge vs official', leftLabel: 'forge-3.4.0-single', rightLabel: 'official-3.4.0' },
    { id: '350_single_vs_official_hc4', title: '3.5.0 (single build): forge vs official (hc=4)', leftLabel: 'forge-3.5.0-single', rightLabel: 'official-3.5.0-hc4' },
    { id: '350_pthreads_pool4_vs_official_hc4', title: '3.5.0 (threads matched, pool=4): forge pthreads vs official (hc=4)', leftLabel: 'forge-3.5.0-pthreads', rightLabel: 'official-3.5.0-hc4' },
  ];
  const comparisons = summarizeNodeComparisons(grouped, nodeSummary.modelIds, comparePairs);

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
    comparisons.md,
    '## Node: TTFS + FFI + reload stress (derived)',
    '',
    ffiSummary.md || '_No FFI measurements found._\n',
    '',
    reloadSummary.md || '_No reload measurements found._\n',
    '## Browser (Play/Simulate HUD)',
    '',
    browserSummary.md,
  ].join('\n');

  fs.mkdirSync(resultsRoot, { recursive: true });
  fs.writeFileSync(outPath, md, 'utf8');

  const summaryJson = {
    schemaVersion: 1,
    generatedAt: new Date().toISOString(),
    sources: {
      rawDir,
      rawBrowserDir,
      nodeFiles: nodeFiles.map((p) => path.relative(resultsRoot, p).replaceAll('\\', '/')),
      browserFiles: browserFiles.map((p) => path.relative(resultsRoot, p).replaceAll('\\', '/')),
    },
    node: {
      labels: nodeSummary.labels,
      tables: {
        sizes: nodeSummary.pkgRowsRaw,
        initMemory: nodeSummary.sysRowsRaw,
        models: nodeSummary.modelRowsRaw,
        ffi: ffiSummary.rowsRaw,
        reload: reloadSummary.rowsRaw,
      },
      comparisons: {
        pairs: comparePairs,
        rows: comparisons.rowsRaw,
      },
    },
    browser: {
      table: browserSummary.rowsRaw,
    },
  };
  fs.writeFileSync(path.join(resultsRoot, 'summary.json'), JSON.stringify(summaryJson, null, 2), 'utf8');
  fs.writeFileSync(path.join(resultsRoot, 'raw_manifest.json'), JSON.stringify(summaryJson.sources, null, 2), 'utf8');

  const tablesDir = path.join(resultsRoot, 'tables');
  writeCsv(path.join(tablesDir, 'node_sizes.csv'), ['label', 'js_bytes', 'wasm_bytes'], nodeSummary.pkgRowsRaw);
  writeCsv(
    path.join(tablesDir, 'node_init_memory.csv'),
    ['label', 'init_ms', 'rss_after_init_bytes', 'rss_peak_bytes', 'rss_after_bench_bytes'],
    nodeSummary.sysRowsRaw,
  );
  writeCsv(
    path.join(tablesDir, 'node_models.csv'),
    ['model', 'label', 'status', 'fs_stage_ms', 'compile_or_load_ms', 'first_step_ms', 'ttfs_ms', 'steps_per_sec', 'ms_per_step'],
    nodeSummary.modelRowsRaw,
  );
  writeCsv(
    path.join(tablesDir, 'node_ffi.csv'),
    ['metric', 'label', 'calls_median', 'ns_per_call_median', 'ns_per_call_p90', 'total_ms_median'],
    ffiSummary.rowsRaw,
  );
  writeCsv(
    path.join(tablesDir, 'node_reload.csv'),
    ['label', 'iterations_median', 'total_ms_median', 'ms_per_iter_median', 'rss_before_bytes', 'rss_after_bytes', 'rss_drift_bytes', 'rss_peak_bytes'],
    reloadSummary.rowsRaw,
  );
  writeCsv(
    path.join(tablesDir, 'node_comparisons.csv'),
    [
      'pair_id',
      'pair_title',
      'model',
      'left_label',
      'right_label',
      'left_compile_ms',
      'right_compile_ms',
      'compile_ratio_right_over_left',
      'left_ms_per_step',
      'right_ms_per_step',
      'step_ratio_right_over_left',
    ],
    comparisons.rowsRaw,
  );
  writeCsv(
    path.join(tablesDir, 'browser_hud.csv'),
    [
      'label',
      'status',
      'model',
      'forge_base',
      'ttfs_ready_to_snapshot_median_ms',
      'ttfs_ready_to_snapshot_p90_ms',
      'ttfs_nav_to_snapshot_median_ms',
      'ttfs_nav_to_snapshot_p90_ms',
      'cpu_median_ms_per_step',
      'cpu_p90',
      'fps_median',
    ],
    browserSummary.rowsRaw,
  );
  // eslint-disable-next-line no-console
  console.log(`[bench] wrote ${outPath}`);
}

main();
