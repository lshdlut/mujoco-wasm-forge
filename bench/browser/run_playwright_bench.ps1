$ErrorActionPreference = "Stop"

function Resolve-NodeExe {
  if ($env:MJWF_NODE_EXE -and (Test-Path $env:MJWF_NODE_EXE)) {
    return $env:MJWF_NODE_EXE
  }
  $cmd = Get-Command node -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }
  throw "node.exe not found. Set MJWF_NODE_EXE or add Node to PATH."
}

$forgeRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\\..")).Path
$playRoot = (Resolve-Path (Join-Path $forgeRoot "..\\mujoco-wasm-play")).Path

if (!(Test-Path (Join-Path $playRoot "tests\\playwright.config.mjs"))) {
  throw "mujoco-wasm-play not found next to mujoco-wasm-forge. Expected: $playRoot"
}

$outDir = Join-Path $forgeRoot "bench\\results\\raw_browser"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$nodeExe = Resolve-NodeExe
$pwCli = Join-Path $playRoot "node_modules\\@playwright\\test\\cli.js"
if (!(Test-Path $pwCli)) {
  throw "Playwright CLI not found at $pwCli. Run `npm ci` or `npm install` in mujoco-wasm-play first."
}

Push-Location $playRoot
try {
  $env:MJWF_BENCH_OUT_DIR = $outDir
  $env:MJWF_PLAY_BENCH = "1"
  if (-not $env:MJWF_BENCH_RUNS) { $env:MJWF_BENCH_RUNS = "3" }
  & $nodeExe $pwCli test --config tests/playwright.config.mjs tests/e2e/bench-forge-variants.local.spec.ts
} finally {
  Pop-Location
}
