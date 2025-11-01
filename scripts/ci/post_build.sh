#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: scripts/ci/post_build.sh --version <mjver> --short <short>" >&2
  exit 2
}

MJVER=""
SHORT=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --version)
      [[ $# -ge 2 ]] || usage
      MJVER="$2"
      shift 2
      ;;
    --short)
      [[ $# -ge 2 ]] || usage
      SHORT="$2"
      shift 2
      ;;
    *)
      usage
      ;;
  esac
done

[[ -n "$MJVER" && -n "$SHORT" ]] || usage

ABI_DIR="dist/${MJVER}/abi"
DIST_JS="dist/${MJVER}/mujoco.js"
DIST_WASM="dist/${MJVER}/mujoco.wasm"
LIBMUJOCO="build/${SHORT}/lib/libmujoco.a"

if [[ ! -f "$DIST_JS" || ! -f "$DIST_WASM" ]]; then
  echo "[post-build] missing dist artifacts for ${MJVER}" >&2
  exit 1
fi

node scripts/mujoco_abi/check_exports.mjs \
  --abi "${ABI_DIR}" \
  --wasm "${DIST_WASM}" \
  --expected "${ABI_DIR}/wrapper_exports.json"

if [[ -f "$LIBMUJOCO" ]]; then
  node scripts/mujoco_abi/nm_coverage.mjs \
    "${LIBMUJOCO}" \
    --out "${ABI_DIR}/nm_coverage.json"
else
  echo "[post-build] warning: ${LIBMUJOCO} not found, skipping nm coverage" >&2
fi

if [[ -f "${ABI_DIR}/dim_map.json" ]]; then
  node scripts/smoke/size-check.mjs \
    "${ABI_DIR}/dim_map.json" \
    "${DIST_JS}"
else
  echo "[post-build] warning: ${ABI_DIR}/dim_map.json missing, skipping size-check" >&2
fi

echo "[post-build] checks completed for ${MJVER}"