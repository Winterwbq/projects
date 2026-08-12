#!/usr/bin/env python3
"""Plot the source-only and two external-field B-guided SEE maps."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np

from generate_b_guided_see_source_maps import read_moose_table


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_TABLE_ROOT = ROOT / "runs/zapdos_b_guided_see_30cm_original_density"
DEFAULT_OUTPUT = ROOT / "post/b_guided_see_maps_original_density.png"
CASES = (
    ("source_only", "Source only"),
    ("four_coil", "Four coil"),
    ("four_coil_img3092", "Image-3092 four coil"),
)


def plot_maps(
    table_root: Path,
    output: Path,
    *,
    log_decades: float = 4.0,
    chamber_radius_cm: float = 25.0,
    chamber_height_cm: float = 30.0,
    wafer_radius_cm: float = 15.0,
) -> None:
    """Plot all three paired SEE spatial-weight maps with a shared log scale."""
    import matplotlib.pyplot as plt
    from matplotlib.colors import LogNorm

    if log_decades <= 0.0:
        raise ValueError("log_decades must be positive")

    loaded: list[tuple[str, np.ndarray, np.ndarray, np.ndarray]] = []
    for directory, title in CASES:
        table = table_root / directory / "moose_tables/see_spatial_weight_m-1.tbl"
        r, z, weight = read_moose_table(table)
        if np.any(weight < 0.0) or not np.all(np.isfinite(weight)):
            raise ValueError(f"{table} must contain finite nonnegative values")
        loaded.append((title, r, z, weight))

    reference_r = loaded[0][1]
    reference_z = loaded[0][2]
    for title, r, z, _ in loaded[1:]:
        if not np.array_equal(r, reference_r) or not np.array_equal(z, reference_z):
            raise ValueError(f"{title} does not use the same R-Z grid as source-only")

    vmax = max(float(np.max(weight)) for _, _, _, weight in loaded)
    if vmax <= 0.0:
        raise ValueError("all SEE spatial-weight maps are zero")
    vmin = vmax * 10.0 ** (-log_decades)
    norm = LogNorm(vmin=vmin, vmax=vmax)

    fig, axes = plt.subplots(1, 3, figsize=(15.5, 5.2), constrained_layout=True)
    image = None
    for index, (ax, (title, r, z, weight)) in enumerate(zip(axes, loaded)):
        image = ax.pcolormesh(
            r * 100.0,
            z * 100.0,
            np.maximum(weight.T, vmin),
            shading="auto",
            cmap="magma",
            norm=norm,
        )
        peak = np.unravel_index(np.argmax(weight), weight.shape)
        peak_r_cm = float(r[peak[0]] * 100.0)
        peak_z_cm = float(z[peak[1]] * 100.0)
        ax.plot(peak_r_cm, peak_z_cm, "wo", markersize=5, label="source maximum")
        ax.plot(
            [0.0, chamber_radius_cm],
            [chamber_height_cm, chamber_height_cm],
            color="cyan",
            linewidth=3.0,
            label="target",
        )
        ax.plot(
            [0.0, wafer_radius_cm],
            [0.0, 0.0],
            color="lime",
            linewidth=4.0,
            label="wafer",
        )
        ax.set_title(f"{title}\npeak: R={peak_r_cm:.2f}, Z={peak_z_cm:.2f} cm")
        ax.set_xlabel("R [cm]")
        ax.set_ylabel("Z [cm]")
        ax.set_xlim(0.0, chamber_radius_cm)
        ax.set_ylim(0.0, chamber_height_cm)
        ax.set_aspect("equal", adjustable="box")
        if index == 0:
            ax.legend(loc="lower right", fontsize=8)

    if image is None:
        raise RuntimeError("no SEE maps were plotted")
    fig.colorbar(
        image,
        ax=axes,
        pad=0.02,
        shrink=0.90,
        label=r"SEE spatial weight $W$ [m$^{-1}$]",
    )
    fig.suptitle("B-guided SEE spatial-weight maps", fontweight="bold")
    output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output, dpi=220)
    plt.close(fig)
    print(f"Wrote {output}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--table-root", type=Path, default=DEFAULT_TABLE_ROOT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--log-decades", type=float, default=4.0)
    parser.add_argument("--chamber-radius-cm", type=float, default=25.0)
    parser.add_argument("--chamber-height-cm", type=float, default=30.0)
    parser.add_argument("--wafer-radius-cm", type=float, default=15.0)
    args = parser.parse_args()
    plot_maps(
        args.table_root,
        args.output,
        log_decades=args.log_decades,
        chamber_radius_cm=args.chamber_radius_cm,
        chamber_height_cm=args.chamber_height_cm,
        wafer_radius_cm=args.wafer_radius_cm,
    )


if __name__ == "__main__":
    main()
