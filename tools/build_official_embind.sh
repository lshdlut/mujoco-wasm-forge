#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat >&2 <<'EOF'
Usage:
  bash tools/build_official_embind.sh --ref <ref> --out <dir> [--clean]

Build Google DeepMind's official MuJoCo Embind WebAssembly bundle and copy
wasm/dist/ artifacts into the requested output directory.

Required:
  --ref <ref>     MuJoCo git ref to build (e.g. 3.5.0 tag, commit SHA).
  --out <dir>     Destination directory for the produced wasm/dist bundle.

Optional:
  --clean         Wipe the workspace before cloning/building.

Environment:
  EMSDK_ROOT      Path to emsdk (recommended: /mnt/c/emsdk). Required.
  EMSDK_VERSION   emsdk version (default: 4.0.10).
  WORK_ROOT       Workspace root (default: <out-parent>/_official_embind_workspace).
  MUJOCO_REPO     MuJoCo git repo URL (default: https://github.com/google-deepmind/mujoco.git).
  PAR_JOBS        Parallel build jobs (default: nproc or 4).
EOF
  exit 2
}

log() { printf '\033[1;34m[official-embind]\033[0m %s\n' "$*"; }
err() { printf '\033[1;31m[official-embind][err]\033[0m %s\n' "$*"; }
die() { err "$1"; exit "${2:-1}"; }

need_cmd() {
  command -v "$1" >/dev/null 2>&1 || die "Missing required command: $1"
}

REF=""
OUT=""
CLEAN="0"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --ref)
      [[ $# -ge 2 ]] || usage
      REF="$2"
      shift 2
      ;;
    --out)
      [[ $# -ge 2 ]] || usage
      OUT="$2"
      shift 2
      ;;
    --clean)
      CLEAN="1"
      shift 1
      ;;
    *)
      usage
      ;;
  esac
done

[[ -n "$REF" && -n "$OUT" ]] || usage

need_cmd git
need_cmd cmake
need_cmd npm

OUT_ABS="$(python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "$OUT" 2>/dev/null || true)"
if [[ -z "$OUT_ABS" ]]; then
  OUT_ABS="$(cd "$(dirname "$OUT")" && pwd)/$(basename "$OUT")"
fi

OUT_PARENT="$(dirname "$OUT_ABS")"
WORK_ROOT="${WORK_ROOT:-$OUT_PARENT/_official_embind_workspace}"
MUJOCO_DIR="$WORK_ROOT/mujoco"
BUILD_DIR="$WORK_ROOT/build"

EMSDK_VERSION="${EMSDK_VERSION:-4.0.10}"
MUJOCO_REPO="${MUJOCO_REPO:-https://github.com/google-deepmind/mujoco.git}"

EMSDK_ROOT_DIR="${EMSDK_ROOT:-}"
[[ -n "$EMSDK_ROOT_DIR" ]] || die "EMSDK_ROOT is required (recommended: /mnt/c/emsdk)"
EMSDK_ENV="$EMSDK_ROOT_DIR/emsdk_env.sh"
[[ -f "$EMSDK_ENV" ]] || die "emsdk_env.sh not found at: $EMSDK_ENV"

if [[ "$CLEAN" == "1" ]]; then
  log "Cleaning workspace: $WORK_ROOT"
  rm -rf "$WORK_ROOT"
fi

mkdir -p "$WORK_ROOT"

if [[ ! -d "$MUJOCO_DIR/.git" ]]; then
  log "Cloning MuJoCo -> $MUJOCO_DIR"
  git clone "$MUJOCO_REPO" "$MUJOCO_DIR"
fi

pushd "$MUJOCO_DIR" >/dev/null
log "Fetching ref: $REF"
git fetch origin "$REF" --depth 1
git checkout --detach FETCH_HEAD
MUJOCO_COMMIT="$(git rev-parse HEAD)"
popd >/dev/null

log "Activating emsdk ($EMSDK_VERSION)"
source "$EMSDK_ENV"

# emsdk on Windows ships node.exe; create a small `node` shim next to it so
# Emscripten helpers (emcmake/emcc) can execute NODE_JS paths without PATHEXT.
if [[ -n "${EMSDK_NODE:-}" && ! -x "${EMSDK_NODE}" && -x "${EMSDK_NODE}.exe" ]]; then
  cat >"${EMSDK_NODE}" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec "${DIR}/node.exe" "$@"
EOF
  chmod +x "${EMSDK_NODE}" || true
fi

# Similar shim for the upstream clang toolchain under WSL.
EMSDK_CLANG="${EMSDK_ROOT_DIR}/upstream/bin/clang"
if [[ ! -x "${EMSDK_CLANG}" && -x "${EMSDK_CLANG}.exe" ]]; then
  cat >"${EMSDK_CLANG}" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec "${DIR}/clang.exe" "$@"
EOF
  chmod +x "${EMSDK_CLANG}" || true
fi
EMSDK_CLANGXX="${EMSDK_ROOT_DIR}/upstream/bin/clang++"
if [[ ! -x "${EMSDK_CLANGXX}" && -x "${EMSDK_CLANGXX}.exe" ]]; then
  cat >"${EMSDK_CLANGXX}" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec "${DIR}/clang++.exe" "$@"
EOF
  chmod +x "${EMSDK_CLANGXX}" || true
fi

if ! emcc -v 2>&1 | grep -q "$EMSDK_VERSION"; then
  log "emsdk $EMSDK_VERSION not active; installing/activating"
  pushd "$EMSDK_ROOT_DIR" >/dev/null
  ./emsdk install "$EMSDK_VERSION"
  ./emsdk activate "$EMSDK_VERSION"
  source "$EMSDK_ENV"
  if [[ -n "${EMSDK_NODE:-}" && ! -x "${EMSDK_NODE}" && -x "${EMSDK_NODE}.exe" ]]; then
    cat >"${EMSDK_NODE}" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec "${DIR}/node.exe" "$@"
EOF
    chmod +x "${EMSDK_NODE}" || true
  fi
  if [[ ! -x "${EMSDK_CLANG}" && -x "${EMSDK_CLANG}.exe" ]]; then
    cat >"${EMSDK_CLANG}" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec "${DIR}/clang.exe" "$@"
EOF
    chmod +x "${EMSDK_CLANG}" || true
  fi
  if [[ ! -x "${EMSDK_CLANGXX}" && -x "${EMSDK_CLANGXX}.exe" ]]; then
    cat >"${EMSDK_CLANGXX}" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
exec "${DIR}/clang++.exe" "$@"
EOF
    chmod +x "${EMSDK_CLANGXX}" || true
  fi
  popd >/dev/null
fi

command -v emcmake >/dev/null 2>&1 || die "emcmake missing after activating emsdk"
command -v emcc >/dev/null 2>&1 || die "emcc missing after activating emsdk"

log "Installing npm dependencies"
pushd "$MUJOCO_DIR/wasm" >/dev/null
npm install
popd >/dev/null

# Emscripten's --emit-tsd invokes `tsc` at link time. The build runs from the
# CMake binary dir, so ensure the wasm-local .bin is on PATH.
export PATH="$MUJOCO_DIR/wasm/node_modules/.bin:$PATH"

mkdir -p "$BUILD_DIR"
log "Configuring (build dir: $BUILD_DIR)"
emcmake cmake -S "$MUJOCO_DIR" -B "$BUILD_DIR" -G Ninja

JOBS="${PAR_JOBS:-}"
if [[ -z "$JOBS" ]]; then
  if command -v nproc >/dev/null 2>&1; then
    JOBS="$(nproc)"
  else
    JOBS=4
  fi
fi

log "Building mujoco_wasm (parallel: $JOBS)"
cmake --build "$BUILD_DIR" --target mujoco_wasm --parallel "$JOBS"

DIST_DIR="$MUJOCO_DIR/wasm/dist"
[[ -d "$DIST_DIR" ]] || die "Expected dist directory missing: $DIST_DIR"

log "Copying dist -> $OUT_ABS"
rm -rf "$OUT_ABS"
mkdir -p "$OUT_ABS"
cp -a "$DIST_DIR/." "$OUT_ABS/"

EMCC_V="$(emcc -v 2>&1 | tr '\n' ' ' | sed 's/  */ /g' | head -c 500)"
META_JSON="$OUT_ABS/build_meta.json"
python3 - "$REF" "$MUJOCO_COMMIT" "$EMSDK_VERSION" "$EMCC_V" "$MUJOCO_REPO" "$OUT_ABS" >"$META_JSON" <<'PY'
import json
import sys
from datetime import datetime, timezone

ref, commit, emsdk_ver, emcc_v, repo, out_dir = sys.argv[1:]
payload = {
  "kind": "official-embind",
  "ref": ref,
  "commit": commit,
  "repo": repo,
  "emsdkVersion": emsdk_ver,
  "emcc": emcc_v,
  "generatedAt": datetime.now(timezone.utc).isoformat(),
  "outDir": out_dir,
}
json.dump(payload, sys.stdout, indent=2, sort_keys=True)
sys.stdout.write("\n")
PY

log "Wrote: $META_JSON"
log "Done: $OUT_ABS"
