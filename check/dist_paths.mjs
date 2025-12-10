import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const repoRoot = path.resolve(__dirname, '..');
const distRoot = path.join(repoRoot, 'dist');

let cachedVersion = null;

function listDistVersions() {
  if (!fs.existsSync(distRoot)) {
    return [];
  }
  return fs
    .readdirSync(distRoot, { withFileTypes: true })
    .filter((entry) => entry.isDirectory())
    .map((entry) => entry.name)
    .sort((a, b) => a.localeCompare(b, undefined, { numeric: true }));
}

function resolveDistVersion() {
  if (cachedVersion) return cachedVersion;
  const envVersion = process.env.MJVER ?? process.env.DIST_VERSION ?? '';
  if (envVersion) {
    cachedVersion = envVersion;
    return cachedVersion;
  }

  const versions = listDistVersions();
  if (!versions.length) {
    throw new Error('No dist/<ver> directory was found; run `npm run build:forge -- <ver>` first.');
  }
  cachedVersion = versions[0];
  return cachedVersion;
}

function distDir(version) {
  const ver = version ?? resolveDistVersion();
  const dir = path.join(distRoot, ver);
  if (!fs.existsSync(dir) || !fs.statSync(dir).isDirectory()) {
    throw new Error(`Dist directory not found for version "${ver}" (expected at ${dir}).`);
  }
  return dir;
}

function resolveDistPath(...segments) {
  return path.join(distDir(), ...segments);
}

export { distRoot, resolveDistVersion as distVersion, distDir, resolveDistPath };
