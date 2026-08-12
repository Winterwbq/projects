#!/usr/bin/env python3
"""Generate reduced, B-guided fast-electron SEE spatial-weight tables.

The model traces deterministic rays from the top-target racetrack.  The local
direction blends straight downward motion with the inward-selected magnetic
field direction.  Each path deposits a conservative exponentially attenuated
fraction into a finite Gaussian plume and is weighted by the prescribed Cu
neutral density.  It is a reduced nonlocal model, not a collision or full-orbit
Monte Carlo model.
"""
from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_NEUTRAL_TABLE = ROOT / "runs/zapdos_hpem_rz_30cm/moose_tables/n_Cu_m3.tbl"
DEFAULT_OUTPUT_ROOT = ROOT / "runs/zapdos_b_guided_see_30cm_original_density"
CASE_BFIELD_DIRS = {
    "source_only": ROOT
    / "runs/zapdos_hpem_rz_30cm_reference_source_only_curved_return/moose_tables",
    "four_coil": ROOT
    / "runs/zapdos_hpem_rz_30cm_reference_four_coil/moose_tables",
    "four_coil_img3092": ROOT
    / "runs/zapdos_hpem_rz_30cm_reference_four_coil_img3092/moose_tables",
}

ORIGINAL_DENSITY_FLOOR = 1.0e16
ORIGINAL_DENSITY_MULTIPLIER = 0.1
ORIGINAL_BACKGROUND_DENSITY = 1.0e17
ORIGINAL_MAGNETRON_PEAK = 3.0e18
ORIGINAL_MAGNETRON_CENTER_R = 0.150
ORIGINAL_MAGNETRON_CENTER_Z = 0.270
ORIGINAL_MAGNETRON_WIDTH_R = 0.080
ORIGINAL_MAGNETRON_WIDTH_Z = 0.090
DEFAULT_GUIDE_FIELD = 0.001
DEFAULT_LAUNCH_WIDTH = 0.015
DEFAULT_LOCAL_ATTENUATION_LENGTH = 0.006
DEFAULT_INITIAL_SIGMA = 0.003


def build_original_effective_density(
    r: np.ndarray, z: np.ndarray, table_density: np.ndarray
) -> np.ndarray:
    """Reconstruct the effective neutral-Cu field used by the reference input."""
    rr, zz = np.meshgrid(r, z, indexing="ij")
    magnetron = np.exp(
        -((rr - ORIGINAL_MAGNETRON_CENTER_R) / ORIGINAL_MAGNETRON_WIDTH_R) ** 2
        - ((zz - ORIGINAL_MAGNETRON_CENTER_Z) / ORIGINAL_MAGNETRON_WIDTH_Z) ** 2
    )
    return (
        ORIGINAL_DENSITY_MULTIPLIER
        * np.maximum(table_density, ORIGINAL_DENSITY_FLOOR)
        + ORIGINAL_BACKGROUND_DENSITY
        + ORIGINAL_MAGNETRON_PEAK * magnetron
    )


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(f"{name} axis must be one-dimensional with at least two points")
    if not np.all(np.isfinite(axis)):
        raise ValueError(f"{name} axis contains non-finite values")
    if not np.all(np.diff(axis) > 0.0):
        raise ValueError(f"{name} axis must be strictly increasing")
    return axis


def _validate_field(
    name: str, values: np.ndarray, r: np.ndarray, z: np.ndarray, *, nonnegative: bool
) -> np.ndarray:
    field = np.asarray(values, dtype=float)
    if field.shape != (r.size, z.size):
        raise ValueError(
            f"{name} shape {field.shape} does not match axes {(r.size, z.size)}"
        )
    if not np.all(np.isfinite(field)):
        raise ValueError(f"{name} contains non-finite values")
    if nonnegative and np.any(field < 0.0):
        raise ValueError(f"{name} must be nonnegative")
    return field


def read_moose_table(path: Path | str) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Read a two-axis MOOSE ``PiecewiseMultilinear`` table."""
    table_path = Path(path)
    lines = [line.strip() for line in table_path.read_text().splitlines() if line.strip()]
    if len(lines) < 6 or lines[0] != "AXIS X":
        raise ValueError(f"{table_path} must start with 'AXIS X'")
    try:
        axis_y_index = lines.index("AXIS Y")
        data_index = lines.index("DATA")
    except ValueError as error:
        raise ValueError(f"{table_path} is missing AXIS Y or DATA") from error
    if axis_y_index != 2 or data_index != 4:
        raise ValueError(f"{table_path} must contain one line per axis before DATA")

    r = _validate_axis("X", np.fromstring(lines[1], sep=" "))
    z = _validate_axis("Y", np.fromstring(lines[3], sep=" "))
    rows = [np.fromstring(line, sep=" ") for line in lines[5:]]
    if len(rows) != z.size or any(row.size != r.size for row in rows):
        raise ValueError(
            f"{table_path} DATA must have {z.size} rows and {r.size} columns"
        )
    values = np.vstack(rows).T
    _validate_field("table DATA", values, r, z, nonnegative=False)
    return r, z, values


def write_moose_table(
    path: Path | str, r: np.ndarray, z: np.ndarray, values: np.ndarray
) -> None:
    """Write an R-Z field in MOOSE ``PiecewiseMultilinear`` format."""
    r = _validate_axis("X", r)
    z = _validate_axis("Y", z)
    values = _validate_field("table DATA", values, r, z, nonnegative=False)
    table_path = Path(path)
    table_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "AXIS X",
        " ".join(f"{value:.12e}" for value in r),
        "",
        "AXIS Y",
        " ".join(f"{value:.12e}" for value in z),
        "",
        "DATA",
    ]
    lines.extend(
        " ".join(f"{value:.12e}" for value in values[:, iz])
        for iz in range(z.size)
    )
    table_path.write_text("\n".join(lines) + "\n")


def _trapezoid_weights(axis: np.ndarray) -> np.ndarray:
    weights = np.empty_like(axis)
    weights[0] = 0.5 * (axis[1] - axis[0])
    weights[-1] = 0.5 * (axis[-1] - axis[-2])
    weights[1:-1] = 0.5 * (axis[2:] - axis[:-2])
    return weights


def rz_quadrature_weights(r: np.ndarray, z: np.ndarray) -> np.ndarray:
    """Return trapezoidal axisymmetric volume weights ``2*pi*r*dr*dz``."""
    r = _validate_axis("r", r)
    z = _validate_axis("z", z)
    return (
        2.0
        * np.pi
        * r[:, None]
        * _trapezoid_weights(r)[:, None]
        * _trapezoid_weights(z)[None, :]
    )


def rz_volume_integral(r: np.ndarray, z: np.ndarray, values: np.ndarray) -> float:
    """Integrate a field over an axisymmetric R-Z volume."""
    r = _validate_axis("r", r)
    z = _validate_axis("z", z)
    values = _validate_field("integrand", values, r, z, nonnegative=False)
    return float(np.sum(values * rz_quadrature_weights(r, z)))


def normalize_rz_weight(
    r: np.ndarray,
    z: np.ndarray,
    raw_weight: np.ndarray,
    *,
    target_radius: float,
    deposition_efficiency: float,
) -> np.ndarray:
    """Normalize a nonnegative map to ``eta*pi*target_radius**2``."""
    r = _validate_axis("r", r)
    z = _validate_axis("z", z)
    raw = _validate_field("raw weight", raw_weight, r, z, nonnegative=True)
    if target_radius <= 0.0 or not np.isfinite(target_radius):
        raise ValueError("target_radius must be positive and finite")
    if not np.isfinite(deposition_efficiency) or not 0.0 <= deposition_efficiency <= 1.0:
        raise ValueError("deposition_efficiency must be in [0, 1]")
    integral = rz_volume_integral(r, z, raw)
    if integral <= 0.0:
        raise ValueError("raw weight has zero axisymmetric volume integral")
    desired = deposition_efficiency * np.pi * target_radius**2
    return raw * (desired / integral)


def interpolate_regular_grid(
    source_r: np.ndarray,
    source_z: np.ndarray,
    source_values: np.ndarray,
    target_r: np.ndarray,
    target_z: np.ndarray,
) -> np.ndarray:
    """Bilinearly interpolate a regular R-Z field without extrapolation."""
    source_r = _validate_axis("source r", source_r)
    source_z = _validate_axis("source z", source_z)
    source_values = _validate_field(
        "source values", source_values, source_r, source_z, nonnegative=False
    )
    target_r = _validate_axis("target r", target_r)
    target_z = _validate_axis("target z", target_z)
    tolerance = 1.0e-12
    if (
        target_r[0] < source_r[0] - tolerance
        or target_r[-1] > source_r[-1] + tolerance
        or target_z[0] < source_z[0] - tolerance
        or target_z[-1] > source_z[-1] + tolerance
    ):
        raise ValueError("target axes extend outside the source table")
    along_r = np.empty((target_r.size, source_z.size))
    for iz in range(source_z.size):
        along_r[:, iz] = np.interp(target_r, source_r, source_values[:, iz])
    result = np.empty((target_r.size, target_z.size))
    for ir in range(target_r.size):
        result[ir, :] = np.interp(target_z, source_z, along_r[ir, :])
    return result


def _bilinear_at(
    r: np.ndarray, z: np.ndarray, values: np.ndarray, point_r: float, point_z: float
) -> float:
    ir = int(np.clip(np.searchsorted(r, point_r) - 1, 0, r.size - 2))
    iz = int(np.clip(np.searchsorted(z, point_z) - 1, 0, z.size - 2))
    fr = (point_r - r[ir]) / (r[ir + 1] - r[ir])
    fz = (point_z - z[iz]) / (z[iz + 1] - z[iz])
    return float(
        (1.0 - fr) * (1.0 - fz) * values[ir, iz]
        + fr * (1.0 - fz) * values[ir + 1, iz]
        + (1.0 - fr) * fz * values[ir, iz + 1]
        + fr * fz * values[ir + 1, iz + 1]
    )


def _guided_direction(
    br: float,
    bz: float,
    guide_field: float,
    max_radial_to_axial_ratio: float = 1.0,
) -> np.ndarray:
    """Return a bounded inward/downward direction for the reduced guide model.

    This is an empirical transport direction rather than a signed magnetic
    field-line tangent.  Absolute field components select the inward and
    chamber-entering branches, while the radial cap prevents a strong target
    field from turning the reduced fast-electron path nearly horizontal.
    """
    magnitude = float(np.hypot(br, bz))
    if magnitude <= 1.0e-30:
        return np.array([0.0, -1.0])
    if max_radial_to_axial_ratio <= 0.0 or not np.isfinite(max_radial_to_axial_ratio):
        raise ValueError("max_radial_to_axial_ratio must be positive and finite")
    field_direction = -np.array([abs(br), abs(bz)], dtype=float) / magnitude
    chi = magnitude**2 / (magnitude**2 + guide_field**2)
    direction = (1.0 - chi) * np.array([0.0, -1.0]) + chi * field_direction
    maximum_radial = max_radial_to_axial_ratio * abs(direction[1])
    direction[0] = max(direction[0], -maximum_radial)
    norm = float(np.linalg.norm(direction))
    if norm <= 1.0e-12:
        return np.array([0.0, -1.0])
    return direction / norm


def trace_guided_source(
    r: np.ndarray,
    z: np.ndarray,
    n_cu: np.ndarray,
    br: np.ndarray,
    bz: np.ndarray,
    *,
    launch_radius: float = 0.15,
    launch_width: float = DEFAULT_LAUNCH_WIDTH,
    launch_count: int = 21,
    guide_field: float = DEFAULT_GUIDE_FIELD,
    step_length: float = 0.0025,
    fast_attenuation_length: float = 0.20,
    local_deposition_fraction: float = 0.65,
    local_attenuation_length: float = DEFAULT_LOCAL_ATTENUATION_LENGTH,
    max_radial_to_axial_ratio: float = 1.0,
    initial_sigma: float = DEFAULT_INITIAL_SIGMA,
    spread_angle_degrees: float = 5.0,
    neutral_power: float = 1.0,
    max_path_length: float = 0.90,
    survival_floor: float = 1.0e-4,
    return_metadata: bool = False,
) -> np.ndarray | tuple[np.ndarray, dict[str, Any]]:
    """Trace the deterministic reduced fast-electron ensemble.

    The returned array is the unnormalized raw deposition map.  With
    ``return_metadata=True``, the second return value includes the geometric
    deposition efficiency and launch-weighted exit length.
    """
    r = _validate_axis("r", r)
    z = _validate_axis("z", z)
    n_cu = _validate_field("n_cu", n_cu, r, z, nonnegative=True)
    br = _validate_field("br", br, r, z, nonnegative=False)
    bz = _validate_field("bz", bz, r, z, nonnegative=False)
    positive_parameters = {
        "launch_width": launch_width,
        "guide_field": guide_field,
        "step_length": step_length,
        "fast_attenuation_length": fast_attenuation_length,
        "local_attenuation_length": local_attenuation_length,
        "max_radial_to_axial_ratio": max_radial_to_axial_ratio,
        "initial_sigma": initial_sigma,
        "max_path_length": max_path_length,
    }
    for name, value in positive_parameters.items():
        if not np.isfinite(value) or value <= 0.0:
            raise ValueError(f"{name} must be positive and finite")
    if launch_count < 1:
        raise ValueError("launch_count must be at least one")
    if neutral_power < 0.0 or not np.isfinite(neutral_power):
        raise ValueError("neutral_power must be finite and nonnegative")
    if not 0.0 <= local_deposition_fraction <= 1.0:
        raise ValueError("local_deposition_fraction must be in [0, 1]")
    if not 0.0 <= survival_floor < 1.0:
        raise ValueError("survival_floor must be in [0, 1)")

    launch_min = max(r[0], launch_radius - 3.0 * launch_width)
    launch_max = min(r[-1], launch_radius + 3.0 * launch_width)
    launch_radii = np.linspace(launch_min, launch_max, launch_count)
    launch_weights = launch_radii * np.exp(
        -0.5 * ((launch_radii - launch_radius) / launch_width) ** 2
    )
    if float(np.sum(launch_weights)) <= 0.0:
        raise ValueError("launch ensemble has zero annular weight")
    launch_weights /= np.sum(launch_weights)

    neutral_max = float(np.max(n_cu))
    if neutral_max <= 0.0:
        raise ValueError("n_cu is zero everywhere")
    neutral_weight = (n_cu / neutral_max) ** neutral_power
    quadrature = rz_quadrature_weights(r, z)
    rr, zz = np.meshgrid(r, z, indexing="ij")
    raw = np.zeros_like(n_cu)
    exit_lengths: list[float] = []
    efficiencies: list[float] = []
    spread_tangent = np.tan(np.deg2rad(spread_angle_degrees))
    z_start = float(z[-1] - min(0.1 * step_length, 0.1 * (z[-1] - z[-2])))

    for launch_r, launch_weight in zip(launch_radii, launch_weights):
        point_r = float(launch_r)
        point_z = z_start
        path_length = 0.0
        local_survival = 1.0
        guided_survival = 1.0
        combined_survival = 1.0
        while path_length < max_path_length and combined_survival > survival_floor:
            local_br = _bilinear_at(r, z, br, point_r, point_z)
            local_bz = _bilinear_at(r, z, bz, point_r, point_z)
            direction = _guided_direction(
                local_br,
                local_bz,
                guide_field,
                max_radial_to_axial_ratio=max_radial_to_axial_ratio,
            )
            ds = min(step_length, max_path_length - path_length)
            next_r = point_r + ds * direction[0]
            next_z = point_z + ds * direction[1]
            midpoint_r = 0.5 * (point_r + next_r)
            midpoint_z = 0.5 * (point_z + next_z)
            next_local_survival = local_survival * np.exp(
                -ds / local_attenuation_length
            )
            next_guided_survival = guided_survival * np.exp(
                -ds / fast_attenuation_length
            )
            deposited_fraction = local_deposition_fraction * (
                local_survival - next_local_survival
            ) + (1.0 - local_deposition_fraction) * (
                guided_survival - next_guided_survival
            )
            sigma = np.sqrt(
                initial_sigma**2 + (path_length + 0.5 * ds) ** 2 * spread_tangent**2
            )
            kernel = np.exp(
                -0.5
                * (
                    ((rr - midpoint_r) / sigma) ** 2
                    + ((zz - midpoint_z) / sigma) ** 2
                )
            )
            kernel_integral = float(np.sum(kernel * quadrature))
            if kernel_integral > 0.0:
                raw += (
                    float(launch_weight)
                    * deposited_fraction
                    * neutral_weight
                    * kernel
                    / kernel_integral
                )
            path_length += ds
            local_survival = next_local_survival
            guided_survival = next_guided_survival
            combined_survival = (
                local_deposition_fraction * local_survival
                + (1.0 - local_deposition_fraction) * guided_survival
            )
            point_r = next_r
            point_z = next_z
            if point_r < r[0] or point_r > r[-1] or point_z < z[0] or point_z > z[-1]:
                break
        exit_lengths.append(path_length)
        efficiencies.append(1.0 - combined_survival)

    metadata: dict[str, Any] = {
        "deposition_efficiency": float(np.dot(launch_weights, efficiencies)),
        "mean_exit_length": float(np.dot(launch_weights, exit_lengths)),
        "exit_lengths": np.asarray(exit_lengths),
        "launch_radii": launch_radii,
        "launch_weights": launch_weights,
    }
    if return_metadata:
        return raw, metadata
    return raw


def generate_case(
    case: str,
    *,
    neutral_table: Path = DEFAULT_NEUTRAL_TABLE,
    output_root: Path = DEFAULT_OUTPUT_ROOT,
    **trace_options: Any,
) -> tuple[Path, dict[str, Any]]:
    """Generate one case table and return its path and trace metadata."""
    if case not in CASE_BFIELD_DIRS:
        raise ValueError(f"unknown case {case!r}; choose from {sorted(CASE_BFIELD_DIRS)}")
    neutral_r, neutral_z, n_cu_all = read_moose_table(neutral_table)
    r_mask = neutral_r <= 0.25 + 1.0e-12
    z_mask = neutral_z <= 0.30 + 1.0e-12
    r = neutral_r[r_mask]
    z = neutral_z[z_mask]
    table_density = n_cu_all[np.ix_(r_mask, z_mask)]
    n_cu = build_original_effective_density(r, z, table_density)

    bfield_dir = CASE_BFIELD_DIRS[case]
    br_r, br_z, br_values = read_moose_table(bfield_dir / "Bx_T.tbl")
    bz_r, bz_z, bz_values = read_moose_table(bfield_dir / "By_T.tbl")
    if not np.array_equal(br_r, bz_r) or not np.array_equal(br_z, bz_z):
        raise ValueError(f"{case} Bx and By tables do not share axes")
    total_br = interpolate_regular_grid(br_r, br_z, br_values, r, z)
    total_bz = interpolate_regular_grid(bz_r, bz_z, bz_values, r, z)

    source_dir = CASE_BFIELD_DIRS["source_only"]
    source_br_r, source_br_z, source_br_values = read_moose_table(
        source_dir / "Bx_T.tbl"
    )
    source_bz_r, source_bz_z, source_bz_values = read_moose_table(
        source_dir / "By_T.tbl"
    )
    source_br = interpolate_regular_grid(
        source_br_r, source_br_z, source_br_values, r, z
    )
    source_bz = interpolate_regular_grid(
        source_bz_r, source_bz_z, source_bz_values, r, z
    )
    external_strength = np.hypot(total_br - source_br, total_bz - source_bz)
    # The local magnetron field controls the near-target component.  Only the
    # added external-field magnitude steers the long-range reduced transport;
    # encode its calibrated inward/downward bias with equal negative components.
    br = -external_strength
    bz = -external_strength

    raw, metadata = trace_guided_source(
        r, z, n_cu, br, bz, return_metadata=True, **trace_options
    )
    normalized = normalize_rz_weight(
        r,
        z,
        raw,
        target_radius=0.25,
        deposition_efficiency=metadata["deposition_efficiency"],
    )
    output_path = output_root / case / "moose_tables/see_spatial_weight_m-1.tbl"
    write_moose_table(output_path, r, z, normalized)
    metadata.update(
        {
            "case": case,
            "output_path": output_path,
            "volume_integral": rz_volume_integral(r, z, normalized),
        }
    )
    return output_path, metadata


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--case",
        action="append",
        choices=sorted(CASE_BFIELD_DIRS),
        help="Case to generate; repeat as needed. Default: all three cases.",
    )
    parser.add_argument("--neutral-table", type=Path, default=DEFAULT_NEUTRAL_TABLE)
    parser.add_argument("--output-root", type=Path, default=DEFAULT_OUTPUT_ROOT)
    parser.add_argument("--launch-radius", type=float, default=0.15)
    parser.add_argument("--launch-width", type=float, default=DEFAULT_LAUNCH_WIDTH)
    parser.add_argument("--launch-count", type=int, default=21)
    parser.add_argument(
        "--guide-field",
        type=float,
        default=DEFAULT_GUIDE_FIELD,
        help=(
            "External B-guidance transition field in tesla. This is the same "
            "default used by trace_guided_source."
        ),
    )
    parser.add_argument("--step-length", type=float, default=0.0025)
    parser.add_argument("--fast-attenuation-length", type=float, default=0.20)
    parser.add_argument("--local-deposition-fraction", type=float, default=0.65)
    parser.add_argument(
        "--local-attenuation-length",
        type=float,
        default=DEFAULT_LOCAL_ATTENUATION_LENGTH,
    )
    parser.add_argument("--max-radial-to-axial-ratio", type=float, default=1.0)
    parser.add_argument("--initial-sigma", type=float, default=DEFAULT_INITIAL_SIGMA)
    parser.add_argument("--spread-angle-degrees", type=float, default=5.0)
    parser.add_argument("--neutral-power", type=float, default=1.0)
    parser.add_argument("--max-path-length", type=float, default=0.90)
    parser.add_argument("--survival-floor", type=float, default=1.0e-4)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    cases = args.case or list(CASE_BFIELD_DIRS)
    trace_options = {
        "launch_radius": args.launch_radius,
        "launch_width": args.launch_width,
        "launch_count": args.launch_count,
        "guide_field": args.guide_field,
        "step_length": args.step_length,
        "fast_attenuation_length": args.fast_attenuation_length,
        "local_deposition_fraction": args.local_deposition_fraction,
        "local_attenuation_length": args.local_attenuation_length,
        "max_radial_to_axial_ratio": args.max_radial_to_axial_ratio,
        "initial_sigma": args.initial_sigma,
        "spread_angle_degrees": args.spread_angle_degrees,
        "neutral_power": args.neutral_power,
        "max_path_length": args.max_path_length,
        "survival_floor": args.survival_floor,
    }
    for case in cases:
        path, metadata = generate_case(
            case,
            neutral_table=args.neutral_table,
            output_root=args.output_root,
            **trace_options,
        )
        print(
            f"{case}: {path} "
            f"eta={metadata['deposition_efficiency']:.6f} "
            f"mean_exit_length={metadata['mean_exit_length']:.6f} m"
        )


if __name__ == "__main__":
    main()
