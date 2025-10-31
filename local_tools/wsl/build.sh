#!/usr/bin/env bash
set -euo pipefail

# See local_tools/forge337_wsl.sh history: unified WSL build for 3.2.5 and 3.3.7
# This script mirrors CI and writes artifacts under dist/<mjver>/ with generic names.

log() { printf "\033[1;32m[wsl-build]\033[0m %s\n" "$*"; }
warn() { printf "\033[1;33m[wsl-build][warn]\033[0m %s\n" "$*"; }
err() { printf "\033[1;31m[wsl-build][err ]\033[0m %s\n" "$*"; }

need_cmd() { command -v "$1" >/dev/null 2>&1 || { err "Missing command: $1"; return 1; }; }

detect_jobs() {
  if [[ -n "${PAR_JOBS:-}" ]]; then echo "$PAR_JOBS"; return; fi
  # Default to 6 to avoid saturating the machine by default
  if command -v nproc >/dev/null 2>&1; then
    local cores; cores=$(nproc)
    if [[ "$cores" -lt 6 ]]; then echo "$cores"; else echo 6; fi
    return
  fi
  if command -v getconf >/dev/null 2>&1; then
    local cores; cores=$(getconf _NPROCESSORS_ONLN)
    if [[ "$cores" -lt 6 ]]; then echo "$cores"; else echo 6; fi
    return
  fi
  echo 6
}

require_basics() {
  local missing=()
  for c in git cmake ninja python3 awk sed; do command -v "$c" >/dev/null 2>&1 || missing+=("$c"); done
  if (( ${#missing[@]} > 0 )); then
    warn "Packages missing: ${missing[*]} - attempting apt-get install"
    if command -v apt-get >/dev/null 2>&1; then
      if command -v sudo >/dev/null 2>&1; then sudo apt-get update -y || true; sudo apt-get install -y git build-essential cmake ninja-build python3 python3-pip ccache || true; else apt-get update -y || true; apt-get install -y git build-essential cmake ninja-build python3 python3-pip ccache || true; fi
    fi
  fi
  local still=(); for c in git cmake ninja python3 awk sed; do command -v "$c" >/dev/null 2>&1 || still+=("$c"); done
  if (( ${#still[@]} > 0 )); then err "Required packages missing: ${still[*]}"; exit 1; fi
}

ensure_emsdk() {
  local WANT=3.1.55
  if command -v emcc >/dev/null 2>&1; then if emcc -v 2>&1 | grep -q "$WANT"; then log "emsdk $WANT active"; return 0; fi; fi
  if [[ ! -d "$HOME/emsdk" ]]; then log "Cloning emsdk"; git clone https://github.com/emscripten-core/emsdk.git "$HOME/emsdk"; fi
  pushd "$HOME/emsdk" >/dev/null; ./emsdk install "$WANT"; ./emsdk activate "$WANT"; source "$HOME/emsdk/emsdk_env.sh"; popd >/dev/null
}

activate_emsdk_env() { [[ -f "$HOME/emsdk/emsdk_env.sh" ]] && source "$HOME/emsdk/emsdk_env.sh"; }

maybe_pin_node20() {
  [[ "${PIN_NODE20:-0}" != "1" ]] && return 0
  if command -v node >/dev/null 2>&1 && node -v | grep -qE '^v20\.'; then log "Node v20.x"; return 0; fi
  export NVM_DIR="$HOME/.nvm"; [[ -s "$NVM_DIR/nvm.sh" ]] && . "$NVM_DIR/nvm.sh"
  if command -v nvm >/dev/null 2>&1; then nvm install 20 >/dev/null 2>&1 || nvm install 20; nvm use 20; else warn "nvm not found"; fi
}

build_one() {
  local short="$1" mjver="$2" app="$3"
  log "=== Build ${mjver} (short=${short}) ==="
  local build="build/${short}" native="build/${short}_native"
  mkdir -p external
  if [[ "${CLEAN:-}" == "1" ]]; then rm -rf "$build" "$native" external/mujoco || true; else [[ -f "$build/CMakeCache.txt" ]] && rm -rf "$build"; [[ -f "$native/CMakeCache.txt" ]] && rm -rf "$native"; fi
  [[ -d external/mujoco ]] && rm -rf external/mujoco || true
  if ! git clone --depth 1 --branch "${mjver}" https://github.com/google-deepmind/mujoco external/mujoco; then
    rm -rf external/mujoco || true
    if ! git clone --depth 1 --branch "v${mjver}" https://github.com/google-deepmind/mujoco external/mujoco; then
      rm -rf external/mujoco || true; git clone https://github.com/google-deepmind/mujoco external/mujoco; pushd external/mujoco >/dev/null; git fetch --tags; git checkout -qf "refs/tags/v${mjver}" || git checkout -qf "refs/tags/${mjver}"; popd >/dev/null
    fi
  fi
  local f="external/mujoco/src/engine/engine_util_errmem.c"; if [[ -f "$f" ]]; then sed -i 's/#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__STDC_VERSION_TIME_H__)/#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__EMSCRIPTEN__) || defined(__STDC_VERSION_TIME_H__)/' "$f"; sed -i 's/#if defined(_POSIX_C_SOURCE) || defined(__APPLE__)/#if defined(_POSIX_C_SOURCE) || defined(__APPLE__) || defined(__EMSCRIPTEN__)/' "$f"; fi
  if [[ "$short" == "337" ]]; then
    emcmake cmake -S "$app" -B "$build" -DCMAKE_BUILD_TYPE=Release -DMUJOCO_ENABLE_QHULL=OFF -DMUJOCO_BUILD_PLUGINS=OFF -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF -DCMAKE_SKIP_INSTALL_RULES=ON -DLIBM_LIBRARY:STRING=-lm -DMJVER="${mjver}" || true
    local QH="${build}/_deps/qhull-src/CMakeLists.txt"; if [[ -f "$QH" ]]; then sed -i 's/\bSHARED\b/STATIC/g' "$QH" || true; awk 'BEGIN{print "set(BUILD_SHARED_LIBS OFF CACHE BOOL \"\" FORCE)"} {print}' "$QH" > "$QH.tmp" && mv "$QH.tmp" "$QH"; fi
  fi
  # Enable ccache if available
  local cacheFlags=()
  if command -v ccache >/dev/null 2>&1; then
    mkdir -p "$HOME/.cache/ccache" || true
    export CCACHE_DIR="${CCACHE_DIR:-$HOME/.cache/ccache}"
    cacheFlags+=( -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache )
  fi
  local JOBS; JOBS=$(detect_jobs)
  export CMAKE_BUILD_PARALLEL_LEVEL="$JOBS"
  emcmake cmake -S "$app" -B "$build" -DCMAKE_BUILD_TYPE=Release -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF -DCMAKE_SKIP_INSTALL_RULES=ON -DLIBM_LIBRARY:STRING=-lm -DMJVER="${mjver}" "${cacheFlags[@]}"
  cmake --build "$build" -j "$JOBS"
  cmake -S "$app" -B "$native" -DCMAKE_BUILD_TYPE=Release -DMUJOCO_BUILD_EXAMPLES=OFF -DMUJOCO_BUILD_SIMULATE=OFF -DMUJOCO_BUILD_TESTS=OFF -DMUJOCO_BUILD_SAMPLES=OFF -DCMAKE_SKIP_INSTALL_RULES=ON "${cacheFlags[@]}"
  cmake --build "$native" -j "$JOBS"
  mkdir -p "dist/${mjver}"; cp "${build}/_wasm/mujoco_wasm${short}.js" "dist/${mjver}/mujoco.js"; cp "${build}/_wasm/mujoco_wasm${short}.wasm" "dist/${mjver}/mujoco.wasm"; [[ -f "${build}/_wasm/mujoco_wasm${short}.wasm.map" ]] && cp "${build}/_wasm/mujoco_wasm${short}.wasm.map" "dist/${mjver}/mujoco.wasm.map"
  need_cmd node; maybe_pin_node20; log "Node $(node -v)"
  local native_bin="${repo_root}/${native}/_wasm/mujoco_compare${short}"; MJ_NATIVE_BIN="$native_bin" node "tests/smoke-${short}.mjs"; MJ_NATIVE_BIN="$native_bin" node "tests/regression-${short}.mjs"; [[ -f "tests/mesh-smoke-${short}.mjs" ]] && node "tests/mesh-smoke-${short}.mjs" || true
  if [[ "${META:-0}" == "1" ]]; then
    local EMVER=${EMSDK_VERSION} MJVER=${mjver} MJ_SHA=$(git -C external/mujoco rev-parse HEAD) JS="dist/${MJVER}/mujoco.js" WASM="dist/${MJVER}/mujoco.wasm"
    local JSB=$(stat -c %s "$JS" 2>/dev/null || echo 0) WSB=$(stat -c %s "$WASM" 2>/dev/null || echo 0) JSUM=$(sha256sum "$JS" | cut -d' ' -f1) WSUM=$(sha256sum "$WASM" | cut -d' ' -f1) NOW=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
    cat > "dist/${MJVER}/version.json" <<JSON
{
  "mujocoVersion": "${MJVER}",
  "emscripten": "${EMVER}",
  "buildTime": "${NOW}",
  "gitSha": "${MJ_SHA}",
  "emsdk_root": "${EMSDK:-}",
  "emsdk_node": "${EMSDK_NODE:-}",
  "emsdk_python": "${EMSDK_PYTHON:-}",
  "flags": ["WASM=1","MODULARIZE=1","EXPORT_ES6=1","ALLOW_MEMORY_GROWTH=1"],
  "features": {"qhull": "static","libccd": "static","plugins": false,"render": false,"exceptions": "off","pthreads": false,"filesystem": true,"env": "node,web"},
  "size": {"wasmBytes": ${WSB}, "jsBytes": ${JSB}},
  "hash": {"wasmSha256": "${WSUM}", "jsSha256": "${JSUM}"}
}
JSON
    local NS="https://local/wsl/sbom/$(date +%s)"; cat > "dist/${MJVER}/sbom.spdx.json" <<SBOM
{
  "spdxVersion": "SPDX-2.3",
  "dataLicense": "CC0-1.0",
  "SPDXID": "SPDXRef-DOCUMENT",
  "name": "mujoco-wasm-forge-${MJVER}",
  "documentNamespace": "${NS}",
  "creationInfo": {"created": "${NOW}", "creators": ["Tool: forge workflow (local)"]},
  "packages": [
    {"name": "mujoco", "SPDXID": "SPDXRef-Pkg-MuJoCo", "versionInfo": "${MJVER}", "downloadLocation": "https://github.com/google-deepmind/mujoco", "sourceInfo": "git@${MJ_SHA}"},
    {"name": "emscripten", "SPDXID": "SPDXRef-Pkg-Emscripten", "versionInfo": "${EMVER}", "downloadLocation": "https://github.com/emscripten-core/emsdk"}
  ]
}
SBOM
    printf "%s  %s\n%s  %s\n" "$JSUM" "$(basename "$JS")" "$WSUM" "$(basename "$WASM")" > "dist/${MJVER}/SHA256SUMS.txt"
    cat > "dist/${MJVER}/RELEASE_NOTES.md" <<EOF

Build with Emscripten ${EMVER}. Artifacts:
- mujoco.js (${JSB} bytes)
- mujoco.wasm (${WSB} bytes)

Quality gates:
- [GATE:SYM] skipped
- [GATE:DTS] skipped
- [GATE:RUN] pass
EOF
  fi
  if [[ -f "dist/${mjver}/abi/wrapper_exports.json" ]]; then
    node scripts/mujoco_abi/check_exports.mjs "dist/${mjver}/abi" "dist/${mjver}/mujoco.wasm" "dist/${mjver}/abi/wrapper_exports.json"
    node scripts/mujoco_abi/nm_coverage.mjs "dist/${mjver}/mujoco.wasm" "dist/${mjver}/abi/wrapper_exports.json" --out "dist/${mjver}/abi/nm_coverage.json"
  else
    warn "dist/${mjver}/abi/wrapper_exports.json missing; skipping export coverage checks"
  fi
  log "Artifacts in dist/${mjver}:"; ls -l "dist/${mjver}" || true
}

main() {
  require_basics; activate_emsdk_env || true; ensure_emsdk
  need_cmd emcmake; need_cmd cmake
  script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  # build.sh lives under local_tools/wsl; repo root is two levels up
  repo_root="$(cd "${script_dir}/../.." && pwd)"
  cd "$repo_root"; log "Repo root: $repo_root"
  export EMSDK_VERSION=3.1.55
  : "${MJVER:=}"
  : "${MJVER_325:=}"
  : "${MJVER_337:=}"
  IFS=',' read -r -a targets <<< "${TARGETS:-325,337}"
  for t in "${targets[@]}"; do case "$t" in 325) build_one 325 "${MJVER_325:-3.2.5}" "wrappers/official_app_325";; 337) build_one 337 "${MJVER_337:-${MJVER:-3.3.7}}" "wrappers/official_app_337";; *) warn "Unknown target '$t'";; esac; done
  log "Done."
}

main "$@"
