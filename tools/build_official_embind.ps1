param(
  [Parameter(Mandatory = $true)]
  [string]$Ref,

  [Parameter(Mandatory = $true)]
  [string]$OutDir,

  [switch]$Clean,

  [string]$MujocoRepo = "https://github.com/google-deepmind/mujoco.git",

  [string]$EmsdkRoot = $(if ($env:EMSDK_HOME) { $env:EMSDK_HOME } elseif (Test-Path "C:\\emsdk") { "C:\\emsdk" } else { "" }),

  [string]$EmsdkVersion = "4.0.10",

  [int]$Jobs = 0
)

$ErrorActionPreference = "Stop"

function Die([string]$Message) {
  throw $Message
}

function Invoke-Native {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Name,

    [Parameter(Mandatory = $true)]
    [scriptblock]$Action
  )

  & $Action
  if ($LASTEXITCODE -ne 0) {
    Die "$Name failed with exit code $LASTEXITCODE"
  }
}

function Resolve-NinjaExe {
  $cmd = Get-Command ninja -ErrorAction SilentlyContinue
  if ($cmd) { return $cmd.Source }

  $vsNinja = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\Ninja\\ninja.exe"
  if (Test-Path $vsNinja) { return $vsNinja }

  Die "ninja.exe not found. Install Ninja or Visual Studio CMake tools, or add Ninja to PATH."
}

function Ensure-Emsdk([string]$Root, [string]$Version) {
  if (-not $Root) { Die "EmsdkRoot is required (set EMSDK_HOME or pass -EmsdkRoot)." }
  if (-not (Test-Path (Join-Path $Root "emsdk.ps1"))) { Die "emsdk.ps1 not found under $Root" }
  if (-not (Test-Path (Join-Path $Root "emsdk_env.ps1"))) { Die "emsdk_env.ps1 not found under $Root" }

  # Activate env for this PowerShell process.
  . (Join-Path $Root "emsdk_env.ps1") | Out-Null

  $emcc = Get-Command emcc -ErrorAction SilentlyContinue
  if (-not $emcc) { Die "emcc not found after sourcing emsdk_env.ps1" }

  $verText = (cmd /c "\"$($emcc.Source)\" -v 2>&1") | Out-String
  if ($verText -notmatch [regex]::Escape($Version)) {
    & (Join-Path $Root "emsdk.ps1") install $Version | Out-Null
    & (Join-Path $Root "emsdk.ps1") activate $Version | Out-Null
    . (Join-Path $Root "emsdk_env.ps1") | Out-Null
  }
}

function Git-Checkout-Ref([string]$RepoDir, [string]$Ref) {
  Push-Location $RepoDir
  try {
    Invoke-Native -Name "git fetch" -Action { git fetch origin $Ref --depth 1 | Out-Null }
    Invoke-Native -Name "git checkout" -Action { git checkout --detach FETCH_HEAD | Out-Null }
    return (& git rev-parse HEAD).Trim()
  } finally {
    Pop-Location
  }
}

function Ensure-WorkspaceDir([string]$Path) {
  New-Item -ItemType Directory -Force -Path $Path | Out-Null
}

function Resolve-RepoRoot {
  $top = (& git rev-parse --show-toplevel 2>$null)
  if ($LASTEXITCODE -eq 0 -and $top) { return $top.Trim() }
  return (Resolve-Path ".").Path
}

$repoRoot = Resolve-RepoRoot
$parentDir = Split-Path $OutDir -Parent
$parentResolved = Resolve-Path -LiteralPath $parentDir -ErrorAction SilentlyContinue
$outParentAbs = if ($parentResolved) { $parentResolved.Path } else { $null }
if (-not $outParentAbs) {
  Ensure-WorkspaceDir (Split-Path $OutDir -Parent)
}
$outParentAbs = (Resolve-Path -LiteralPath (Split-Path $OutDir -Parent)).Path
$outAbs = Join-Path $outParentAbs (Split-Path $OutDir -Leaf)

$workRoot = Join-Path (Split-Path $outAbs -Parent) "_official_embind_workspace"
$mujocoDir = Join-Path $workRoot "mujoco"
$buildDir = Join-Path $workRoot "build"

if ($Clean) {
  Write-Host "[official-embind] Cleaning workspace: $workRoot"
  if (Test-Path $workRoot) { Remove-Item -Recurse -Force $workRoot }
}

Ensure-WorkspaceDir $workRoot

if (-not (Test-Path (Join-Path $mujocoDir ".git"))) {
  Write-Host "[official-embind] Cloning MuJoCo -> $mujocoDir"
  Invoke-Native -Name "git clone" -Action { git clone $MujocoRepo $mujocoDir | Out-Null }
}

Write-Host "[official-embind] Ensuring emsdk $EmsdkVersion"
Ensure-Emsdk -Root $EmsdkRoot -Version $EmsdkVersion

$emscriptenDir = Join-Path $EmsdkRoot "upstream\\emscripten"
$emcmakeBat = Join-Path $emscriptenDir "emcmake.bat"
if (-not (Test-Path $emcmakeBat)) { Die "emcmake.bat not found at $emcmakeBat" }
$nodeBin = Split-Path ($env:EMSDK_NODE) -Parent
$npmCmd = Join-Path $nodeBin "npm.cmd"
if (-not (Test-Path $npmCmd)) { Die "npm.cmd not found at $npmCmd (EMSDK_NODE=$env:EMSDK_NODE)" }

$commit = Git-Checkout-Ref -RepoDir $mujocoDir -Ref $Ref
Write-Host "[official-embind] MuJoCo ref=$Ref commit=$commit"

# Install JS deps (uses emsdk-provided npm).
$wasmDir = Join-Path $mujocoDir "wasm"
if (-not (Test-Path (Join-Path $wasmDir "package.json"))) { Die "Expected wasm/package.json missing: $wasmDir" }
Write-Host "[official-embind] npm install (wasm/)"
Push-Location $wasmDir
try {
  Invoke-Native -Name "npm install (wasm/)" -Action { & $npmCmd install }
} finally {
  Pop-Location
}

# Emscripten's --emit-tsd invokes `tsc` at link time. The build runs from the
# CMake binary dir, so ensure the wasm-local .bin is on PATH.
$wasmBin = Join-Path $wasmDir "node_modules\\.bin"
if (-not (Test-Path $wasmBin)) { Die "Expected wasm/node_modules/.bin missing after npm install: $wasmBin" }
$env:PATH = "$wasmBin;$env:PATH"

$ninjaExe = Resolve-NinjaExe
Ensure-WorkspaceDir $buildDir

Write-Host "[official-embind] Configure (Ninja) -> $buildDir"
Invoke-Native -Name "cmake configure" -Action { & $emcmakeBat cmake -S $mujocoDir -B $buildDir -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninjaExe" "-DCMAKE_BUILD_TYPE=Release" | Out-Null }

if (-not $Jobs -or $Jobs -le 0) {
  $Jobs = [Environment]::ProcessorCount
  if (-not $Jobs -or $Jobs -le 0) { $Jobs = 4 }
}
Write-Host "[official-embind] Build mujoco_wasm (jobs=$Jobs)"
Invoke-Native -Name "cmake build mujoco_wasm" -Action { cmake --build $buildDir --target mujoco_wasm --parallel $Jobs | Out-Null }

$distDir = Join-Path $mujocoDir "wasm\\dist"
if (-not (Test-Path $distDir)) { Die "Expected dist dir missing: $distDir" }
if (-not (Test-Path (Join-Path $distDir "mujoco_wasm.js"))) { Die "Expected dist entrypoint missing: $distDir\\mujoco_wasm.js" }

Write-Host "[official-embind] Copy dist -> $outAbs"
if (Test-Path $outAbs) { Remove-Item -Recurse -Force $outAbs }
Ensure-WorkspaceDir $outAbs
Copy-Item -Recurse -Force (Join-Path $distDir "*") $outAbs

$meta = @{
  kind = "official-embind"
  ref = $Ref
  commit = $commit
  repo = $MujocoRepo
  emsdkVersion = $EmsdkVersion
  generatedAt = (Get-Date).ToUniversalTime().ToString("o")
  outDir = $outAbs
}
$metaPath = Join-Path $outAbs "build_meta.json"
$meta | ConvertTo-Json -Depth 4 | Out-File -FilePath $metaPath -Encoding utf8

Write-Host "[official-embind] Wrote: $metaPath"
Write-Host "[official-embind] Done: $outAbs"
