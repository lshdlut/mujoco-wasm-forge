// MuJoCo ABI diff tool
// Compare two ABI directories and generate a machine + human readable report
// Usage:
//   node scripts/mujoco_abi/diff.mjs dist/3.2.5/abi dist/3.3.7/abi

import { readFileSync, writeFileSync } from 'node:fs';
import { join as pathJoin } from 'node:path';

function loadJSON(p) { return JSON.parse(readFileSync(p, 'utf8')); }

function toSet(arr) { const s = new Set(); for (const a of arr) s.add(a); return s; }

function diffSets(a, b) {
  const add = [], del = [], keep = [];
  const sa = toSet(a), sb = toSet(b);
  for (const x of sa) { if (!sb.has(x)) del.push(x); else keep.push(x); }
  for (const x of sb) { if (!sa.has(x)) add.push(x); }
  return { add: add.sort(), del: del.sort(), keep: keep.sort() };
}

function coverage(total, good) { return total ? Math.round(1000 * good / total) / 10 : 100; }

function loadSetFromFunctions(dir) {
  const jf = loadJSON(pathJoin(dir, 'functions.json'));
  return jf.functions.map(f => f.name);
}

function loadStructFields(dir, structName) {
  const js = loadJSON(pathJoin(dir, 'structs.json'));
  const s = js.structs?.[structName];
  if (!s || !s.fields) return [];
  return s.fields.map(f => f.name);
}

function gateCheck(dir) {
  const g = loadJSON(pathJoin(dir, 'gate.json')).gate;
  const names = new Set(loadSetFromFunctions(dir));
  const missReq = (g.required || []).filter(p => !matchAny(names, p));
  const missOpt = (g.optional || []).filter(p => !matchAny(names, p));
  const leaks = (g.excluded || []).filter(p => matchAny(names, p));
  return { miss_required: missReq, miss_optional: missOpt, excluded_leaks: leaks };
}

function globToRegex(pat) {
  return new RegExp('^' + pat.split('*').map(s => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).join('.*') + '$');
}
function matchAny(nameSet, pat) {
  const re = globToRegex(pat);
  for (const n of nameSet) if (re.test(n)) return true;
  return false;
}

function enumsDiff(dirA, dirB) {
  const ea = loadJSON(pathJoin(dirA, 'enums.json')).enums;
  const eb = loadJSON(pathJoin(dirB, 'enums.json')).enums;
  const ma = new Map(ea.map(e => [e.name, e]));
  const mb = new Map(eb.map(e => [e.name, e]));
  const namesA = Array.from(ma.keys());
  const namesB = Array.from(mb.keys());
  const s = diffSets(namesA, namesB);
  const changes = [];
  for (const k of s.keep) {
    const A = ma.get(k), B = mb.get(k);
    const memA = A.members.map(m => m.name);
    const memB = B.members.map(m => m.name);
    const d = diffSets(memA, memB);
    if (d.add.length || d.del.length) changes.push({ enum: k, added: d.add, removed: d.del });
  }
  return { added: s.add, removed: s.del, changed: changes };
}

function markdownSummary(report) {
  const f = report.functions;
  const g = report.gate;
  const cov = f.coverage_grouping;
  const lines = [];
  lines.push(`# MuJoCo ABI Diff Report`);
  lines.push(``);
  lines.push(`Functions grouped coverage: ${cov}%`);
  lines.push(`Required missing: ${g.miss_required.length}`);
  lines.push(`Optional missing: ${g.miss_optional.length}`);
  lines.push(`Excluded leaks: ${g.excluded_leaks.length}`);
  lines.push(``);
  if (g.miss_required.length) {
    lines.push(`## Missing Required`);
    lines.push(g.miss_required.map(x => `- ${x}`).join('\n'));
  }
  if (g.excluded_leaks.length) {
    lines.push(`## Excluded Leaks`);
    lines.push(g.excluded_leaks.map(x => `- ${x}`).join('\n'));
  }
  return lines.join('\n');
}

function main() {
  const [,, dirA, dirB] = process.argv;
  if (!dirA || !dirB) {
    console.error('Usage: node scripts/mujoco_abi/diff.mjs <abiA> <abiB>');
    process.exit(2);
  }
  const fa = loadSetFromFunctions(dirA);
  const fb = loadSetFromFunctions(dirB);
  const fDiff = diffSets(fa, fb);
  const fJsonB = loadJSON(pathJoin(dirB, 'functions.json'));
  const grouped = fJsonB.functions.filter(x => x.group && x.group !== 'other').length;
  const covGrouping = coverage(fJsonB.functions.length, grouped);

  const sA_model = loadStructFields(dirA, 'mjModel');
  const sB_model = loadStructFields(dirB, 'mjModel');
  const sA_data = loadStructFields(dirA, 'mjData');
  const sB_data = loadStructFields(dirB, 'mjData');
  const sdModel = diffSets(sA_model, sB_model);
  const sdData = diffSets(sA_data, sB_data);

  const eDiff = enumsDiff(dirA, dirB);
  const gate = gateCheck(dirB);

  const report = {
    compared: { from: dirA, to: dirB },
    functions: {
      added: fDiff.add, removed: fDiff.del, kept: fDiff.keep,
      coverage_grouping: covGrouping
    },
    structs: {
      mjModel: { added: sdModel.add, removed: sdModel.del },
      mjData: { added: sdData.add, removed: sdData.del }
    },
    enums: eDiff,
    gate
  };

  const outPath = pathJoin(dirB, 'diff_report.json');
  const mdPath = pathJoin(dirB, 'abi_report.md');
  report.summary_md = markdownSummary(report);
  writeFileSync(outPath, JSON.stringify(report, null, 2));
  writeFileSync(mdPath, report.summary_md);
  console.log(`[abi-diff] Report written to ${outPath}`);
  console.log(`[abi-diff] Summary written to ${mdPath}`);
}

main();
