#!/usr/bin/env python3
"""
Generate enums.json for forge ABI from official introspect output.

Inputs (defaults from dist/<ver>/abi):
  - enums_introspect_like.json
      Produced by introspect/forge/scan_clang_introspect.py; contains the full
      MuJoCo enum table as seen by the official introspect pipeline.

Outputs:
  - enums.json
      Simple, stable shape for downstream tools/JS:
        {
          "generatedAt": "<iso8601>",
          "source": "enums_introspect_like.json",
          "count": <number of enums>,
          "enums": {
            "mjtGeom": {
              "name": "mjtGeom",
              "values": { "mjGEOM_PLANE": 0, ... }
            },
            ...
          }
        }

Design notes:
  - This is intentionally a thin translation layer over the official
    introspect enums table. We do not maintain any handwritten enum lists
    or whitelists here; upstream introspect is the single source of truth.
  - C/WASM side continues to treat enums as plain integers. JS/TS can
    consume enums.json to build constants or type definitions.
"""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, Optional

from dist_version import abi_dir as resolve_abi_dir, dist_version as detect_dist_version


def _repo_root() -> Path:
  # .../abi_exports -> repo root at parents[1]
  return Path(__file__).resolve().parents[1]


def load_enums_introspect(path: Path) -> Dict[str, Any]:
  data = json.loads(path.read_text(encoding="utf-8"))
  enums = data.get("enums") or {}
  out: Dict[str, Any] = {}
  for name, entry in enums.items():
    values = entry.get("values") or {}
    out[name] = {
      "name": str(entry.get("name") or name),
      "values": {str(k): int(v) for k, v in values.items()},
    }
  return out


def build_payload(enums: Dict[str, Any], source: str) -> Dict[str, Any]:
  generated_at = datetime.now(timezone.utc).isoformat()
  return {
    "generatedAt": generated_at,
    "source": source,
    "count": len(enums),
    "enums": enums,
  }


def main(argv: Optional[list[str]] = None) -> int:
  parser = argparse.ArgumentParser()
  parser.add_argument(
    "--abi",
    type=Path,
    help="Path to dist/<ver>/abi directory.",
  )
  parser.add_argument(
    "--infile",
    type=Path,
    help="Optional explicit path to enums_introspect_like.json.",
  )
  parser.add_argument(
    "--outfile",
    type=Path,
    help="Optional explicit path for enums.json.",
  )
  args = parser.parse_args(argv)

  repo_root = _repo_root()

  target_version = detect_dist_version()
  abi_dir = args.abi or resolve_abi_dir(target_version)
  infile = args.infile or (abi_dir / "enums_introspect_like.json")
  outfile = args.outfile or (abi_dir / "enums.json")

  if not infile.is_file():
    raise SystemExit(f"enums_introspect_like.json not found at {infile}")

  enums = load_enums_introspect(infile)
  payload = build_payload(enums, source=infile.name)

  outfile.parent.mkdir(parents=True, exist_ok=True)
  outfile.write_text(json.dumps(payload, indent=2, sort_keys=False), encoding="utf-8")

  print(f"[gen-enums-from-introspect] wrote {outfile} (enums={payload['count']})")
  return 0


if __name__ == "__main__":
  raise SystemExit(main())
