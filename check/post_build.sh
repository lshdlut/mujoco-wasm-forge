#!/usr/bin/env bash

set -euo pipefail

usage() {
  echo "Usage: scripts/ci/post_build.sh --version <mjver> --short <short> [--variant <name>]" >&2
  exit 2
}

MJVER=""
SHORT=""
VARIANT=""

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
    --variant)
      [[ $# -ge 2 ]] || usage
      VARIANT="$2"
      shift 2
      ;;
    *)
      usage
      ;;
  esac
done

[[ -n "$MJVER" && -n "$SHORT" ]] || usage

ABI_DIR="dist/${MJVER}/abi"
DIST_ROOT="dist/${MJVER}"
if [[ -n "${VARIANT}" ]]; then
  DIST_ROOT="dist/${MJVER}/${VARIANT}"
fi
DIST_JS="${DIST_ROOT}/mujoco.js"
DIST_WASM="${DIST_ROOT}/mujoco.wasm"
LIBMUJOCO="build/${SHORT}/lib/libmujoco.a"
NODE_BIN="${NODE:-node}"

if [[ ! -f "$DIST_JS" || ! -f "$DIST_WASM" ]]; then
  echo "[post-build] missing dist artifacts for ${MJVER}" >&2
  exit 1
fi

EXPECTED_JSON="${ABI_DIR}/wrapper_exports.json"
if [[ ! -f "${EXPECTED_JSON}" ]]; then
  EXPECTED_JSON="${ABI_DIR}/wrapper_exports_funcs.json"
fi

EXPORTS_CHECK_OUT="${ABI_DIR}/exports_check.json"
if [[ -n "${VARIANT}" ]]; then
  EXPORTS_CHECK_OUT="${ABI_DIR}/exports_check.${VARIANT}.json"
fi

"${NODE_BIN}" check/check_exports.mjs \
  --abi "${ABI_DIR}" \
  --wasm "${DIST_WASM}" \
  --expected "${EXPECTED_JSON}" \
  --out "${EXPORTS_CHECK_OUT}"

if [[ -f "$LIBMUJOCO" ]]; then
  "${NODE_BIN}" abi_impl/nm_coverage.mjs \
    "${LIBMUJOCO}" \
    --out "${ABI_DIR}/nm_coverage.json"
else
  echo "[post-build] warning: ${LIBMUJOCO} not found, skipping nm coverage" >&2
fi

echo "[post-build] checks completed for ${MJVER}"
