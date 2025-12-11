#!/usr/bin/env python3
"""Unified forge CLI for local and CI workflows.

This script centralizes small but critical pieces of process logic that were
previously embedded directly in GitHub Actions or ad-hoc shell one-liners:

  - Collecting available dist versions from ``dist/``.
  - Normalizing metadata and diffing ``dist/<ver>`` vs ``ci-build/dist/<ver>``.

It intentionally stays thin and delegates heavy work (building, introspection,
ABI generation) to the existing scripts and CMake configuration so that there
is a single, reusable entrypoint for CI and local verification.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path
from typing import List, Mapping, MutableMapping, Sequence

from dist_version import list_dist_versions


REPO_ROOT = Path(__file__).resolve().parent


# Local copy of the upstream qhull Emscripten support patch used by MuJoCo.
_QHULL_EMSCRIPTEN_PATCH = """diff --git a/CMakeLists.txt b/CMakeLists.txt
index 0423820..c5295c1 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -360,6 +360,7 @@ if (NOT DEFINED CMAKE_BUILD_WITH_INSTALL_RPATH)
     set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
 endif ()
 
+if (NOT EMSCRIPTEN)
 add_library(${qhull_SHAREDR} SHARED 
         ${libqhullr_SOURCES}
         src/libqhull_r/qhull_r-exports.def)
@@ -420,7 +421,7 @@ set_target_properties(${qhull_SHAREDP} PROPERTIES
 if(UNIX)
     target_link_libraries(${qhull_SHAREDP} m)
 endif(UNIX)
-
+endif (NOT EMSCRIPTEN)
 # ---------------------------------------
 # Define static libraries qhullstatic (non-reentrant) and qhullstatic_r (reentrant)
 # ---------------------------------------
@@ -475,7 +476,7 @@ if(NOT ${BUILD_STATIC_LIBS})
     set_target_properties(${qhull_STATICR} PROPERTIES EXCLUDE_FROM_ALL TRUE)
     set_target_properties(${qhull_CPP} PROPERTIES EXCLUDE_FROM_ALL TRUE)
 endif()
-if(NOT ${BUILD_SHARED_LIBS})
+if(NOT ${BUILD_SHARED_LIBS} AND NOT EMSCRIPTEN)
     set_target_properties(${qhull_SHARED} PROPERTIES EXCLUDE_FROM_ALL TRUE)
     set_target_properties(${qhull_SHAREDR} PROPERTIES EXCLUDE_FROM_ALL TRUE)
     set_target_properties(${qhull_SHAREDP} PROPERTIES EXCLUDE_FROM_ALL TRUE)
"""


def _collect_dist_versions() -> List[str]:
  """Return sorted dist versions based on directories under dist/."""
  return list_dist_versions()


def _base_env_for_version(version: str, abi_dir: Path) -> MutableMapping[str, str]:
  """Return a base environment with MJVER / DIST_VERSION / MJWF_EXPORTS_LIST set.

  This mirrors the environment that run-forge.sh used to provide so that
  dist_version.py and the CMake configuration observe a consistent view of the
  active dist tree.
  """
  env: MutableMapping[str, str] = dict(os.environ)
  env["MJVER"] = version
  env["DIST_VERSION"] = version
  env["MJWF_EXPORTS_LIST"] = str(abi_dir / "exports.lst")
  return env


def _compute_short(version: str, override: str | None) -> str:
  """Derive the short build identifier used by post_build."""
  if override:
    return override
  digits = "".join(ch for ch in version if ch.isdigit())
  return digits or "337"


def _prepare_mujoco(version: str) -> None:
  """Clone or update external/mujoco for the requested version."""
  mujoco_dir = REPO_ROOT / "external" / "mujoco"
  git_dir = mujoco_dir / ".git"
  if not git_dir.is_dir():
    # Fresh clone for a detached external/mujoco checkout.
    if mujoco_dir.exists():
      shutil.rmtree(mujoco_dir)
    subprocess.run(
        ["git", "clone", "https://github.com/google-deepmind/mujoco", str(mujoco_dir)],
        check=True,
        cwd=str(REPO_ROOT),
    )

  ref = version
  # Prefer tags when they exist so that 3.3.7 resolves to refs/tags/3.3.7.
  try:
    subprocess.run(
        [
            "git",
            "-C",
            str(mujoco_dir),
            "ls-remote",
            "--exit-code",
            "--tags",
            "origin",
            version,
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=True,
    )
    ref = f"refs/tags/{version}"
  except subprocess.CalledProcessError:
    # Fallback to using the version as-is (branch or commit hash).
    ref = version

  subprocess.run(
      ["git", "-C", str(mujoco_dir), "fetch", "--depth", "1", "origin", ref],
      check=True,
  )
  subprocess.run(
      ["git", "-C", str(mujoco_dir), "checkout", "--detach", "FETCH_HEAD"],
      check=True,
  )


def _run_introspect(abi_dir: Path, env: Mapping[str, str]) -> None:
  """Generate dist/<ver>/abi/* introspection artifacts via clang introspect."""
  abi_dir.mkdir(parents=True, exist_ok=True)
  header = REPO_ROOT / "external" / "mujoco" / "include" / "mujoco" / "mujoco.h"
  script = REPO_ROOT / "introspect" / "forge" / "scan_clang_introspect.py"
  subprocess.run(
      [
          "python3",
          str(script),
          "--header",
          str(header),
          "--out-dir",
          str(abi_dir),
      ],
      check=True,
      cwd=str(REPO_ROOT),
      env=dict(env),
  )


def _run_abi_generators(version: str, env: Mapping[str, str]) -> None:
  """Run ABI generators (structs + functions) for the given version."""
  env_for_py = dict(env)
  root_str = str(REPO_ROOT)
  existing = env_for_py.get("PYTHONPATH")
  env_for_py["PYTHONPATH"] = root_str if not existing else f"{root_str}{os.pathsep}{existing}"

  gen_structs = REPO_ROOT / "abi_exports" / "gen_structs.py"
  out_h = REPO_ROOT / "app" / "mjwf_abi_structs.h"
  out_c = REPO_ROOT / "app" / "mjwf_abi_structs.c"
  subprocess.run(
      ["python3", str(gen_structs), str(out_h), str(out_c)],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )
  # gen_funcs is shipped as a module; keep using -m to avoid duplicating its entrypoint.
  subprocess.run(
      ["python3", "-m", "abi_exports.gen_funcs", "--version", version],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )


def _configure_and_build(version: str, dist_dir: Path, build_dir: Path, env: Mapping[str, str]) -> None:
  """Configure the Emscripten + CMake build and emit dist/<ver> artifacts."""
  build_dir.mkdir(parents=True, exist_ok=True)
  dist_dir.mkdir(parents=True, exist_ok=True)

  def _apply_qhull_emscripten_patch(build_root: Path) -> bool:
    """Apply the qhull Emscripten patch in the fetched qhull git checkout.

    This follows the documented two-stage emcmake flow: let CMake populate
    _deps/qhull-src once, then patch its CMakeLists via git apply and retry
    configuration on a clean build tree.
    """
    qhull_src = build_root / "_deps" / "qhull-src"
    if not qhull_src.is_dir():
      return False

    patch_path = qhull_src / "qhull-support-emscripten.patch"
    if not patch_path.is_file():
      patch_path.write_text(_QHULL_EMSCRIPTEN_PATCH, encoding="utf-8")

    # Only apply the patch if it would succeed; this keeps the helper idempotent.
    try:
      subprocess.run(
          ["git", "apply", "--check", str(patch_path)],
          cwd=str(qhull_src),
          stdout=subprocess.DEVNULL,
          stderr=subprocess.DEVNULL,
          check=True,
      )
    except subprocess.CalledProcessError:
      return False

    subprocess.run(
        ["git", "apply", str(patch_path)],
        cwd=str(qhull_src),
        check=True,
    )
    print("[forge-cli] applied qhull Emscripten patch in _deps/qhull-src", file=sys.stderr)
    return True

  def _reset_qhull_state(build_root: Path) -> None:
    """Reset CMake state while preserving fetched dependencies in _deps/."""
    for child in build_root.iterdir():
      if child.name == "_deps":
        continue
      if child.is_dir():
        shutil.rmtree(child)
      else:
        child.unlink()

    deps_dir = build_root / "_deps"
    if not deps_dir.is_dir():
      return
    for dep_child in deps_dir.iterdir():
      if dep_child.name in ("qhull-build", "qhull-subbuild"):
        shutil.rmtree(dep_child)
      elif dep_child.name == "qhull-src":
        cache = dep_child / "CMakeCache.txt"
        if cache.is_file():
          cache.unlink()
        cmake_files = dep_child / "CMakeFiles"
        if cmake_files.is_dir():
          shutil.rmtree(cmake_files)

  app_dir = REPO_ROOT / "app"
  # Mirror run-forge.sh: source emsdk env when available and use emcmake to
  # configure, then build. Prefer the EMSDK environment variable (used in CI)
  # and fall back to $HOME/emsdk for local setups.
  emsdk_env_snippet = (
      'if [ -n "${EMSDK:-}" ] && [ -f "${EMSDK}/emsdk_env.sh" ]; then '
      '  . "${EMSDK}/emsdk_env.sh"; '
      'elif [ -f "$HOME/emsdk/emsdk_env.sh" ]; then '
      '  . "$HOME/emsdk/emsdk_env.sh"; '
      'fi; '
  )
  configure_cmd = (
      "set -euo pipefail; "
      + emsdk_env_snippet +
      f"emcmake cmake "
      f"-S '{app_dir}' "
      f"-B '{build_dir}' "
      "-DCMAKE_BUILD_TYPE=Release "
      "-DMUJOCO_ENABLE_QHULL=OFF "
      "-DMUJOCO_BUILD_PLUGINS=OFF "
      "-DMUJOCO_BUILD_EXAMPLES=OFF "
      "-DMUJOCO_BUILD_SIMULATE=OFF "
      "-DMUJOCO_BUILD_TESTS=OFF "
      "-DMUJOCO_BUILD_SAMPLES=OFF "
      "-DCMAKE_SKIP_INSTALL_RULES=ON "
      "-DLIBM_LIBRARY:STRING=-lm "
      "-DMJWF_PROFILE=default "
      f"-DMJVER='{version}'"
  )
  build_cmd = (
      "set -euo pipefail; "
      + emsdk_env_snippet +
      f"cmake --build '{build_dir}' -- -j \"$(nproc)\""
  )

  def _run_configure(check: bool) -> subprocess.CompletedProcess:
    return subprocess.run(
        ["bash", "-lc", configure_cmd],
        cwd=str(REPO_ROOT),
        env=dict(env),
        check=check,
    )

  # First configure pass: may fail on qhull SHARED libraries under Emscripten
  # but will populate _deps/qhull-src so we can patch it. Only if this pass
  # fails do we fall back to patching qhull and reconfiguring.
  proc = _run_configure(check=False)
  if proc.returncode != 0:
    patched = _apply_qhull_emscripten_patch(build_dir)
    if not patched:
      # The failure was not the known qhull dynamic-linking issue; propagate.
      proc.check_returncode()
    _reset_qhull_state(build_dir)
    _run_configure(check=True)

  subprocess.run(
      ["bash", "-lc", build_cmd],
      check=True,
      cwd=str(REPO_ROOT),
      env=dict(env),
  )

  wasm_dir = build_dir / "_wasm"
  js_src = wasm_dir / "mujoco_wasm.js"
  wasm_src = wasm_dir / "mujoco_wasm.wasm"
  map_src = wasm_dir / "mujoco_wasm.wasm.map"

  shutil.copy2(js_src, dist_dir / "mujoco.js")
  shutil.copy2(wasm_src, dist_dir / "mujoco.wasm")
  if map_src.is_file():
    shutil.copy2(map_src, dist_dir / "mujoco.wasm.map")


def _run_post_build(version: str, short: str, env: Mapping[str, str]) -> None:
  """Run check/post_build.sh for the built version."""
  post = REPO_ROOT / "check" / "post_build.sh"
  subprocess.run(
      ["bash", str(post), "--version", version, "--short", short],
      check=True,
      cwd=str(REPO_ROOT),
      env=dict(env),
  )


def _run_checks(env: Mapping[str, str]) -> None:
  """Run smoke/mesh/gates checks against the active dist/<ver> tree."""
  emsdk_env_snippet = (
      'if [ -n "${EMSDK:-}" ] && [ -f "${EMSDK}/emsdk_env.sh" ]; then '
      '  . "${EMSDK}/emsdk_env.sh"; '
      'elif [ -f "$HOME/emsdk/emsdk_env.sh" ]; then '
      '  . "$HOME/emsdk/emsdk_env.sh"; '
      'fi; '
  )
  cmd = (
      "set -euo pipefail; "
      + emsdk_env_snippet +
      "node check/tests/smoke.mjs; "
      "node check/tests/mesh-smoke.mjs; "
      "node check/tests/gates.mjs"
  )
  subprocess.run(
      ["bash", "-lc", cmd],
      check=True,
      cwd=str(REPO_ROOT),
      env=dict(env),
  )


def cmd_collect_versions(args: argparse.Namespace) -> int:
  """Collect dist versions for local use or GitHub Actions outputs."""
  versions = _collect_dist_versions()
  has_versions = bool(versions)

  if args.github_output:
    # Emit lines suitable for appending directly to $GITHUB_OUTPUT.
    print(f"has_versions={'true' if has_versions else 'false'}")
    print(f"versions={json.dumps(versions)}")
  else:
    payload = {"versions": versions, "has_versions": has_versions}
    json.dump(payload, sys.stdout)
    sys.stdout.write("\n")

  return 0


def _sanitize_meta(dist_dir: Path) -> None:
  """Normalize non-deterministic metadata in a dist/<ver> tree in-place."""
  abi_dir = dist_dir / "abi"
  if not abi_dir.is_dir():
    return

  # Normalize human-readable timestamp in exports_report.md
  report = abi_dir / "exports_report.md"
  if report.is_file():
    lines = report.read_text(encoding="utf-8").splitlines()
    normalized: List[str] = []
    for line in lines:
      if line.startswith("Generated at: "):
        normalized.append("Generated at: NORMALIZED_TIME")
      else:
        normalized.append(line)
    report.write_text("\n".join(normalized) + "\n", encoding="utf-8")

  # Normalize JSON generatedAt fields in mjapi.json and wrapper_exports.json
  for name in ("mjapi.json", "wrapper_exports.json", "wrapper_exports_funcs.json"):
    f = abi_dir / name
    if not f.is_file():
      continue
    text = f.read_text(encoding="utf-8")
    text = re.sub(
        r'"generatedAt": *"[^"]*"',
        '"generatedAt": "NORMALIZED_TIME"',
        text,
    )
    f.write_text(text, encoding="utf-8")

  # Normalize introspect metadata paths that depend on the checkout location.
  for name in (
      "enums_introspect_like.json",
      "functions_introspect_like.json",
      "structs_introspect_like.json",
  ):
    f = abi_dir / name
    if not f.is_file():
      continue
    text = f.read_text(encoding="utf-8")
    text = re.sub(
        r'"ast_json": *"[^"]*"',
        '"ast_json": "NORMALIZED_AST_JSON"',
        text,
    )
    text = re.sub(
        r'"header": *"[^"]*"',
        '"header": "NORMALIZED_HEADER"',
        text,
    )
    f.write_text(text, encoding="utf-8")

  # Normalize clang AST node ids in mujoco_ast.json which are inherently
  # unstable and not relevant for functional equivalence.
  ast_path = abi_dir / "mujoco_ast.json"
  if ast_path.is_file():
    text = ast_path.read_text(encoding="utf-8")
    text = re.sub(r'"0x[0-9a-fA-F]+"', '"0xNORMALIZED"', text)
    text = re.sub(
        r'"file": *"[^"]*/external/mujoco/([^"]*)"',
        r'"file": "/external/mujoco/\1"',
        text,
    )
    text = re.sub(r'/mnt/c/[^"]*/external/mujoco/', '/external/mujoco/', text)
    ast_path.write_text(text, encoding="utf-8")


def cmd_verify_dist(args: argparse.Namespace) -> int:
  """Verify that dist/<ver> matches ci-build/dist/<ver> after normalization."""
  if args.version:
    versions = list(dict.fromkeys(args.version))
  else:
    versions = _collect_dist_versions()

  if not versions:
    print("[forge-cli] no dist versions to verify", file=sys.stderr)
    return 0

  ci_root = REPO_ROOT / args.ci_build_dir
  for ver in versions:
    base = REPO_ROOT / "dist" / ver
    ci_copy = ci_root / "dist" / ver
    if not base.is_dir():
      raise SystemExit(f"dist/{ver} not found at {base}")
    if not ci_copy.is_dir():
      raise SystemExit(f"ci-build dist/{ver} not found at {ci_copy}")

    print(f"[forge-cli] verifying dist/{ver} vs {args.ci_build_dir}/dist/{ver}", file=sys.stderr)
    _sanitize_meta(base)
    _sanitize_meta(ci_copy)

    # Use diff -ru to match CI behavior; exit code 1 indicates differences.
    proc = subprocess.run(
        ["diff", "-ru", str(base), str(ci_copy)],
        cwd=str(REPO_ROOT),
    )
    if proc.returncode not in (0, 1):
      # Propagate unexpected diff or tool errors.
      raise SystemExit(proc.returncode)
    if proc.returncode == 1:
      # Differences found.
      raise SystemExit(
          f"dist/{ver} differs from {args.ci_build_dir}/dist/{ver}"
      )

  return 0


def cmd_build(args: argparse.Namespace) -> int:
  """Entry point: prepare → introspect → ABI → build → post_build → checks."""
  version: str = args.version
  short = _compute_short(version, args.short)
  dist_dir = REPO_ROOT / "dist" / version
  abi_dir = dist_dir / "abi"
  build_dir = REPO_ROOT / "build" / "forge"

  print(f"[forge-cli] build version {version} (short={short})", file=sys.stderr)

  env = _base_env_for_version(version, abi_dir)

  _prepare_mujoco(version)
  _run_introspect(abi_dir, env)
  _run_abi_generators(version, env)
  _configure_and_build(version, dist_dir, build_dir, env)
  _run_post_build(version, short, env)

  if args.with_checks:
    _run_checks(env)

  print(f"[forge-cli] finished dist/{version}", file=sys.stderr)
  return 0


def build_parser() -> argparse.ArgumentParser:
  parser = argparse.ArgumentParser(prog="forge-cli")
  subparsers = parser.add_subparsers(dest="command", required=True)

  p_build = subparsers.add_parser(
      "build",
      help="Build dist/<ver> (prepare → introspect → ABI → build → post_build → checks).",
  )
  p_build.add_argument(
      "--version",
      required=True,
      help="MuJoCo version string (e.g. 3.3.7 or git ref) used for dist/<ver>.",
  )
  p_build.add_argument(
      "--short",
      help="Optional short build name passed to post_build (default: digits from version or 337).",
  )
  p_build.add_argument(
      "--with-checks",
      action="store_true",
      help="Run smoke/mesh/gates scripts after post-build.",
  )
  p_build.set_defaults(func=cmd_build)

  p_collect = subparsers.add_parser(
      "collect-versions",
      help="Collect dist versions from dist/.",
  )
  p_collect.add_argument(
      "--github-output",
      action="store_true",
      help="Emit outputs suitable for $GITHUB_OUTPUT.",
  )
  p_collect.set_defaults(func=cmd_collect_versions)

  p_verify = subparsers.add_parser(
      "verify-dist",
      help="Verify dist/<ver> vs ci-build/dist/<ver>.",
  )
  p_verify.add_argument(
      "-v",
      "--version",
      action="append",
      help="Version to verify (default: all dist/* directories). "
           "Can be provided multiple times.",
  )
  p_verify.add_argument(
      "--ci-build-dir",
      default="ci-build",
      help="Path to reproducible build checkout (default: ci-build).",
  )
  p_verify.set_defaults(func=cmd_verify_dist)

  return parser


def main(argv: Sequence[str] | None = None) -> int:
  parser = build_parser()
  args = parser.parse_args(argv)
  func = getattr(args, "func", None)
  if func is None:
    parser.error("missing command")
  return int(func(args))


if __name__ == "__main__":
  raise SystemExit(main(sys.argv[1:]))
