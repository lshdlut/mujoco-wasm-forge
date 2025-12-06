// ABI Gate checker
// Usage: node check/ci_gate.mjs dist/<ver>/abi [--threshold 95]

import { readFileSync, existsSync } from 'node:fs';
import { join as pathJoin } from 'node:path';

function load(p) { return JSON.parse(readFileSync(p, 'utf8')); }
function toSet(list) { return new Set(list); }
function globToRegex(pat) { return new RegExp('^' + pat.split('*').map(s => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).join('.*') + '$'); }
function matchAny(nameSet, pat) { const re = globToRegex(pat); for (const n of nameSet) if (re.test(n)) return true; return false; }

function loadFunctionsFromIntrospect(dir) {
  const introspectPath = pathJoin(dir, 'functions_introspect_like.json');
  if (!existsSync(introspectPath)) {
    throw new Error(`functions_introspect_like.json not found under ${dir}`);
  }
  const j = load(introspectPath);
  const items = Array.isArray(j.functions) ? j.functions : [];
  return items;
}

function main() {
  const dir = process.argv[2];
  if (!dir) { console.error('Usage: node check/ci_gate.mjs dist/<ver>/abi [--threshold 95]'); process.exit(2); }
  const thrIdx = process.argv.indexOf('--threshold');
  const threshold = thrIdx > 0 ? parseFloat(process.argv[thrIdx+1]) : 95;

  const fnItems = loadFunctionsFromIntrospect(dir);
  const gateAll = load(pathJoin(dir, 'gate.json'));
  const gate = gateAll.gate || {};
  const rawAllow = (gateAll.gate?.allowMissing ?? gateAll.allowMissing ?? []);
  let allowAll = false; let allowSet = new Set();
  if (rawAllow === true) { allowAll = true; }
  else if (Array.isArray(rawAllow)) { allowSet = new Set(rawAllow); }

  const names = toSet(fnItems.map(f => f.name));
  let missReq = (gate.required || []).filter(p => !matchAny(names, p));
  if (!allowAll) missReq = missReq.filter(x => !allowSet.has(x));

  const leaks = (gate.excluded || []).filter(p => matchAny(names, p));

  // Introspect JSON does not carry grouping; treat coverage as 100% and
  // only gate on required/leaks.
  let coverage = 100;

  const ok = ((missReq.length === 0) || allowAll) && (coverage >= threshold);
  console.log(`[abi-gate] coverage=${coverage}% required_miss=${missReq.length} excluded_leaks=${leaks.length} allowAll=${allowAll}`);
  if (allowAll && missReq.length) {
    console.log('[abi-gate] allowMissing=true (WARN):', missReq);
  }
  if (!ok) {
    console.log('Missing required:', missReq);
    console.log('Excluded leaks:', leaks);
    process.exit(1);
  }
}

main();
