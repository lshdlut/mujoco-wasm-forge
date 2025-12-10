from __future__ import annotations

import os
from pathlib import Path
from typing import List


def repo_root() -> Path:
    return Path(__file__).resolve().parent


def dist_root() -> Path:
    return repo_root() / "dist"


def list_dist_versions() -> List[str]:
    root = dist_root()
    if not root.is_dir():
        return []
    return sorted([entry.name for entry in root.iterdir() if entry.is_dir()])


def dist_version() -> str:
    env = os.environ.get("MJVER") or os.environ.get("DIST_VERSION")
    if env:
        return env
    versions = list_dist_versions()
    if not versions:
        raise FileNotFoundError(
            "No dist/<ver> directories found. Run `npm run build:forge -- <ver>` first "
            "or set MJVER/DIST_VERSION."
        )
    return versions[0]


def dist_dir(version: str | None = None) -> Path:
    version = version or dist_version()
    path = dist_root() / version
    if not path.is_dir():
        raise FileNotFoundError(f"dist/{version} is missing.")
    return path


def abi_dir(version: str | None = None) -> Path:
    return dist_dir(version) / "abi"
