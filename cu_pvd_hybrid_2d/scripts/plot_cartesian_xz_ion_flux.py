#!/usr/bin/env python3
"""Plot Cartesian X-Z Cu+ flux maps and wafer-span profiles from Exodus."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from plot_cu_ion_flux_profiles import (  # noqa: E402
    load_final_exodus,
    reconstruct_ion_flux,
)


DEFAULT_EXODUS = (
    ROOT
    / "zapdos_templates"
    / "Outputs"
    / "cu_pvd_hybrid_hpem_xz_30cm_newsourceB_guidedSEE_1.e"
)
DEFAULT_OUTPUT = ROOT / "post" / "cartesian_xz_cu_ion_flux.png"
DEFAULT_Z_PLANES_CM = (1.0, 5.0, 10.0)
DEFAULT_WAFER_X_CM = (7.0, 17.0)
DEFAULT_TARGET_X_CM = (19.0, 22.0)


def select_wafer_profiles(
    flux: dict[str, np.ndarray],
    *,
    z_planes_cm: list[float] | tuple[float, ...] = DEFAULT_Z_PLANES_CM,
    wafer_x_cm: tuple[float, float] = DEFAULT_WAFER_X_CM,
) -> dict[float, dict[str, np.ndarray | float]]:
    """Select nearest element rows and retain only X positions above the wafer."""
    requested_planes = [float(value) for value in z_planes_cm]
    if not requested_planes or np.any(~np.isfinite(requested_planes)):
        raise ValueError("z_planes_cm must contain finite values")
    wafer_min_cm, wafer_max_cm = map(float, wafer_x_cm)
    if not np.isfinite([wafer_min_cm, wafer_max_cm]).all() or not (
        wafer_min_cm < wafer_max_cm
    ):
        raise ValueError("wafer_x_cm must be a finite increasing pair")

    x = np.asarray(flux["r"], dtype=float)
    z = np.asarray(flux["z"], dtype=float)
    if x.ndim != 1 or z.shape != x.shape or x.size == 0:
        raise ValueError("flux X and Z coordinates must be nonempty matching arrays")
    axial_rows = np.unique(np.round(z, 12))

    profiles: dict[float, dict[str, np.ndarray | float]] = {}
    for requested_cm in requested_planes:
        requested_m = requested_cm / 100.0
        actual_z = float(axial_rows[np.argmin(np.abs(axial_rows - requested_m))])
        row_mask = np.isclose(z, actual_z, atol=1.0e-11, rtol=0.0)
        wafer_mask = (x >= wafer_min_cm / 100.0 - 1.0e-12) & (
            x <= wafer_max_cm / 100.0 + 1.0e-12
        )
        selected = row_mask & wafer_mask
        if not np.any(selected):
            raise ValueError(
                f"no flux samples found at z={actual_z * 100.0:g} cm "
                f"over X={wafer_min_cm:g}-{wafer_max_cm:g} cm"
            )
        indices = np.flatnonzero(selected)
        indices = indices[np.argsort(x[indices])]
        profile: dict[str, np.ndarray | float] = {
            "actual_z": actual_z,
            "x": x[indices],
        }
        for name, values in flux.items():
            array = np.asarray(values)
            if array.shape == x.shape:
                profile[name] = array[indices]
        profiles[requested_cm] = profile
    return profiles


def _positive_percentile_limits(values: np.ndarray) -> tuple[float, float]:
    positive = values[np.isfinite(values) & (values > 0.0)]
    if positive.size == 0:
        raise ValueError("Cu+ flux magnitude has no positive finite values")
    low, high = np.percentile(positive, (1.0, 99.0))
    low = float(low)
    high = float(high)
    if low >= high:
        low = max(high * 0.1, 1.0e-300)
        high = max(high, low * 10.0)
    return low, high


def _signed_limit(values: np.ndarray) -> float:
    finite = np.abs(values[np.isfinite(values)])
    if finite.size == 0:
        raise ValueError("signed Cu+ flux has no finite values")
    limit = float(np.percentile(finite, 99.0))
    return limit if limit > 0.0 else 1.0


def _direction_sample_mask(
    center_x: np.ndarray,
    center_z: np.ndarray,
    *,
    target_columns: int = 17,
    target_rows: int = 20,
) -> np.ndarray:
    x_rows = np.unique(np.round(center_x, 12))
    z_rows = np.unique(np.round(center_z, 12))
    x_index = np.searchsorted(x_rows, np.round(center_x, 12))
    z_index = np.searchsorted(z_rows, np.round(center_z, 12))
    x_stride = max(1, len(x_rows) // target_columns)
    z_stride = max(1, len(z_rows) // target_rows)
    return (x_index % x_stride == x_stride // 2) & (
        z_index % z_stride == z_stride // 2
    )


def _annotate_geometry(axis, domain_x_cm: tuple[float, float], domain_z_cm: tuple[float, float]) -> None:
    wall_color = "#d5d9e0"
    target_color = "#ff315a"
    wafer_color = "#28d9ff"
    xmin, xmax = domain_x_cm
    zmin, zmax = domain_z_cm
    axis.plot([xmin, xmax], [zmin, zmin], color=wall_color, linewidth=2.4)
    axis.plot([xmin, xmax], [zmax, zmax], color=wall_color, linewidth=2.4)
    axis.plot([xmin, xmin], [zmin, zmax], color=wall_color, linewidth=2.4)
    axis.plot([xmax, xmax], [zmin, zmax], color=wall_color, linewidth=2.4)
    axis.plot(
        DEFAULT_WAFER_X_CM,
        [zmin, zmin],
        color=wafer_color,
        linewidth=6.0,
        solid_capstyle="butt",
        zorder=8,
    )
    axis.plot(
        DEFAULT_TARGET_X_CM,
        [zmax, zmax],
        color=target_color,
        linewidth=6.0,
        solid_capstyle="butt",
        zorder=8,
    )


def plot_cartesian_ion_flux(
    exodus_path: str | Path = DEFAULT_EXODUS,
    output_path: str | Path = DEFAULT_OUTPUT,
    *,
    z_planes_cm: list[float] | tuple[float, ...] = DEFAULT_Z_PLANES_CM,
    wafer_x_cm: tuple[float, float] = DEFAULT_WAFER_X_CM,
    target_time: float | None = None,
    dpi: int = 180,
) -> Path:
    """Render magnitude/direction, wafer-directed flux, and X profiles."""
    if dpi <= 0:
        raise ValueError("dpi must be positive")
    exodus = Path(exodus_path)
    if not exodus.is_file():
        raise FileNotFoundError(f"Exodus result not found: {exodus}")

    data = load_final_exodus(exodus, target_time)
    flux = reconstruct_ion_flux(data)
    profiles = select_wafer_profiles(
        flux, z_planes_cm=z_planes_cm, wafer_x_cm=wafer_x_cm
    )

    try:
        import matplotlib
    except ModuleNotFoundError as error:
        raise ModuleNotFoundError(
            "Matplotlib is required. Run with /opt/miniconda3/bin/python3 "
            "or install matplotlib in your Python environment."
        ) from error

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import LogNorm, SymLogNorm
    import matplotlib.tri as mtri

    x_cm = np.asarray(data["x"]) * 100.0
    z_cm = np.asarray(data["z"]) * 100.0
    connect = np.asarray(data["connect"])
    triangles = np.vstack((connect[:, [0, 1, 2]], connect[:, [0, 2, 3]]))
    triangulation = mtri.Triangulation(x_cm, z_cm, triangles)
    magnitude = np.asarray(flux["magnitude"])
    toward_wafer = np.asarray(flux["gamma_to_wafer"])
    magnitude_limits = _positive_percentile_limits(magnitude)
    magnitude_norm = LogNorm(vmin=magnitude_limits[0], vmax=magnitude_limits[1])
    signed_limit = _signed_limit(toward_wafer)
    signed_norm = SymLogNorm(
        linthresh=max(signed_limit * 0.01, 1.0),
        linscale=1.0,
        vmin=-signed_limit,
        vmax=signed_limit,
    )

    figure = plt.figure(figsize=(13.5, 12.0), constrained_layout=True)
    grid = figure.add_gridspec(2, 2, height_ratios=(1.55, 1.0))
    magnitude_axis = figure.add_subplot(grid[0, 0])
    signed_axis = figure.add_subplot(grid[0, 1])
    profile_axis = figure.add_subplot(grid[1, :])

    magnitude_plot = magnitude_axis.tripcolor(
        triangulation,
        facecolors=np.concatenate((magnitude, magnitude)),
        shading="flat",
        cmap="viridis",
        norm=magnitude_norm,
        rasterized=True,
    )
    signed_plot = signed_axis.tripcolor(
        triangulation,
        facecolors=np.concatenate((toward_wafer, toward_wafer)),
        shading="flat",
        cmap="coolwarm",
        norm=signed_norm,
        rasterized=True,
    )

    center_x = np.asarray(flux["r"])
    center_z = np.asarray(flux["z"])
    arrow_mask = _direction_sample_mask(center_x, center_z)
    safe_magnitude = np.maximum(magnitude, 1.0e-300)
    magnitude_axis.quiver(
        center_x[arrow_mask] * 100.0,
        center_z[arrow_mask] * 100.0,
        np.asarray(flux["gamma_r"])[arrow_mask] / safe_magnitude[arrow_mask],
        np.asarray(flux["gamma_z"])[arrow_mask] / safe_magnitude[arrow_mask],
        color="white",
        angles="xy",
        scale_units="xy",
        scale=1.15,
        width=0.0032,
        alpha=0.88,
        pivot="middle",
    )

    domain_x_cm = (float(np.min(x_cm)), float(np.max(x_cm)))
    domain_z_cm = (float(np.min(z_cm)), float(np.max(z_cm)))
    for axis in (magnitude_axis, signed_axis):
        _annotate_geometry(axis, domain_x_cm, domain_z_cm)
        axis.set_xlim(domain_x_cm)
        axis.set_ylim(domain_z_cm)
        axis.set_aspect("equal", adjustable="box")
        axis.set_xlabel("X (cm)")
        axis.set_ylabel("Z (cm)")
    magnitude_axis.set_title(r"Cu$^+$ flux magnitude; arrows show direction")
    signed_axis.set_title(r"Signed wafer-directed flux $-\Gamma_z$ (positive downward)")

    magnitude_colorbar = figure.colorbar(
        magnitude_plot, ax=magnitude_axis, pad=0.025, shrink=0.94
    )
    magnitude_colorbar.set_label(
        r"$|\boldsymbol{\Gamma}_{Cu^+}|$ (m$^{-2}$ s$^{-1}$)"
    )
    signed_colorbar = figure.colorbar(
        signed_plot, ax=signed_axis, pad=0.025, shrink=0.94
    )
    signed_colorbar.set_label(
        r"$-\Gamma_z$ (m$^{-2}$ s$^{-1}$; positive toward wafer)"
    )

    colors = plt.colormaps["plasma"](
        np.linspace(0.15, 0.85, max(len(profiles), 2))
    )
    for color, (requested_z_cm, profile) in zip(colors, profiles.items()):
        actual_z_cm = float(profile["actual_z"]) * 100.0
        profile_axis.semilogy(
            np.asarray(profile["x"]) * 100.0,
            np.asarray(profile["magnitude"]),
            color=color,
            linewidth=2.1,
            label=(
                f"requested Z={requested_z_cm:g} cm "
                f"(mesh Z={actual_z_cm:.3f} cm)"
            ),
        )
    profile_axis.axvspan(
        wafer_x_cm[0], wafer_x_cm[1], color="#28d9ff", alpha=0.08, zorder=0
    )
    profile_axis.set_xlim(wafer_x_cm)
    profile_axis.set_xlabel("X over wafer (cm)")
    profile_axis.set_ylabel(r"$|\boldsymbol{\Gamma}_{Cu^+}|$ (m$^{-2}$ s$^{-1}$)")
    profile_axis.set_title(r"Cu$^+$ flux-magnitude profiles above the wafer")
    profile_axis.grid(True, which="both", alpha=0.22)
    profile_axis.legend(frameon=False, ncol=min(3, len(profiles)))

    time_microseconds = float(data["time"]) * 1.0e6
    figure.suptitle(
        rf"Cartesian X–Z Cu$^+$ drift-diffusion flux at $t={time_microseconds:g}\ \mu$s",
        fontsize=15,
        fontweight="bold",
    )

    output = Path(output_path)
    output.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output, dpi=int(dpi), facecolor="white")
    plt.close(figure)

    print(f"Wrote {output}")
    print(f"Selected saved time: {float(data['time']):.9e} s")
    for requested_z_cm, profile in profiles.items():
        values = np.asarray(profile["magnitude"])
        print(
            f"Z={requested_z_cm:g} cm (mesh {float(profile['actual_z']) * 100.0:.6f} cm): "
            f"wafer-span |Gamma|=[{np.min(values):.6e}, {np.max(values):.6e}] m^-2 s^-1"
        )
    return output


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exodus", type=Path, default=DEFAULT_EXODUS)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--z-cm", type=float, nargs="+", default=DEFAULT_Z_PLANES_CM)
    parser.add_argument(
        "--wafer-x-cm",
        type=float,
        nargs=2,
        metavar=("XMIN", "XMAX"),
        default=DEFAULT_WAFER_X_CM,
    )
    parser.add_argument(
        "--time",
        type=float,
        default=None,
        help="Physical time in seconds; defaults to the last saved timestep.",
    )
    parser.add_argument("--dpi", type=int, default=180)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    plot_cartesian_ion_flux(
        args.exodus,
        args.output,
        z_planes_cm=args.z_cm,
        wafer_x_cm=tuple(args.wafer_x_cm),
        target_time=args.time,
        dpi=args.dpi,
    )


if __name__ == "__main__":
    main()
