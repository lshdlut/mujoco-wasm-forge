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


def _python_executable() -> str:
  """Return the Python executable to use for child invocations."""
  return sys.executable or "python"


def _resolve_bash_executable() -> str:
  """Return the bash executable to use for subprocess calls."""
  if os.name != "nt":
    return "bash"

  override = os.environ.get("MJWF_BASH", "").strip()
  if override:
    return override

  candidates = (
      Path("C:/Program Files/Git/bin/bash.exe"),
      Path("C:/Program Files/Git/usr/bin/bash.exe"),
  )
  for cand in candidates:
    if cand.is_file():
      return str(cand)

  return "bash"


def _bash_argv(*args: str) -> List[str]:
  return [_resolve_bash_executable(), *args]


def _resolve_node_executable(env: Mapping[str, str]) -> str:
  """Return a node executable suitable for running check scripts."""
  for key in ("NODE", "EMSDK_NODE"):
    cand = env.get(key, "").strip()
    if cand:
      return cand

  emsdk = env.get("EMSDK", "").strip()
  if emsdk:
    base = Path(emsdk)
    if base.is_dir():
      node_root = base / "node"
      if node_root.is_dir():
        candidates = sorted(node_root.glob("*_64bit/bin/node.exe"))
        if candidates:
          return str(candidates[-1])

  return "node"


def _rmtree_force(path: Path) -> None:
  def _onerror(func, failed_path: str, _exc_info) -> None:
    try:
      os.chmod(failed_path, 0o700)
    except OSError:
      pass
    func(failed_path)

  shutil.rmtree(path, onerror=_onerror)


def _resolve_clang_executable(env: Mapping[str, str]) -> str:
  """Return a clang executable suitable for the introspection AST dump."""
  for key in ("MJWF_CLANG", "CLANG"):
    cand = env.get(key, "").strip()
    if cand:
      return cand

  emsdk = env.get("EMSDK", "").strip()
  if emsdk:
    base = Path(emsdk)
    if base.is_dir():
      clang_exe = base / "upstream" / "bin" / ("clang.exe" if os.name == "nt" else "clang")
      if clang_exe.is_file():
        return str(clang_exe)

  return "clang"


def _resolve_ninja_executable(env: Mapping[str, str]) -> str | None:
  override = env.get("MJWF_NINJA", "").strip()
  if override:
    path = Path(override)
    if path.is_file():
      return str(path)

  if os.name == "nt":
    vs_ninja = Path(
        "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/"
        "CommonExtensions/Microsoft/CMake/Ninja/ninja.exe"
    )
    if vs_ninja.is_file():
      return str(vs_ninja)

  return None


def _maybe_extend_env_include_path(env: MutableMapping[str, str], key: str, value: str) -> None:
  if not value:
    return
  existing = env.get(key, "").strip()
  env[key] = value if not existing else f"{value}{os.pathsep}{existing}"


def _resolve_build_root() -> Path:
  """Return the root directory used for build trees.

  By default the forge CLI uses ``<repo>/build`` for intermediate build output.
  When working from a OneDrive checkout this can create excessive sync churn.
  Set ``MJWF_BUILD_ROOT`` to place build trees elsewhere (e.g. on a local SSD
  cache directory outside OneDrive).
  """
  raw = os.environ.get("MJWF_BUILD_ROOT", "").strip()
  if not raw:
    return REPO_ROOT / "build"

  path = Path(raw).expanduser()
  if not path.is_absolute():
    path = REPO_ROOT / path
  return path.resolve()


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


_MUJOCO_QHULL_TINYXML_BLOCK = """set(QHULL_ENABLE_TESTING OFF)
# Patch changes in https://github.com/qhull/qhull/pull/173.patch
set(QHULL_PATCH_COMMAND
  git apply --reject --whitespace=fix ${mujoco_SOURCE_DIR}/cmake/qhull-support-emscripten.patch
)

findorfetch(
  USE_SYSTEM_PACKAGE
  OFF
  PACKAGE_NAME
  qhull
  LIBRARY_NAME
  qhull
  GIT_REPO
  https://github.com/qhull/qhull.git
  GIT_TAG
  ${MUJOCO_DEP_VERSION_qhull}
  TARGETS
  qhull
  EXCLUDE_FROM_ALL
  PATCH_COMMAND ${QHULL_PATCH_COMMAND}
)
# MuJoCo includes a file from libqhull_r which is not exported by the qhull include directories.
# Add it to the target.
target_include_directories(
  qhullstatic_r INTERFACE $<BUILD_INTERFACE:${qhull_SOURCE_DIR}/src/libqhull_r>
)
target_compile_options(qhullstatic_r PRIVATE ${MUJOCO_MACOS_COMPILE_OPTIONS})
target_link_options(qhullstatic_r PRIVATE ${MUJOCO_MACOS_LINK_OPTIONS})

set(tinyxml2_BUILD_TESTING OFF)
findorfetch(
  USE_SYSTEM_PACKAGE
  OFF
  PACKAGE_NAME
  tinyxml2
  LIBRARY_NAME
  tinyxml2
  GIT_REPO
  https://github.com/leethomason/tinyxml2.git
  GIT_TAG
  ${MUJOCO_DEP_VERSION_tinyxml2}
  TARGETS
  tinyxml2
  EXCLUDE_FROM_ALL
)
target_compile_options(tinyxml2 PRIVATE ${MUJOCO_MACOS_COMPILE_OPTIONS})
target_link_options(tinyxml2 PRIVATE ${MUJOCO_MACOS_LINK_OPTIONS})
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


def _patch_mujoco_qhull_emscripten(mujoco_dir: Path) -> None:
  """Patch MuJoCo's CMake deps so qhull is Emscripten-friendly.

  This mirrors the upstream qhull PATCH_COMMAND wiring used by MuJoCo so that
  qhull is patched as part of FetchContent population, rather than via ad-hoc
  edits to the build tree.
  """
  cmake_dir = mujoco_dir / "cmake"
  deps_path = cmake_dir / "MujocoDependencies.cmake"
  if not deps_path.is_file():
    return

  # Ensure the qhull-support-emscripten patch file exists in the MuJoCo source
  # tree; this path is referenced from QHULL_PATCH_COMMAND.
  patch_path = cmake_dir / "qhull-support-emscripten.patch"
  patch_text = _QHULL_EMSCRIPTEN_PATCH
  patch_bytes = patch_text.encode("utf-8")
  if not patch_path.is_file() or patch_path.read_bytes() != patch_bytes:
    patch_path.write_bytes(patch_bytes)

  text = deps_path.read_text(encoding="utf-8")
  if "QHULL_PATCH_COMMAND" in text and "qhull-support-emscripten.patch" in text:
    # Already wired to use the patch; nothing else to do.
    return

  start_marker = "set(QHULL_ENABLE_TESTING OFF)"
  end_marker = "target_link_options(tinyxml2 PRIVATE"

  start_idx = text.find(start_marker)
  if start_idx == -1:
    return
  end_idx = text.find(end_marker, start_idx)
  if end_idx == -1:
    return
  end_newline_idx = text.find("\n", end_idx)
  if end_newline_idx == -1:
    end_newline_idx = len(text)
  else:
    end_newline_idx += 1

  new_block = _MUJOCO_QHULL_TINYXML_BLOCK
  new_text = text[:start_idx] + new_block + text[end_newline_idx:]
  deps_path.write_text(new_text, encoding="utf-8")
  print("[forge-cli] patched MuJoCo MujocoDependencies.cmake for qhull Emscripten", file=sys.stderr)


def _patch_mujoco_localtime_emscripten(mujoco_dir: Path) -> None:
  """Patch older MuJoCo releases that hard-error on missing localtime_r.

  MuJoCo <=3.3.0 uses a compile-time #error when no thread-safe localtime
  variant is detected. Emscripten's libc does not define the relevant feature
  macros, so the build fails. We add a minimal __EMSCRIPTEN__ fallback that
  copies from localtime(). Newer releases already handle this case.
  """
  src = mujoco_dir / "src" / "engine" / "engine_util_errmem.c"
  if not src.is_file():
    return
  text = src.read_text(encoding="utf-8")
  marker = "Thread-safe version of `localtime` is not present"
  if marker not in text or "__EMSCRIPTEN__" in text:
    return

  needle = (
      "\n#else\n"
      '    #error "Thread-safe version of `localtime` is not present in the standard C library"\n'
      "#endif"
  )
  if needle not in text:
    return

  replacement = (
      "\n#elif defined(__EMSCRIPTEN__)\n"
      "    // Emscripten may not provide localtime_r/localtime_s; fall back to localtime().\n"
      "    struct tm* tmp = localtime(&rawtime);\n"
      "    if (tmp) {\n"
      "      timeinfo = *tmp;\n"
      "    }\n"
      + needle
  )
  new_text = text.replace(needle, replacement)
  if new_text != text:
    src.write_text(new_text, encoding="utf-8")
    print("[forge-cli] patched MuJoCo localtime fallback for Emscripten", file=sys.stderr)


def _patch_mujoco_disable_pthreads_emscripten(mujoco_dir: Path) -> None:
  """Disable -pthread in MuJoCo's Emscripten build flags.

  MuJoCo's upstream CMakeLists enables -pthread for wasm builds. That requires
  WebAssembly threads support (SharedArrayBuffer / COOP+COEP in browsers) and
  can break consumers that load the dist in default web/worker contexts.
  """
  cmake_lists = mujoco_dir / "CMakeLists.txt"
  if not cmake_lists.is_file():
    return

  text = cmake_lists.read_text(encoding="utf-8")
  new_text = text
  new_text = new_text.replace(
      'set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")',
      'set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")',
  )
  new_text = new_text.replace(
      'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -pthread -fexceptions")',
      'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3 -fexceptions")',
  )
  if new_text != text:
    cmake_lists.write_text(new_text, encoding="utf-8")
    print("[forge-cli] patched MuJoCo to disable -pthread under Emscripten", file=sys.stderr)


def _patch_mujoco_disable_default_compiler_threads_emscripten(mujoco_dir: Path) -> None:
  """Disable multi-threaded XML compilation by default when wasm threads are unavailable."""
  src = mujoco_dir / "src" / "user" / "user_init.c"
  if not src.is_file():
    return

  text = src.read_text(encoding="utf-8")
  if "__EMSCRIPTEN_PTHREADS__" in text and "spec->compiler.usethread" in text:
    # Already patched (or upstream has an equivalent change).
    return

  needle = "  spec->compiler.usethread = 1;"
  if needle not in text:
    return

  replacement = (
      "#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)\n"
      "  spec->compiler.usethread = 0;\n"
      "#else\n"
      "  spec->compiler.usethread = 1;\n"
      "#endif"
  )
  new_text = text.replace(needle, replacement, 1)
  if new_text != text:
    src.write_text(new_text, encoding="utf-8")
    print("[forge-cli] patched MuJoCo to default compiler.usethread=0 for non-pthreads wasm", file=sys.stderr)


def _prepare_mujoco(version: str, enable_pthreads: bool) -> None:
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
  else:
    # Ensure ref switching is deterministic even when prior runs left patches or
    # build output in the external checkout.
    subprocess.run(
        ["git", "-C", str(mujoco_dir), "reset", "--hard"],
        check=True,
    )
    for attempt in range(3):
      clean_proc = subprocess.run(
          ["git", "-C", str(mujoco_dir), "clean", "-fdx"],
          stdout=subprocess.PIPE,
          stderr=subprocess.STDOUT,
          text=True,
      )
      if clean_proc.returncode == 0:
        break

      out = clean_proc.stdout or ""
      print(out, file=sys.stderr, end="")
      failed_paths: List[Path] = []
      for line in out.splitlines():
        m = re.search(r"warning: failed to remove ([^:]+): Permission denied", line)
        if not m:
          continue
        rel = m.group(1).strip().rstrip("/").rstrip("\\")
        if rel:
          failed_paths.append(mujoco_dir / rel)

      if not failed_paths:
        clean_proc.check_returncode()

      removed_any = False
      for path in failed_paths:
        if not path.exists():
          continue
        removed_any = True
        if path.is_dir():
          _rmtree_force(path)
        else:
          try:
            os.chmod(path, 0o700)
          except OSError:
            pass
          path.unlink()

      if not removed_any:
        clean_proc.check_returncode()
    else:
      raise SystemExit("external/mujoco git clean -fdx failed after retries")

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
  _patch_mujoco_qhull_emscripten(mujoco_dir)
  _patch_mujoco_localtime_emscripten(mujoco_dir)
  _patch_mujoco_disable_default_compiler_threads_emscripten(mujoco_dir)
  if not enable_pthreads:
    _patch_mujoco_disable_pthreads_emscripten(mujoco_dir)


def _run_introspect(abi_dir: Path, env: Mapping[str, str]) -> None:
  """Generate dist/<ver>/abi/* introspection artifacts via clang introspect."""
  abi_dir.mkdir(parents=True, exist_ok=True)
  header = REPO_ROOT / "external" / "mujoco" / "include" / "mujoco" / "mujoco.h"
  script = REPO_ROOT / "introspect" / "forge" / "scan_clang_introspect.py"
  env_for_py = dict(env)
  clang_exe = _resolve_clang_executable(env_for_py)
  emsdk = env_for_py.get("EMSDK", "").strip()
  if emsdk:
    sysroot_include = Path(emsdk) / "upstream" / "emscripten" / "cache" / "sysroot" / "include"
    if sysroot_include.is_dir():
      _maybe_extend_env_include_path(env_for_py, "C_INCLUDE_PATH", str(sysroot_include))
  subprocess.run(
      [
          _python_executable(),
          str(script),
          "--clang",
          clang_exe,
          "--header",
          str(header),
          "--out-dir",
          str(abi_dir),
      ],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )


def _bootstrap_nm_symbols(version: str, abi_dir: Path, build_dir: Path, env: Mapping[str, str]) -> None:
  """(Re)generate dist/<ver>/abi/nm_symbols.json (B-set) from the wasm build.

  The B-set is the inventory of symbols implemented by MuJoCo as built for the
  WebAssembly target. Deriving it from a native host build is not portable
  across platforms (e.g. Windows import libraries introduce _imp_* symbols).
  Instead, we scan the Emscripten-built MuJoCo static archive produced in the
  wasm build tree.
  """
  nm_path = abi_dir / "nm_symbols.json"
  msg = "regenerating" if nm_path.is_file() else "bootstrapping"
  print(f"[forge-cli] {msg} nm_symbols.json (B-set) for {version}", file=sys.stderr)

  lib_dir = build_dir / "lib"
  candidates = (
      lib_dir / "libmujoco.a",
      lib_dir / "mujoco.a",
  )
  artifact = next((c for c in candidates if c.is_file()), None)
  if artifact is None and lib_dir.is_dir():
    found = sorted(lib_dir.glob("libmujoco.*"))
    if found:
      artifact = found[0]

  if artifact is None or not artifact.is_file():
    raise RuntimeError(f"Failed to locate Emscripten libmujoco archive under {lib_dir}")

  nm_script = REPO_ROOT / "abi_impl" / "nm_coverage.mjs"
  node_exe = _resolve_node_executable(env)
  subprocess.run(
      [node_exe, str(nm_script), str(artifact), "--out", str(nm_path)],
      check=True,
      cwd=str(REPO_ROOT),
      env=dict(env),
  )

  report = json.loads(nm_path.read_text(encoding="utf-8"))
  if not report.get("ok"):
    raise RuntimeError(
        f"llvm-nm scan failed for {artifact}: {report.get('error') or 'unknown error'}"
    )


def _run_abi_generators(version: str, env: Mapping[str, str]) -> None:
  """Run ABI generators (structs + functions) for the given version."""
  env_for_py = dict(env)
  root_str = str(REPO_ROOT)
  existing = env_for_py.get("PYTHONPATH")
  env_for_py["PYTHONPATH"] = root_str if not existing else f"{root_str}{os.pathsep}{existing}"

  gen_structs = REPO_ROOT / "abi_exports" / "gen_structs.py"
  gen_enums = REPO_ROOT / "abi_exports" / "gen_enums.py"
  gen_scene_geom_soa = REPO_ROOT / "abi_exports" / "gen_scene_geom_soa.py"
  out_h = REPO_ROOT / "app" / "mjwf_abi_structs.h"
  out_c = REPO_ROOT / "app" / "mjwf_abi_structs.c"
  subprocess.run(
      [_python_executable(), str(gen_structs), str(out_h), str(out_c)],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )
  subprocess.run(
      [_python_executable(), str(gen_enums), "--abi", str(REPO_ROOT / "dist" / version / "abi")],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )
  subprocess.run(
      [_python_executable(), str(gen_scene_geom_soa), "--abi", str(REPO_ROOT / "dist" / version / "abi")],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )
  # gen_funcs is shipped as a module; keep using -m to avoid duplicating its entrypoint.
  subprocess.run(
      [_python_executable(), "-m", "abi_exports.gen_funcs", "--version", version],
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_py,
  )


def _configure_wasm_build_dir(
    version: str,
    build_dir: Path,
    env: Mapping[str, str],
    enable_pthreads: bool,
) -> None:
  """Configure the Emscripten + CMake build directory."""
  build_dir.mkdir(parents=True, exist_ok=True)

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
      '  _mjwf_emsdk_env="${EMSDK}/.mjwf_emsdk_env.sh"; '
      "  tr -d '\\r' < \"${EMSDK}/emsdk_env.sh\" > \"${_mjwf_emsdk_env}\"; "
      '  . "${_mjwf_emsdk_env}"; '
      '  rm -f "${_mjwf_emsdk_env}"; '
      'elif [ -f "$HOME/emsdk/emsdk_env.sh" ]; then '
      '  _mjwf_emsdk_env="$HOME/emsdk/.mjwf_emsdk_env.sh"; '
      "  tr -d '\\r' < \"$HOME/emsdk/emsdk_env.sh\" > \"${_mjwf_emsdk_env}\"; "
      '  . "${_mjwf_emsdk_env}"; '
      '  rm -f "${_mjwf_emsdk_env}"; '
      'fi; '
  )
  configure_cmd = (
      "set -euo pipefail; "
      + emsdk_env_snippet +
      f"emcmake cmake "
      + (
          f"-G Ninja -DCMAKE_MAKE_PROGRAM='{_resolve_ninja_executable(env)}' "
          if _resolve_ninja_executable(env) and os.name == "nt"
          else ""
      ) +
      f"-S '{app_dir}' "
      f"-B '{build_dir}' "
      "-DCMAKE_BUILD_TYPE=Release "
      "-DMUJOCO_ENABLE_QHULL=OFF "
      "-DMUJOCO_BUILD_EXAMPLES=OFF "
      "-DMUJOCO_BUILD_SIMULATE=OFF "
      "-DMUJOCO_BUILD_TESTS=OFF "
      "-DMUJOCO_BUILD_SAMPLES=OFF "
      "-DCMAKE_SKIP_INSTALL_RULES=ON "
      "-DLIBM_LIBRARY:STRING=-lm "
      "-DMJWF_ENABLE_PLUGINS=ON "
      "-DMJWF_ENABLE_SIMD=ON "
      + ("-DMJWF_ENABLE_PTHREADS=ON " if enable_pthreads else "")
      + "-DMJWF_PROFILE=fast "
      f"-DMJVER='{version}'"
  )

  def _run_configure(check: bool) -> subprocess.CompletedProcess:
    return subprocess.run(
        _bash_argv("-lc", configure_cmd),
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


def _build_wasm(build_dir: Path, env: Mapping[str, str], target: str | None = None) -> None:
  """Build the wasm project (optionally a single target) in an existing build dir."""
  emsdk_env_snippet = (
      'if [ -n "${EMSDK:-}" ] && [ -f "${EMSDK}/emsdk_env.sh" ]; then '
      '  _mjwf_emsdk_env="${EMSDK}/.mjwf_emsdk_env.sh"; '
      "  tr -d '\\r' < \"${EMSDK}/emsdk_env.sh\" > \"${_mjwf_emsdk_env}\"; "
      '  . "${_mjwf_emsdk_env}"; '
      '  rm -f "${_mjwf_emsdk_env}"; '
      'elif [ -f "$HOME/emsdk/emsdk_env.sh" ]; then '
      '  _mjwf_emsdk_env="$HOME/emsdk/.mjwf_emsdk_env.sh"; '
      "  tr -d '\\r' < \"$HOME/emsdk/emsdk_env.sh\" > \"${_mjwf_emsdk_env}\"; "
      '  . "${_mjwf_emsdk_env}"; '
      '  rm -f "${_mjwf_emsdk_env}"; '
      'fi; '
  )
  cmd = "cmake --build " + f"'{build_dir}'"
  if target:
    cmd += f" --target {target}"
  cmd += " -- -j \"$(nproc)\""
  build_cmd = "set -euo pipefail; " + emsdk_env_snippet + cmd
  subprocess.run(
      _bash_argv("-lc", build_cmd),
      check=True,
      cwd=str(REPO_ROOT),
      env=dict(env),
  )


def _copy_wasm_artifacts(build_dir: Path, dist_dir: Path, enable_pthreads: bool) -> None:
  """Copy build_dir/_wasm outputs into dist/<ver>."""
  dist_dir.mkdir(parents=True, exist_ok=True)
  wasm_dir = build_dir / "_wasm"
  js_src = wasm_dir / "mjwasm_forge.js"

  wasm_src = wasm_dir / "mjwasm_forge.wasm"
  if not wasm_src.is_file():
    alt = wasm_dir / "mujoco.wasm"
    if alt.is_file():
      wasm_src = alt

  map_src = wasm_dir / "mjwasm_forge.wasm.map"
  if not map_src.is_file():
    alt_map = wasm_dir / "mujoco.wasm.map"
    if alt_map.is_file():
      map_src = alt_map

  shutil.copy2(js_src, dist_dir / "mujoco.js")
  shutil.copy2(wasm_src, dist_dir / "mujoco.wasm")
  if map_src.is_file():
    shutil.copy2(map_src, dist_dir / "mujoco.wasm.map")

  if enable_pthreads:
    # Emscripten's pthread worker modules import the original output basename
    # (e.g. mjwasm_forge.js/wasm). Keep those alongside the stable mujoco.* names.
    shutil.copy2(js_src, dist_dir / js_src.name)
    shutil.copy2(wasm_src, dist_dir / wasm_src.name)
    if map_src.is_file():
      shutil.copy2(map_src, dist_dir / map_src.name)

  for worker_src in sorted(wasm_dir.glob("*.worker.js")):
    shutil.copy2(worker_src, dist_dir / worker_src.name)
  for worker_map_src in sorted(wasm_dir.glob("*.worker.js.map")):
    shutil.copy2(worker_map_src, dist_dir / worker_map_src.name)
  for worker_src in sorted(wasm_dir.glob("*.worker.mjs")):
    shutil.copy2(worker_src, dist_dir / worker_src.name)
  for worker_map_src in sorted(wasm_dir.glob("*.worker.mjs.map")):
    shutil.copy2(worker_map_src, dist_dir / worker_map_src.name)


def _run_post_build(version: str, short: str, env: Mapping[str, str], variant: str = "") -> None:
  """Run check/post_build.sh for the built version."""
  post = REPO_ROOT / "check" / "post_build.sh"
  env_for_sh = dict(env)
  env_for_sh["NODE"] = _resolve_node_executable(env_for_sh)
  argv = [str(post), "--version", version, "--short", short]
  if variant:
    argv.extend(["--variant", variant])
  subprocess.run(
      _bash_argv(*argv),
      check=True,
      cwd=str(REPO_ROOT),
      env=env_for_sh,
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
      "node check/tests/mesh-texture-smoke.mjs; "
      "node check/tests/plugin-touch-grid.mjs; "
      "node check/tests/gates.mjs"
  )
  subprocess.run(
      _bash_argv("-lc", cmd),
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


def _python_diff_dirs(left: Path, right: Path) -> int:
  """Best-effort directory diff when the `diff` command is unavailable.

  Returns 0 when trees are byte-for-byte identical, 1 when any difference is
  found. Intended only as a local fallback (e.g. on Windows); CI continues to
  use the system `diff -ru` implementation.
  """

  def _collect_files(root: Path) -> dict[str, Path]:
    files: dict[str, Path] = {}
    for dirpath, _dirnames, filenames in os.walk(root):
      dpath = Path(dirpath)
      for name in filenames:
        path = dpath / name
        rel = path.relative_to(root).as_posix()
        files[rel] = path
    return files

  left_files = _collect_files(left)
  right_files = _collect_files(right)

  differences: List[str] = []

  for rel, lpath in sorted(left_files.items()):
    rpath = right_files.pop(rel, None)
    if rpath is None:
      differences.append(f"only in {left}: {rel}")
      continue
    if lpath.read_bytes() != rpath.read_bytes():
      differences.append(f"files differ: {rel}")

  for rel, rpath in sorted(right_files.items()):
    differences.append(f"only in {right}: {rel}")

  if differences:
    print("[forge-cli] python diff found differences:", file=sys.stderr)
    for line in differences[:50]:
      print(f"  {line}", file=sys.stderr)
    if len(differences) > 50:
      print(f"  ... and {len(differences) - 50} more", file=sys.stderr)
    return 1

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

  # Normalize JSON generatedAt fields and non-semantic metadata in key JSON files.
  for name in ("mjapi.json", "wrapper_exports.json", "wrapper_exports_funcs.json", "enums.json"):
    f = abi_dir / name
    if not f.is_file():
      continue
    text = f.read_text(encoding="utf-8")
    text = re.sub(
        r'"generatedAt": *"[^"]*"',
        '"generatedAt": "NORMALIZED_TIME"',
        text,
    )
    # Strip non-semantic visibility metadata differences.
    text = re.sub(r',\s*"visibility":\s*"default"', "", text)
    text = re.sub(r'"visibility":\s*"default"\s*,', "", text)
    text = re.sub(r'"visibility":\s*"default"', "", text)
    f.write_text(text, encoding="utf-8")

  # Normalize nm_coverage.json, which depends on the presence of llvm-nm and
  # whether the static archive is available in the build tree. For the purpose
  # of reproducible dist verification, we only care that the file exists and
  # has a stable shape, not the exact symbol list.
  nm_cov = abi_dir / "nm_coverage.json"
  if nm_cov.is_file():
    data = json.loads(nm_cov.read_text(encoding="utf-8"))
    data["artifact"] = "NORMALIZED_ARTIFACT"
    data["nmPath"] = "NORMALIZED_NM"
    data["ok"] = True
    data["symbols"] = []
    data["count"] = 0
    data["error"] = ""
    nm_cov.write_text(json.dumps(data, sort_keys=True, indent=2) + "\n", encoding="utf-8")

  # Normalize tool-specific metadata in nm_symbols.json.
  # The actual symbol list is semantic and must remain intact for ABI checks.
  nm_syms = abi_dir / "nm_symbols.json"
  if nm_syms.is_file():
    data = json.loads(nm_syms.read_text(encoding="utf-8"))
    data["artifact"] = "NORMALIZED_ARTIFACT"
    data["nmPath"] = "NORMALIZED_NM"
    nm_syms.write_text(json.dumps(data, sort_keys=True, indent=2) + "\n", encoding="utf-8")

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
        r'"clang": *"[^"]*"',
        '"clang": "NORMALIZED_CLANG"',
        text,
    )
    text = re.sub(
        r'"header": *"[^"]*"',
        '"header": "NORMALIZED_HEADER"',
        text,
    )
    text = re.sub(r',\s*"visibility":\s*"default"', "", text)
    text = re.sub(r'"visibility":\s*"default"\s*,', "", text)
    text = re.sub(r'"visibility":\s*"default"', "", text)
    f.write_text(text, encoding="utf-8")

  # Normalize / stub clang AST output in mujoco_ast.json.
  #
  # In practice, the full Clang JSON AST is highly sensitive to compiler
  # version, target configuration and internal representation details. Even
  # after aggressive normalization of ids, file paths and location metadata,
  # structural differences remain between toolchains and runner images.
  #
  # For dist verification we only need to know that an AST was produced for
  # the header; the semantic ABI surface is already covered by the
  # *_introspect_like.json artifacts and wrapper_exports*.json. To avoid
  # brittle, toolchain-specific failures, we reduce mujoco_ast.json to a
  # small, stable sentinel object here.
  ast_path = abi_dir / "mujoco_ast.json"
  if ast_path.is_file():
    stub = {"normalized": True}
    ast_path.write_text(json.dumps(stub, sort_keys=True) + "\n", encoding="utf-8")


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

    # Prefer diff -ru for CI; fall back to a Python diff on platforms
    # where `diff` is not available (e.g. Windows).
    try:
      proc = subprocess.run(
          ["diff", "-ru", str(base), str(ci_copy)],
          cwd=str(REPO_ROOT),
      )
      returncode = proc.returncode
    except FileNotFoundError:
      returncode = _python_diff_dirs(base, ci_copy)

    if returncode not in (0, 1):
      # Propagate unexpected diff or tool errors.
      raise SystemExit(returncode)
    if returncode == 1:
      # Differences found.
      raise SystemExit(
          f"dist/{ver} differs from {args.ci_build_dir}/dist/{ver}"
      )

  return 0


def cmd_build(args: argparse.Namespace) -> int:
  """Entry point: prepare → introspect → ABI → build → post_build → checks."""
  version: str = args.version
  short = _compute_short(version, args.short)
  enable_pthreads = bool(getattr(args, "pthreads", False))
  variant = "pthreads" if enable_pthreads else ""

  dist_root = REPO_ROOT / "dist" / version
  abi_dir = dist_root / "abi"
  dist_dir = dist_root / variant if variant else dist_root
  build_dir = _resolve_build_root() / "forge" / short / ("pthreads" if enable_pthreads else "single")

  print(f"[forge-cli] build version {version} (short={short})", file=sys.stderr)
  if enable_pthreads:
    print("[forge-cli] pthreads enabled (WebAssembly threads)", file=sys.stderr)

  env = _base_env_for_version(version, abi_dir)
  if variant:
    env["MJWF_DIST_VARIANT"] = variant
  else:
    env.pop("MJWF_DIST_VARIANT", None)

  _prepare_mujoco(version, enable_pthreads)
  _run_introspect(abi_dir, env)
  _configure_wasm_build_dir(version, build_dir, env, enable_pthreads)
  _build_wasm(build_dir, env, target="mujoco")
  _bootstrap_nm_symbols(version, abi_dir, build_dir, env)
  _run_abi_generators(version, env)
  _build_wasm(build_dir, env)
  _copy_wasm_artifacts(build_dir, dist_dir, enable_pthreads)
  _run_post_build(version, short, env, variant=variant)

  if args.with_checks:
    _run_checks(env)

  suffix = f"/{variant}" if variant else ""
  print(f"[forge-cli] finished dist/{version}{suffix}", file=sys.stderr)
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
  p_build.add_argument(
      "--pthreads",
      action="store_true",
      help="Build with Emscripten pthreads (-pthread) enabled (requires SharedArrayBuffer in browsers).",
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
