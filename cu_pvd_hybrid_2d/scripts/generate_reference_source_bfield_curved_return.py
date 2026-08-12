#!/usr/bin/env python3
"""Generate a dipole-loop variant of the source-only B field.

The approved source-only generator and its tables remain unchanged.  This
variant translates the original magnitude profile from R=125 mm to R=150 mm
without changing its shape or range, and uses a curved dipole topology about
the same R=150 mm center.  Model Z=300 mm is mapped to the reference image's
magnet underside near Z=235 mm, placing the virtual dipole center above the
model domain at Z=325 mm.

MOOSE/Zapdos component convention:
    Bx_T.tbl = B_R [T]
    By_T.tbl = B_Z [T]
"""
from __future__ import annotations

import argparse
from pathlib import Path
import sys

import numpy as np


SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from generate_reference_source_bfield import (  # noqa: E402
    DEFAULT_OUT_DIR as ORIGINAL_OUT_DIR,
    DEFAULT_PLOT_PATH as ORIGINAL_PLOT_PATH,
    GAUSS_TO_TESLA,
    REFERENCE_LEVELS_G,
    build_source_bfield,
    write_moose_table,
)


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_source_only_curved_return"
    / "moose_tables"
)
DEFAULT_PLOT_PATH = (
    ROOT / "post" / "reference_source_only_bfield_curved_return_comparison_30cm.png"
)


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 3:
        raise ValueError(f"{name} must be one-dimensional with at least three points")
    if not np.all(np.isfinite(axis)) or np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must contain finite, strictly increasing values")
    return axis


def _dipole_loop_direction(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    magnet_r_m: float,
    magnet_z_m: float,
) -> tuple[np.ndarray, np.ndarray]:
    """Return a normalized 2-D meridional dipole-loop direction.

    The dipole moment points in +Z.  Directly below the magnet the field points
    upward; farther to either side it turns downward to form the return branch.
    ``B_R`` is tapered to zero on R=0 before normalization so the generated
    table obeys the axisymmetric regularity condition without changing |B|.
    """
    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    magnet_r = float(magnet_r_m)
    magnet_z = float(magnet_z_m)
    if not all(np.isfinite(value) for value in (magnet_r, magnet_z)):
        raise ValueError("dipole-center parameters must be finite")
    if not r[0] < magnet_r < r[-1]:
        raise ValueError("magnet_r_m must lie inside the radial domain")
    if magnet_z <= z[-1]:
        raise ValueError("magnet_z_m must lie behind the top target")

    rr, zz = np.meshgrid(r, z, indexing="ij")
    dr = rr - magnet_r
    dz = zz - magnet_z

    # Direction of an axial point dipole in its meridional plane.  The common
    # positive 1/rho**5 factor is deliberately omitted because this generator
    # imports |B| pointwise from the approved source-only magnitude map.
    br = 3.0 * dr * dz
    bz = 2.0 * dz * dz - dr * dr
    # Exact R=0 symmetry is required by the axisymmetric plasma mesh.
    br *= 1.0 - np.exp(-((rr / 0.003) ** 2))
    magnitude = np.hypot(br, bz)
    if not np.all(np.isfinite(magnitude)) or np.any(magnitude <= 0.0):
        raise ValueError("dipole-loop direction contains a zero field")
    return br / magnitude, bz / magnitude


def build_curved_return_source_bfield(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    source_r_m: float = 0.150,
    source_z_m: float = 0.300,
    min_b_t: float = 1.0 * GAUSS_TO_TESLA,
    max_b_t: float = 1000.0 * GAUSS_TO_TESLA,
    pole_separation_m: float = 0.040,
    pole_depth_m: float = 0.006,
    softening_m: float = 0.010,
    null_offset_m: float = 0.010,
    magnet_r_m: float = 0.150,
    magnet_z_m: float = 0.325,
    polarity: float = -1.0,
) -> tuple[np.ndarray, np.ndarray]:
    """Return a full-chamber dipole-loop direction with unchanged pointwise |B|."""
    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)

    original_br, original_bz = build_source_bfield(
        r,
        z,
        source_r_m=source_r_m,
        source_z_m=source_z_m,
        min_b_t=min_b_t,
        max_b_t=max_b_t,
        pole_separation_m=pole_separation_m,
        pole_depth_m=pole_depth_m,
        softening_m=softening_m,
        null_offset_m=null_offset_m,
        polarity=polarity,
    )
    direction_r, direction_z = _dipole_loop_direction(
        r,
        z,
        magnet_r_m=magnet_r_m,
        magnet_z_m=magnet_z_m,
    )

    original_magnitude = np.hypot(original_br, original_bz)
    br = original_magnitude * direction_r
    bz = original_magnitude * direction_z
    return br, bz


def plot_curved_return_comparison(
    path: str | Path,
    r_m: np.ndarray,
    z_m: np.ndarray,
    original_br_t: np.ndarray,
    original_bz_t: np.ndarray,
    curved_br_t: np.ndarray,
    curved_bz_t: np.ndarray,
    *,
    plasma_radius_m: float = 0.250,
) -> None:
    """Plot original and curved-return field lines on their common magnitude map."""
    try:
        import matplotlib
    except ModuleNotFoundError:
        _plot_curved_return_comparison_pillow(
            path,
            r_m,
            z_m,
            original_br_t,
            original_bz_t,
            curved_br_t,
            curved_bz_t,
            plasma_radius_m=plasma_radius_m,
        )
        return

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import BoundaryNorm

    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    expected_shape = (r.size, z.size)
    fields = (original_br_t, original_bz_t, curved_br_t, curved_bz_t)
    if any(np.asarray(field).shape != expected_shape for field in fields):
        raise ValueError(f"all field components must have shape {expected_shape}")
    if any(not np.all(np.isfinite(field)) for field in fields):
        raise ValueError("field components must contain only finite values")

    r_mm = 1000.0 * r
    z_mm = 1000.0 * z
    magnitude_g = np.clip(
        np.hypot(curved_br_t, curved_bz_t) / GAUSS_TO_TESLA,
        REFERENCE_LEVELS_G[0],
        REFERENCE_LEVELS_G[-1],
    )
    colormap = plt.colormaps["turbo"].resampled(REFERENCE_LEVELS_G.size - 1)
    normalization = BoundaryNorm(REFERENCE_LEVELS_G, colormap.N, clip=True)
    figure, axes = plt.subplots(1, 2, figsize=(13.0, 7.2), constrained_layout=True)
    last_filled = None
    for axis, title, br, bz in (
        (axes[0], "Original source field", original_br_t, original_bz_t),
        (axes[1], "R=150 mm dipole-loop variant", curved_br_t, curved_bz_t),
    ):
        last_filled = axis.contourf(
            r_mm,
            z_mm,
            magnitude_g.T,
            levels=REFERENCE_LEVELS_G,
            cmap=colormap,
            norm=normalization,
        )
        axis.streamplot(
            r_mm,
            z_mm,
            np.asarray(br).T,
            np.asarray(bz).T,
            color="white",
            density=(1.55, 1.45),
            linewidth=0.85,
            arrowsize=0.7,
            broken_streamlines=True,
        )
        axis.axvline(1000.0 * plasma_radius_m, color="#ff2d2d", linewidth=1.8)
        axis.set_xlim(0.0, 1000.0 * plasma_radius_m)
        axis.set_ylim(0.0, 300.0)
        axis.set_aspect("equal", adjustable="box")
        axis.set_xlabel("R (mm)")
        axis.set_title(title)
    axes[0].set_ylabel("Z (mm)")
    figure.suptitle("Source B-field topology comparison; identical |B| map")
    assert last_filled is not None
    colorbar = figure.colorbar(last_filled, ax=axes, ticks=REFERENCE_LEVELS_G, pad=0.02)
    colorbar.set_label("|B| (G)")
    output_path = Path(path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_path, dpi=180)
    plt.close(figure)


def _plot_curved_return_comparison_pillow(
    path: str | Path,
    r_m: np.ndarray,
    z_m: np.ndarray,
    original_br_t: np.ndarray,
    original_bz_t: np.ndarray,
    curved_br_t: np.ndarray,
    curved_bz_t: np.ndarray,
    *,
    plasma_radius_m: float,
) -> None:
    """Dependency-light fallback that still renders field-line comparisons."""
    from PIL import Image, ImageDraw, ImageFont

    r = np.asarray(r_m, dtype=float)
    z = np.asarray(z_m, dtype=float)
    full_height = z >= 0.0
    radial = r <= plasma_radius_m
    magnitude_g = np.hypot(curved_br_t, curved_bz_t) / GAUSS_TO_TESLA
    log_magnitude = np.log10(
        np.clip(magnitude_g[np.ix_(radial, full_height)], 1.0, 1000.0)
    )
    normalized = np.clip(log_magnitude / 3.0, 0.0, 1.0)
    color_stops = np.asarray(
        [
            [20, 25, 100],
            [20, 150, 200],
            [45, 190, 95],
            [245, 220, 45],
            [235, 95, 30],
            [120, 10, 15],
        ],
        dtype=float,
    )
    scaled = normalized * (color_stops.shape[0] - 1)
    lower_i = np.floor(scaled).astype(int)
    upper_i = np.minimum(lower_i + 1, color_stops.shape[0] - 1)
    fraction = (scaled - lower_i)[..., None]
    rgb = (
        (1.0 - fraction) * color_stops[lower_i]
        + fraction * color_stops[upper_i]
    ).astype(np.uint8)
    heatmap = Image.fromarray(np.flipud(np.swapaxes(rgb, 0, 1)))

    canvas_width = 1120
    canvas_height = 760
    panel_width = 450
    panel_height = 540
    panel_top = 120
    panel_lefts = (80, 590)
    canvas = Image.new("RGB", (canvas_width, canvas_height), "white")
    draw = ImageDraw.Draw(canvas)
    font = ImageFont.load_default()
    draw.text(
        (canvas_width // 2, 26),
        "Source B-field topology comparison; identical |B| map",
        fill="black",
        font=font,
        anchor="ma",
    )

    def interpolate(values: np.ndarray, point: np.ndarray) -> float:
        radial_values = np.asarray(
            [np.interp(point[0], r, values[:, axial_i]) for axial_i in range(z.size)]
        )
        return float(np.interp(point[1], z, radial_values))

    def trace(br: np.ndarray, bz: np.ndarray, launch_r: float) -> list[np.ndarray]:
        point = np.asarray([launch_r, 0.2995], dtype=float)
        initial = np.asarray([interpolate(br, point), interpolate(bz, point)])
        sign = -1.0 if initial[1] > 0.0 else 1.0
        points = [point.copy()]
        step = 4.0e-4
        for _ in range(700):
            field = sign * np.asarray([interpolate(br, point), interpolate(bz, point)])
            norm = float(np.linalg.norm(field))
            if norm == 0.0:
                break
            midpoint = point + 0.5 * step * field / norm
            if not (0.0 <= midpoint[0] <= plasma_radius_m and 0.0 <= midpoint[1] <= 0.3):
                break
            middle_field = sign * np.asarray(
                [interpolate(br, midpoint), interpolate(bz, midpoint)]
            )
            middle_norm = float(np.linalg.norm(middle_field))
            if middle_norm == 0.0:
                break
            point = point + step * middle_field / middle_norm
            points.append(point.copy())
            if point[1] >= 0.3 or point[1] <= 0.0 or not 0.0 <= point[0] <= plasma_radius_m:
                break
        return points

    def pixel(point: np.ndarray, left: int) -> tuple[int, int]:
        x_pixel = left + int(panel_width * point[0] / plasma_radius_m)
        y_pixel = panel_top + int(panel_height * (0.300 - point[1]) / 0.300)
        return x_pixel, y_pixel

    for left, title, br, bz in (
        (panel_lefts[0], "Original source field", original_br_t, original_bz_t),
        (panel_lefts[1], "R=150 mm dipole-loop variant", curved_br_t, curved_bz_t),
    ):
        resized = heatmap.resize((panel_width, panel_height), Image.Resampling.BILINEAR)
        canvas.paste(resized, (left, panel_top))
        draw.rectangle(
            (left, panel_top, left + panel_width, panel_top + panel_height),
            outline="black",
            width=2,
        )
        draw.text(
            (left + panel_width // 2, panel_top - 25),
            title,
            fill="black",
            font=font,
            anchor="ma",
        )
        for launch_r in np.linspace(0.010, plasma_radius_m - 0.010, 38):
            points = trace(np.asarray(br), np.asarray(bz), float(launch_r))
            if len(points) >= 2:
                draw.line([pixel(point, left) for point in points], fill="white", width=1)
        draw.text(
            (left + panel_width // 2, panel_top + panel_height + 28),
            f"R = 0..{1000.0 * plasma_radius_m:.0f} mm",
            fill="black",
            font=font,
            anchor="ma",
        )
    draw.text(
        (25, panel_top + panel_height // 2),
        f"Z = 0..{1000.0 * z[-1]:.0f} mm",
        fill="black",
        font=font,
        anchor="mm",
    )
    output_path = Path(path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    canvas.save(output_path)


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate a separate curved, target-returning source B-field variant."
    )
    parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    parser.add_argument("--plot-path", type=Path, default=DEFAULT_PLOT_PATH)
    parser.add_argument("--nx", type=int, default=161)
    parser.add_argument("--ny", type=int, default=301)
    parser.add_argument("--r-max-mm", type=float, default=400.0)
    parser.add_argument("--z-max-mm", type=float, default=300.0)
    parser.add_argument("--plasma-radius-mm", type=float, default=250.0)
    parser.add_argument("--source-r-mm", type=float, default=150.0)
    parser.add_argument("--min-gauss", type=float, default=1.0)
    parser.add_argument("--max-gauss", type=float, default=1000.0)
    parser.add_argument("--pole-separation-mm", type=float, default=40.0)
    parser.add_argument("--pole-depth-mm", type=float, default=6.0)
    parser.add_argument("--softening-mm", type=float, default=10.0)
    parser.add_argument("--null-offset-mm", type=float, default=10.0)
    parser.add_argument("--magnet-r-mm", type=float, default=150.0)
    parser.add_argument("--magnet-z-mm", type=float, default=325.0)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    if args.nx < 3 or args.ny < 3:
        raise ValueError("--nx and --ny must each be at least 3")
    r_max_m = 1.0e-3 * float(args.r_max_mm)
    z_max_m = 1.0e-3 * float(args.z_max_mm)
    plasma_radius_m = 1.0e-3 * float(args.plasma_radius_mm)
    if r_max_m <= 0.0 or z_max_m <= 0.0:
        raise ValueError("domain dimensions must be positive")
    if not 0.0 < plasma_radius_m < r_max_m:
        raise ValueError("--plasma-radius-mm must lie inside the radial domain")

    r_m = np.linspace(0.0, r_max_m, args.nx)
    z_m = np.linspace(0.0, z_max_m, args.ny)
    common = dict(
        source_r_m=1.0e-3 * float(args.source_r_mm),
        source_z_m=z_max_m,
        min_b_t=float(args.min_gauss) * GAUSS_TO_TESLA,
        max_b_t=float(args.max_gauss) * GAUSS_TO_TESLA,
        pole_separation_m=1.0e-3 * float(args.pole_separation_mm),
        pole_depth_m=1.0e-3 * float(args.pole_depth_mm),
        softening_m=1.0e-3 * float(args.softening_mm),
        null_offset_m=1.0e-3 * float(args.null_offset_mm),
    )
    original_br, original_bz = build_source_bfield(r_m, z_m, **common)
    curved_br, curved_bz = build_curved_return_source_bfield(
        r_m,
        z_m,
        **common,
        magnet_r_m=1.0e-3 * float(args.magnet_r_mm),
        magnet_z_m=1.0e-3 * float(args.magnet_z_mm),
    )
    bx_path = args.out_dir / "Bx_T.tbl"
    by_path = args.out_dir / "By_T.tbl"
    write_moose_table(bx_path, r_m, z_m, curved_br)
    write_moose_table(by_path, r_m, z_m, curved_bz)
    plot_curved_return_comparison(
        args.plot_path,
        r_m,
        z_m,
        original_br,
        original_bz,
        curved_br,
        curved_bz,
        plasma_radius_m=plasma_radius_m,
    )
    magnitude_g = np.hypot(curved_br, curved_bz) / GAUSS_TO_TESLA
    print(f"wrote curved-return radial field table: {bx_path}")
    print(f"wrote curved-return axial field table: {by_path}")
    print(f"wrote original/curved comparison: {args.plot_path}")
    print(f"|B| range preserved: {magnitude_g.min():.2f}..{magnitude_g.max():.2f} G")
    print(
        "dipole-loop center: "
        f"(R={float(args.magnet_r_mm):.1f}, Z={float(args.magnet_z_mm):.1f}) mm; "
        "model Z=300 mm maps to the reference magnet underside near Z=235 mm"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
