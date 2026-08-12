#!/usr/bin/env python3
"""Copy a MOOSE table directory and scale only the magnetic-field tables.

Use this to isolate magnetic transport effects in Zapdos:

    python3 scripts/make_scaled_bfield_tables.py \
      --input-dir runs/zapdos_initial_input/moose_tables \
      --output-dir runs/zapdos_initial_input/moose_tables_B0 \
      --scale 0.0

The source and neutral tables are copied unchanged. Only ``Bx_T.tbl`` and
``By_T.tbl`` are multiplied by ``--scale``.
"""
from __future__ import annotations

import argparse
import shutil
from pathlib import Path


def scale_table_values(path: Path, scale: float) -> None:
    lines = path.read_text(encoding="utf-8").splitlines()
    output: list[str] = []
    in_data = False

    for raw_line in lines:
        line = raw_line.strip()
        if line == "DATA":
            in_data = True
            output.append(raw_line)
            continue
        if not in_data or not line:
            output.append(raw_line)
            continue

        values = [float(part) * scale for part in line.split()]
        output.append(" ".join(f"{value:.8e}" for value in values))

    path.write_text("\n".join(output) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description="Copy MOOSE tables and scale Bx_T/By_T only.")
    parser.add_argument("--input-dir", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--scale", type=float, required=True)
    args = parser.parse_args()

    if not args.input_dir.is_dir():
        raise FileNotFoundError(f"input directory does not exist: {args.input_dir}")

    args.output_dir.mkdir(parents=True, exist_ok=True)
    for src in args.input_dir.glob("*.tbl"):
        shutil.copy2(src, args.output_dir / src.name)

    for name in ("Bx_T.tbl", "By_T.tbl"):
        path = args.output_dir / name
        if not path.exists():
            raise FileNotFoundError(f"missing magnetic-field table: {path}")
        scale_table_values(path, args.scale)

    print(f"copied {args.input_dir} -> {args.output_dir}")
    print(f"scaled Bx_T.tbl and By_T.tbl by {args.scale:g}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
