#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import zipfile
from pathlib import Path
from typing import Any


LEGAL_FILE_NAMES = {
    "license",
    "license.txt",
    "notice",
    "notice.txt",
    "third_party_notices",
    "third_party_notices.txt",
}
LEGAL_DIR_NAMES = {"licenses", "notices"}
METADATA_CANDIDATES = (
    "abi/build_metadata.json",
    "abi/build_provenance.json",
    "abi/provenance.json",
    "build_metadata.json",
    "build_provenance.json",
    "provenance.json",
)
PROVENANCE_FILE_NAMES = {"build_metadata.json", "build_provenance.json", "provenance.json"}


def is_legal_file(path: Path) -> bool:
    name = path.name.lower()
    return name in LEGAL_FILE_NAMES or name.startswith(
        ("license", "notice", "third_party_notice", "third-party-notice", "copying")
    )


def build_version_payload(dist_id: str, dist_version: str, git_sha: str, asset_kind: str) -> dict[str, Any]:
    return {
        "schemaVersion": 1,
        "git_sha": git_sha,
        "distId": dist_id,
        "mujocoVersion": dist_version,
        "assetKind": asset_kind,
        "provenancePath": "provenance.json",
    }


def write_json(path: Path, payload: dict[str, Any]) -> None:
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


def _read_json_object(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError) as exc:
        raise SystemExit(f"invalid JSON metadata file: {path}: {exc}") from exc
    if not isinstance(value, dict):
        raise SystemExit(f"metadata file must contain a JSON object: {path}")
    return value


def load_build_metadata(src_ver_dir: Path, metadata_arg: str | None) -> tuple[dict[str, Any], str | None]:
    """Load an optional build receipt without making the packager host-specific.

    The build pipeline may add a receipt under ``abi/``.  Older deliverables do
    not have one, so absence is represented explicitly in the release manifest
    rather than inferred from the MuJoCo version or from a local SDK path.
    """
    if metadata_arg:
        requested = Path(metadata_arg)
        candidates = [requested]
        if not requested.is_absolute():
            candidates.extend((src_ver_dir / requested, src_ver_dir / "abi" / requested))
        metadata_path = next((path for path in candidates if path.is_file()), None)
        if metadata_path is None:
            raise SystemExit(f"build metadata file not found: {metadata_arg}")
        return _read_json_object(metadata_path), metadata_path

    for relative_path in METADATA_CANDIDATES:
        metadata_path = src_ver_dir / relative_path
        if metadata_path.is_file():
            return _read_json_object(metadata_path), metadata_path
    return {}, None


def _find_scalar(value: Any, aliases: set[str]) -> Any:
    normalized_aliases = {alias.lower() for alias in aliases}
    if isinstance(value, dict):
        for key, candidate in value.items():
            if (
                str(key).lower() in normalized_aliases
                and candidate is not None
                and isinstance(candidate, (str, int, float, bool))
            ):
                return candidate
        for child in value.values():
            found = _find_scalar(child, aliases)
            if found is not None:
                return found
    elif isinstance(value, list):
        for child in value:
            found = _find_scalar(child, aliases)
            if found is not None:
                return found
    return None


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def collect_runtime_file_hashes(src_ver_dir: Path) -> dict[str, str]:
    """Hash only runtime files, with repository-independent POSIX paths."""
    hashes: dict[str, str] = {}
    for path in sorted(src_ver_dir.rglob("*")):
        if not path.is_file():
            continue
        relative = path.relative_to(src_ver_dir)
        if "abi" in relative.parts:
            continue
        if any(part.lower() in LEGAL_DIR_NAMES for part in relative.parts):
            continue
        if path.name.lower() in {"version.json", *PROVENANCE_FILE_NAMES}:
            continue
        if is_legal_file(path):
            continue
        hashes[relative.as_posix()] = _sha256(path)
    return hashes


def collect_abi_artifact_refs(src_ver_dir: Path) -> dict[str, Any]:
    """Record path/sha pairs for the ABI receipts consumed by release review."""
    abi_dir = src_ver_dir / "abi"
    refs: dict[str, Any] = {}
    if not abi_dir.is_dir():
        return refs
    capability = abi_dir / "js_signature_capabilities.json"
    if capability.is_file():
        refs["jsSignatureCapabilities"] = {
            "path": "abi/js_signature_capabilities.json",
            "sha256": _sha256(capability),
        }
    coverage: dict[str, Any] = {}
    for flavor in ("single", "pthreads"):
        path = abi_dir / f"nm_coverage.{flavor}.json"
        if path.is_file():
            coverage[flavor] = {"path": f"abi/nm_coverage.{flavor}.json", "sha256": _sha256(path)}
    if coverage:
        refs["nmCoverage"] = coverage
    legacy_coverage = abi_dir / "nm_coverage.json"
    if legacy_coverage.is_file():
        refs["legacyNmCoverage"] = {"path": "abi/nm_coverage.json", "sha256": _sha256(legacy_coverage)}
    return refs


def detect_flavors(src_ver_dir: Path) -> list[str]:
    flavors: list[str] = []
    if (src_ver_dir / "mujoco.js").is_file() and (src_ver_dir / "mujoco.wasm").is_file():
        flavors.append("single")
    pthreads_dir = src_ver_dir / "pthreads"
    if pthreads_dir.is_dir() and (pthreads_dir / "mujoco.js").is_file() and (pthreads_dir / "mujoco.wasm").is_file():
        flavors.append("pthreads")
    return flavors


def validate_runtime_contract(src_ver_dir: Path) -> None:
    flavors = detect_flavors(src_ver_dir)
    if not flavors:
        raise SystemExit(
            f"runtime deliverable must contain a single-threaded mujoco.js/mujoco.wasm pair: {src_ver_dir}"
        )

    pthreads_dir = src_ver_dir / "pthreads"
    if pthreads_dir.is_dir():
        pthread_runtime = {pthreads_dir / "mujoco.js", pthreads_dir / "mujoco.wasm"}
        missing = sorted(path.name for path in pthread_runtime if not path.is_file())
        if missing:
            raise SystemExit(f"pthreads runtime is incomplete; missing: {', '.join(missing)}")
        aliases = [pthreads_dir / "mjwasm_forge.js", pthreads_dir / "mjwasm_forge.wasm"]
        alias_state = [path.is_file() for path in aliases]
        if any(alias_state) and not all(alias_state):
            raise SystemExit("pthreads worker alias must contain both mjwasm_forge.js and mjwasm_forge.wasm")


def collect_legal_materials(src_ver_dir: Path, legal_dir: Path) -> list[tuple[Path, Path]]:
    """Return existing license/notice files relative to their package root."""
    found: dict[str, Path] = {}
    search_roots = [src_ver_dir, legal_dir]
    for root in search_roots:
        if not root.is_dir():
            continue
        for path in sorted(root.rglob("*")):
            if not path.is_file():
                continue
            relative = path.relative_to(root)
            first = relative.parts[0].lower() if relative.parts else ""
            if len(relative.parts) == 1 and is_legal_file(path):
                found[relative.as_posix()] = path
            elif first in LEGAL_DIR_NAMES:
                found[relative.as_posix()] = path
    if not any(Path(relative).name.lower().startswith(("license", "copying")) for relative in found):
        raise SystemExit(f"no LICENSE material found in {src_ver_dir} or {legal_dir}")
    return [(Path(relative), path) for relative, path in sorted(found.items())]


def copy_legal_materials(materials: list[tuple[Path, Path]], dest: Path) -> None:
    for relative, source in materials:
        target = dest / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)


def build_provenance_payload(
    dist_id: str,
    dist_version: str,
    git_sha: str,
    asset_kind: str,
    flavors: list[str],
    runtime_file_hashes: dict[str, str],
    abi_artifact_refs: dict[str, Any],
    metadata: dict[str, Any],
    metadata_source: str | None,
) -> dict[str, Any]:
    upstream_sha = _find_scalar(
        metadata,
        {
            "upstreamResolvedSHA",
            "upstreamResolvedSha",
            "upstreamresolvedSHA",
            "upstream_sha",
            "upstreamSHA",
            "mujocoSHA",
            "mujocoCommit",
            "resolvedSHA",
            "resolvedSha",
        },
    )
    sdk = _find_scalar(
        metadata,
        {"sdk", "SDK", "sdkVersion", "emscriptenSDK", "emsdkVersion", "emscriptenVersion", "emsdk"},
    )
    status = "verified" if upstream_sha is not None and sdk is not None else "unknown"
    if upstream_sha is not None and sdk is None:
        status = "partial"
    if upstream_sha is None and sdk is not None:
        status = "partial"
    return {
        "schemaVersion": 1,
        "distId": dist_id,
        "mujocoVersion": dist_version,
        "git_sha": git_sha,
        "assetKind": asset_kind,
        "provenanceStatus": status,
        "upstreamResolvedSHA": upstream_sha,
        "sdk": sdk,
        "flavor": "+".join(flavors) if flavors else asset_kind,
        "flavors": flavors,
        "runtimeFileSHA": runtime_file_hashes,
        "abiArtifacts": abi_artifact_refs,
        "metadataSource": metadata_source,
    }


def build_runtime_root(
    src_ver_dir: Path,
    out_root: Path,
    version_payload: dict[str, Any],
    provenance_payload: dict[str, Any],
    legal_materials: list[tuple[Path, Path]],
) -> None:
    reset_dir(out_root)
    copy_children(src_ver_dir, out_root, exclude={"abi", "provenance.json", "version.json", *PROVENANCE_FILE_NAMES})
    copy_legal_materials(legal_materials, out_root)
    write_json(out_root / "version.json", version_payload)
    write_json(out_root / "provenance.json", provenance_payload)
    pthreads_dir = out_root / "pthreads"
    if pthreads_dir.is_dir():
        write_json(pthreads_dir / "version.json", version_payload)
        pthread_provenance = dict(provenance_payload)
        pthread_provenance["flavor"] = "pthreads"
        pthread_provenance["runtimeFileSHA"] = {
            path: digest
            for path, digest in provenance_payload["runtimeFileSHA"].items()
            if path.startswith("pthreads/")
        }
        write_json(pthreads_dir / "provenance.json", pthread_provenance)


def build_audit_root(
    src_ver_dir: Path,
    out_root: Path,
    version_payload: dict[str, Any],
    provenance_payload: dict[str, Any],
    legal_materials: list[tuple[Path, Path]],
) -> None:
    reset_dir(out_root)
    copy_children(src_ver_dir, out_root, include={"abi"})
    copy_legal_materials(legal_materials, out_root)
    write_json(out_root / "version.json", version_payload)
    write_json(out_root / "provenance.json", provenance_payload)


def zip_tree(root: Path, out_zip: Path) -> None:
    out_zip.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(out_zip, "w", compression=zipfile.ZIP_DEFLATED) as handle:
        for path in sorted(root.rglob("*")):
            if path.is_dir():
                continue
            handle.write(path, path.relative_to(root).as_posix())


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Package forge runtime/audit release assets.")
    parser.add_argument("--dist-dir", default="deliverables", help="Directory containing deliverables/<ver>/")
    parser.add_argument("--version", required=True, help="MuJoCo version directory name under deliverables/")
    parser.add_argument("--dist-id", required=True, help="Release dist id (e.g. 3.5.0-r2)")
    parser.add_argument("--git-sha", required=True, help="Git SHA for version.json")
    parser.add_argument("--out-dir", default=".", help="Output directory for generated zips")
    parser.add_argument(
        "--legal-dir",
        default=None,
        help="Optional directory containing LICENSE/NOTICE material (defaults to the repository root)",
    )
    parser.add_argument(
        "--build-metadata",
        "--provenance",
        dest="build_metadata",
        default=None,
        help=(
            "Optional build receipt JSON (alias: --provenance); absent legacy receipts produce explicit unknown "
            "provenance fields"
        ),
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    src_ver_dir = Path(args.dist_dir) / args.version
    if not src_ver_dir.is_dir():
        raise SystemExit(f"deliverable version directory not found: {src_ver_dir}")
    validate_runtime_contract(src_ver_dir)

    out_dir = Path(args.out_dir)
    work_root = out_dir / "release-root"
    runtime_root = work_root / "runtime"
    audit_root = work_root / "audit"

    runtime_payload = build_version_payload(args.dist_id, args.version, args.git_sha, "runtime")
    audit_payload = build_version_payload(args.dist_id, args.version, args.git_sha, "audit")
    metadata, metadata_path = load_build_metadata(src_ver_dir, args.build_metadata)
    metadata_source = None
    if metadata_path is not None:
        try:
            metadata_source = metadata_path.relative_to(src_ver_dir).as_posix()
        except ValueError:
            metadata_source = "external"
    legal_dir = Path(args.legal_dir) if args.legal_dir else Path(__file__).resolve().parents[1]
    legal_materials = collect_legal_materials(src_ver_dir, legal_dir)
    runtime_file_hashes = collect_runtime_file_hashes(src_ver_dir)
    abi_artifact_refs = collect_abi_artifact_refs(src_ver_dir)
    flavors = detect_flavors(src_ver_dir)
    runtime_provenance = build_provenance_payload(
        args.dist_id,
        args.version,
        args.git_sha,
        "runtime",
        flavors,
        runtime_file_hashes,
        abi_artifact_refs,
        metadata,
        metadata_source,
    )
    audit_provenance = build_provenance_payload(
        args.dist_id,
        args.version,
        args.git_sha,
        "audit",
        flavors,
        runtime_file_hashes,
        abi_artifact_refs,
        metadata,
        metadata_source,
    )

    build_runtime_root(src_ver_dir, runtime_root, runtime_payload, runtime_provenance, legal_materials)
    build_audit_root(src_ver_dir, audit_root, audit_payload, audit_provenance, legal_materials)

    zip_tree(runtime_root, out_dir / "dist-runtime.zip")
    zip_tree(audit_root, out_dir / "dist-audit.zip")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
