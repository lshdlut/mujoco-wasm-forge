param(
  [string]$EmsdkRoot = "C:\emsdk",
  [string]$Python = "C:\Users\63427\miniforge3\envs\myconda\python.exe",
  [string[]]$Versions = @('3.2.5','3.3.7'),
  [switch]$RunRegression
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Info($m){ Write-Host "[INFO] $m" -ForegroundColor Cyan }
function Warn($m){ Write-Host "[WARN] $m" -ForegroundColor Yellow }
function Err ($m){ Write-Host "[ERR ] $m" -ForegroundColor Red }

function Ensure-Emsdk30155 {
  param([string]$Root)
  if (-not (Test-Path $Root)) { throw "emsdk root not found: $Root" }
  $env:PATH = (Split-Path $Python) + ';' + $env:PATH
  $emsdkBat = Join-Path $Root 'emsdk.bat'
  $emsdkEnv = Join-Path $Root 'emsdk_env.ps1'
  if (-not (Test-Path $emsdkBat)) { throw "emsdk.bat not found under $Root" }
  # Try to detect current active version
  try { & $emsdkBat list | Out-Null } catch { Warn "emsdk list failed, continuing" }
  try {
    & $emsdkBat install 3.1.55 | Out-Null
  } catch {
    Warn "emsdk install 3.1.55 failed (might already exist): $_"
  }
  try { & $emsdkBat activate 3.1.55 | Out-Null } catch { Warn "emsdk activate warning: $_" }
  . $emsdkEnv | Out-Null
  $ver = (& emcc --version) 2>$null | Select-Object -First 1
  Info "emcc version: $ver"
  if ($ver -notmatch '3\.1\.55') { Warn "Active Emscripten not 3.1.55; builds may diverge from CI." }
}

function Ensure-Ninja {
  if (Get-Command ninja -ErrorAction SilentlyContinue) { return }
  $tmp = Join-Path $env:TEMP ("ninja_" + [guid]::NewGuid().ToString('n'))
  New-Item -ItemType Directory -Force -Path $tmp | Out-Null
  $zip = Join-Path $tmp 'ninja-win.zip'
  Info "Downloading ninja to $tmp"
  Invoke-WebRequest -Uri 'https://github.com/ninja-build/ninja/releases/latest/download/ninja-win.zip' -OutFile $zip | Out-Null
  Expand-Archive -Path $zip -DestinationPath $tmp -Force
  $env:PATH = $tmp + ';' + $env:PATH
  Info "ninja $(ninja --version)"
}

function Clone-MuJoCo([string]$ver){
  if (Test-Path external/mujoco) { Remove-Item -Recurse -Force external/mujoco }
  New-Item -ItemType Directory -Force -Path external | Out-Null
  Info "Cloning MuJoCo $ver"
  try { & git clone --depth 1 --branch $ver https://github.com/google-deepmind/mujoco external/mujoco | Out-Null; return }
  catch { Warn "branch $ver failed, fallback to v$ver" }
  & git clone --depth 1 --branch "v$ver" https://github.com/google-deepmind/mujoco external/mujoco | Out-Null
}

function Patch-Qhull-Static([string]$short){
  $qh = "build/$short/_deps/qhull-src/CMakeLists.txt"
  if (-not (Test-Path $qh)) { Warn "qhull CMakeLists not found: $qh"; return }
  (Get-Content $qh) -replace '\bSHARED\b','STATIC' | Set-Content $qh
  $inject = 'set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)'
  $orig = Get-Content $qh -Raw
  if ($orig -notmatch 'BUILD_SHARED_LIBS') { ("$inject`n" + $orig) | Set-Content $qh }
  Info "Patched qhull for static linkage"
}

function Configure-And-Build([string]$app,[string]$short){
  # Stage-1 configure (337 only) to fetch deps; allow failure
  if ($short -eq '337') {
    try {
      Info "Stage-1 configure (fetch deps)"
      & emcmake cmake -G Ninja -S $app -B "build/$short" `
        -DCMAKE_BUILD_TYPE=Release `
        -DMUJOCO_ENABLE_QHULL=OFF `
        -DMUJOCO_BUILD_PLUGINS=OFF `
        -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF `
        -DCMAKE_SKIP_INSTALL_RULES=ON `
        -DLIBM_LIBRARY:STRING=-lm `
        -DPython3_EXECUTABLE=$Python | Out-Null
    } catch { Warn "Stage-1 configure failed (expected ok), continue to patch qhull" }
    Patch-Qhull-Static -short $short
  }
  Info "Final configure"
  & emcmake cmake -G Ninja -S $app -B "build/$short" `
    -DCMAKE_BUILD_TYPE=Release `
    -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF `
    -DCMAKE_SKIP_INSTALL_RULES=ON `
    -DLIBM_LIBRARY:STRING=-lm `
    -DPython3_EXECUTABLE=$Python | Out-Null

  Info "Build (WASM)"
  & cmake --build "build/$short" -j 2 | Out-Null
}

function Build-Native([string]$app,[string]$short){
  try {
    Info "Configure (Native)"
    & cmake -S $app -B "build/${short}_native" `
      -DCMAKE_BUILD_TYPE=Release `
      -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF `
      -DCMAKE_SKIP_INSTALL_RULES=ON | Out-Null
    Info "Build (Native)"
    & cmake --build "build/${short}_native" -j 2 | Out-Null
    return $true
  } catch {
    Warn "Native build failed or toolchain not available; regression test will be skipped. $_"
    return $false
  }
}

function Collect-Artifacts([string]$mjver,[string]$short){
  New-Item -ItemType Directory -Force -Path dist | Out-Null
  Copy-Item "build/$short/_wasm/mujoco_wasm$short.js" "dist/mujoco-$mjver.js" -Force
  Copy-Item "build/$short/_wasm/mujoco_wasm$short.wasm" "dist/mujoco-$mjver.wasm" -Force
  if (Test-Path "build/$short/_wasm/mujoco_wasm$short.wasm.map") {
    Copy-Item "build/$short/_wasm/mujoco_wasm$short.wasm.map" "dist/mujoco-$mjver.wasm.map" -Force
  }
}

function Gen-VersionJson([string]$mjver){
  $ems = $env:EMSDK_VERSION; if (-not $ems) { $ems = '3.1.55' }
  $js = "dist/mujoco-$mjver.js"; $wasm = "dist/mujoco-$mjver.wasm"
  $jsB = (Get-Item $js).Length; $wsB = (Get-Item $wasm).Length
  $jsSha = (Get-FileHash $js -Algorithm SHA256).Hash.ToLower()
  $wsSha = (Get-FileHash $wasm -Algorithm SHA256).Hash.ToLower()
  $now = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
  $mjSha = (git -C external/mujoco rev-parse HEAD)
  $obj = [ordered]@{
    mujocoVersion = $mjver
    emscripten = $ems
    buildTime = $now
    gitSha = $mjSha
    emsdk_root = $env:EMSDK
    emsdk_node = $env:EMSDK_NODE
    emsdk_python = $env:EMSDK_PYTHON
    flags = @('WASM=1','MODULARIZE=1','EXPORT_ES6=1','ALLOW_MEMORY_GROWTH=1')
    features = @{ qhull='static'; libccd='static'; plugins=$false; render=$false; exceptions='off'; pthreads=$false; filesystem=$true; env='node,web' }
    size = @{ wasmBytes = $wsB; jsBytes = $jsB }
    hash = @{ wasmSha256 = $wsSha; jsSha256 = $jsSha }
  }
  ($obj | ConvertTo-Json -Depth 6) | Set-Content -Encoding UTF8 "dist/version.json"
}

function Gen-SBOM([string]$mjver){
  $now = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
  $runId = $env:GITHUB_RUN_ID; if (-not $runId) { $runId = "local-" + (Get-Date -Format 'yyyyMMddHHmmss') }
  $ns = "https://github.com/lshdlut/mujoco-wasm-forge/sbom/$runId"
  $ems = $env:EMSDK_VERSION; if (-not $ems) { $ems = '3.1.55' }
  $mjSha = (git -C external/mujoco rev-parse HEAD)
  $sbom = [ordered]@{
    spdxVersion='SPDX-2.3'; dataLicense='CC0-1.0'; SPDXID='SPDXRef-DOCUMENT';
    name = "mujoco-wasm-forge-$mjver"; documentNamespace=$ns; creationInfo=@{created=$now; creators=@('Tool: ci_replay')};
    packages = @(
      @{ name='mujoco'; SPDXID='SPDXRef-Pkg-MuJoCo'; versionInfo=$mjver; downloadLocation='https://github.com/google-deepmind/mujoco'; sourceInfo="git@$mjSha" },
      @{ name='emscripten'; SPDXID='SPDXRef-Pkg-Emscripten'; versionInfo=$ems; downloadLocation='https://github.com/emscripten-core/emsdk' }
    )
  }
  ($sbom | ConvertTo-Json -Depth 8) | Set-Content -Encoding UTF8 'dist/sbom.spdx.json'
}

function Gen-Checksums([string]$mjver){
  $js = "dist/mujoco-$mjver.js"; $wasm = "dist/mujoco-$mjver.wasm"
  $jsSha = (Get-FileHash $js -Algorithm SHA256).Hash.ToLower()
  $wsSha = (Get-FileHash $wasm -Algorithm SHA256).Hash.ToLower()
  @(
    "$jsSha  $(Split-Path $js -Leaf)"
    "$wsSha  $(Split-Path $wasm -Leaf)"
  ) | Set-Content -Encoding ASCII 'dist/SHA256SUMS.txt'
}

function Run-Tests([string]$short,[bool]$nativeOk){
  Info "[GATE:RUN] Smoke"
  & node "tests/smoke-$short.mjs"
  if ($nativeOk -and $RunRegression) {
    $env:MJ_NATIVE_BIN = (Resolve-Path "build/${short}_native/_wasm/mujoco_compare$short").Path
    Info "[GATE:RUN] Regression"
    & node "tests/regression-$short.mjs"
  } else {
    Warn "[GATE:RUN] Regression skipped (native not built or switch off)"
  }
  if ($short -eq '337' -and (Test-Path "tests/mesh-smoke-337.mjs")) {
    Info "[GATE:RUN] Mesh smoke"
    & node tests/mesh-smoke-337.mjs
  }
}

# Entry
Ensure-Emsdk30155 -Root $EmsdkRoot
Ensure-Ninja

foreach ($ver in $Versions) {
  $short = ($ver -replace '\.','')
  $app = if ($ver -eq '3.3.7') { 'wrappers/official_app_337' } else { 'wrappers/official_app_325' }
  Clone-MuJoCo -ver $ver
  Configure-And-Build -app $app -short $short
  $nativeOk = Build-Native -app $app -short $short
  Collect-Artifacts -mjver $ver -short $short
  Gen-VersionJson -mjver $ver
  Gen-SBOM -mjver $ver
  Gen-Checksums -mjver $ver
  Run-Tests -short $short -nativeOk:$nativeOk
}

Info "Done. Artifacts under dist/, version.json + sbom + checksums generated."

