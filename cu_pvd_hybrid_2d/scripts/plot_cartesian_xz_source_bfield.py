#!/usr/bin/env python3
"""Plot the Cartesian X-Z source-only |B| map with field-line streamlines."""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from generate_real_b_fieldline_see_source_maps import read_moose_table  # noqa: E402
from generate_reference_source_bfield import (  # noqa: E402
    GAUSS_TO_TESLA,
    REFERENCE_LEVELS_G,
)


DEFAULT_TABLE_DIR = (
    ROOT / "runs" / "zapdos_cartesian_xz_25x30_source_only" / "moose_tables"
)
DEFAULT_OUTPUT = ROOT / "post" / "cartesian_xz_source_only_bfield.png"


def _case_title(metadata: dict) -> str:
    case_titles = {
        "four_coil": "Cartesian X–Z standard four-coil magnetic field",
        "four_coil_img3092": "Cartesian X–Z four-coil IMG_3092 magnetic field",
    }
    return case_titles.get(
        metadata.get("case"),
        "Cartesian X–Z source-only magnetic field (compressed magnitude)",
    )


def plot_cartesian_bfield(
    table_dir: str | Path = DEFAULT_TABLE_DIR,
    output: str | Path = DEFAULT_OUTPUT,
    *,
    stream_density: float = 1.6,
    dpi: int = 180,
) -> Path:
    """Render a single calibrated |B| panel from MOOSE component tables."""
    if not np.isfinite(stream_density) or stream_density <= 0.0:
        raise ValueError("stream_density must be positive and finite")
    if dpi <= 0:
        raise ValueError("dpi must be positive")

    table_path = Path(table_dir)
    x_bx, z_bx, bx_t = read_moose_table(table_path / "Bx_T.tbl")
    x_bz, z_bz, bz_t = read_moose_table(table_path / "By_T.tbl")
    if not np.array_equal(x_bx, x_bz) or not np.array_equal(z_bx, z_bz):
        raise ValueError("Bx_T.tbl and By_T.tbl must share identical axes")
    if np.any(~np.isfinite(bx_t)) or np.any(~np.isfinite(bz_t)):
        raise ValueError("B-field component tables must contain finite values")
    metadata_path = table_path / "generation_metadata.json"
    if metadata_path.is_file():
        metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
        target_x_cm = 100.0 * np.asarray(metadata["powered_target_x_m"], dtype=float)
        wafer_x_cm = 100.0 * np.asarray(
            metadata["boundary_layout_m"]["wafer_x"], dtype=float
        )
        magnetic_centers_cm = 100.0 * np.asarray(
            metadata["magnetic_pole_centers_m"], dtype=float
        )
        compression = metadata.get("bfield_axial_compression")
        floor_height_cm = (
            100.0 * float(compression["compressed_z_m"][0])
            if compression is not None
            else None
        )
    else:
        metadata = {}
        target_x_cm = np.asarray([19.0, 22.0])
        wafer_x_cm = np.asarray([7.0, 17.0])
        magnetic_centers_cm = np.asarray([19.5, 21.5])
        floor_height_cm = 12.0

    try:
        import matplotlib
    except ModuleNotFoundError as error:
        raise ModuleNotFoundError(
            "Matplotlib is required. Run with /opt/miniconda3/bin/python3 "
            "or install matplotlib in your Python environment."
        ) from error

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import BoundaryNorm
    from matplotlib.lines import Line2D

    x_cm = 100.0 * x_bx
    z_cm = 100.0 * z_bx
    magnitude_g = np.hypot(bx_t, bz_t) / GAUSS_TO_TESLA
    plotted_magnitude = np.clip(
        magnitude_g, REFERENCE_LEVELS_G[0], REFERENCE_LEVELS_G[-1]
    )

    number_of_bands = REFERENCE_LEVELS_G.size - 1
    colormap = plt.colormaps["turbo"].resampled(number_of_bands)
    normalization = BoundaryNorm(REFERENCE_LEVELS_G, colormap.N, clip=True)

    figure, axis = plt.subplots(figsize=(10.5, 8.0), constrained_layout=True)
    filled = axis.contourf(
        x_cm,
        z_cm,
        plotted_magnitude.T,
        levels=REFERENCE_LEVELS_G,
        cmap=colormap,
        norm=normalization,
        extend="neither",
    )
    axis.streamplot(
        x_cm,
        z_cm,
        bx_t.T,
        bz_t.T,
        color="white",
        density=stream_density,
        linewidth=0.8,
        arrowsize=0.8,
        broken_streamlines=True,
    )

    # Physical boundaries in the requested one-sided Cartesian chamber.
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
    if floor_height_cm is not None:
        axis.axhline(
            floor_height_cm,
            color="#7df9ff",
            linewidth=1.2,
            linestyle="--",
            alpha=0.9,
            zorder=7,
        )

    legend_handles = [
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
    ]
    if floor_height_cm is not None:
        legend_handles.append(
            Line2D(
                [0],
                [0],
                color="#38d9e6",
                linewidth=1.2,
                linestyle="--",
                label=f"Uniform 1 G for Z ≤ {floor_height_cm:g} cm",
            )
        )
    axis.legend(
        handles=legend_handles,
        loc="lower left",
        framealpha=0.90,
        fontsize=8.5,
    )

    axis.set_xlim(0.0, 25.0)
    axis.set_ylim(0.0, 30.0)
    axis.set_aspect("equal", adjustable="box")
    axis.set_xlabel("X (cm)")
    axis.set_ylabel("Z (cm)")
    axis.set_title(_case_title(metadata))
    axis.grid(color="white", alpha=0.16, linewidth=0.5)

    colorbar = figure.colorbar(
        filled,
        ax=axis,
        boundaries=REFERENCE_LEVELS_G,
        ticks=REFERENCE_LEVELS_G,
        pad=0.025,
    )
    colorbar.set_label("|B| (G)")
    colorbar.ax.set_yticklabels(
        [f"{level:g}" for level in REFERENCE_LEVELS_G]
    )

    output_path = Path(output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_path, dpi=int(dpi), facecolor="white")
    plt.close(figure)
    return output_path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--table-dir", type=Path, default=DEFAULT_TABLE_DIR)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--stream-density", type=float, default=1.6)
    parser.add_argument("--dpi", type=int, default=180)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    output = plot_cartesian_bfield(
        args.table_dir,
        args.output,
        stream_density=args.stream_density,
        dpi=args.dpi,
    )
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
