#!/usr/bin/env python3
"""Plot the Cartesian X-Z SEE spatial-weight map with B-field streamlines."""
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


DEFAULT_TABLE_DIR = (
    ROOT / "runs" / "zapdos_cartesian_xz_25x30_source_only" / "moose_tables"
)
DEFAULT_OUTPUT = ROOT / "post" / "cartesian_xz_source_only_see_map.png"


def _direction_label(direction_model: str) -> str:
    labels = {
        "vertical_downward": "vertical downward",
        "bfield_guided": "B-field guided",
        "mostly_vertical_bfield_guided": "mostly vertical, gently B-guided",
    }
    return labels.get(direction_model, str(direction_model).replace("_", " "))


def _load_metadata(table_dir: Path) -> dict:
    metadata_path = table_dir / "generation_metadata.json"
    if metadata_path.is_file():
        return json.loads(metadata_path.read_text(encoding="utf-8"))
    return {
        "powered_target_x_m": [0.19, 0.22],
        "launch_lobe_centers_m": [0.195, 0.215],
        "plasma_efficiency": float("nan"),
        "transport_settings": {
            "local_fraction": 0.995,
            "local_attenuation_length_m": 0.006,
            "guided_attenuation_length_m": 0.05,
            "spread_angle_degrees": 5.0,
            "transition_field_t": 0.001,
        },
        "termination_counts": {},
        "boundary_layout_m": {"wafer_x": [0.07, 0.17]},
    }


def _require_shared_axes(
    reference_x: np.ndarray,
    reference_z: np.ndarray,
    other_x: np.ndarray,
    other_z: np.ndarray,
    other_name: str,
) -> None:
    if not np.array_equal(reference_x, other_x) or not np.array_equal(
        reference_z, other_z
    ):
        raise ValueError(f"{other_name} must share identical axes with Bx_T.tbl")


def plot_cartesian_see_map(
    table_dir: str | Path = DEFAULT_TABLE_DIR,
    output: str | Path = DEFAULT_OUTPUT,
    *,
    log_decades: float = 7.0,
    stream_density: float = 1.15,
    dpi: int = 180,
    absolute_vmax: float | None = None,
) -> Path:
    """Render one log-scaled SEE panel for the full Cartesian chamber."""
    if not np.isfinite(log_decades) or log_decades <= 0.0:
        raise ValueError("log_decades must be positive and finite")
    if not np.isfinite(stream_density) or stream_density <= 0.0:
        raise ValueError("stream_density must be positive and finite")
    if dpi <= 0:
        raise ValueError("dpi must be positive")

    table_path = Path(table_dir)
    x, z, bx_t = read_moose_table(table_path / "Bx_T.tbl")
    x_bz, z_bz, bz_t = read_moose_table(table_path / "By_T.tbl")
    x_see, z_see, see_weight = read_moose_table(
        table_path / "see_spatial_weight_m-1.tbl"
    )
    _require_shared_axes(x, z, x_bz, z_bz, "By_T.tbl")
    _require_shared_axes(x, z, x_see, z_see, "see_spatial_weight_m-1.tbl")
    if np.any(~np.isfinite(bx_t)) or np.any(~np.isfinite(bz_t)):
        raise ValueError("B-field component tables must contain finite values")
    if np.any(~np.isfinite(see_weight)) or np.any(see_weight < 0.0):
        raise ValueError("SEE spatial weights must be finite and nonnegative")
    maximum_weight = float(np.max(see_weight))
    if maximum_weight <= 0.0:
        raise ValueError("SEE spatial-weight map must contain a positive value")
    color_scale_maximum = (
        maximum_weight if absolute_vmax is None else float(absolute_vmax)
    )
    if not np.isfinite(color_scale_maximum) or color_scale_maximum <= 0.0:
        raise ValueError("absolute_vmax must be positive and finite")

    metadata = _load_metadata(table_path)
    target_x_cm = 100.0 * np.asarray(metadata["powered_target_x_m"], dtype=float)
    launch_x_cm = 100.0 * np.asarray(
        metadata["launch_lobe_centers_m"], dtype=float
    )
    boundary_layout = metadata.get("boundary_layout_m", {})
    wafer_x_cm = 100.0 * np.asarray(
        boundary_layout.get("wafer_x", [0.07, 0.17]), dtype=float
    )
    settings = metadata.get("transport_settings", {})
    terminations = metadata.get("termination_counts", {})

    try:
        import matplotlib
    except ModuleNotFoundError as error:
        raise ModuleNotFoundError(
            "Matplotlib is required. Run with /opt/miniconda3/bin/python3 "
            "or install matplotlib in your Python environment."
        ) from error

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import LogNorm
    from matplotlib.lines import Line2D

    x_cm = 100.0 * x
    z_cm = 100.0 * z
    minimum_plotted_weight = color_scale_maximum * 10.0 ** (-float(log_decades))
    plotted_weight = np.maximum(see_weight, minimum_plotted_weight)
    normalization = LogNorm(
        vmin=minimum_plotted_weight, vmax=color_scale_maximum, clip=True
    )

    figure, axis = plt.subplots(figsize=(11.2, 8.2), constrained_layout=True)
    image = axis.pcolormesh(
        x_cm,
        z_cm,
        plotted_weight.T,
        shading="auto",
        cmap="magma",
        norm=normalization,
        rasterized=True,
    )
    axis.streamplot(
        x_cm,
        z_cm,
        bx_t.T,
        bz_t.T,
        color=(1.0, 1.0, 1.0, 0.70),
        density=float(stream_density),
        linewidth=0.75,
        arrowsize=0.75,
        broken_streamlines=True,
    )

    wall_color = "#c9d0da"
    target_color = "#ff315a"
    wafer_color = "#2ed8ff"
    domain_x_cm = [float(x_cm[0]), float(x_cm[-1])]
    domain_z_cm = [float(z_cm[0]), float(z_cm[-1])]
    axis.plot(domain_x_cm, [domain_z_cm[1]] * 2, color=wall_color, linewidth=2.6)
    axis.plot(domain_x_cm, [domain_z_cm[0]] * 2, color=wall_color, linewidth=2.6)
    axis.plot([domain_x_cm[0]] * 2, domain_z_cm, color=wall_color, linewidth=2.6)
    axis.plot([domain_x_cm[1]] * 2, domain_z_cm, color=wall_color, linewidth=2.6)
    axis.plot(
        target_x_cm,
        [domain_z_cm[1]] * 2,
        color=target_color,
        linewidth=6.0,
        solid_capstyle="butt",
        zorder=8,
    )
    axis.plot(
        wafer_x_cm,
        [domain_z_cm[0]] * 2,
        color=wafer_color,
        linewidth=6.0,
        solid_capstyle="butt",
        zorder=8,
    )
    lobe_z_cm = domain_z_cm[1] - 0.45
    axis.scatter(
        launch_x_cm,
        np.full(launch_x_cm.shape, lobe_z_cm),
        marker="v",
        s=68,
        facecolor=target_color,
        edgecolor="white",
        linewidth=0.9,
        zorder=9,
    )

    peak_index = np.unravel_index(int(np.argmax(see_weight)), see_weight.shape)
    peak_x_cm = float(x_cm[peak_index[0]])
    peak_z_cm = float(z_cm[peak_index[1]])
    axis.scatter(
        [peak_x_cm],
        [peak_z_cm],
        marker="*",
        s=115,
        facecolor="#fff36a",
        edgecolor="#191919",
        linewidth=0.8,
        zorder=10,
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
        Line2D(
            [0],
            [0],
            marker="v",
            linestyle="none",
            markerfacecolor=target_color,
            markeredgecolor="white",
            markersize=7,
            label=f"SEE launch lobes ({launch_x_cm[0]:g}, {launch_x_cm[1]:g} cm)",
        ),
        Line2D(
            [0],
            [0],
            marker="*",
            linestyle="none",
            markerfacecolor="#fff36a",
            markeredgecolor="#191919",
            markersize=9,
            label=f"Map peak ({peak_x_cm:.2f}, {peak_z_cm:.2f} cm)",
        ),
    ]
    axis.legend(handles=legend_handles, loc="lower left", framealpha=0.90, fontsize=8.3)

    see_integral_m = float(np.trapezoid(np.trapezoid(see_weight, z, axis=1), x))
    efficiency = float(metadata.get("plasma_efficiency", float("nan")))
    local_fraction = 100.0 * float(settings.get("local_fraction", float("nan")))
    local_length_mm = 1000.0 * float(
        settings.get("local_attenuation_length_m", float("nan"))
    )
    guided_length_cm = 100.0 * float(
        settings.get("guided_attenuation_length_m", float("nan"))
    )
    spread_degrees = float(settings.get("spread_angle_degrees", float("nan")))
    transition_mt = 1000.0 * float(
        settings.get("transition_field_t", float("nan"))
    )
    guidance_percent = 100.0 * float(
        settings.get("bfield_guidance_fraction", 1.0)
    )
    direction_model = metadata.get("see_transport_direction", "B-field guided")
    direction_text = _direction_label(direction_model)
    magnitude_scale = float(metadata.get("see_magnitude_scale", 1.0))
    termination_text = ", ".join(
        f"{name.replace('_', ' ')}={int(count)}"
        for name, count in sorted(terminations.items())
    ) or "not recorded"
    annotation = (
        rf"$\int w_{{SEE}}\,dA$ = {see_integral_m:.5g} m"
        f"\nPlasma efficiency = {efficiency:.4f}"
        f"\nSEE direction = {direction_text} (±{spread_degrees:g}° spread)"
        f"\nPrescribed magnitude scale = {magnitude_scale:g}×"
        f"\nTransport: {local_fraction:g}% local, {local_length_mm:g} mm local attenuation"
        f"\n{guided_length_cm:g} cm guided attenuation, "
        f"{transition_mt:g} mT transition"
        f"\nB-field directional guidance = {guidance_percent:g}%"
        f"\nRay terminations: {termination_text}"
    )
    axis.text(
        0.018,
        0.982,
        annotation,
        transform=axis.transAxes,
        ha="left",
        va="top",
        fontsize=8.0,
        color="white",
        bbox={
            "boxstyle": "round,pad=0.45",
            "facecolor": (0.05, 0.04, 0.08, 0.82),
            "edgecolor": (1.0, 1.0, 1.0, 0.45),
        },
        zorder=11,
    )

    axis.set_xlim(domain_x_cm)
    axis.set_ylim(domain_z_cm)
    axis.set_aspect("equal", adjustable="box")
    axis.set_xlabel("X (cm)")
    axis.set_ylabel("Z (cm)")
    axis.set_title(f"Cartesian X–Z {direction_text} SEE map with B streamlines")
    axis.grid(color="white", alpha=0.12, linewidth=0.45)

    colorbar = figure.colorbar(image, ax=axis, pad=0.025)
    colorbar.set_label(r"SEE spatial weight $w_{SEE}$ (m$^{-1}$)")

    output_path = Path(output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_path, dpi=int(dpi), facecolor="white")
    plt.close(figure)
    return output_path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--table-dir", type=Path, default=DEFAULT_TABLE_DIR)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--log-decades", type=float, default=7.0)
    parser.add_argument("--stream-density", type=float, default=1.15)
    parser.add_argument("--dpi", type=int, default=180)
    parser.add_argument("--absolute-vmax", type=float)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    output = plot_cartesian_see_map(
        args.table_dir,
        args.output,
        log_decades=args.log_decades,
        stream_density=args.stream_density,
        dpi=args.dpi,
        absolute_vmax=args.absolute_vmax,
    )
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
