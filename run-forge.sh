#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<EOF
Usage: $(basename "$0") --version <mjver> [--short <short>] [--with-checks]

Options:
  --version <mjver>    MuJoCo version string (e.g., 3.3.7 or git ref) used for dist/<ver>.
  --short <short>      Build short name passed to post_build (default: 337).
  --with-checks        Run smoke/mesh/gates scripts after post-build.
EOF
  exit 2
}

MJVER=""
SHORT=""
WITH_CHECKS=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --version) [[ $# -ge 2 ]] || usage; MJVER="$2"; shift 2 ;;
    --short) [[ $# -ge 2 ]] || usage; SHORT="$2"; shift 2 ;;
    --with-checks) WITH_CHECKS=1; shift ;;
    *) usage ;;
  esac
done

[[ -n "$MJVER" ]] || usage

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
DIST_DIR="$REPO_ROOT/dist/$MJVER"
ABI_DIR="$DIST_DIR/abi"
BUILD_DIR="$REPO_ROOT/build/forge"

if [[ -z "$SHORT" ]]; then
  digits="$(echo "$MJVER" | tr -dc '0-9')"
  SHORT="${digits:-337}"
fi

export MJVER
export DIST_VERSION="$MJVER"

prepare_mujoco() {
  if [[ -d "$REPO_ROOT/external/mujoco/.git" ]]; then
    git -C "$REPO_ROOT/external/mujoco" fetch --depth 1 origin "$MJVER" >/dev/null 2>&1 || true
  else
    rm -rf "$REPO_ROOT/external/mujoco"
    git clone --depth 1 https://github.com/google-deepmind/mujoco "$REPO_ROOT/external/mujoco"
  fi
  git -C "$REPO_ROOT/external/mujoco" checkout --detach "$MJVER"
}

run_introspect() {
  mkdir -p "$ABI_DIR"
  python3 "$REPO_ROOT/introspect/forge/scan_clang_introspect.py" \
    --header "$REPO_ROOT/external/mujoco/include/mujoco/mujoco.h" \
    --out-dir "$ABI_DIR"
}

run_abi_generators() {
  export PYTHONPATH="$REPO_ROOT:$PYTHONPATH"
  python3 "$REPO_ROOT/abi_exports/gen_structs.py" "$REPO_ROOT/app/mjwf_abi_structs.h" "$REPO_ROOT/app/mjwf_abi_structs.c"
  python3 -m abi_exports.gen_funcs --version "$MJVER"
}

configure_and_build() {
  mkdir -p "$BUILD_DIR"
  export MJWF_EXPORTS_LIST="$ABI_DIR/exports.lst"
  source ~/emsdk/emsdk_env.sh
  emcmake cmake \
    -S "$REPO_ROOT/app" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DMUJOCO_ENABLE_QHULL=OFF \
    -DMUJOCO_BUILD_PLUGINS=OFF \
    -DMUJOCO_BUILD_EXAMPLES=OFF \
    -DMUJOCO_BUILD_SIMULATE=OFF \
    -DMUJOCO_BUILD_TESTS=OFF \
    -DMUJOCO_BUILD_SAMPLES=OFF \
    -DCMAKE_SKIP_INSTALL_RULES=ON \
    -DLIBM_LIBRARY:STRING=-lm \
    -DMJWF_PROFILE=default \
    -DMJVER="$MJVER"
  cmake --build "$BUILD_DIR" -- -j "$(nproc)"
  mkdir -p "$DIST_DIR"
  cp "$BUILD_DIR/_wasm/mujoco_wasm.js" "$DIST_DIR/mujoco.js"
  cp "$BUILD_DIR/_wasm/mujoco_wasm.wasm" "$DIST_DIR/mujoco.wasm"
  cp "$BUILD_DIR/_wasm/mujoco_wasm.wasm.map" "$DIST_DIR/mujoco.wasm.map" 2>/dev/null || true
}

run_post_build() {
  ./check/post_build.sh --version "$MJVER" --short "$SHORT"
}

run_checks() {
  source ~/emsdk/emsdk_env.sh
  node check/tests/smoke.mjs
  node check/tests/mesh-smoke.mjs
  node check/tests/gates.mjs
}

echo "[run-forge] using version $MJVER (short=$SHORT)"
prepare_mujoco
run_introspect
run_abi_generators
configure_and_build
run_post_build
if [[ "$WITH_CHECKS" == "1" ]]; then
  run_checks
fi

echo "[run-forge] finished dist/$MJVER"
