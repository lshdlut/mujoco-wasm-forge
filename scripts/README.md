# Scripts

- uild_all.ps1: Experimental end-to-end build driver used during research. It bootstraps emsdk and attempts three builds:
  1) mujoco_webfork (embind) if present under _ext/mujoco_webfork
  2) Official MuJoCo v3.2.5 minimal wrapper if present under _ext/mujoco_official_v3.2.5
  3) Official MuJoCo v3.3.7 minimal wrapper if present under _ext/mujoco_official_v3.3.7

Outputs are placed under _dist/. This script expects the corresponding sources to exist locally (not vendored in this repo). See comments in the script for workarounds applied to qhull/libccd under Emscripten.

Note: This script is a stopgap to unblock development and will be replaced by a standardized CMake-based build with a pinned emscripten toolchain and regression harness.
