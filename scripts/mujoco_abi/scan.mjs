// Minimal MuJoCo ABI scanner.
// - Extracts MJAPI functions from mujoco.h with file+line evidence
// - Extracts mjModel/mjData fields from headers
// - Extracts public enums from include/mujoco/*.h
// - Assembles mappings and diagnostics subsets
//
// Usage:
//   node scripts/mujoco_abi/scan.mjs --repo local_tools/mujoco --ref 3.3.7 --out dist/3.3.7/abi
//
// Notes:
// - Reads header contents via `git show <ref>:<path>`; does not mutate the repo checkout.
// - Heuristic-only parsing to bootstrap automation; refine as needed.

import { execFileSync } from 'node:child_process';
import { mkdirSync, writeFileSync, readFileSync, existsSync } from 'node:fs';
import { dirname, join as pathJoin, resolve as pathResolve } from 'node:path';

function sh(args, opts = {}) {
  const { cwd } = opts;
  return execFileSync(args[0], args.slice(1), {
    cwd,
    encoding: 'utf8',
    stdio: ['ignore', 'pipe', 'pipe'],
  }).trimEnd();
}

function parseArgs(argv) {
  const out = {};
  for (let i = 2; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--repo') out.repo = argv[++i];
    else if (a === '--ref') out.ref = argv[++i];
    else if (a === '--out') out.out = argv[++i];
    else if (a === '--debug') out.debug = true;
    else throw new Error(`Unknown arg: ${a}`);
  }
  return out;
}

function ensureDir(p) {
  mkdirSync(p, { recursive: true });
}

function gitRemote(repo) {
  try {
    const remotes = sh(['git', '-C', repo, 'remote', '-v']);
    const origin = remotes.split('\n').find(l => l.startsWith('origin')) || remotes.split('\n')[0];
    return origin ? origin.split(/\s+/)[1] : 'unknown';
  } catch {
    return 'unknown';
  }
}

function gitReadFile(repo, ref, relPath) {
  const spec = `${ref}:${relPath.replaceAll('\\', '/')}`;
  try {
    const content = sh(['git', '-C', repo, 'show', spec]);
    return content;
  } catch (e) {
    return null;
  }
}

function gitListIncludeHeaders(repo, ref) {
  const listing = sh(['git', '-C', repo, 'ls-tree', '-r', '--name-only', ref, 'include/mujoco/']);
  return listing.split('\n').filter(Boolean);
}

function stripBlockCommentsKeepLines(s) {
  return s.replace(/\/\*[\s\S]*?\*\//g, m => m.replace(/[^\n]/g, ''));
}

function normalizeWhitespace(s) {
  return s.replace(/\r\n/g, '\n');
}

function collectPrototypes(content) {
  const src = normalizeWhitespace(stripBlockCommentsKeepLines(content));
  const lines = src.split('\n');
  const out = [];
  let buf = '';
  let startLine = -1;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (buf === '' && /\bMJAPI\b/.test(line)) {
      startLine = i + 1; // 1-based
    }
    if (startLine !== -1) {
      buf += (buf ? '\n' : '') + line;
      if (/[;]\s*$/.test(line)) {
        out.push({ text: buf, line: startLine });
        buf = '';
        startLine = -1;
      }
    }
  }
  return out;
}

function parsePrototype(proto) {
  // Combine whitespace, but keep enough for types.
  const one = proto.text.replace(/\s+/g, ' ').trim();
  // Example: MJAPI int mj_name2id(const mjModel* m, int type, const char* name);
  const m = one.match(/MJAPI\s+([^\(]+?)\s+(m[jujsp]\w+)\s*\(([^)]*)\)\s*;/);
  if (!m) return null;
  const ret = m[1].trim();
  const name = m[2].trim();
  const paramsRaw = m[3].trim();
  const params = paramsRaw === 'void' || paramsRaw === '' ? [] : paramsRaw.split(',').map(p => p.trim()).map(p => {
    // Split last token as name, rest as type. Handle function pointers loosely.
    const mm = p.match(/(.+?)\s+([\w\*\[\]]+)$/);
    if (!mm) return { raw: p };
    return { type: mm[1].trim(), name: mm[2].trim() };
  });
  return { name, return: ret, params, line: proto.line };
}

function loadFunctionGroupingMap() {
  const mapPath = pathResolve('scripts/mujoco_abi/functions_map.json');
  if (!existsSync(mapPath)) return null;
  try {
    const txt = readFileSync(mapPath, 'utf8');
    const j = JSON.parse(txt);
    return j;
  } catch {
    return null;
  }
}

function groupFromRules(name, rules) {
  if (!rules) return null;
  for (const r of rules) {
    const type = r.type || 'regex';
    if (type === 'exact' && r.match === name) return r.group;
    if (type === 'prefix' && name.startsWith(r.match)) return r.group;
    if (type === 'regex') {
      const re = new RegExp(r.match);
      if (re.test(name)) return r.group;
    }
  }
  return null;
}

function parseFunctions(hContent, filePath) {
  const protos = collectPrototypes(hContent);
  const out = [];
  const fmap = loadFunctionGroupingMap();
  const rules = fmap?.rules || null;
  for (const p of protos) {
    const f = parsePrototype(p);
    if (!f) continue;
    f.file = filePath;
    const n = f.name;
    // Prefer mapping rules; fallback heuristics kept as last resort
    let group = groupFromRules(n, rules);
    if (!group) {
      if (/^mj_(step|forward|inverse)/.test(n)) group = 'simulation';
      else if (/^mj_(load|save)/.test(n)) group = 'io';
      else if (/^mj_.*VFS/.test(n) || /VFS/.test(n)) group = 'vfs';
      else if (/^mj_(name2id|id2name)/.test(n)) group = 'names';
      else if (/^mj_ray/.test(n)) group = 'ray';
      else if (/^mju_user_(error|warning)/.test(n) || /^mju_error/.test(n) || /^mj_version/.test(n)) group = 'diagnostics';
      else if (/^mj_(make|delete|copy|reset|set|get|free|print|parse|factor|fullM|mul|is|clear|check|project)/.test(n)) group = 'support';
      else group = 'support';
    }
    f.group = group;
    out.push(f);
  }
  return out;
}

function parseStructBlock(content, structName) {
  const src = normalizeWhitespace(stripBlockCommentsKeepLines(content));
  // Try typedef-form first: `typedef struct X_ { ... } mjModel;`
  let re = new RegExp(`typedef\\s+struct\\s+\\w*\\s*{([\\s\\S]*?)^}\\s*${structName}\\s*;`, 'm');
  let m = src.match(re);
  let body, startLine;
  if (m) {
    body = m[1];
    startLine = src.slice(0, m.index).split('\n').length + 1;
  } else {
    // Fallback: separate tag/typedef: `struct mjModel_ { ... };` (and later `typedef struct mjModel_ mjModel;`)
    re = new RegExp(`struct\\s+${structName}_\\s*{([\\s\\S]*?)^};`, 'm');
    m = src.match(re);
    if (!m) return null;
    body = m[1];
    startLine = src.slice(0, m.index).split('\n').length + 1;
  }
  const fields = [];
  const lines = body.split('\n');
  let lineNo = startLine;
  for (const raw of lines) {
    const line = raw.trim();
    if (!line || line.startsWith('#')) { lineNo++; continue; }
    // Separate trailing comment first
    const [declWithSemi, comment] = line.split('//');
    const decl = declWithSemi.trim();
    // Ensure there is a semicolon in the declaration portion
    if (!/;\s*$/.test(decl)) { lineNo++; continue; }
    // Handle multiple declarators: type a, b[c]; -> split by , while preserving array brackets
    const mm = decl.match(/^(.+?)\s+(.+);$/);
    if (!mm) { lineNo++; continue; }
    const type = mm[1].trim();
    const rest = mm[2].trim();
    // Split by commas not inside brackets
    const parts = [];
    let cur = '';
    let depth = 0;
    for (const ch of rest) {
      if (ch === '[') depth++;
      if (ch === ']') depth--;
      if (ch === ',' && depth === 0) { parts.push(cur.trim()); cur = ''; continue; }
      cur += ch;
    }
    if (cur.trim()) parts.push(cur.trim());
    for (const p of parts) {
      // p like: name, name[DIM], *name, *name[DIM]
      const mname = p.match(/\**([A-Za-z_][A-Za-z0-9_]*)/);
      const name = mname ? mname[1] : p;
      const arr = (p.match(/\[[^\]]*\]/g) || []).map(s => s.slice(1, -1));
      fields.push({ name, type, array: arr.length ? arr : undefined, comment: comment?.trim(), line: lineNo });
    }
    lineNo++;
  }
  return { name: structName, fields };
}

function parseEnums(contentsByPath) {
  const enums = [];
  for (const [path, content] of contentsByPath) {
    const src = normalizeWhitespace(stripBlockCommentsKeepLines(content));
    const re = /typedef\s+enum\s+(\w+)\s*{([\s\S]*?)^}\s*(\w+)\s*;/mg;
    let m;
    while ((m = re.exec(src))) {
      const tagName = m[1];
      const typedefName = m[3];
      const body = m[2];
      const startLine = src.slice(0, m.index).split('\n').length + 1;
      const members = [];
      let offset = 0;
      for (const raw of body.split('\n')) {
        const line = raw.trim();
        if (!line || line.startsWith('#')) { offset++; continue; }
        const mm = line.match(/^([A-Za-z_][A-Za-z0-9_]*)(\s*=\s*[^,]+)?\s*,?/);
        if (mm) {
          members.push({ name: mm[1], value: mm[2]?.replace(/^\s*=\s*/, '').trim() });
        }
        offset++;
      }
      enums.push({ name: typedefName, tag: tagName, file: path, line: startLine, members });
    }
  }
  return enums;
}

function buildMappings(functions, enums, modelStruct) {
  const objEnum = enums.find(e => e.name === 'mjtObj') || enums.find(e => (e.name || '').toLowerCase().includes('obj'));
  const name2id = functions.find(f => f.name === 'mj_name2id');
  const id2name = functions.find(f => f.name === 'mj_id2name');
  const fields = modelStruct?.fields || [];
  const nameBuffer = fields.find(f => f.name === 'names');
  const countField = fields.find(f => f.name === 'nnames');
  // Heuristic: collect common topology id/adr fields for convenience
  const topoCandidates = fields.filter(f => /(_id|_adr)$/.test(f.name));
  return {
    name_functions: {
      name2id: name2id ? { file: name2id.file, line: name2id.line, params: name2id.params } : null,
      id2name: id2name ? { file: id2name.file, line: id2name.line, params: id2name.params } : null,
    },
    object_enum: objEnum ? { name: objEnum.name, file: objEnum.file, line: objEnum.line, members: objEnum.members } : null,
    name_buffer: nameBuffer ? { field: 'mjModel.names', file: 'include/mujoco/mjmodel.h', approx: true } : null,
    names_count: countField ? { field: 'mjModel.nnames' } : null,
    topology_fields: topoCandidates.map(f => ({ name: f.name, type: f.type, line: f.line })),
  };
}

function buildDiagnostics(functions, allHeaders) {
  const findFn = n => functions.find(f => f.name === n);
  const findAny = n => {
    const byFn = findFn(n);
    if (byFn) return { exists: true, file: byFn.file, line: byFn.line };
    // scan raw headers for symbol text
    for (const [path, content] of allHeaders) {
      const idx = content.indexOf(n);
      if (idx >= 0) {
        const pre = content.slice(0, idx);
        const line = pre.split('\n').length;
        return { exists: true, file: path, line };
      }
    }
    return { exists: false };
  };
  const diags = {};
  const errors = ['mju_user_error', 'mju_user_warning', 'mju_error'];
  diags.error_warning = errors.map(n => ({ name: n, ...findAny(n) }));
  const versions = ['mj_versionString', 'mj_versionNumber', 'mj_layoutHash'];
  diags.versioning = versions.map(n => ({ name: n, ...findAny(n) }));
  diags.requires = buildStageRequirements(functions, allHeaders);
  return diags;
}

function buildStageRequirements(functions, allHeaders) {
  const req = {};
  // Canonical forward/inverse/step ordering and subfunctions
  const order = {
    forward: [
      'forward.position',
      'forward.velocity',
      'forward.actuation',
      'forward.acceleration',
      'forward.constraint'
    ],
    inverse: [
      'inverse.position',
      'inverse.velocity',
      'inverse.constraint'
    ],
    step: [
      // Integrator does position/velocity integration then forward update as needed
      'integrate',
      'forward.position',
      'forward.velocity',
      'forward.actuation',
      'forward.acceleration',
      'forward.constraint'
    ]
  };
  const funcs = {
    fwd_position: 'mj_fwdPosition',
    fwd_velocity: 'mj_fwdVelocity',
    fwd_actuation: 'mj_fwdActuation',
    fwd_acceleration: 'mj_fwdAcceleration',
    fwd_constraint: 'mj_fwdConstraint',
    inv_position: 'mj_invPosition',
    inv_velocity: 'mj_invVelocity',
    inv_constraint: 'mj_invConstraint'
  };
  // Field prefix → stage requirement (heuristic, derived from docs and code structure)
  const fieldRules = [
    // Kinematics (global positions/orientations/frames)
    { match: /^.*\bx(pos|quat|mat)\b/, stage: 'forward.position' },
    { match: /^geom_(pos|quat|xpos|xmat|aabb)/, stage: 'forward.position' },
    { match: /^site_(xpos|xmat)/, stage: 'forward.position' },
    { match: /^cam_(xpos|xmat)/, stage: 'forward.position' },
    { match: /^light_(xpos|xdir)/, stage: 'forward.position' },
    // Velocity-related (com/centripetal bias, coriolis, dof jacobians)
    { match: /^(cvel|cdof|qfrc_bias)\b/, stage: 'forward.velocity' },
    { match: /^ten_velocity|tendon_velocity/, stage: 'forward.velocity' },
    // Actuation forces
    { match: /^qfrc_actuator|actuator_force/, stage: 'forward.actuation' },
    // Accelerations and total forces
    { match: /^qacc(_.*)?$/, stage: 'forward.acceleration' },
    // Constraints and contact solver arrays
    { match: /^efc_|^nefc$|^contact|^ten_J|^solver/, stage: 'forward.constraint' },
    // Sensors aggregate requires forward; assume position stage minimally
    { match: /^sensordata|^nSENSOR/i, stage: 'forward.position' }
  ];
  const fields = [];
  for (const r of fieldRules) {
    fields.push({ pattern: r.match.toString(), stage: r.stage });
  }
  req.stage_order = order;
  req.stage_functions = funcs;
  req.fields = fields;
  return req;
}

function writeJson(outDir, name, obj) {
  const p = pathJoin(outDir, name);
  ensureDir(dirname(p));
  writeFileSync(p, JSON.stringify(obj, null, 2), 'utf8');
}

function buildGate() {
  return {
    required: [
      'mj_loadXML', 'mj_deleteModel', 'mj_makeData', 'mj_deleteData',
      'mj_resetData', 'mj_step', 'mj_forward', 'mj_name2id', 'mj_id2name',
      'mj_defaultVFS', 'mj_addFileVFS', 'mj_deleteVFS', 'mj_versionString'
    ],
    optional: [
      'mj_layoutHash', 'mj_inverse', 'mj_ray*', 'mjspec_*', 'solver_stats', 'mj_*Sub*'
    ],
    excluded: [
      // Native visualization/UI/GL surface not exported to Web
      'mjv_*', 'mjr_*', 'mjui_*', 'mjthread_*'
    ]
  };
}

function matchPattern(name, pat) {
  // support glob-style '*' only, translate to regex
  if (pat.includes('*')) {
    const re = new RegExp('^' + pat.split('*').map(s => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).join('.*') + '$');
    return re.test(name);
  }
  return name === pat;
}

function tagGateForFunctions(functions, gate) {
  for (const f of functions) {
    const n = f.name;
    if (gate.excluded?.some(p => matchPattern(n, p))) f.gate = 'excluded';
    else if (gate.required?.some(p => matchPattern(n, p))) f.gate = 'required';
    else if (gate.optional?.some(p => matchPattern(n, p))) f.gate = 'optional';
    else f.gate = 'none';
  }
}

function loadExposureMap() {
  const mapPath = pathResolve('scripts/mujoco_abi/exposure_map.json');
  if (!existsSync(mapPath)) return null;
  try {
    return JSON.parse(readFileSync(mapPath, 'utf8'));
  } catch {
    return null;
  }
}

function tagExposure(functions, enums, exposureMap) {
  if (!exposureMap) return;
  const fMap = exposureMap.functions || {};
  const eMap = exposureMap.enums || {};
  // functions: allow glob keys
  for (const f of functions) {
    for (const key of Object.keys(fMap)) {
      if (matchPattern(f.name, key)) { f.exposure = fMap[key]; break; }
    }
  }
  // enums by exact typedef name
  for (const e of enums) {
    if (eMap[e.name]) e.exposure = eMap[e.name];
  }
}

function buildDimMap(structs) {
  const out = [];
  const counts = new Set();
  for (const s of [structs.mjModel, structs.mjData]) {
    if (!s?.fields) continue;
    for (const f of s.fields) {
      if (f.type === 'int' && /^n[a-z]/.test(f.name)) counts.add(f.name);
    }
  }
  const countsArr = Array.from(counts);
  function addDim(count, where) {
    out.push({ count, ...where });
  }
  const structsArr = [ ['mjModel', structs.mjModel], ['mjData', structs.mjData] ];
  for (const [sname, s] of structsArr) {
    if (!s?.fields) continue;
    for (const f of s.fields) {
      // bracket dims
      if (f.array) {
        for (const dim of f.array) {
          const token = String(dim).replace(/\s+/g, '');
          // only map simple identifiers that look like counts
          if (/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(token) && counts.has(token)) {
            addDim(token, { struct: sname, field: f.name, via: 'bracket', line: f.line });
          }
        }
      }
      // comment dims: try to find tokens like (nxx x ...)
      if (f.comment) {
        const m = f.comment.match(/\(([^)]+)\)/);
        if (m) {
          const expr = m[1];
          const parts = expr.split(/x|\*/).map(t => t.trim());
          for (const p of parts) {
            if (/^[a-zA-Z_][a-zA-Z0-9_]*$/.test(p) && counts.has(p)) {
              addDim(p, { struct: sname, field: f.name, via: 'comment', line: f.line });
            }
          }
        }
      }
    }
  }
  // group by count
  const grouped = {};
  for (const d of out) {
    if (!grouped[d.count]) grouped[d.count] = [];
    grouped[d.count].push({ struct: d.struct, field: d.field, via: d.via, line: d.line });
  }
  return grouped;
}

function buildExtensions(headers, enums, functions) {
  const pluginHeader = 'include/mujoco/mjplugin.h';
  const mjspecHeader = 'include/mujoco/mjspec.h';
  const pluginEnums = enums.filter(e => e.file === pluginHeader);
  const mjspecEnums = enums.filter(e => e.file === mjspecHeader);
  const mjspecFuncs = functions.filter(f => /^mjs_/.test(f.name));
  return {
    mjplugin: { header: pluginHeader, enums: pluginEnums, exposure: 'caution' },
    mjspec: { header: mjspecHeader, enums: mjspecEnums, functions: mjspecFuncs, exposure: 'caution' },
  };
}

function buildProbeSpec(gate, dimMap) {
  const funcs = gate.required || [];
  const arrays = [];
  for (const [count, refs] of Object.entries(dimMap)) {
    // prefer canonical counts commonly probed
    if (/^n(q|v|u|a|geom|body|site|jnt)/.test(count)) {
      for (const r of refs) {
        arrays.push({ count, struct: r.struct, field: r.field });
        if (arrays.length > 30) break; // cap size
      }
    }
    if (arrays.length > 30) break;
  }
  return {
    functions_present: funcs,
    arrays_size_consistency: arrays,
    name_mapping: { required: ['mj_name2id', 'mj_id2name'], buffers: ['names', 'nnames'] },
    vfs_required: ['mj_defaultVFS', 'mj_addFileVFS', 'mj_deleteVFS']
  };
}

async function main() {
  const args = parseArgs(process.argv);
  const repo = pathResolve(args.repo || 'local_tools/mujoco');
  const ref = args.ref || '3.3.7';
  const outDir = pathResolve(args.out || `dist/${ref}/abi`);
  ensureDir(outDir);

  const remote = gitRemote(repo);
  const headers = gitListIncludeHeaders(repo, ref);
  if (!headers.length) throw new Error(`No headers under include/mujoco for ref ${ref}`);

  // Read mujoco.h first for functions
  const mujocoH = 'include/mujoco/mujoco.h';
  const mujocoContent = gitReadFile(repo, ref, mujocoH);
  if (!mujocoContent) throw new Error(`Cannot read ${mujocoH} at ${ref}`);
  const functions = parseFunctions(mujocoContent, mujocoH);

  // Read structs from mjmodel.h, mjdata.h
  const mjmodelH = 'include/mujoco/mjmodel.h';
  const mjdataH = 'include/mujoco/mjdata.h';
  const mjmodelContent = gitReadFile(repo, ref, mjmodelH);
  const mjdataContent = gitReadFile(repo, ref, mjdataH);
  const modelStruct = mjmodelContent ? parseStructBlock(mjmodelContent, 'mjModel') : null;
  const dataStruct = mjdataContent ? parseStructBlock(mjdataContent, 'mjData') : null;

  // Enums across all headers
  const contentsByPath = headers.map(p => [p, gitReadFile(repo, ref, p)]).filter(([, c]) => !!c);
  const enums = parseEnums(contentsByPath);

  // Derived mappings and diagnostics
  const mappings = buildMappings(functions, enums, modelStruct);
  const diagnostics = buildDiagnostics(functions, contentsByPath);
  const gate = buildGate();
  tagGateForFunctions(functions, gate);
  const exposureMap = loadExposureMap();
  tagExposure(functions, enums, exposureMap);
  const structs = { mjModel: modelStruct, mjData: dataStruct };
  const dimMap = buildDimMap(structs);
  const extensions = buildExtensions(headers, enums, functions);
  const probeSpec = buildProbeSpec(gate, dimMap);

  const versionMeta = {
    ref,
    source_repo: remote,
    generated_at: new Date().toISOString(),
  };

  // Write outputs
  writeJson(outDir, 'functions.json', { meta: versionMeta, count: functions.length, functions });
  writeJson(outDir, 'structs.json', { meta: versionMeta, structs });
  writeJson(outDir, 'enums.json', { meta: versionMeta, enums });
  writeJson(outDir, 'mappings.json', { meta: versionMeta, mappings });
  writeJson(outDir, 'diagnostics.json', { meta: versionMeta, diagnostics });
  writeJson(outDir, 'gate.json', { meta: versionMeta, gate });
  writeJson(outDir, 'dim_map.json', { meta: versionMeta, dim_map: dimMap });
  writeJson(outDir, 'extensions.json', { meta: versionMeta, extensions });
  writeJson(outDir, 'probe_spec.json', { meta: versionMeta, probe: probeSpec });

  if (args.debug) {
    console.error(`Scanned ${headers.length} headers. Found functions=${functions.length}, enums=${enums.length}.`);
  }
}

main().catch(err => {
  console.error('[scan] Error:', err.message);
  process.exit(1);
});
