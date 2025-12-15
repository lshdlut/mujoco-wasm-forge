#!/usr/bin/env python3
"""
Generate a minimal AoS->SoA adapter for mjvScene.geoms (mjvGeom[]).

Design goals (forge-aligned):
  - Keep MuJoCo headers and upstream sources unmodified.
  - Consume only the official introspect JSON:
      dist/<ver>/abi/structs_introspect_like.json
  - Auto-derive the mjvGeom field list (type + fixed extents) and generate:
      * a per-handle scene state (mjvScene + mjvOption/mjvCamera/mjvPerturb)
      * mjwf_scene_update_and_pack(h, catmask)
      * mjwf_scene_geoms_<field>_ptr(h) for every supported field
      * an exports list fragment (app/mjwf_scene_geom_soa.lst)
  - Merge the generated exports into app/mjwf_abi_structs.lst so that
    gen_funcs.py picks them up without any special-case handling.

This script intentionally generates SoA buffers (one contiguous buffer per
field) rather than exporting mjvGeom AoS memory + strides. JS can then wrap
each buffer with TypedArray views and feed them to WebGL/WebGPU with minimal
per-frame logic.
"""

from __future__ import annotations

import argparse
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple

from dist_version import abi_dir as resolve_abi_dir, dist_version as detect_dist_version


REPO_ROOT = Path(__file__).resolve().parents[1]


CTYPE_BY_DTYPE = {
    "f32": "float*",
    "i32": "int32_t*",
    "u8": "uint8_t*",
}

BASETYPE_BY_DTYPE = {
    "f32": "float",
    "i32": "int32_t",
    "u8": "uint8_t",
}

AUTO_DTYPE_BY_BASE = {
    "float": "f32",
    "int": "i32",
    "mjtByte": "u8",
    "char": "u8",
}


@dataclass(frozen=True)
class GeomField:
    name: str
    dtype: str
    basetype: str
    width: int
    is_array: bool


def _parse_fixed_width(t: Dict[str, object]) -> Optional[Tuple[str, int, bool]]:
    """Return (base_type, width, is_array) for ValueType/ArrayType with fixed extents."""
    kind = t.get("kind")
    if kind == "ValueType":
        base = str(t.get("name"))
        return (base, 1, False)
    if kind == "ArrayType":
        inner = t.get("inner") or {}
        inner_kind = inner.get("kind")
        if inner_kind != "ValueType":
            return None
        base = str(inner.get("name"))
        extents = t.get("extents") or []
        if not extents:
            return None
        try:
            width = 1
            for e in extents:
                width *= int(str(e), 10)
        except ValueError:
            return None
        return (base, width, True)
    return None


def load_mjvgeom_fields(structs_json: Path) -> List[GeomField]:
    data = json.loads(structs_json.read_text(encoding="utf-8"))
    structs = data.get("structs", {}) or {}
    geom = structs.get("mjvGeom") or {}
    fields = geom.get("fields", []) or []

    out: List[GeomField] = []
    for f in fields:
        name = str(f.get("name") or "")
        if not name:
            continue
        t = f.get("type") or {}
        parsed = _parse_fixed_width(t)
        if parsed is None:
            continue
        base, width, is_array = parsed
        dtype = AUTO_DTYPE_BY_BASE.get(base)
        if dtype is None:
            continue
        out.append(
            GeomField(
                name=name,
                dtype=dtype,
                basetype=BASETYPE_BY_DTYPE[dtype],
                width=width,
                is_array=is_array,
            )
        )
    return out


def _emit_header(fields: Sequence[GeomField]) -> str:
    lines: List[str] = []
    lines.append("// AUTO-GENERATED. Do not edit by hand.")
    lines.append("// Source: abi_exports/gen_scene_geom_soa.py")
    lines.append("#pragma once")
    lines.append("#include <stdint.h>")
    lines.append("#if defined(__EMSCRIPTEN__)")
    lines.append("#include <emscripten/emscripten.h>")
    lines.append("#else")
    lines.append("#ifndef EMSCRIPTEN_KEEPALIVE")
    lines.append("#define EMSCRIPTEN_KEEPALIVE")
    lines.append("#endif")
    lines.append("#endif")
    lines.append("#ifdef __cplusplus")
    lines.append('extern "C" {')
    lines.append("#endif")
    lines.append("")
    lines.append("EMSCRIPTEN_KEEPALIVE int mjwf_scene_update_and_pack(int h, int catmask);")
    lines.append("")
    for f in fields:
        ctype = CTYPE_BY_DTYPE[f.dtype]
        lines.append(f"EMSCRIPTEN_KEEPALIVE {ctype} mjwf_scene_geoms_{f.name}_ptr(int h);")
    lines.append("")
    lines.append("#ifdef __cplusplus")
    lines.append("}")
    lines.append("#endif")
    lines.append("")
    return "\n".join(lines)


def _emit_source(fields: Sequence[GeomField]) -> str:
    lines: List[str] = []
    lines.append("// AUTO-GENERATED. Do not edit by hand.")
    lines.append("// Source: abi_exports/gen_scene_geom_soa.py")
    lines.append('#include "mjwf_scene_geom_soa.h"')
    lines.append("#include <mujoco/mujoco.h>")
    lines.append("#include <stddef.h>")
    lines.append("#include <stdint.h>")
    lines.append("#include <stdlib.h>")
    lines.append("#include <string.h>")
    lines.append("")
    lines.append("#if defined(__EMSCRIPTEN__)")
    lines.append("#include <emscripten/emscripten.h>")
    lines.append("#else")
    lines.append("#ifndef EMSCRIPTEN_KEEPALIVE")
    lines.append("#define EMSCRIPTEN_KEEPALIVE")
    lines.append("#endif")
    lines.append("#endif")
    lines.append("")
    lines.append("#ifndef MJWF_MAXH")
    lines.append("#define MJWF_MAXH 64")
    lines.append("#endif")
    lines.append("")
    lines.append("// helper entrypoints are defined in mjwf_handles.c")
    lines.append("int mjwf_helper_valid(int h);")
    lines.append("extern mjModel* _mjwf_model_of(int h);")
    lines.append("extern mjData*  _mjwf_data_of(int h);")
    lines.append("")
    lines.append("typedef struct MjwfVisState {")
    lines.append("  int init;")
    lines.append("  int maxgeom;")
    lines.append("  mjvOption vopt;")
    lines.append("  mjvCamera cam;")
    lines.append("  mjvPerturb pert;")
    lines.append("  mjvScene scn;")
    for f in fields:
        lines.append(f"  {f.basetype}* geoms_{f.name};")
    lines.append("} MjwfVisState;")
    lines.append("")
    lines.append("static MjwfVisState g_vis[MJWF_MAXH];")
    lines.append("")
    lines.append("static void mjwf_vis_clear_slot(int h) {")
    lines.append("  if (h <= 0 || h >= MJWF_MAXH) return;")
    lines.append("  MjwfVisState* st = &g_vis[h];")
    lines.append("  if (!st->init) return;")
    lines.append("  mjv_freeScene(&st->scn);")
    for f in fields:
        lines.append(f"  free(st->geoms_{f.name});")
        lines.append(f"  st->geoms_{f.name} = NULL;")
    lines.append("  st->maxgeom = 0;")
    lines.append("  st->init = 0;")
    lines.append("}")
    lines.append("")
    lines.append("void mjwf_scene_reset(int h) {")
    lines.append("  mjwf_vis_clear_slot(h);")
    lines.append("}")
    lines.append("")
    lines.append("static MjwfVisState* mjwf_vis_state(int h) {")
    lines.append("  if (!mjwf_helper_valid(h)) return NULL;")
    lines.append("  if (h <= 0 || h >= MJWF_MAXH) return NULL;")
    lines.append("  MjwfVisState* st = &g_vis[h];")
    lines.append("  if (!st->init) {")
    lines.append("    memset(st, 0, sizeof(*st));")
    lines.append("    mjv_defaultOption(&st->vopt);")
    lines.append("    mjv_defaultCamera(&st->cam);")
    lines.append("    mjv_defaultPerturb(&st->pert);")
    lines.append("    mjv_defaultScene(&st->scn);")
    lines.append("    st->init = 1;")
    lines.append("  }")
    lines.append("  return st;")
    lines.append("}")
    lines.append("")
    lines.append("static int mjwf_vis_reserve(MjwfVisState* st, const mjModel* m, int maxgeom) {")
    lines.append("  if (!st || !m) return 0;")
    lines.append("  if (maxgeom <= st->maxgeom) return 1;")
    lines.append("")
    lines.append("  // (Re)allocate the MuJoCo scene buffers.")
    lines.append("  if (st->scn.maxgeom > 0) {")
    lines.append("    mjv_freeScene(&st->scn);")
    lines.append("    mjv_defaultScene(&st->scn);")
    lines.append("  }")
    lines.append("  mjv_makeScene(m, &st->scn, maxgeom);")
    lines.append("")
    lines.append("  // (Re)allocate SoA attribute buffers.")
    lines.append("  int ok = 1;")
    for f in fields:
        elem = f"((size_t){f.width}) * (size_t)maxgeom"
        lines.append(
            f"  void* tmp_{f.name} = realloc(st->geoms_{f.name}, sizeof({f.basetype})*{elem});"
        )
        lines.append(f"  if (!tmp_{f.name}) ok = 0;")
        lines.append(
            f"  else st->geoms_{f.name} = ({f.basetype}*)tmp_{f.name};"
        )
    lines.append("  if (!ok) return 0;")
    lines.append("  st->maxgeom = maxgeom;")
    lines.append("  return 1;")
    lines.append("}")
    lines.append("")
    lines.append("mjvScene* _mjwf_scene_of(int h) {")
    lines.append("  MjwfVisState* st = mjwf_vis_state(h);")
    lines.append("  return st ? &st->scn : NULL;")
    lines.append("}")
    lines.append("")
    lines.append("mjvOption* _mjwf_vopt_of(int h) {")
    lines.append("  MjwfVisState* st = mjwf_vis_state(h);")
    lines.append("  return st ? &st->vopt : NULL;")
    lines.append("}")
    lines.append("")
    lines.append("mjvCamera* _mjwf_cam_of(int h) {")
    lines.append("  MjwfVisState* st = mjwf_vis_state(h);")
    lines.append("  return st ? &st->cam : NULL;")
    lines.append("}")
    lines.append("")
    lines.append("mjvPerturb* _mjwf_pert_of(int h) {")
    lines.append("  MjwfVisState* st = mjwf_vis_state(h);")
    lines.append("  return st ? &st->pert : NULL;")
    lines.append("}")
    lines.append("")
    lines.append("EMSCRIPTEN_KEEPALIVE int mjwf_scene_update_and_pack(int h, int catmask) {")
    lines.append("  if (!mjwf_helper_valid(h)) return -1;")
    lines.append("  mjModel* m = _mjwf_model_of(h);")
    lines.append("  mjData*  d = _mjwf_data_of(h);")
    lines.append("  if (!m || !d) return -1;")
    lines.append("  MjwfVisState* st = mjwf_vis_state(h);")
    lines.append("  if (!st) return -1;")
    lines.append("")
    lines.append("  int maxgeom = st->maxgeom > 0 ? st->maxgeom : 1024;")
    lines.append("  for (int attempt = 0; attempt < 4; ++attempt) {")
    lines.append("    if (!mjwf_vis_reserve(st, m, maxgeom)) return -1;")
    lines.append("    mjv_updateScene(m, d, &st->vopt, &st->pert, &st->cam, catmask, &st->scn);")
    lines.append("    if (!st->scn.status) break;")
    lines.append("    // Geom buffer exhausted; grow and retry.")
    lines.append("    maxgeom *= 2;")
    lines.append("  }")
    lines.append("  if (st->scn.status) return -1;")
    lines.append("")
    lines.append("  const int n = st->scn.ngeom;")
    lines.append("  if (n <= 0) return 0;")
    lines.append("")
    lines.append("  for (int i = 0; i < n; ++i) {")
    lines.append("    const mjvGeom* g = &st->scn.geoms[i];")
    for f in fields:
        if f.is_array:
            lines.append(
                f"    memcpy(&st->geoms_{f.name}[((size_t)i)*{f.width}], g->{f.name}, sizeof({f.basetype})*{f.width});"
            )
        else:
            lines.append(f"    st->geoms_{f.name}[i] = ({f.basetype})g->{f.name};")
    lines.append("  }")
    lines.append("")
    lines.append("  return n;")
    lines.append("}")
    lines.append("")
    for f in fields:
        ctype = CTYPE_BY_DTYPE[f.dtype]
        lines.append(f"EMSCRIPTEN_KEEPALIVE {ctype} mjwf_scene_geoms_{f.name}_ptr(int h) {{")
        lines.append("  MjwfVisState* st = mjwf_vis_state(h);")
        lines.append(f"  return st ? ({ctype})(st->geoms_{f.name}) : NULL;")
        lines.append("}")
        lines.append("")
    return "\n".join(lines)


def _merge_exports(target: Path, extra: Iterable[str]) -> None:
    existing: List[str] = []
    if target.is_file():
        for line in target.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            existing.append(line)
    merged = sorted(set(existing) | set(extra))
    target.write_text("\n".join(merged) + "\n", encoding="utf-8")


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--abi",
        type=Path,
        help="Optional path to dist/<ver>/abi (defaults to the first dist entry).",
    )
    parser.add_argument(
        "--out-h",
        type=Path,
        default=REPO_ROOT / "app" / "mjwf_scene_geom_soa.h",
        help="Output header path.",
    )
    parser.add_argument(
        "--out-c",
        type=Path,
        default=REPO_ROOT / "app" / "mjwf_scene_geom_soa.c",
        help="Output source path.",
    )
    parser.add_argument(
        "--out-lst",
        type=Path,
        default=REPO_ROOT / "app" / "mjwf_scene_geom_soa.lst",
        help="Output exports fragment list path.",
    )
    parser.add_argument(
        "--merge-into",
        type=Path,
        default=REPO_ROOT / "app" / "mjwf_abi_structs.lst",
        help="Exports list to merge into (used by gen_funcs.py).",
    )
    args = parser.parse_args(list(argv) if argv is not None else None)

    target_version = detect_dist_version()
    abi_dir = args.abi or resolve_abi_dir(target_version)
    structs_json = (abi_dir / "structs_introspect_like.json").resolve()
    if not structs_json.is_file():
        raise SystemExit(f"structs_introspect_like.json not found at {structs_json}")

    fields = load_mjvgeom_fields(structs_json)
    if not fields:
        raise SystemExit("No supported mjvGeom fields found in introspect JSON")

    out_h = args.out_h.resolve()
    out_c = args.out_c.resolve()
    out_lst = args.out_lst.resolve()
    merge_into = args.merge_into.resolve()

    out_h.parent.mkdir(parents=True, exist_ok=True)
    out_c.parent.mkdir(parents=True, exist_ok=True)
    out_lst.parent.mkdir(parents=True, exist_ok=True)

    out_h.write_text(_emit_header(fields), encoding="utf-8")
    out_c.write_text(_emit_source(fields), encoding="utf-8")

    export_names: List[str] = ["scene_update_and_pack"]
    export_names.extend([f"scene_geoms_{f.name}_ptr" for f in fields])
    out_lst.write_text("\n".join(sorted(export_names)) + "\n", encoding="utf-8")

    _merge_exports(merge_into, export_names)

    print(f"[gen-scene-geom-soa] fields={len(fields)} exports={len(export_names)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
