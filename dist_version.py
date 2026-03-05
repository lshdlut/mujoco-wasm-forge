from __future__ import annotations

import os
import re
from pathlib import Path
from typing import List


def repo_root() -> Path:
    return Path(__file__).resolve().parent


def dist_root() -> Path:
    return repo_root() / "dist"


_SEMVER_RE = re.compile(r"^(\d+)\.(\d+)\.(\d+)(?:[-+].*)?$")


def version_sort_key(version: str):
    match = _SEMVER_RE.match(version)
    if not match:
        return (0, version)
    major, minor, patch = (int(match.group(i)) for i in range(1, 4))
    prerelease = ""
    if "-" in version:
        prerelease = version.split("-", 1)[1]
    is_release = 0 if prerelease else 1
    return (1, major, minor, patch, is_release, prerelease)


def list_dist_versions() -> List[str]:
    root = dist_root()
    if not root.is_dir():
        return []
    return sorted(
        [entry.name for entry in root.iterdir() if entry.is_dir()],
        key=version_sort_key,
    )


def dist_version() -> str:
    env = os.environ.get("MJVER") or os.environ.get("DIST_VERSION")
    if env:
        return env
    versions = list_dist_versions()
    if not versions:
        raise FileNotFoundError(
            "No dist/<ver> directories found. Run `python forge_cli.py build --version <ver>` "
            "(or `npm run build:forge -- --version <ver>`) first, or set MJVER/DIST_VERSION."
        )
    return versions[-1]


def dist_dir(version: str | None = None) -> Path:
    version = version or dist_version()
    path = dist_root() / version
    if not path.is_dir():
        raise FileNotFoundError(f"dist/{version} is missing.")
    return path


def abi_dir(version: str | None = None) -> Path:
    return dist_dir(version) / "abi"
