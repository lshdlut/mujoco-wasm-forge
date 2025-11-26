#!/usr/bin/env node

import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import path from 'node:path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const repoRoot = path.resolve(__dirname, '..');

const versionArg = process.argv.slice(2).find((arg) => !arg.startsWith('-'));
let forgeVersion = process.env.FORGE_VERSION || '';

if (versionArg) {
  forgeVersion = versionArg;
}

if (!forgeVersion) {
  console.error(
    'Usage: npm run build:forge -- <ver>\n' +
      'Example: npm run build:forge -- 3.3.7',
  );
  process.exit(1);
}

// Normalise version (e.g. "v3.3.7" -> "3.3.7").
forgeVersion = forgeVersion.replace(/^v/, '');

/** Known MuJoCo versions and their short IDs used by the wrappers. */
const VERSION_MAP = {
  '3.2.5': { short: '325' },
  '3.3.7': { short: '337' },
  '3.3.8-alpha': { short: '338' },
};

const entry = VERSION_MAP[forgeVersion];
if (!entry) {
  console.error(
    `Unknown MuJoCo version "${forgeVersion}". Known versions: ${Object.keys(
      VERSION_MAP,
    ).join(', ')}`,
  );
  process.exit(1);
}

const env = {
  ...process.env,
  // Select exactly one target in local_tools/wsl/build.sh.
  TARGETS: entry.short,
  MJVER: forgeVersion,
  // Ensure metadata and checks are generated for releaseable dist/.
  META: process.env.META ?? '1',
  RUN_TESTS: process.env.RUN_TESTS ?? '1',
};

const result = spawnSync('bash', ['local_tools/wsl/build.sh'], {
  cwd: repoRoot,
  env,
  stdio: 'inherit',
});

if (result.error) {
  console.error('[build:forge] Failed to spawn bash:', result.error);
  process.exit(1);
}

if (typeof result.status === 'number') {
  process.exit(result.status);
}

process.exit(1);

