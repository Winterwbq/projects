#!/usr/bin/env python3
"""Generate the scaled source-plus-four-coil R-Z magnetic-field map.

The model combines the approved analytical S-N-S source field with four
softened circular Biot-Savart loops.  It covers the full prescribed domain
R=0..400 mm and Z=0..300 mm while marking R=250 mm as the plasma boundary.

MOOSE/Zapdos component convention:
    Bx_T.tbl = B_R [T]
    By_T.tbl = B_Z [T]
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np

from generate_reference_source_bfield import (
    GAUSS_TO_TESLA,
    REFERENCE_LEVELS_G,
    build_source_bfield,
    write_moose_table,
)


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_four_coil"
    / "moose_tables"
)
DEFAULT_PLOT_PATH = ROOT / "post" / "reference_four_coil_bfield_30cm.png"

# Scaled positions read from IMG_3086.JPG.  The two 22.3 A upper coils are
# represented as one effective group with weight 44.6.
DEFAULT_COIL_RADIUS_M = 0.280
DEFAULT_COIL_Z_M = (0.250, 0.150, 0.055, 0.030)
DEFAULT_COIL_WEIGHTS = (44.6, 12.6, 3.2, -5.2)
DEFAULT_COIL_SOFTENING_M = 0.006
DEFAULT_COIL_QUADRATURE = 256
DEFAULT_COIL_PEAK_T = 227.58 * GAUSS_TO_TESLA

# Coil-only bulk guide strengths inferred from the horizontal color bands in
# IMG_3086.JPG after halving the original Z coordinate.  The upper guide is
# intentionally held in the 52--84 G band; a smooth elliptical dome below
# adds the photographed circular 85--139 G region over Z=200..275 mm.
REFERENCE_COIL_BULK_Z_M = 1.0e-3 * np.asarray(
    [0.0, 25.0, 50.0, 90.0, 135.0, 200.0, 275.0, 300.0],
    dtype=float,
)
REFERENCE_COIL_BULK_G = np.asarray(
    [3.40, 6.00, 10.30, 29.40, 48.60, 55.00, 55.00, 55.00],
    dtype=float,
)
UPPER_DOME_CENTER_R_M = 0.125
UPPER_DOME_CENTER_Z_M = 0.245
UPPER_DOME_SIGMA_R_M = 0.150
UPPER_DOME_SIGMA_Z_M = 0.055
UPPER_DOME_PEAK_G = 45.0

LOWER_VOID_CENTER_R_M = 0.150
LOWER_VOID_SIGMA_R_M = 0.075
LOWER_VOID_SIGMA_Z_M = 0.018
LOWER_VOID_DEPTH = 0.90

LOWER_STEERING_CENTER_R_M = 0.150
LOWER_STEERING_SIGMA_R_M = 0.075
LOWER_STEERING_DECAY_Z_M = 0.080
LOWER_STEERING_GAIN = 1.80


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    """Return a validated, strictly increasing one-dimensional axis."""
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(
            f"{name} must be a one-dimensional array with at least two points"
        )
    if not np.all(np.isfinite(axis)):
        raise ValueError(f"{name} must contain only finite values")
    if np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must be strictly increasing")
    return axis


def _smooth_interpolate(
    values: np.ndarray,
    control_x: np.ndarray,
    control_y: np.ndarray,
) -> np.ndarray:
    """Interpolate control points with C1 smoothstep segments."""
    query = np.asarray(values, dtype=float)
    x = np.asarray(control_x, dtype=float)
    y = np.asarray(control_y, dtype=float)
    if x.ndim != 1 or y.shape != x.shape or x.size < 2:
        raise ValueError("smooth interpolation controls must be matching 1-D arrays")
    if np.any(np.diff(x) <= 0.0):
        raise ValueError("smooth interpolation coordinates must increase")

    segment = np.searchsorted(x, query, side="right") - 1
    segment = np.clip(segment, 0, x.size - 2)
    x0 = x[segment]
    x1 = x[segment + 1]
    fraction = np.clip((query - x0) / (x1 - x0), 0.0, 1.0)
    smooth_fraction = fraction * fraction * (3.0 - 2.0 * fraction)
    return y[segment] + smooth_fraction * (y[segment + 1] - y[segment])


def build_four_coil_bfield(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    coil_radius_m: float = DEFAULT_COIL_RADIUS_M,
    coil_z_m: tuple[float, ...] = DEFAULT_COIL_Z_M,
    coil_weights: tuple[float, ...] = DEFAULT_COIL_WEIGHTS,
    softening_m: float = DEFAULT_COIL_SOFTENING_M,
    quadrature: int = DEFAULT_COIL_QUADRATURE,
    target_peak_t: float = DEFAULT_COIL_PEAK_T,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the analytical four-coil ``(B_R, B_Z)`` field in tesla.

    Each coil is an axisymmetric circular current loop.  The azimuthal
    Biot-Savart integral is evaluated by periodic midpoint quadrature.  A
    finite softening length represents the conductor cross-section and avoids
    a singular value at the loop center.  The common current and mu_0/(4*pi)
    factors are absorbed into one scale calibrated near the upper coil.
    """
    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    coil_radius = float(coil_radius_m)
    centers = np.asarray(coil_z_m, dtype=float)
    weights = np.asarray(coil_weights, dtype=float)
    softening = float(softening_m)
    number_of_angles = int(quadrature)
    target_peak = float(target_peak_t)

    if not np.isfinite(coil_radius) or not r[0] < coil_radius < r[-1]:
        raise ValueError("coil_radius_m must lie inside the prescribed R domain")
    if centers.ndim != 1 or centers.size != 4:
        raise ValueError("coil_z_m must contain exactly four axial centers")
    if not np.all(np.isfinite(centers)):
        raise ValueError("coil_z_m must contain only finite values")
    if np.any(centers <= z[0]) or np.any(centers >= z[-1]):
        raise ValueError("all coil axial centers must lie inside the Z domain")
    if weights.shape != centers.shape or not np.all(np.isfinite(weights)):
        raise ValueError("coil_weights must contain four finite values")
    if not np.any(weights > 0.0) or not np.any(weights < 0.0):
        raise ValueError("coil_weights must retain positive and negative groups")
    if not np.isfinite(softening) or softening <= 0.0:
        raise ValueError("softening_m must be finite and positive")
    if number_of_angles < 32:
        raise ValueError("quadrature must be at least 32")
    if not np.isfinite(target_peak) or target_peak <= 0.0:
        raise ValueError("target_peak_t must be finite and positive")

    normalized_weights = weights / float(np.max(np.abs(weights)))
    rr = r[:, None, None]
    raw_br = np.zeros((r.size, z.size), dtype=float)
    raw_bz = np.zeros_like(raw_br)

    phi = (
        np.arange(number_of_angles, dtype=float) + 0.5
    ) * (2.0 * np.pi / number_of_angles)
    dphi = 2.0 * np.pi / number_of_angles
    chunk_size = 32

    for center_z, current_weight in zip(centers, normalized_weights):
        axial_delta = z[None, :] - float(center_z)
        axial_delta_3d = axial_delta[:, :, None]

        for start in range(0, number_of_angles, chunk_size):
            stop = min(start + chunk_size, number_of_angles)
            cosine = np.cos(phi[start:stop])[None, None, :]
            distance_squared = (
                rr * rr
                + coil_radius * coil_radius
                - 2.0 * rr * coil_radius * cosine
                + axial_delta_3d * axial_delta_3d
                + softening * softening
            )
            inverse_cube = distance_squared ** (-1.5)

            raw_br += (
                current_weight
                * dphi
                * coil_radius
                * axial_delta
                * np.sum(cosine * inverse_cube, axis=2)
            )
            raw_bz += (
                current_weight
                * dphi
                * coil_radius
                * np.sum(
                    (coil_radius - rr * cosine) * inverse_cube,
                    axis=2,
                )
            )

    # Axisymmetry requires an exactly zero radial component at R=0.
    raw_br[0, :] = 0.0

    rr_2d, zz_2d = np.meshgrid(r, z, indexing="ij")
    upper_coil_neighborhood = (
        (np.abs(rr_2d - coil_radius) <= 2.0 * softening)
        & (np.abs(zz_2d - centers[0]) <= 2.0 * softening)
    )
    raw_magnitude = np.hypot(raw_br, raw_bz)
    upper_peak = float(np.max(raw_magnitude[upper_coil_neighborhood]))
    if not np.isfinite(upper_peak) or upper_peak <= 0.0:
        raise ValueError("the upper-coil calibration neighborhood has zero field")

    scale = target_peak / upper_peak
    return scale * raw_br, scale * raw_bz


def build_reference_four_coil_bfield(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    source_r_m: float = 0.125,
    source_z_m: float = 0.300,
    plasma_radius_m: float = 0.250,
    min_b_t: float = 1.0 * GAUSS_TO_TESLA,
    max_b_t: float = 1000.0 * GAUSS_TO_TESLA,
    coil_radius_m: float = DEFAULT_COIL_RADIUS_M,
    coil_z_m: tuple[float, ...] = DEFAULT_COIL_Z_M,
    coil_weights: tuple[float, ...] = DEFAULT_COIL_WEIGHTS,
    coil_softening_m: float = DEFAULT_COIL_SOFTENING_M,
    coil_quadrature: int = DEFAULT_COIL_QUADRATURE,
    coil_target_peak_t: float = DEFAULT_COIL_PEAK_T,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the combined source-plus-four-coil field in tesla."""
    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    source_r = float(source_r_m)
    source_z = float(source_z_m)
    plasma_radius = float(plasma_radius_m)
    minimum = float(min_b_t)
    maximum = float(max_b_t)

    if not r[0] < plasma_radius < r[-1]:
        raise ValueError("plasma_radius_m must lie inside the radial domain")
    if not r[0] <= source_r <= r[-1]:
        raise ValueError("source_r_m must lie inside the radial domain")
    if not z[0] <= source_z <= z[-1]:
        raise ValueError("source_z_m must lie inside the axial domain")
    if minimum <= 0.0 or maximum <= minimum:
        raise ValueError("field limits must satisfy 0 < min_b_t < max_b_t")

    source_br, source_bz = build_source_bfield(
        r,
        z,
        source_r_m=source_r,
        source_z_m=source_z,
        min_b_t=minimum,
        max_b_t=maximum,
    )
    coil_br, coil_bz = build_four_coil_bfield(
        r,
        z,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
        coil_weights=coil_weights,
        softening_m=coil_softening_m,
        quadrature=coil_quadrature,
        target_peak_t=coil_target_peak_t,
    )

    # Fit the coil-only bulk strength to the scaled photographic bands.  The
    # same factor multiplies B_R and B_Z on each Z slice, so the Biot-Savart
    # field-line direction is retained.  R=20..200 mm avoids both the exact
    # axis and the localized conductor lobes when evaluating the bulk guide.
    bulk_r = (r >= 0.020) & (r <= 0.200)
    coil_magnitude = np.hypot(coil_br, coil_bz)
    coil_bulk_median = np.median(coil_magnitude[bulk_r, :], axis=0)
    if np.any(coil_bulk_median <= 0.0) or not np.all(
        np.isfinite(coil_bulk_median)
    ):
        raise ValueError("four-coil field has an invalid bulk axial profile")

    desired_coil_bulk_t = GAUSS_TO_TESLA * _smooth_interpolate(
        z,
        REFERENCE_COIL_BULK_Z_M,
        REFERENCE_COIL_BULK_G,
    )
    axial_scale = desired_coil_bulk_t / coil_bulk_median
    coil_br *= axial_scale[None, :]
    coil_bz *= axial_scale[None, :]

    # Add a broad, smooth field-strength dome aligned with the local coil
    # direction.  Its elliptical level sets produce the photographed circular
    # upper contours, with the 52--84 G guide field surrounding the dome.
    rr, zz = np.meshgrid(r, z, indexing="ij")
    upper_dome = (
        UPPER_DOME_PEAK_G
        * GAUSS_TO_TESLA
        * np.exp(
            -0.5
            * (
                ((rr - UPPER_DOME_CENTER_R_M) / UPPER_DOME_SIGMA_R_M) ** 2
                + ((zz - UPPER_DOME_CENTER_Z_M) / UPPER_DOME_SIGMA_Z_M) ** 2
            )
        )
    )
    coil_magnitude = np.hypot(coil_br, coil_bz)
    safe_coil_magnitude = np.maximum(coil_magnitude, np.finfo(float).tiny)
    coil_br += upper_dome * coil_br / safe_coil_magnitude
    coil_bz += upper_dome * coil_bz / safe_coil_magnitude

    # Keep localized conductor lobes within the photographed orange band.
    coil_magnitude = np.hypot(coil_br, coil_bz)
    coil_peak_scale = np.minimum(
        1.0,
        float(coil_target_peak_t)
        / np.maximum(coil_magnitude, np.finfo(float).tiny),
    )
    coil_br *= coil_peak_scale
    coil_bz *= coil_peak_scale

    # The source and all four coils are a true vector superposition everywhere.
    # No source-protection mask or upper-region replacement is applied.
    br = source_br + coil_br
    bz = source_bz + coil_bz

    # Increase lower-chamber radial steering only around R=100..200 mm.  The
    # enhancement decays smoothly with Z and preserves the upward B_Z sign.
    lower_steering = 1.0 + LOWER_STEERING_GAIN * np.exp(
        -0.5
        * ((rr - LOWER_STEERING_CENTER_R_M) / LOWER_STEERING_SIGMA_R_M) ** 2
    ) * np.exp(-zz / LOWER_STEERING_DECAY_Z_M)
    br *= lower_steering
    br[0, :] = 0.0

    # Localize the photographed 1 G bottom void around R=100..200 mm instead
    # of making the entire lower boundary the minimum-strength band.
    magnitude = np.hypot(br, bz)
    lower_void = np.exp(
        -0.5
        * (
            ((rr - LOWER_VOID_CENTER_R_M) / LOWER_VOID_SIGMA_R_M) ** 2
            + (zz / LOWER_VOID_SIGMA_Z_M) ** 2
        )
    )
    notched_magnitude = magnitude * (1.0 - LOWER_VOID_DEPTH * lower_void)

    # Preserve direction while enforcing the photographed 1--1000 G scale.
    safe_magnitude = np.maximum(magnitude, np.finfo(float).tiny)
    target_magnitude = np.clip(notched_magnitude, minimum, maximum)
    direction_scale = target_magnitude / safe_magnitude
    br *= direction_scale
    bz *= direction_scale

    zero_mask = magnitude <= np.finfo(float).tiny
    if np.any(zero_mask):
        br[zero_mask] = 0.0
        bz[zero_mask] = minimum

    return br, bz


def plot_reference_four_coil_bfield(
    path: str | Path,
    r_m: np.ndarray,
    z_m: np.ndarray,
    br_t: np.ndarray,
    bz_t: np.ndarray,
    *,
    plasma_radius_m: float = 0.250,
    coil_radius_m: float = DEFAULT_COIL_RADIUS_M,
    coil_z_m: tuple[float, ...] = DEFAULT_COIL_Z_M,
) -> None:
    """Write a reference-style four-coil magnitude and streamline plot."""
    import matplotlib

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import BoundaryNorm

    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    br = np.asarray(br_t, dtype=float)
    bz = np.asarray(bz_t, dtype=float)
    expected_shape = (r.size, z.size)
    if br.shape != expected_shape or bz.shape != expected_shape:
        raise ValueError(f"B_R and B_Z must both have shape {expected_shape}")
    if not np.all(np.isfinite(br)) or not np.all(np.isfinite(bz)):
        raise ValueError("field components must contain only finite values")

    plasma_radius = float(plasma_radius_m)
    coil_radius = float(coil_radius_m)
    coil_z = np.asarray(coil_z_m, dtype=float)
    if not r[0] < plasma_radius < r[-1]:
        raise ValueError("plasma_radius_m must lie inside the radial plot domain")
    if not r[0] < coil_radius < r[-1]:
        raise ValueError("coil_radius_m must lie inside the radial plot domain")
    if coil_z.shape != (4,) or not np.all(np.isfinite(coil_z)):
        raise ValueError("coil_z_m must contain four finite axial centers")

    r_mm = 1000.0 * r
    z_mm = 1000.0 * z
    bmag_g = np.hypot(br, bz) / GAUSS_TO_TESLA
    displayed_bmag_g = np.clip(
        bmag_g,
        REFERENCE_LEVELS_G[0],
        REFERENCE_LEVELS_G[-1],
    )

    number_of_bands = REFERENCE_LEVELS_G.size - 1
    colormap = plt.colormaps["turbo"].resampled(number_of_bands)
    normalization = BoundaryNorm(
        REFERENCE_LEVELS_G,
        ncolors=colormap.N,
        clip=True,
    )

    figure, axis = plt.subplots(figsize=(11.0, 8.5), constrained_layout=True)
    filled = axis.contourf(
        r_mm,
        z_mm,
        displayed_bmag_g.T,
        levels=REFERENCE_LEVELS_G,
        cmap=colormap,
        norm=normalization,
    )
    axis.streamplot(
        r_mm,
        z_mm,
        br.T,
        bz.T,
        color="black",
        density=(1.6, 1.35),
        linewidth=0.85,
        arrowsize=0.80,
        arrowstyle="-|>",
        broken_streamlines=True,
    )
    axis.axvline(
        1000.0 * plasma_radius,
        color="red",
        linewidth=2.0,
        label=f"plasma boundary ({1000.0 * plasma_radius:.0f} mm)",
    )
    axis.scatter(
        np.full(4, 1000.0 * coil_radius),
        1000.0 * coil_z,
        s=34.0,
        facecolors="none",
        edgecolors="black",
        linewidths=1.1,
        label="effective coil centers",
        zorder=5,
    )

    axis.set_xlim(r_mm[0], r_mm[-1])
    axis.set_ylim(z_mm[0], z_mm[-1])
    axis.set_aspect("equal", adjustable="box")
    axis.set_xlabel("R (mm)")
    axis.set_ylabel("Z (mm)")
    axis.set_title("S-N-S source + four-coil total field (G) and direction")
    axis.legend(loc="lower right", framealpha=0.92)

    colorbar = figure.colorbar(
        filled,
        ax=axis,
        boundaries=REFERENCE_LEVELS_G,
        ticks=REFERENCE_LEVELS_G,
        spacing="uniform",
        pad=0.04,
    )
    colorbar.set_label("|B| (G)")
    colorbar.ax.set_yticklabels(
        [f"{level:.2f}" for level in REFERENCE_LEVELS_G]
    )

    output_path = Path(path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_path, dpi=180)
    plt.close(figure)


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    """Parse command-line configuration."""
    parser = argparse.ArgumentParser(
        description=(
            "Generate the scaled analytical source-plus-four-coil B-field "
            "for MOOSE/Zapdos."
        )
    )
    parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    parser.add_argument("--plot-path", type=Path, default=DEFAULT_PLOT_PATH)
    parser.add_argument("--nx", type=int, default=161)
    parser.add_argument("--ny", type=int, default=301)
    parser.add_argument("--r-max-mm", type=float, default=400.0)
    parser.add_argument("--z-max-mm", type=float, default=300.0)
    parser.add_argument("--plasma-radius-mm", type=float, default=250.0)
    parser.add_argument("--source-r-mm", type=float, default=125.0)
    parser.add_argument("--min-gauss", type=float, default=1.0)
    parser.add_argument("--max-gauss", type=float, default=1000.0)
    parser.add_argument("--coil-radius-mm", type=float, default=280.0)
    parser.add_argument(
        "--coil-z-mm",
        type=float,
        nargs=4,
        default=[250.0, 150.0, 55.0, 30.0],
        metavar=("Z1", "Z2", "Z3", "Z4"),
    )
    parser.add_argument(
        "--coil-weights",
        type=float,
        nargs=4,
        default=[44.6, 12.6, 3.2, -5.2],
        metavar=("W1", "W2", "W3", "W4"),
    )
    parser.add_argument("--coil-softening-mm", type=float, default=6.0)
    parser.add_argument("--coil-quadrature", type=int, default=256)
    parser.add_argument("--coil-peak-gauss", type=float, default=227.58)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    """Generate the MOOSE tables and diagnostic plot."""
    args = parse_args(argv)
    if args.nx < 3 or args.ny < 3:
        raise ValueError("--nx and --ny must each be at least 3")

    r_max_m = 1.0e-3 * float(args.r_max_mm)
    z_max_m = 1.0e-3 * float(args.z_max_mm)
    plasma_radius_m = 1.0e-3 * float(args.plasma_radius_mm)
    source_r_m = 1.0e-3 * float(args.source_r_mm)
    coil_radius_m = 1.0e-3 * float(args.coil_radius_mm)
    coil_z_m = tuple(1.0e-3 * float(value) for value in args.coil_z_mm)
    coil_weights = tuple(float(value) for value in args.coil_weights)

    if r_max_m <= 0.0 or z_max_m <= 0.0:
        raise ValueError("--r-max-mm and --z-max-mm must be positive")
    if not 0.0 < plasma_radius_m < r_max_m:
        raise ValueError("--plasma-radius-mm must lie inside the radial domain")
    if not plasma_radius_m < coil_radius_m < r_max_m:
        raise ValueError(
            "--coil-radius-mm must lie outside the plasma and inside the map"
        )

    r_m = np.linspace(0.0, r_max_m, args.nx)
    z_m = np.linspace(0.0, z_max_m, args.ny)
    br_t, bz_t = build_reference_four_coil_bfield(
        r_m,
        z_m,
        source_r_m=source_r_m,
        source_z_m=z_max_m,
        plasma_radius_m=plasma_radius_m,
        min_b_t=float(args.min_gauss) * GAUSS_TO_TESLA,
        max_b_t=float(args.max_gauss) * GAUSS_TO_TESLA,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
        coil_weights=coil_weights,
        coil_softening_m=float(args.coil_softening_mm) * 1.0e-3,
        coil_quadrature=int(args.coil_quadrature),
        coil_target_peak_t=float(args.coil_peak_gauss) * GAUSS_TO_TESLA,
    )

    bx_path = args.out_dir / "Bx_T.tbl"
    by_path = args.out_dir / "By_T.tbl"
    write_moose_table(bx_path, r_m, z_m, br_t)
    write_moose_table(by_path, r_m, z_m, bz_t)
    plot_reference_four_coil_bfield(
        args.plot_path,
        r_m,
        z_m,
        br_t,
        bz_t,
        plasma_radius_m=plasma_radius_m,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
    )

    bmag_g = np.hypot(br_t, bz_t) / GAUSS_TO_TESLA
    plasma_mask = r_m[:, None] <= plasma_radius_m
    upward_fraction = float(np.mean(bz_t[plasma_mask.repeat(z_m.size, axis=1)] > 0.0))

    print(f"wrote radial field table: {bx_path}")
    print(f"wrote axial field table: {by_path}")
    print(f"wrote diagnostic plot: {args.plot_path}")
    print(
        "domain: "
        f"R=0..{1000.0 * r_m[-1]:.1f} mm, "
        f"Z=0..{1000.0 * z_m[-1]:.1f} mm, "
        f"plasma R=0..{1000.0 * plasma_radius_m:.1f} mm"
    )
    print(
        "coils: "
        f"R={1000.0 * coil_radius_m:.1f} mm, "
        "Z=("
        + ", ".join(f"{1000.0 * center:.1f}" for center in coil_z_m)
        + ") mm, weights=("
        + ", ".join(f"{weight:.1f}" for weight in coil_weights)
        + ")"
    )
    print(
        f"|B| range: {np.min(bmag_g):.2f}..{np.max(bmag_g):.2f} G; "
        f"upward plasma fraction: {100.0 * upward_fraction:.1f}%"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
