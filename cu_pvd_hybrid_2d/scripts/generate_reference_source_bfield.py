#!/usr/bin/env python3
"""Generate a source-only R-Z magnetic-field map for the 30 cm chamber.

The field is an analytical approximation of the supplied 60 cm reference
figure.  Its axial profile is compressed to 30 cm, its radial profile remains
available through 400 mm, and the Zapdos plasma region may use R <= 250 mm.

MOOSE/Zapdos component convention:
    Bx_T.tbl = B_R [T]
    By_T.tbl = B_Z [T]
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


GAUSS_TO_TESLA = 1.0e-4
ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_source_only"
    / "moose_tables"
)
DEFAULT_PLOT_PATH = ROOT / "post" / "reference_source_only_bfield_30cm.png"
REFERENCE_LEVELS_G = np.asarray(
    [
        1.00,
        1.64,
        2.68,
        4.39,
        7.20,
        11.79,
        19.31,
        31.62,
        51.79,
        84.83,
        138.95,
        227.58,
        372.76,
        610.54,
        1000.00,
    ],
    dtype=float,
)
REFERENCE_PROFILE_DISTANCE_M = 1.0e-3 * np.asarray(
    [0.0, 9.0, 13.0, 18.0, 45.0, 65.0, 75.0, 105.0, 135.0, 192.0, 245.0, 300.0],
    dtype=float,
)
REFERENCE_PROFILE_MAX_G = np.asarray(
    [1000.00, 138.95, 84.83, 51.79, 31.62, 19.31, 11.79, 7.20, 4.39, 2.68, 1.64, 1.00],
    dtype=float,
)


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(f"{name} must be a one-dimensional array with at least two points")
    if not np.all(np.isfinite(axis)):
        raise ValueError(f"{name} must contain only finite values")
    if np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must be strictly increasing")
    return axis


def build_source_bfield(
    r_m: np.ndarray,
    z_m: np.ndarray,
    *,
    source_r_m: float = 0.125,
    source_z_m: float = 0.300,
    min_b_t: float = 1.0 * GAUSS_TO_TESLA,
    max_b_t: float = 1000.0 * GAUSS_TO_TESLA,
    pole_separation_m: float = 0.040,
    pole_depth_m: float = 0.006,
    softening_m: float = 0.010,
    null_offset_m: float = 0.010,
    polarity: float = -1.0,
) -> tuple[np.ndarray, np.ndarray]:
    """Return analytical source-only ``(B_R, B_Z)`` arrays in tesla.

    Three softened virtual poles with alternating S-N-S amplitudes sit behind
    the top target. Their amplitudes are solved so the two surface half-cycles
    meet at a magnetic null ``null_offset_m`` below the source center. The
    polarity points the lower-chamber field from Z=0 toward the source.

    A common axial magnitude profile is fitted in log space to the contour
    depths measured from the supplied figure after its 0.5x axial scaling.
    Multiplying both components by the same slice factor leaves vector
    direction unchanged.
    """
    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    source_r = float(source_r_m)
    source_z = float(source_z_m)
    minimum = float(min_b_t)
    maximum = float(max_b_t)
    pole_separation = float(pole_separation_m)
    pole_depth = float(pole_depth_m)
    softening = float(softening_m)
    null_offset = float(null_offset_m)
    field_polarity = float(polarity)

    scalar_values = {
        "source_r_m": source_r,
        "source_z_m": source_z,
        "min_b_t": minimum,
        "max_b_t": maximum,
        "pole_separation_m": pole_separation,
        "pole_depth_m": pole_depth,
        "softening_m": softening,
        "null_offset_m": null_offset,
        "polarity": field_polarity,
    }
    if not all(np.isfinite(value) for value in scalar_values.values()):
        raise ValueError("field parameters must be finite")
    if not r[0] <= source_r <= r[-1]:
        raise ValueError("source_r_m must lie inside the radial domain")
    if not z[0] <= source_z <= z[-1]:
        raise ValueError("source_z_m must lie inside the axial domain")
    if minimum <= 0.0 or maximum <= minimum:
        raise ValueError("field strengths must satisfy 0 < min_b_t < max_b_t")
    if pole_separation <= 0.0 or pole_depth <= 0.0 or softening <= 0.0:
        raise ValueError(
            "pole_separation_m, pole_depth_m, and softening_m must be positive"
        )
    if source_r - pole_separation <= r[0] or source_r + pole_separation >= r[-1]:
        raise ValueError("all three S-N-S poles must lie inside the radial domain")
    if not 0.0 < null_offset < source_z - z[0]:
        raise ValueError("null_offset_m must place the null inside the axial domain")
    if field_polarity not in (-1.0, 1.0):
        raise ValueError("polarity must be either -1 or +1")

    rr, zz = np.meshgrid(r, z, indexing="ij")
    pole_z = source_z + pole_depth
    pole_radii = np.asarray(
        [source_r - pole_separation, source_r, source_r + pole_separation]
    )

    def pole_field(
        pole_r: float,
        query_r: np.ndarray,
        query_z: np.ndarray,
    ) -> tuple[np.ndarray, np.ndarray]:
        dr = query_r - pole_r
        dz = query_z - pole_z
        distance_squared = dr**2 + dz**2 + softening**2
        inverse_cube = distance_squared ** (-1.5)
        return dr * inverse_cube, dz * inverse_cube

    basis_fields = [pole_field(pole_r, rr, zz) for pole_r in pole_radii]
    null_r = np.asarray([[source_r]])
    null_z = np.asarray([[source_z - null_offset]])
    null_fields = [
        pole_field(pole_r, null_r, null_z)
        for pole_r in pole_radii
    ]
    calibration_matrix = np.asarray(
        [
            [null_fields[1][0][0, 0], null_fields[2][0][0, 0]],
            [null_fields[1][1][0, 0], null_fields[2][1][0, 0]],
        ]
    )
    calibration_rhs = -np.asarray(
        [null_fields[0][0][0, 0], null_fields[0][1][0, 0]]
    )
    condition_number = float(np.linalg.cond(calibration_matrix))
    if not np.isfinite(condition_number) or condition_number > 1.0e10:
        raise ValueError("S-N-S null calibration matrix is ill-conditioned")
    amplitudes = np.concatenate(
        ([1.0], np.linalg.solve(calibration_matrix, calibration_rhs))
    )
    if not (
        amplitudes[0] > 0.0
        and amplitudes[1] < 0.0
        and amplitudes[2] > 0.0
    ):
        raise ValueError("S-N-S null calibration lost alternating pole signs")

    br_raw = sum(
        amplitude * field[0]
        for amplitude, field in zip(amplitudes, basis_fields)
    )
    bz_raw = sum(
        amplitude * field[1]
        for amplitude, field in zip(amplitudes, basis_fields)
    )

    # Keep B_R exactly zero on the R-Z symmetry axis while allowing the first
    # interior field lines to curve immediately, as in the photographed map.
    axis_taper = 1.0 - np.exp(-((rr / 0.003) ** 2))
    br_raw *= axis_taper
    br_raw *= field_polarity
    bz_raw *= field_polarity

    raw_magnitude = np.hypot(br_raw, bz_raw)
    slice_maximum = np.max(raw_magnitude, axis=0)
    if np.any(slice_maximum <= 0.0) or not np.all(np.isfinite(slice_maximum)):
        raise ValueError("analytical S-N-S field has an invalid axial slice")

    distance_from_source = np.maximum(source_z - z, 0.0)
    reference_log_profile = np.interp(
        distance_from_source,
        REFERENCE_PROFILE_DISTANCE_M,
        np.log(REFERENCE_PROFILE_MAX_G / REFERENCE_PROFILE_MAX_G[0]),
    )
    desired_slice_maximum = maximum * np.exp(reference_log_profile)
    slice_scale = desired_slice_maximum / slice_maximum
    br_raw *= slice_scale[None, :]
    bz_raw *= slice_scale[None, :]

    raw_magnitude = np.hypot(br_raw, bz_raw)
    raw_maximum = float(np.max(raw_magnitude))
    if raw_maximum <= 0.0 or not np.isfinite(raw_maximum):
        raise ValueError("analytical source field is zero")

    br = br_raw * (maximum / raw_maximum)
    bz = bz_raw * (maximum / raw_maximum)
    magnitude = np.hypot(br, bz)

    # The supplied reference uses a 1 G lower bound. Raise weaker points to
    # that value without rotating the analytical vector.
    safe_magnitude = np.maximum(magnitude, np.finfo(float).tiny)
    magnitude_minimum = float(np.min(magnitude))
    if magnitude_minimum > minimum:
        target_magnitude = minimum + (
            (magnitude - magnitude_minimum)
            * (maximum - minimum)
            / (maximum - magnitude_minimum)
        )
    else:
        target_magnitude = np.maximum(magnitude, minimum)
    direction_scale = target_magnitude / safe_magnitude
    br *= direction_scale
    bz *= direction_scale

    # Give the calibrated null a deterministic 1 G value for table
    # interpolation and logarithmic plotting.
    zero_mask = magnitude <= 1.0e-12 * maximum
    if np.any(zero_mask):
        br[zero_mask] = 0.0
        bz[zero_mask] = minimum

    return br, bz


def write_moose_table(
    path: str | Path,
    r_m: np.ndarray,
    z_m: np.ndarray,
    values: np.ndarray,
) -> None:
    """Write one R-Z component in the MOOSE piecewise-bilinear table format."""
    r = _validate_axis("r_m", r_m)
    z = _validate_axis("z_m", z_m)
    field = np.asarray(values, dtype=float)
    expected_shape = (r.size, z.size)
    if field.shape != expected_shape:
        raise ValueError(
            f"table values have shape {field.shape}; expected {expected_shape}"
        )
    if not np.all(np.isfinite(field)):
        raise ValueError("table values must contain only finite numbers")

    output_path = Path(path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", encoding="utf-8") as handle:
        handle.write("AXIS X\n")
        handle.write(" ".join(f"{value:.8e}" for value in r) + "\n\n")
        handle.write("AXIS Y\n")
        handle.write(" ".join(f"{value:.8e}" for value in z) + "\n\n")
        handle.write("DATA\n")
        for z_index in range(z.size):
            handle.write(
                " ".join(f"{value:.8e}" for value in field[:, z_index]) + "\n"
            )


def plot_source_bfield(
    path: str | Path,
    r_m: np.ndarray,
    z_m: np.ndarray,
    br_t: np.ndarray,
    bz_t: np.ndarray,
    *,
    plasma_radius_m: float = 0.250,
) -> None:
    """Plot reference-style discrete magnitude bands and field direction."""
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
        raise ValueError("field components must contain only finite numbers")

    plasma_radius = float(plasma_radius_m)
    if not np.isfinite(plasma_radius) or not r[0] < plasma_radius < r[-1]:
        raise ValueError("plasma_radius_m must lie inside the radial plot domain")

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

    axis.set_xlim(r_mm[0], r_mm[-1])
    axis.set_ylim(z_mm[0], z_mm[-1])
    axis.set_aspect("equal", adjustable="box")
    axis.set_xlabel("R (mm)")
    axis.set_ylabel("Z (mm)")
    axis.set_title("S-N-S source-only total field (G) and direction")
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
        [
            "1.00",
            "1.64",
            "2.68",
            "4.39",
            "7.20",
            "11.79",
            "19.31",
            "31.62",
            "51.79",
            "84.83",
            "138.95",
            "227.58",
            "372.76",
            "610.54",
            "1000.00",
        ]
    )

    output_path = Path(path)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output_path, dpi=180)
    plt.close(figure)


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Generate an analytical source-only B-field on a full 400 mm by "
            "300 mm R-Z domain for MOOSE/Zapdos."
        )
    )
    parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    parser.add_argument("--plot-path", type=Path, default=DEFAULT_PLOT_PATH)
    parser.add_argument(
        "--nx",
        type=int,
        default=161,
        help="Number of radial table points (default: 161).",
    )
    parser.add_argument(
        "--ny",
        type=int,
        default=301,
        help=(
            "Number of axial table points (default: 301, giving 1 mm "
            "resolution across the thin near-source bands)."
        ),
    )
    parser.add_argument("--r-max-mm", type=float, default=400.0)
    parser.add_argument("--z-max-mm", type=float, default=300.0)
    parser.add_argument("--plasma-radius-mm", type=float, default=250.0)
    parser.add_argument("--source-r-mm", type=float, default=125.0)
    parser.add_argument("--min-gauss", type=float, default=1.0)
    parser.add_argument("--max-gauss", type=float, default=1000.0)
    parser.add_argument("--pole-separation-mm", type=float, default=40.0)
    parser.add_argument("--pole-depth-mm", type=float, default=6.0)
    parser.add_argument("--softening-mm", type=float, default=10.0)
    parser.add_argument("--null-offset-mm", type=float, default=10.0)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    if args.nx < 3 or args.ny < 3:
        raise ValueError("--nx and --ny must each be at least 3")

    r_max_m = 1.0e-3 * float(args.r_max_mm)
    z_max_m = 1.0e-3 * float(args.z_max_mm)
    plasma_radius_m = 1.0e-3 * float(args.plasma_radius_mm)
    source_r_m = 1.0e-3 * float(args.source_r_mm)
    if r_max_m <= 0.0 or z_max_m <= 0.0:
        raise ValueError("--r-max-mm and --z-max-mm must be positive")
    if not 0.0 < plasma_radius_m < r_max_m:
        raise ValueError("--plasma-radius-mm must lie inside the radial domain")

    r_m = np.linspace(0.0, r_max_m, args.nx)
    z_m = np.linspace(0.0, z_max_m, args.ny)
    br_t, bz_t = build_source_bfield(
        r_m,
        z_m,
        source_r_m=source_r_m,
        source_z_m=z_max_m,
        min_b_t=float(args.min_gauss) * GAUSS_TO_TESLA,
        max_b_t=float(args.max_gauss) * GAUSS_TO_TESLA,
        pole_separation_m=float(args.pole_separation_mm) * 1.0e-3,
        pole_depth_m=float(args.pole_depth_mm) * 1.0e-3,
        softening_m=float(args.softening_mm) * 1.0e-3,
        null_offset_m=float(args.null_offset_mm) * 1.0e-3,
    )

    bx_path = args.out_dir / "Bx_T.tbl"
    by_path = args.out_dir / "By_T.tbl"
    write_moose_table(bx_path, r_m, z_m, br_t)
    write_moose_table(by_path, r_m, z_m, bz_t)
    plot_source_bfield(
        args.plot_path,
        r_m,
        z_m,
        br_t,
        bz_t,
        plasma_radius_m=plasma_radius_m,
    )

    bmag_g = np.hypot(br_t, bz_t) / GAUSS_TO_TESLA
    top_j = len(z_m) - 1
    left_mask = r_m < source_r_m
    right_mask = r_m > source_r_m
    left_peak_i = np.flatnonzero(left_mask)[
        np.argmax(bmag_g[left_mask, top_j])
    ]
    right_peak_i = np.flatnonzero(right_mask)[
        np.argmax(bmag_g[right_mask, top_j])
    ]
    null_i = int(np.argmin(np.abs(r_m - source_r_m)))
    null_z_m = z_max_m - float(args.null_offset_mm) * 1.0e-3
    null_j = int(np.argmin(np.abs(z_m - null_z_m)))
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
        f"|B| range: {np.min(bmag_g):.2f}..{np.max(bmag_g):.2f} G; "
        f"S-N-S center R={1000.0 * source_r_m:.1f} mm"
    )
    print(
        "surface peaks: "
        f"R={1000.0 * r_m[left_peak_i]:.1f} mm "
        f"({bmag_g[left_peak_i, top_j]:.2f} G), "
        f"R={1000.0 * r_m[right_peak_i]:.1f} mm "
        f"({bmag_g[right_peak_i, top_j]:.2f} G)"
    )
    print(
        f"null: R={1000.0 * r_m[null_i]:.1f} mm, "
        f"Z={1000.0 * z_m[null_j]:.1f} mm, "
        f"|B|={bmag_g[null_i, null_j]:.2f} G"
    )
    profile_summary = []
    for distance_m, target_g in zip(
        REFERENCE_PROFILE_DISTANCE_M[1:7],
        REFERENCE_PROFILE_MAX_G[1:7],
    ):
        profile_j = int(
            np.argmin(np.abs(z_m - (z_max_m - distance_m)))
        )
        profile_summary.append(
            f"{1000.0 * distance_m:.0f} mm: "
            f"{np.max(bmag_g[:, profile_j]):.2f} G"
        )
    print("scaled axial profile (distance below source): " + ", ".join(profile_summary))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
