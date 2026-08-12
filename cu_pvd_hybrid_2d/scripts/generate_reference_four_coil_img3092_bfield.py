#!/usr/bin/env python3
"""Generate the IMG_3092 source-plus-four-coil analytical B-field.

This generator is intentionally separate from the two earlier reference
generators and writes to its own output paths.

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
    / "zapdos_hpem_rz_30cm_reference_four_coil_img3092"
    / "moose_tables"
)
DEFAULT_PLOT_PATH = ROOT / "post" / "reference_four_coil_img3092_bfield_30cm.png"

DEFAULT_COIL_RADIUS_M = 0.280
DEFAULT_COIL_Z_M = (0.250, 0.150, 0.055, 0.030)
DEFAULT_COIL_WEIGHTS = (44.6, 12.6, 3.2, 13.7)
DEFAULT_COIL_SOFTENING_M = 0.006
DEFAULT_COIL_QUADRATURE = 256
DEFAULT_COIL_PEAK_T = 227.58 * GAUSS_TO_TESLA

# Broad left-side guide strengths read from IMG_3092 after its 0.5x axial
# scaling. Smooth interpolation avoids visible band kinks.
REFERENCE_GUIDE_Z_M = np.asarray(
    [0.000, 0.025, 0.055, 0.100, 0.150, 0.200, 0.250, 0.300],
    dtype=float,
)
REFERENCE_GUIDE_G = np.asarray(
    [51.79, 84.83, 105.0, 115.0, 110.0, 78.0, 68.0, 84.83],
    dtype=float,
)
DEFAULT_GUIDE_G = tuple(float(value) for value in REFERENCE_GUIDE_G)
CHAMBER_STRAIGHTENING_FLOOR = 0.35
CHAMBER_STRAIGHTENING_CENTER_R_M = 0.250
CHAMBER_STRAIGHTENING_WIDTH_M = 0.012
BOTTOM_LOBE_CENTER_R_M = 0.280
BOTTOM_LOBE_CENTER_Z_M = 0.040
BOTTOM_LOBE_SIGMA_R_M = 0.050
BOTTOM_LOBE_SIGMA_Z_M = 0.040
BOTTOM_LOBE_BOOST_G = 160.0
OUTER_TAPER_CENTER_R_M = 0.315
OUTER_TAPER_WIDTH_M = 0.020
OUTER_TAPER_FLOOR = 0.10


def _smooth_interpolate(
    values: np.ndarray,
    control_x: np.ndarray,
    control_y: np.ndarray,
) -> np.ndarray:
    """Interpolate monotone control coordinates with C1 smoothstep segments."""
    query = np.asarray(values, dtype=float)
    x = np.asarray(control_x, dtype=float)
    y = np.asarray(control_y, dtype=float)
    if x.ndim != 1 or y.shape != x.shape or x.size < 2:
        raise ValueError("guide controls must be matching one-dimensional arrays")
    if np.any(np.diff(x) <= 0.0):
        raise ValueError("guide coordinates must be strictly increasing")

    segment = np.searchsorted(x, query, side="right") - 1
    segment = np.clip(segment, 0, x.size - 2)
    x0 = x[segment]
    x1 = x[segment + 1]
    fraction = np.clip((query - x0) / (x1 - x0), 0.0, 1.0)
    smooth_fraction = fraction * fraction * (3.0 - 2.0 * fraction)
    return y[segment] + smooth_fraction * (y[segment + 1] - y[segment])


def _build_softened_circular_coils(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    coil_radius_m: float,
    coil_z_m: tuple[float, ...],
    coil_weights: tuple[float, ...],
    softening_m: float,
    quadrature: int,
    target_peak_t: float,
) -> tuple[np.ndarray, np.ndarray]:
    """Evaluate four all-positive or mixed-sign circular current loops."""
    r = np.asarray(r_m, dtype=float)
    z = np.asarray(z_m, dtype=float)
    centers = np.asarray(coil_z_m, dtype=float)
    weights = np.asarray(coil_weights, dtype=float)
    radius = float(coil_radius_m)
    softening = float(softening_m)
    number_of_angles = int(quadrature)
    target_peak = float(target_peak_t)

    if r.ndim != 1 or z.ndim != 1 or r.size < 3 or z.size < 3:
        raise ValueError("R and Z axes must be one-dimensional with at least 3 points")
    if np.any(np.diff(r) <= 0.0) or np.any(np.diff(z) <= 0.0):
        raise ValueError("R and Z axes must be strictly increasing")
    if not np.all(np.isfinite(r)) or not np.all(np.isfinite(z)):
        raise ValueError("R and Z axes must contain finite values")
    if not np.isfinite(radius) or not r[0] < radius < r[-1]:
        raise ValueError("coil_radius_m must lie inside the radial domain")
    if centers.shape != (4,) or weights.shape != (4,):
        raise ValueError("coil_z_m and coil_weights must each contain four values")
    if not np.all(np.isfinite(centers)) or not np.all(np.isfinite(weights)):
        raise ValueError("coil positions and weights must be finite")
    if np.any(centers <= z[0]) or np.any(centers >= z[-1]):
        raise ValueError("all coil centers must lie inside the axial domain")
    if not np.any(np.abs(weights) > 0.0):
        raise ValueError("at least one coil weight must be nonzero")
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
                + radius * radius
                - 2.0 * rr * radius * cosine
                + axial_delta_3d * axial_delta_3d
                + softening * softening
            )
            inverse_cube = distance_squared ** (-1.5)
            raw_br += (
                current_weight
                * dphi
                * radius
                * axial_delta
                * np.sum(cosine * inverse_cube, axis=2)
            )
            raw_bz += (
                current_weight
                * dphi
                * radius
                * np.sum((radius - rr * cosine) * inverse_cube, axis=2)
            )

    raw_br[0, :] = 0.0
    rr_2d, zz_2d = np.meshgrid(r, z, indexing="ij")
    upper_neighborhood = (
        (np.abs(rr_2d - radius) <= 2.0 * softening)
        & (np.abs(zz_2d - centers[0]) <= 2.0 * softening)
    )
    raw_magnitude = np.hypot(raw_br, raw_bz)
    upper_peak = float(np.max(raw_magnitude[upper_neighborhood]))
    if not np.isfinite(upper_peak) or upper_peak <= 0.0:
        raise ValueError("upper-coil calibration neighborhood has zero field")
    scale = target_peak / upper_peak
    return scale * raw_br, scale * raw_bz


def build_img3092_coil_bfield(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    coil_radius_m: float = DEFAULT_COIL_RADIUS_M,
    coil_z_m: tuple[float, ...] = DEFAULT_COIL_Z_M,
    coil_weights: tuple[float, ...] = DEFAULT_COIL_WEIGHTS,
    softening_m: float = DEFAULT_COIL_SOFTENING_M,
    quadrature: int = DEFAULT_COIL_QUADRATURE,
    target_peak_t: float = DEFAULT_COIL_PEAK_T,
    guide_g: tuple[float, ...] = DEFAULT_GUIDE_G,
    bottom_lobe_boost_g: float = BOTTOM_LOBE_BOOST_G,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the smoothly calibrated IMG_3092 four-coil field in tesla."""
    r = np.asarray(r_m, dtype=float)
    z = np.asarray(z_m, dtype=float)
    coil_br, coil_bz = _build_softened_circular_coils(
        r,
        z,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
        coil_weights=coil_weights,
        softening_m=softening_m,
        quadrature=quadrature,
        target_peak_t=target_peak_t,
    )

    bulk_r = (r >= 0.020) & (r <= 0.180)
    if not np.any(bulk_r):
        raise ValueError("radial grid must sample the 20..180 mm guide region")
    magnitude = np.hypot(coil_br, coil_bz)
    bulk_median = np.median(magnitude[bulk_r, :], axis=0)
    if np.any(bulk_median <= 0.0) or not np.all(np.isfinite(bulk_median)):
        raise ValueError("IMG_3092 coil field has an invalid guide profile")

    guide_strengths = np.asarray(guide_g, dtype=float)
    if guide_strengths.shape != REFERENCE_GUIDE_Z_M.shape:
        raise ValueError("guide_g must contain eight axial guide strengths")
    if not np.all(np.isfinite(guide_strengths)) or np.any(guide_strengths <= 0.0):
        raise ValueError("guide_g must contain positive finite strengths")
    bottom_boost_g = float(bottom_lobe_boost_g)
    if not np.isfinite(bottom_boost_g) or bottom_boost_g < 0.0:
        raise ValueError("bottom_lobe_boost_g must be finite and nonnegative")

    desired_guide_t = GAUSS_TO_TESLA * _smooth_interpolate(
        z,
        REFERENCE_GUIDE_Z_M,
        guide_strengths,
    )
    axial_scale = desired_guide_t / bulk_median
    coil_br *= axial_scale[None, :]
    coil_bz *= axial_scale[None, :]

    # IMG_3092 has a strong rounded bottom lobe because BM is +13.7 A.
    # Add a smooth direction-preserving calibration only for a positive bottom
    # current; the earlier -5.2 A configuration receives no such correction.
    bottom_positive_fraction = max(float(coil_weights[3]), 0.0) / 13.7
    if bottom_positive_fraction > 0.0:
        rr, zz = np.meshgrid(r, z, indexing="ij")
        bottom_boost = (
            bottom_positive_fraction
            * bottom_boost_g
            * GAUSS_TO_TESLA
            * np.exp(
                -0.5
                * (
                    ((rr - BOTTOM_LOBE_CENTER_R_M) / BOTTOM_LOBE_SIGMA_R_M) ** 2
                    + ((zz - BOTTOM_LOBE_CENTER_Z_M) / BOTTOM_LOBE_SIGMA_Z_M) ** 2
                )
            )
        )
        magnitude = np.hypot(coil_br, coil_bz)
        safe_magnitude = np.maximum(magnitude, np.finfo(float).tiny)
        coil_br += bottom_boost * coil_br / safe_magnitude
        coil_bz += bottom_boost * coil_bz / safe_magnitude

    # IMG_3092 shows nearly axial field lines through the plasma chamber.
    # Smoothly attenuate only the coil-driven radial component there, then
    # restore the circular-loop direction before reaching the R=280 mm coils.
    chamber_straightening = CHAMBER_STRAIGHTENING_FLOOR + (
        1.0 - CHAMBER_STRAIGHTENING_FLOOR
    ) / (
        1.0
        + np.exp(
            -(r - CHAMBER_STRAIGHTENING_CENTER_R_M)
            / CHAMBER_STRAIGHTENING_WIDTH_M
        )
    )
    coil_br *= chamber_straightening[:, None]

    # The photograph falls rapidly from each R≈280 mm lobe into the blue
    # outer-right valleys. The slice-only guide fit otherwise leaves too much
    # far-field strength at R≈350 mm, so apply a smooth radial attenuation to
    # the calibrated coil contribution outside the effective coil radius.
    outer_taper = OUTER_TAPER_FLOOR + (1.0 - OUTER_TAPER_FLOOR) / (
        1.0 + np.exp((r - OUTER_TAPER_CENTER_R_M) / OUTER_TAPER_WIDTH_M)
    )
    coil_br *= outer_taper[:, None]
    coil_bz *= outer_taper[:, None]
    coil_br[0, :] = 0.0
    return coil_br, coil_bz


def combine_source_and_coils(
    source_br: np.ndarray,
    source_bz: np.ndarray,
    coil_br: np.ndarray,
    coil_bz: np.ndarray,
    *,
    min_b_t: float,
    max_b_t: float,
) -> tuple[np.ndarray, np.ndarray]:
    """Add source and coil vectors, then limit magnitude without rotation."""
    components = [
        np.asarray(source_br, dtype=float),
        np.asarray(source_bz, dtype=float),
        np.asarray(coil_br, dtype=float),
        np.asarray(coil_bz, dtype=float),
    ]
    if len({component.shape for component in components}) != 1:
        raise ValueError("source and coil components must have matching shapes")
    if not all(np.all(np.isfinite(component)) for component in components):
        raise ValueError("source and coil components must contain finite values")
    minimum = float(min_b_t)
    maximum = float(max_b_t)
    if not np.isfinite(minimum) or not np.isfinite(maximum):
        raise ValueError("field limits must be finite")
    if minimum <= 0.0 or maximum <= minimum:
        raise ValueError("field limits must satisfy 0 < min_b_t < max_b_t")

    br = components[0] + components[2]
    bz = components[1] + components[3]
    magnitude = np.hypot(br, bz)
    nonzero = magnitude > np.finfo(float).tiny
    target_magnitude = np.clip(magnitude, minimum, maximum)
    scale = np.ones_like(magnitude)
    scale[nonzero] = target_magnitude[nonzero] / magnitude[nonzero]
    br *= scale
    bz *= scale
    br[~nonzero] = 0.0
    bz[~nonzero] = minimum
    return br, bz


def build_reference_four_coil_img3092_bfield(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    source_r_m: float = 0.125,
    source_z_m: float = 0.300,
    min_b_t: float = 1.0 * GAUSS_TO_TESLA,
    max_b_t: float = 1000.0 * GAUSS_TO_TESLA,
    coil_radius_m: float = DEFAULT_COIL_RADIUS_M,
    coil_z_m: tuple[float, ...] = DEFAULT_COIL_Z_M,
    coil_weights: tuple[float, ...] = DEFAULT_COIL_WEIGHTS,
    coil_softening_m: float = DEFAULT_COIL_SOFTENING_M,
    coil_quadrature: int = DEFAULT_COIL_QUADRATURE,
    coil_target_peak_t: float = DEFAULT_COIL_PEAK_T,
    guide_g: tuple[float, ...] = DEFAULT_GUIDE_G,
    bottom_lobe_boost_g: float = BOTTOM_LOBE_BOOST_G,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the IMG_3092 S-N-S source plus four-coil field in tesla."""
    r = np.asarray(r_m, dtype=float)
    z = np.asarray(z_m, dtype=float)
    source_br, source_bz = build_source_bfield(
        r,
        z,
        source_r_m=source_r_m,
        source_z_m=source_z_m,
        min_b_t=min_b_t,
        max_b_t=max_b_t,
    )
    coil_br, coil_bz = build_img3092_coil_bfield(
        r,
        z,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
        coil_weights=coil_weights,
        softening_m=coil_softening_m,
        quadrature=coil_quadrature,
        target_peak_t=coil_target_peak_t,
        guide_g=guide_g,
        bottom_lobe_boost_g=bottom_lobe_boost_g,
    )
    br, bz = combine_source_and_coils(
        source_br,
        source_bz,
        coil_br,
        coil_bz,
        min_b_t=min_b_t,
        max_b_t=max_b_t,
    )
    br[0, :] = 0.0
    return br, bz


def plot_reference_four_coil_img3092_bfield(
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
    """Write the full-domain IMG_3092 magnitude and streamline diagnostic."""
    import matplotlib

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import BoundaryNorm

    r = np.asarray(r_m, dtype=float)
    z = np.asarray(z_m, dtype=float)
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
        raise ValueError("plasma_radius_m must lie inside the radial domain")
    if not r[0] < coil_radius < r[-1]:
        raise ValueError("coil_radius_m must lie inside the radial domain")
    if coil_z.shape != (4,) or not np.all(np.isfinite(coil_z)):
        raise ValueError("coil_z_m must contain four finite centers")

    r_mm = 1000.0 * r
    z_mm = 1000.0 * z
    magnitude_g = np.hypot(br, bz) / GAUSS_TO_TESLA
    displayed_g = np.clip(
        magnitude_g,
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
        displayed_g.T,
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
    axis.set_title("IMG_3092 total field (G) and direction", pad=10.0)
    axis.legend(loc="lower left", framealpha=0.92)

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
    """Parse the standalone IMG_3092 generator configuration."""
    parser = argparse.ArgumentParser(
        description=(
            "Generate the scaled IMG_3092 S-N-S source-plus-four-coil "
            "B-field for MOOSE/Zapdos."
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
        default=list(DEFAULT_COIL_WEIGHTS),
        metavar=("W1", "W2", "W3", "W4"),
    )
    parser.add_argument("--coil-softening-mm", type=float, default=6.0)
    parser.add_argument("--coil-quadrature", type=int, default=256)
    parser.add_argument("--coil-peak-gauss", type=float, default=227.58)
    parser.add_argument(
        "--guide-gauss",
        type=float,
        nargs=8,
        default=list(DEFAULT_GUIDE_G),
        metavar=("G0", "G25", "G55", "G100", "G150", "G200", "G250", "G300"),
    )
    parser.add_argument(
        "--bottom-lobe-boost-gauss",
        type=float,
        default=BOTTOM_LOBE_BOOST_G,
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    """Generate the dedicated IMG_3092 MOOSE tables and diagnostic plot."""
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
    guide_g = tuple(float(value) for value in args.guide_gauss)
    if r_max_m <= 0.0 or z_max_m <= 0.0:
        raise ValueError("--r-max-mm and --z-max-mm must be positive")
    if not 0.0 < plasma_radius_m < coil_radius_m < r_max_m:
        raise ValueError(
            "plasma radius must be smaller than the coil radius and map radius"
        )

    r_m = np.linspace(0.0, r_max_m, args.nx)
    z_m = np.linspace(0.0, z_max_m, args.ny)
    br_t, bz_t = build_reference_four_coil_img3092_bfield(
        r_m,
        z_m,
        source_r_m=source_r_m,
        source_z_m=z_max_m,
        min_b_t=float(args.min_gauss) * GAUSS_TO_TESLA,
        max_b_t=float(args.max_gauss) * GAUSS_TO_TESLA,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
        coil_weights=coil_weights,
        coil_softening_m=float(args.coil_softening_mm) * 1.0e-3,
        coil_quadrature=int(args.coil_quadrature),
        coil_target_peak_t=float(args.coil_peak_gauss) * GAUSS_TO_TESLA,
        guide_g=guide_g,
        bottom_lobe_boost_g=float(args.bottom_lobe_boost_gauss),
    )

    bx_path = args.out_dir / "Bx_T.tbl"
    by_path = args.out_dir / "By_T.tbl"
    write_moose_table(bx_path, r_m, z_m, br_t)
    write_moose_table(by_path, r_m, z_m, bz_t)
    plot_reference_four_coil_img3092_bfield(
        args.plot_path,
        r_m,
        z_m,
        br_t,
        bz_t,
        plasma_radius_m=plasma_radius_m,
        coil_radius_m=coil_radius_m,
        coil_z_m=coil_z_m,
    )

    magnitude_g = np.hypot(br_t, bz_t) / GAUSS_TO_TESLA
    plasma_mask = np.broadcast_to(
        (r_m[:, None] <= plasma_radius_m),
        br_t.shape,
    )
    lower_plasma_mask = plasma_mask & np.broadcast_to(
        (z_m[None, :] <= 0.200),
        br_t.shape,
    )
    upward_fraction = float(np.mean(bz_t[lower_plasma_mask] > 0.0))
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
        + ", ".join(f"{1000.0 * value:.1f}" for value in coil_z_m)
        + ") mm, weights=("
        + ", ".join(f"{value:.1f}" for value in coil_weights)
        + ")"
    )
    print(
        f"|B| range: {np.min(magnitude_g):.2f}..{np.max(magnitude_g):.2f} G; "
        f"lower-plasma upward fraction: {100.0 * upward_fraction:.1f}%"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
