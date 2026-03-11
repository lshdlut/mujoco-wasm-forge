#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import shutil
import zipfile
from pathlib import Path


def build_version_payload(dist_id: str, dist_version: str, git_sha: str, asset_kind: str) -> dict:
    return {
        "schemaVersion": 1,
        "git_sha": git_sha,
        "distId": dist_id,
        "mujocoVersion": dist_version,
        "assetKind": asset_kind,
    }


def write_json(path: Path, payload: dict) -> None:
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def reset_dir(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True, exist_ok=True)


def copy_children(src: Path, dest: Path, *, exclude: set[str] | None = None, include: set[str] | None = None) -> None:
    exclude = exclude or set()
    include = include or set()
    for child in sorted(src.iterdir(), key=lambda item: item.name):
        if include and child.name not in include:
            continue
        if child.name in exclude:
            continue
        target = dest / child.name
        if child.is_dir():
            shutil.copytree(child, target)
        else:
            shutil.copy2(child, target)


def build_runtime_root(src_ver_dir: Path, out_root: Path, version_payload: dict) -> None:
    reset_dir(out_root)
    copy_children(src_ver_dir, out_root, exclude={"abi"})
    write_json(out_root / "version.json", version_payload)
    pthreads_dir = out_root / "pthreads"
    if pthreads_dir.is_dir():
        write_json(pthreads_dir / "version.json", version_payload)


def build_audit_root(src_ver_dir: Path, out_root: Path, version_payload: dict) -> None:
    reset_dir(out_root)
    copy_children(src_ver_dir, out_root, include={"abi"})
    write_json(out_root / "version.json", version_payload)


def zip_tree(root: Path, out_zip: Path) -> None:
    out_zip.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out_zip, "w", compression=zipfile.ZIP_DEFLATED) as handle:
        for path in sorted(root.rglob("*")):
            if path.is_dir():
                continue
            handle.write(path, path.relative_to(root).as_posix())


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Package forge runtime/audit release assets.")
    parser.add_argument("--dist-dir", default="dist", help="Directory containing dist/<ver>/")
    parser.add_argument("--version", required=True, help="MuJoCo version directory name under dist/")
    parser.add_argument("--dist-id", required=True, help="Release dist id (e.g. 3.5.0-r2)")
    parser.add_argument("--git-sha", required=True, help="Git SHA for version.json")
    parser.add_argument("--out-dir", default=".", help="Output directory for generated zips")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    src_ver_dir = Path(args.dist_dir) / args.version
    if not src_ver_dir.is_dir():
        raise SystemExit(f"dist version directory not found: {src_ver_dir}")

    out_dir = Path(args.out_dir)
    work_root = out_dir / "release-root"
    runtime_root = work_root / "runtime"
    audit_root = work_root / "audit"

    runtime_payload = build_version_payload(args.dist_id, args.version, args.git_sha, "runtime")
    audit_payload = build_version_payload(args.dist_id, args.version, args.git_sha, "audit")

    build_runtime_root(src_ver_dir, runtime_root, runtime_payload)
    build_audit_root(src_ver_dir, audit_root, audit_payload)

    zip_tree(runtime_root, out_dir / "dist-runtime.zip")
    zip_tree(audit_root, out_dir / "dist-audit.zip")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
