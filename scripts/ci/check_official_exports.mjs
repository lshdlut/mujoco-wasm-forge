#!/usr/bin/env node

import assert from "node:assert/strict";
import fs from "node:fs/promises";
import path from "node:path";
import { pathToFileURL } from "node:url";

function usage() {
  console.log("Usage: node scripts/ci/check_official_exports.mjs --dist <dist/3.3.7>");
  process.exit(2);
}

function parseArgs(argv) {
  const out = {};
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (arg === "--dist") {
      out.dist = argv[++i];
    } else if (arg === "--expect") {
      out.expect = argv[++i].split(",");
    } else if (arg === "--help" || arg === "-h") {
      usage();
    } else {
      console.error(`Unknown argument: ${arg}`);
      usage();
    }
  }
  return out;
}

async function main() {
  const args = parseArgs(process.argv.slice(2));
  const distRoot = path.resolve(args.dist || path.join("dist", "3.3.7"));
  const jsPath = path.join(distRoot, "mujoco.js");
  const wasmPath = path.join(distRoot, "mujoco.wasm");

  try {
    await fs.access(jsPath);
    await fs.access(wasmPath);
  } catch (err) {
    throw new Error(`dist assets missing; expected ${jsPath} and ${wasmPath}`);
  }

  const wasmBinary = await fs.readFile(wasmPath);
  const loadModule = (await import(pathToFileURL(jsPath).href)).default;
  const moduleInstance = await loadModule({
    wasmBinary,
    printErr: () => {},
    print: () => {},
  });

  const moduleKeys = new Set(Object.keys(moduleInstance));
  const expected = args.expect || [
    "_mjwf_geom_xpos_ptr",
    "_mjwf_geom_xmat_ptr",
    "_mjwf_geom_size_ptr",
    "_mjwf_geom_type_ptr",
    "_mjwf_geom_matid_ptr",
    "_mjwf_geom_bodyid_ptr",
    "_mjwf_geom_dataid_ptr",
    "_mjwf_nmesh",
    "_mjwf_mesh_vertadr_ptr",
    "_mjwf_mesh_vertnum_ptr",
    "_mjwf_mesh_faceadr_ptr",
    "_mjwf_mesh_facenum_ptr",
    "_mjwf_mesh_texcoordadr_ptr",
    "_mjwf_mesh_texcoordnum_ptr",
    "_mjwf_mesh_vert_ptr",
    "_mjwf_mesh_vert_count",
    "_mjwf_mesh_face_ptr",
    "_mjwf_mesh_face_count",
    "_mjwf_mesh_texcoord_ptr",
    "_mjwf_mesh_texcoord_count",
  ];

  const missing = expected.filter((name) => !moduleKeys.has(name));
  assert.strictEqual(
    missing.length,
    0,
    `Missing required exports from official wrapper: ${missing.join(", ")}`,
  );

  console.log(
    `official exports OK (${expected.length} checked) from ${path.relative(
      process.cwd(),
      distRoot,
    )}`,
  );
}

main().catch((err) => {
  console.error(`[check_official_exports] ${err.message}`);
  process.exit(1);
});
