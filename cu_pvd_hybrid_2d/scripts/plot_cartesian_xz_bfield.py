#!/usr/bin/env python3
"""Plot one Cartesian X-Z magnetic-field case with black field lines."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np

from generate_cartesian_xz_bfields import CASE_NAMES, CASE_OUTPUT_DIRS
from generate_cartesian_xz_see_maps import read_moose_table
from plot_cartesian_xz_input_maps import LEGACY_BFIELD_LEVELS_G


ROOT = Path(__file__).resolve().parents[1]
CASE_TITLES = {
    "source_only": "Cartesian X-Z source-only magnetic field",
    "four_coil": "Cartesian X-Z standard four-coil magnetic field",
    "four_coil_left_bend": "Cartesian X-Z left-bending four-coil magnetic field",
    "four_coil_img3092": "Cartesian X-Z IMG-3092 four-coil magnetic field",
}


def plot_bfield(
    case: str,
    table_dir: Path | str | None = None,
    output: Path | str | None = None,
    *,
    stream_density: float = 1.6,
    dpi: int = 180,
) -> Path:
    """Render a calibrated magnitude map and prominent black streamlines."""
    if case not in CASE_NAMES:
        raise ValueError(f"unknown case {case!r}; choose one of {', '.join(CASE_NAMES)}")
    if not np.isfinite(stream_density) or stream_density <= 0.0:
        raise ValueError("stream_density must be positive and finite")
    if dpi <= 0:
        raise ValueError("dpi must be positive")

    directory = Path(table_dir) if table_dir is not None else CASE_OUTPUT_DIRS[case]
    output_path = (
        Path(output)
        if output is not None
        else ROOT / "post" / f"cartesian_xz_{case}_bfield.png"
    )
    x, z, bx = read_moose_table(directory / "Bx_T.tbl")
    bz_x, bz_z, bz = read_moose_table(directory / "By_T.tbl")
    if not np.array_equal(x, bz_x) or not np.array_equal(z, bz_z):
        raise ValueError("Bx_T.tbl and By_T.tbl must share identical axes")
    if np.any(~np.isfinite(bx)) or np.any(~np.isfinite(bz)):
        raise ValueError("B-field component tables must contain finite values")

    metadata_path = directory / "bfield_metadata.json"
    metadata = (
        json.loads(metadata_path.read_text(encoding="utf-8"))
        if metadata_path.is_file()
        else {}
    )
    target_x_cm = 100.0 * np.asarray(
        metadata.get("powered_target_x_m", [0.19, 0.22]), dtype=float
    )
    magnetic_centers_cm = 100.0 * np.asarray(
        metadata.get("magnetic_pole_centers_m", [0.195, 0.215]), dtype=float
    )
    wafer_x_cm = np.asarray([7.0, 17.0])

    import matplotlib

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import BoundaryNorm
    from matplotlib.lines import Line2D

    x_cm = 100.0 * x
    z_cm = 100.0 * z
    magnitude_g = 1.0e4 * np.hypot(bx, bz)
    plotted_magnitude = np.clip(
        magnitude_g, LEGACY_BFIELD_LEVELS_G[0], LEGACY_BFIELD_LEVELS_G[-1]
    )
    colormap = plt.colormaps["turbo"].resampled(LEGACY_BFIELD_LEVELS_G.size - 1)
    normalization = BoundaryNorm(LEGACY_BFIELD_LEVELS_G, colormap.N, clip=True)

    figure, axis = plt.subplots(figsize=(10.5, 8.0), constrained_layout=True)
    filled = axis.contourf(
        x_cm,
        z_cm,
        plotted_magnitude.T,
        levels=LEGACY_BFIELD_LEVELS_G,
        cmap=colormap,
        norm=normalization,
        extend="neither",
    )
    axis.streamplot(
        x_cm,
        z_cm,
        bx.T,
        bz.T,
        color="black",
        density=stream_density,
        linewidth=0.9,
        arrowsize=0.85,
        broken_streamlines=True,
        zorder=6,
    )

    wall_color = "#30343b"
    target_color = "#ff365e"
    wafer_color = "#30c4ff"
    axis.plot([0.0, 25.0], [30.0, 30.0], color=wall_color, linewidth=3.0)
    axis.plot([0.0, 25.0], [0.0, 0.0], color=wall_color, linewidth=3.0)
    axis.plot([0.0, 0.0], [0.0, 30.0], color=wall_color, linewidth=3.0)
    axis.plot([25.0, 25.0], [0.0, 30.0], color=wall_color, linewidth=3.0)
    axis.plot(
        target_x_cm,
        [30.0, 30.0],
        color=target_color,
        linewidth=6.0,
        solid_capstyle="butt",
    )
    axis.plot(
        wafer_x_cm,
        [0.0, 0.0],
        color=wafer_color,
        linewidth=6.0,
        solid_capstyle="butt",
    )
    axis.scatter(
        magnetic_centers_cm,
        [29.55, 29.55],
        marker="v",
        s=58,
        facecolor=target_color,
        edgecolor="white",
        linewidth=0.8,
        zorder=8,
    )
    axis.legend(
        handles=[
            Line2D(
                [0],
                [0],
                color=target_color,
                linewidth=5,
                label=f"Powered target ({target_x_cm[0]:g}–{target_x_cm[1]:g} cm)",
            ),
            Line2D(
                [0],
                [0],
                color=wafer_color,
                linewidth=5,
                label=f"Wafer ({wafer_x_cm[0]:g}–{wafer_x_cm[1]:g} cm)",
            ),
            Line2D([0], [0], color=wall_color, linewidth=3, label="Loss wall"),
            Line2D(
                [0],
                [0],
                marker="v",
                linestyle="none",
                markerfacecolor=target_color,
                markeredgecolor="white",
                markersize=7,
                label=(
                    f"Magnetic centers ({magnetic_centers_cm[0]:g}, "
                    f"{magnetic_centers_cm[1]:g} cm)"
                ),
            ),
            Line2D([0], [0], color="black", linewidth=1.2, label="B-field line"),
        ],
        loc="lower left",
        framealpha=0.90,
        fontsize=8.5,
    )
    axis.set_xlim(0.0, 25.0)
    axis.set_ylim(0.0, 30.0)
    axis.set_aspect("equal", adjustable="box")
    axis.set_xlabel("X (cm)")
    axis.set_ylabel("Z (cm)")
    axis.set_title(CASE_TITLES[case])
    axis.grid(color="white", alpha=0.16, linewidth=0.5)

    colorbar = figure.colorbar(
        filled,
        ax=axis,
        boundaries=LEGACY_BFIELD_LEVELS_G,
        ticks=LEGACY_BFIELD_LEVELS_G,
        pad=0.025,
    )
    colorbar.set_label("|B| (G)")
    colorbar.ax.set_yticklabels([f"{level:g}" for level in LEGACY_BFIELD_LEVELS_G])

    output_path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_path, dpi=int(dpi), facecolor="white")
    plt.close(figure)
    return output_path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", choices=CASE_NAMES, required=True)
    parser.add_argument("--table-dir", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--stream-density", type=float, default=1.6)
    parser.add_argument("--dpi", type=int, default=180)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    output = plot_bfield(
        args.case,
        table_dir=args.table_dir,
        output=args.output,
        stream_density=args.stream_density,
        dpi=args.dpi,
    )
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
