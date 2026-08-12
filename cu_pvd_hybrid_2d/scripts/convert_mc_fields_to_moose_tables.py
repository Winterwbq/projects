#!/usr/bin/env python
"""Convert mc_fields.csv from prepare_zapdos_inputs.py into MOOSE PiecewiseMultilinear tables.

MOOSE PiecewiseMultilinear 2D format (x = first axis, y = second axis):

    AXIS X
    x0 x1 x2 ...

    AXIS Y
    y0 y1 y2 ...

    DATA
    f(x0,y0) f(x1,y0) f(x2,y0) ...
    f(x0,y1) f(x1,y1) f(x2,y1) ...
    ...

The DATA block is ordered with y (rows) varying slowest and x (columns) varying fastest,
matching the MOOSE convention for PiecewiseMultilinear.

Usage
-----
    python scripts/convert_mc_fields_to_moose_tables.py \\
        --mc-fields runs/zapdos_initial_input/mc_fields.csv \\
        --out-dir   runs/zapdos_initial_input/moose_tables
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))


def write_moose_table(path: Path, x: np.ndarray, y: np.ndarray, values: np.ndarray) -> None:
    """Write a 2D field to a MOOSE PiecewiseMultilinear table file.

    Parameters
    ----------
    path:   output .tbl file path
    x:      1-D array of x cell-centre coordinates, shape (nx,)
    y:      1-D array of y cell-centre coordinates, shape (ny,)
    values: 2-D array of field values, shape (nx, ny), C-order (x varies fastest in memory)
    """
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as f:
        f.write("AXIS X\n")
        f.write(" ".join(f"{v:.8e}" for v in x) + "\n\n")
        f.write("AXIS Y\n")
        f.write(" ".join(f"{v:.8e}" for v in y) + "\n\n")
        f.write("DATA\n")
        # DATA rows correspond to y values (outer loop), columns to x values (inner loop).
        for iy in range(len(y)):
            row = values[:, iy]   # shape (nx,)
            f.write(" ".join(f"{v:.8e}" for v in row) + "\n")


def main() -> int:
    parser = argparse.ArgumentParser(description="Convert MC CSV fields to MOOSE .tbl format")
    parser.add_argument(
        "--mc-fields",
        default=str(ROOT / "runs" / "zapdos_initial_input" / "mc_fields.csv"),
        help="Path to mc_fields.csv written by prepare_zapdos_inputs.py",
    )
    parser.add_argument(
        "--out-dir",
        default=str(ROOT / "runs" / "zapdos_initial_input" / "moose_tables"),
        help="Output directory for .tbl files",
    )
    args = parser.parse_args()

    mc_path = Path(args.mc_fields)
    if not mc_path.exists():
        print(f"ERROR: {mc_path} not found. Run prepare_zapdos_inputs.py first.", file=sys.stderr)
        return 1

    out_dir = Path(args.out_dir)

    # ------------------------------------------------------------------
    # Load mc_fields.csv
    # Columns: x_m, y_m, n_Cu_m3, S_iz_Cu_m3_s, Bx_T, By_T, Bmag_T
    # ------------------------------------------------------------------
    data = np.genfromtxt(mc_path, delimiter=",", names=True)

    x_all = np.asarray(data["x_m"], dtype=float)
    y_all = np.asarray(data["y_m"], dtype=float)

    x_unique = np.unique(x_all)
    y_unique = np.unique(y_all)
    nx, ny = x_unique.size, y_unique.size

    if nx * ny != data.size:
        print(
            f"ERROR: expected {nx}×{ny}={nx*ny} rows but found {data.size}. "
            "CSV may be malformed.",
            file=sys.stderr,
        )
        return 1

    # Sort into (nx, ny) arrays: x varies along axis-0, y along axis-1.
    order = np.lexsort((y_all, x_all))

    def reshape(col_name: str) -> np.ndarray:
        return np.asarray(data[col_name], dtype=float)[order].reshape(nx, ny)

    n_cu  = reshape("n_Cu_m3")
    s_iz  = reshape("S_iz_Cu_m3_s")
    bx    = reshape("Bx_T")
    by    = reshape("By_T")

    # ------------------------------------------------------------------
    # Write tables
    # ------------------------------------------------------------------
    fields = {
        "n_Cu_m3":      n_cu,
        "S_iz_Cu_m3_s": s_iz,
        "Bx_T":         bx,
        "By_T":         by,
    }
    for name, arr in fields.items():
        dest = out_dir / f"{name}.tbl"
        write_moose_table(dest, x_unique, y_unique, arr)
        print(f"  wrote {dest}")

    print(f"\nAll tables written to {out_dir}/")
    print("Expected by cu_pvd_hybrid_template.i:")
    for name in ("n_Cu_m3", "S_iz_Cu_m3_s", "Bx_T", "By_T"):
        print(f"  {out_dir / (name + '.tbl')}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
