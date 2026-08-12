#!/usr/bin/env python3
"""Plot an R-Z magnetic field from HPEM-like MOOSE tables."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np

from plot_effective_source_tables import read_moose_table


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_TABLE_DIR = ROOT / "runs" / "zapdos_hpem_rz_30cm" / "moose_tables"
DEFAULT_OUTPUT = ROOT / "post" / "hpem_rz_30cm_tables.png"


def plot_tables(
    table_dir: Path,
    output: Path,
    b_vmin_g: float = 1.0,
    b_vmax_g: float = 1000.0,
    b_level_count: int = 15,
) -> None:
    import matplotlib.pyplot as plt
    from matplotlib.colors import BoundaryNorm

    r_bx, z_bx, br = read_moose_table(table_dir / "Bx_T.tbl")
    r_by, z_by, bz = read_moose_table(table_dir / "By_T.tbl")

    if not (np.allclose(r_bx, r_by) and np.allclose(z_bx, z_by)):
        raise ValueError("By_T.tbl grid does not match Bx_T.tbl")
    if b_vmin_g <= 0.0:
        raise ValueError("--b-vmin-g must be positive for geometric levels")
    if b_vmin_g >= b_vmax_g:
        raise ValueError("--b-vmin-g must be smaller than --b-vmax-g")
    if b_level_count < 2:
        raise ValueError("--b-level-count must be at least 2")

    gauss_per_tesla = 1.0e4
    br_g = br * gauss_per_tesla
    bz_g = bz * gauss_per_tesla
    bmag_g = np.hypot(br_g, bz_g)
    b_levels_g = np.geomspace(b_vmin_g, b_vmax_g, b_level_count)
    b_cmap = plt.get_cmap("turbo", b_level_count - 1)
    b_norm = BoundaryNorm(b_levels_g, b_cmap.N, clip=True)
    rr_cm, zz_cm = np.meshgrid(r_bx * 100.0, z_bx * 100.0, indexing="xy")

    fig, ax = plt.subplots(figsize=(7.2, 6.2), constrained_layout=True)
    pcm = ax.pcolormesh(
        r_bx * 100.0,
        z_bx * 100.0,
        bmag_g,
        shading="auto",
        cmap=b_cmap,
        norm=b_norm,
    )
    colorbar = fig.colorbar(pcm, ax=ax, pad=0.025, shrink=0.92)
    colorbar.set_ticks(b_levels_g)
    colorbar.set_ticklabels(
        [
            f"{value:.2f}" if value < 10.0 else f"{value:.1f}" if value < 100.0 else f"{value:.0f}"
            for value in b_levels_g
        ]
    )
    colorbar.set_label("|B| [G]")

    stride = max(1, len(r_bx) // 40)
    ax.streamplot(
        rr_cm,
        zz_cm,
        br_g,
        bz_g,
        color="white",
        density=1.2,
        linewidth=0.8,
        arrowsize=0.7,
    )
    safe_bmag = np.maximum(bmag_g, 1.0e-30)
    ax.quiver(
        rr_cm[::stride, ::stride],
        zz_cm[::stride, ::stride],
        (br_g / safe_bmag)[::stride, ::stride],
        (bz_g / safe_bmag)[::stride, ::stride],
        color="black",
        alpha=0.35,
        angles="xy",
        scale_units="xy",
        scale=2.5,
        width=0.002,
    )

    ax.axhline(0.0, color="black", linewidth=1.0)
    ax.axhline(float(z_bx[-1] * 100.0), color="black", linewidth=1.0)
    ax.axvline(0.0, color="black", linewidth=1.0)
    ax.axvline(float(r_bx[-1] * 100.0), color="black", linewidth=1.0)
    ax.set_title("Magnetic field magnitude")
    ax.set_xlabel("R [cm]")
    ax.set_ylabel("Z [cm]")
    ax.set_aspect("equal", adjustable="box")

    output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output, dpi=220)
    plt.close(fig)
    print(f"wrote {output}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Plot an HPEM-like R-Z B-field in Gauss.")
    parser.add_argument("--table-dir", type=Path, default=DEFAULT_TABLE_DIR)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument(
        "--b-vmin-g",
        type=float,
        default=1.0,
        help="Minimum geometric |B| boundary in Gauss.",
    )
    parser.add_argument(
        "--b-vmax-g",
        type=float,
        default=1000.0,
        help="Maximum geometric |B| boundary in Gauss.",
    )
    parser.add_argument(
        "--b-level-count",
        type=int,
        default=15,
        help="Number of geometric |B| boundaries, including minimum and maximum.",
    )
    args = parser.parse_args()
    plot_tables(
        args.table_dir,
        args.output,
        b_vmin_g=args.b_vmin_g,
        b_vmax_g=args.b_vmax_g,
        b_level_count=args.b_level_count,
    )


if __name__ == "__main__":
    main()
