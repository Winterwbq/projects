#!/usr/bin/env python3
"""Plot common- and case-fraction real-B field-line SEE source maps."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np

from generate_real_b_fieldline_see_source_maps import (
    read_moose_table,
    rz_volume_integral,
)


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_COMMON_ROOT = (
    ROOT
    / "runs/zapdos_real_b_fieldline_see_30cm_common_fraction_newsourceB_two_lobe"
)
DEFAULT_CASE_ROOT = (
    ROOT / "runs/zapdos_real_b_fieldline_see_30cm_case_fraction_newsourceB_two_lobe"
)
DEFAULT_OUTPUT = ROOT / "post/real_b_fieldline_see_source_maps_comparison.png"
CASES = (
    ("source_only", "Source only"),
    ("four_coil", "Four coil"),
    ("four_coil_img3092", "Image-3092 four coil"),
)
MODE_TITLES = {
    "common": r"Common fraction: $f_{\mathrm{local}}=0.65$",
    "case": "Case-specific local fractions",
}


def source_shape_metrics(
    r: np.ndarray,
    z: np.ndarray,
    weight: np.ndarray,
    *,
    center_radius: float = 0.15,
) -> dict[str, float]:
    """Return compact diagnostics for the lobe bridge and axial column."""
    r = np.asarray(r, dtype=float)
    z = np.asarray(z, dtype=float)
    weight = np.asarray(weight, dtype=float)
    if weight.shape != (r.size, z.size):
        raise ValueError("weight must match the R-Z grid")
    if np.any(weight < 0.0) or not np.all(np.isfinite(weight)):
        raise ValueError("weight must be finite and nonnegative")
    peak = np.unravel_index(np.argmax(weight), weight.shape)
    peak_value = float(weight[peak])
    if peak_value <= 0.0:
        raise ValueError("weight must contain a positive value")
    center = int(np.argmin(np.abs(r - center_radius)))
    return {
        "bridge_ratio": float(weight[center, peak[1]] / peak_value),
        "wafer_column_ratio": float(weight[center, 0] / peak_value),
        "peak_r": float(r[peak[0]]),
        "peak_z": float(z[peak[1]]),
    }


def load_mode_maps(
    mode: str, table_root: Path
) -> list[tuple[str, np.ndarray, np.ndarray, np.ndarray]]:
    """Load and validate the three maps belonging to one fraction mode."""
    if mode not in MODE_TITLES:
        raise ValueError("mode must be 'common' or 'case'")
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
            raise ValueError(f"{mode} {title} does not share the source-only grid")
    return loaded


def plot_maps(
    *,
    fraction_mode: str,
    common_root: Path,
    case_root: Path,
    output: Path,
    log_decades: float = 6.0,
    chamber_radius_cm: float = 25.0,
    chamber_height_cm: float = 30.0,
    wafer_radius_cm: float = 15.0,
) -> None:
    """Plot one or both fraction modes with one shared logarithmic color scale."""
    import matplotlib.pyplot as plt
    from matplotlib.colors import LogNorm

    if fraction_mode not in ("common", "case", "both"):
        raise ValueError("fraction_mode must be 'common', 'case', or 'both'")
    if log_decades <= 0.0 or not np.isfinite(log_decades):
        raise ValueError("log_decades must be positive and finite")
    modes = ("common", "case") if fraction_mode == "both" else (fraction_mode,)
    roots = {"common": common_root, "case": case_root}
    loaded = {mode: load_mode_maps(mode, roots[mode]) for mode in modes}

    reference_r = loaded[modes[0]][0][1]
    reference_z = loaded[modes[0]][0][2]
    for mode in modes[1:]:
        r = loaded[mode][0][1]
        z = loaded[mode][0][2]
        if not np.array_equal(r, reference_r) or not np.array_equal(z, reference_z):
            raise ValueError("common and case modes do not share the same R-Z grid")

    vmax = max(
        float(np.max(weight))
        for mode in modes
        for _, _, _, weight in loaded[mode]
    )
    if vmax <= 0.0:
        raise ValueError("all selected SEE source maps are zero")
    vmin = vmax * 10.0 ** (-log_decades)
    norm = LogNorm(vmin=vmin, vmax=vmax)

    nrows = len(modes)
    fig, axes = plt.subplots(
        nrows,
        len(CASES),
        figsize=(15.8, 5.2 * nrows),
        squeeze=False,
        constrained_layout=True,
    )
    image = None
    target_area = np.pi * (chamber_radius_cm / 100.0) ** 2
    for row, mode in enumerate(modes):
        for column, (ax, (title, r, z, weight)) in enumerate(
            zip(axes[row], loaded[mode])
        ):
            image = ax.pcolormesh(
                r * 100.0,
                z * 100.0,
                np.maximum(weight.T, vmin),
                shading="auto",
                cmap="magma",
                norm=norm,
            )
            metrics = source_shape_metrics(
                r,
                z,
                weight,
                center_radius=wafer_radius_cm / 100.0,
            )
            peak_r_cm = metrics["peak_r"] * 100.0
            peak_z_cm = metrics["peak_z"] * 100.0
            efficiency = rz_volume_integral(r, z, weight) / target_area
            ax.plot(
                peak_r_cm,
                peak_z_cm,
                marker="o",
                markersize=5,
                markerfacecolor="white",
                markeredgecolor="black",
                linestyle="none",
                label="source maximum",
            )
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
            ax.axvline(
                wafer_radius_cm,
                color="white",
                linewidth=1.0,
                linestyle="--",
                alpha=0.65,
                label="source/wafer center",
            )
            ax.set_title(
                f"{title}\n"
                rf"$\eta_{{\mathrm{{plasma}}}}={efficiency:.4f}$; "
                f"peak=({peak_r_cm:.2f}, {peak_z_cm:.2f}) cm\n"
                rf"bridge/peak={metrics['bridge_ratio']:.3f}; "
                rf"wafer column/peak={metrics['wafer_column_ratio']:.2e}"
            )
            ax.set_xlabel("R [cm]")
            ax.set_ylabel("Z [cm]")
            ax.set_xlim(0.0, chamber_radius_cm)
            ax.set_ylim(0.0, chamber_height_cm)
            ax.set_aspect("equal", adjustable="box")
            if column == 0:
                ax.text(
                    -0.26,
                    0.5,
                    MODE_TITLES[mode],
                    transform=ax.transAxes,
                    rotation=90,
                    va="center",
                    ha="center",
                    fontsize=12,
                    fontweight="bold",
                )
                ax.legend(loc="lower right", fontsize=8)

    if image is None:
        raise RuntimeError("no SEE source maps were plotted")
    fig.colorbar(
        image,
        ax=axes,
        pad=0.02,
        shrink=0.92,
        label=r"SEE spatial weight $W$ [m$^{-1}$]",
    )
    fig.suptitle(
        "Real-B field-line SEE spatial-weight maps",
        fontsize=16,
        fontweight="bold",
    )
    output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output, dpi=220)
    plt.close(fig)
    mode_description = "common and case" if fraction_mode == "both" else fraction_mode
    print(f"Wrote {mode_description} SEE source-map comparison: {output}")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--fraction-mode", choices=("common", "case", "both"), default="both"
    )
    parser.add_argument("--common-root", type=Path, default=DEFAULT_COMMON_ROOT)
    parser.add_argument("--case-root", type=Path, default=DEFAULT_CASE_ROOT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--log-decades", type=float, default=6.0)
    parser.add_argument("--chamber-radius-cm", type=float, default=25.0)
    parser.add_argument("--chamber-height-cm", type=float, default=30.0)
    parser.add_argument("--wafer-radius-cm", type=float, default=15.0)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    plot_maps(
        fraction_mode=args.fraction_mode,
        common_root=args.common_root,
        case_root=args.case_root,
        output=args.output,
        log_decades=args.log_decades,
        chamber_radius_cm=args.chamber_radius_cm,
        chamber_height_cm=args.chamber_height_cm,
        wafer_radius_cm=args.wafer_radius_cm,
    )


if __name__ == "__main__":
    main()
