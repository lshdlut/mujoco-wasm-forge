#!/usr/bin/env node
import { readFileSync } from "node:fs";
import assert from "node:assert/strict";

const [,, dimMapPath] = process.argv;
if (!dimMapPath) {
  console.error("Usage: node scripts/smoke/size-check.mjs dist/<ver>/abi/dim_map.json");
  process.exit(2);
}

const dimData = JSON.parse(readFileSync(dimMapPath, "utf8"));
const dimMap = dimData.dim_map || {};
const required = ["nq", "nv", "nu", "nbody", "ngeom", "nsite", "njnt"];
const missing = required.filter((key) => !(Array.isArray(dimMap[key]) && dimMap[key].length));

assert.ok(missing.length === 0, `dim_map missing entries for: ${missing.join(', ')}`);

console.log("size-check OK");
