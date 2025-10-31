Param(
  [string]$Repo = "local_tools/mujoco",
  [string]$Ref = "3.3.7",
  [string]$OutDir = "dist/3.3.7/abi",
  [switch]$Debug
)

$ErrorActionPreference = 'Stop'

if (!(Test-Path $Repo)) {
  throw "Repo path not found: $Repo"
}

$node = Join-Path "local_tools" "bin/node.exe"
if (!(Test-Path $node)) {
  $node = "node"
}

$script = Join-Path "scripts" "mujoco_abi/scan.mjs"

$args = @("$script", "--repo", "$Repo", "--ref", "$Ref", "--out", "$OutDir")
if ($Debug) { $args += "--debug" }

Write-Host "[abi-scan] Repo=$Repo Ref=$Ref Out=$OutDir"
& $node @args
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "[abi-scan] Done. Outputs under $OutDir"

