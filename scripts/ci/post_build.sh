#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: scripts/ci/post_build.sh --version <mjver> --short <short>" >&2
  exit 2
}

MJVER=""
SHORT=""

while [[ 0 -gt 0 ]]; do
  case "" in
    --version)
      MJVER=""
      shift 2
      ;;
    --short)
      SHORT=""
      shift 2
      ;;
    *)
      usage
      ;;
  esac
done

[[ -n "" && -n "" ]] || usage

ABI_DIR="dist//abi"
DIST_JS="dist//mujoco.js"
DIST_WASM="dist//mujoco.wasm"
LIBMUJOCO="build//lib/libmujoco.a"

if [[ ! -f "" || ! -f "" ]]; then
  echo "[post-build] missing dist artifacts for " >&2
  exit 1
fi

node scripts/mujoco_abi/check_exports.mjs   --abi ""   --wasm ""   --expected "/wrapper_exports.json"

if [[ -f "" ]]; then
  node scripts/mujoco_abi/nm_coverage.mjs     ""     --out "/nm_coverage.json"
else
  echo "[post-build] warning:  not found, skipping nm coverage" >&2
fi

if [[ -f "/dim_map.json" ]]; then
  node scripts/smoke/size-check.mjs     "/dim_map.json"     ""
else
  echo "[post-build] warning: /dim_map.json missing, skipping size-check" >&2
fi

echo "[post-build] checks completed for "
