#!/usr/bin/env python3
"""Generate reduced SEE source maps by tracing the real signed total B field."""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_NEUTRAL_TABLE = ROOT / "runs/zapdos_hpem_rz_30cm/moose_tables/n_Cu_m3.tbl"
COMMON_OUTPUT_ROOT = (
    ROOT
    / "runs/zapdos_real_b_fieldline_see_30cm_common_fraction_newsourceB_two_lobe"
)
CASE_OUTPUT_ROOT = (
    ROOT / "runs/zapdos_real_b_fieldline_see_30cm_case_fraction_newsourceB_two_lobe"
)
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
DEFAULT_LAUNCH_WIDTH = 0.015
DEFAULT_LAUNCH_PROFILE = "double_gaussian"
DEFAULT_LAUNCH_LOBE_OFFSET = 0.045
DEFAULT_LAUNCH_LOBE_WIDTH = 0.010
DEFAULT_LOCAL_ATTENUATION_LENGTH = 0.006
DEFAULT_INITIAL_SIGMA = 0.003
DEFAULT_SPREAD_ANGLE_DEGREES = 20.0
DEFAULT_SOURCE_ONLY_INNER_TRACE_RADIUS = 0.05
DEFAULT_SOURCE_ONLY_FAST_ATTENUATION_LENGTH = 0.05
DEFAULT_SOURCE_ONLY_CENTER_RADIUS = 0.15
DEFAULT_SOURCE_ONLY_BRIDGE_TARGET_RATIO = 0.30
DEFAULT_SOURCE_ONLY_BRIDGE_RADIAL_WIDTH = 0.010
DEFAULT_SOURCE_ONLY_BRIDGE_AXIAL_LENGTH = 0.006
DEFAULT_SOURCE_ONLY_COLUMN_TOP_RATIO = 0.0
DEFAULT_SOURCE_ONLY_COLUMN_RADIAL_WIDTH = 0.010
DEFAULT_SOURCE_ONLY_COLUMN_ATTENUATION_LENGTH = 0.20


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
    name: str,
    values: np.ndarray,
    r: np.ndarray,
    z: np.ndarray,
    *,
    nonnegative: bool,
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


def _validate_fraction(name: str, value: float) -> float:
    if not np.isfinite(value) or not 0.0 <= value <= 1.0:
        raise ValueError(f"{name} must be finite and lie in [0, 1]")
    return float(value)


def fractions_for_mode(
    mode: str,
    *,
    common_fraction: float = 0.65,
    source_only_fraction: float = 0.95,
    four_coil_fraction: float = 0.70,
    four_coil_img3092_fraction: float = 0.55,
) -> dict[str, float]:
    """Return the local-deposition fraction assigned to each B-field case."""
    if mode == "common":
        value = _validate_fraction("common_fraction", common_fraction)
        return {case: value for case in CASE_BFIELD_DIRS}
    if mode == "case":
        return {
            "source_only": _validate_fraction(
                "source_only_fraction", source_only_fraction
            ),
            "four_coil": _validate_fraction(
                "four_coil_fraction", four_coil_fraction
            ),
            "four_coil_img3092": _validate_fraction(
                "four_coil_img3092_fraction", four_coil_img3092_fraction
            ),
        }
    raise ValueError("fraction mode must be 'common' or 'case'")


def output_root_for_mode(mode: str) -> Path:
    """Choose the original-density output root for a fraction mode."""
    if mode == "common":
        return COMMON_OUTPUT_ROOT
    if mode == "case":
        return CASE_OUTPUT_ROOT
    raise ValueError("fraction mode must be 'common' or 'case'")


@dataclass(frozen=True)
class RK2Step:
    """One accepted full-length midpoint-RK2 path proposal."""

    point: np.ndarray
    midpoint: np.ndarray
    start_direction: np.ndarray
    midpoint_direction: np.ndarray
    selected_field_tangent: np.ndarray | None


@dataclass(frozen=True)
class RayTrace:
    """Accepted path segments and conservative energy accounting for one ray."""

    points: np.ndarray
    segment_midpoints: np.ndarray
    segment_lengths: np.ndarray
    deposited_fractions: np.ndarray
    termination: str
    remaining_survival: float
    deposited_fraction: float
    path_length: float


@dataclass(frozen=True)
class SourceTraceMetadata:
    """Launch-weighted plasma deposition and ray-loss fractions."""

    plasma_efficiency: float
    target_efficiency: float
    bottom_efficiency: float
    side_efficiency: float
    path_limit_efficiency: float
    invalid_efficiency: float
    cutoff_efficiency: float
    mean_path_length: float
    termination_counts: dict[str, int]

    @property
    def accounted_efficiency(self) -> float:
        return (
            self.plasma_efficiency
            + self.target_efficiency
            + self.bottom_efficiency
            + self.side_efficiency
            + self.path_limit_efficiency
            + self.invalid_efficiency
            + self.cutoff_efficiency
        )


def select_field_tangent(
    br: float,
    bz: float,
    previous_field_tangent: np.ndarray | None,
) -> np.ndarray:
    """Orient a nonzero signed B tangent continuously into the chamber/path."""
    components = np.asarray([br, bz], dtype=float)
    magnitude = float(np.linalg.norm(components))
    if not np.all(np.isfinite(components)) or magnitude <= 0.0:
        raise ValueError("field tangent requires a finite nonzero B field")
    unsigned = components / magnitude
    reference = (
        np.asarray([0.0, -1.0])
        if previous_field_tangent is None
        else np.asarray(previous_field_tangent, dtype=float)
    )
    if reference.shape != (2,) or not np.all(np.isfinite(reference)):
        raise ValueError("previous field tangent must be a finite two-vector")
    return unsigned if float(np.dot(unsigned, reference)) >= 0.0 else -unsigned


def effective_direction(
    br: float,
    bz: float,
    previous_field_tangent: np.ndarray | None,
    *,
    transition_field: float,
    field_floor: float,
) -> tuple[np.ndarray, np.ndarray | None]:
    """Blend weak-field downward motion with the continuous real-B tangent."""
    if not np.isfinite(transition_field) or transition_field <= 0.0:
        raise ValueError("transition_field must be positive and finite")
    if not np.isfinite(field_floor) or field_floor < 0.0:
        raise ValueError("field_floor must be finite and nonnegative")
    components = np.asarray([br, bz], dtype=float)
    if not np.all(np.isfinite(components)):
        raise ValueError("B components must be finite")
    magnitude = float(np.linalg.norm(components))
    if magnitude <= field_floor:
        return np.asarray([0.0, -1.0]), previous_field_tangent
    selected = select_field_tangent(br, bz, previous_field_tangent)
    chi = magnitude**2 / (magnitude**2 + transition_field**2)
    blended = (1.0 - chi) * np.asarray([0.0, -1.0]) + chi * selected
    norm = float(np.linalg.norm(blended))
    if norm <= 1.0e-14:
        return np.asarray([0.0, -1.0]), selected
    return blended / norm, selected


def _bilinear_at(
    r: np.ndarray,
    z: np.ndarray,
    values: np.ndarray,
    point: np.ndarray,
    *,
    axisymmetric_radial_component: bool = False,
) -> float:
    """Interpolate an R-Z field, extending it through the symmetry axis."""
    point = np.asarray(point, dtype=float)
    if point.shape != (2,) or not np.all(np.isfinite(point)):
        raise ValueError("interpolation point must be a finite two-vector")
    signed_r, point_z = float(point[0]), float(point[1])
    point_r = abs(signed_r)
    tolerance = 1.0e-12
    if (
        point_r > r[-1] + tolerance
        or point_z < z[0] - tolerance
        or point_z > z[-1] + tolerance
    ):
        raise ValueError("interpolation point lies outside the R-Z table")
    point_r = float(np.clip(point_r, r[0], r[-1]))
    point_z = float(np.clip(point_z, z[0], z[-1]))
    ir = int(np.clip(np.searchsorted(r, point_r) - 1, 0, r.size - 2))
    iz = int(np.clip(np.searchsorted(z, point_z) - 1, 0, z.size - 2))
    fr = (point_r - r[ir]) / (r[ir + 1] - r[ir])
    fz = (point_z - z[iz]) / (z[iz + 1] - z[iz])
    result = float(
        (1.0 - fr) * (1.0 - fz) * values[ir, iz]
        + fr * (1.0 - fz) * values[ir + 1, iz]
        + (1.0 - fr) * fz * values[ir, iz + 1]
        + fr * fz * values[ir + 1, iz + 1]
    )
    if axisymmetric_radial_component and signed_r < 0.0:
        result = -result
    return result


def midpoint_rk2_step(
    *,
    point: np.ndarray,
    previous_field_tangent: np.ndarray | None,
    step_length: float,
    r: np.ndarray,
    z: np.ndarray,
    br: np.ndarray,
    bz: np.ndarray,
    transition_field: float,
    field_floor: float,
) -> RK2Step:
    """Advance one fixed arc-length step with explicit midpoint RK2."""
    point = np.asarray(point, dtype=float)
    if point.shape != (2,) or not np.all(np.isfinite(point)):
        raise ValueError("point must be a finite two-vector")
    if not np.isfinite(step_length) or step_length <= 0.0:
        raise ValueError("step_length must be positive and finite")
    if br.shape != (r.size, z.size) or bz.shape != br.shape:
        raise ValueError("B arrays must match the R-Z grid")
    start_direction, start_tangent = effective_direction(
        _bilinear_at(r, z, br, point, axisymmetric_radial_component=True),
        _bilinear_at(r, z, bz, point),
        previous_field_tangent,
        transition_field=transition_field,
        field_floor=field_floor,
    )
    midpoint = point + 0.5 * step_length * start_direction
    midpoint_direction, midpoint_tangent = effective_direction(
        _bilinear_at(r, z, br, midpoint, axisymmetric_radial_component=True),
        _bilinear_at(r, z, bz, midpoint),
        start_tangent,
        transition_field=transition_field,
        field_floor=field_floor,
    )
    return RK2Step(
        point=point + step_length * midpoint_direction,
        midpoint=midpoint,
        start_direction=start_direction,
        midpoint_direction=midpoint_direction,
        selected_field_tangent=midpoint_tangent,
    )


def _first_loss_crossing(
    start: np.ndarray,
    end: np.ndarray,
    *,
    r_max: float,
    z_max: float,
    target_enabled: bool,
    inner_radial_loss: float | None = None,
) -> tuple[float, str] | None:
    """Return the first loss-boundary crossing fraction of a straight segment."""
    start = np.asarray(start, dtype=float)
    end = np.asarray(end, dtype=float)
    delta = end - start
    crossings: list[tuple[float, str]] = []
    tolerance = 1.0e-12
    if end[1] <= 0.0 < start[1] and delta[1] != 0.0:
        crossings.append(((0.0 - start[1]) / delta[1], "bottom"))
    if target_enabled and end[1] >= z_max > start[1] and delta[1] != 0.0:
        crossings.append(((z_max - start[1]) / delta[1], "target"))
    if (
        abs(start[0]) >= r_max - tolerance
        and abs(end[0]) > abs(start[0])
    ):
        crossings.append((0.0, "side"))
    elif abs(end[0]) >= r_max > abs(start[0]) and delta[0] != 0.0:
        boundary = np.copysign(r_max, end[0])
        crossings.append(((boundary - start[0]) / delta[0], "side"))
    if inner_radial_loss is not None:
        if not 0.0 <= inner_radial_loss < r_max:
            raise ValueError("inner_radial_loss must lie in [0, r_max)")
        start_radius = abs(start[0])
        end_radius = abs(end[0])
        if start_radius <= inner_radial_loss + tolerance and end_radius < start_radius:
            crossings.append((0.0, "side"))
        elif end_radius <= inner_radial_loss < start_radius:
            boundary = np.copysign(inner_radial_loss, start[0])
            crossings.append(((boundary - start[0]) / delta[0], "side"))
    valid = [(fraction, label) for fraction, label in crossings if 0.0 <= fraction <= 1.0]
    return min(valid, default=None, key=lambda item: item[0])


def trace_single_ray(
    *,
    r: np.ndarray,
    z: np.ndarray,
    br: np.ndarray,
    bz: np.ndarray,
    launch_radius: float,
    step_length: float,
    transition_field: float,
    field_floor: float,
    local_fraction: float,
    local_attenuation_length: float,
    fast_attenuation_length: float,
    max_path_length: float,
    survival_floor: float,
    inner_radial_loss: float | None = None,
) -> RayTrace:
    """Trace one reduced fast-electron ray and conserve its launch weight."""
    positive = {
        "step_length": step_length,
        "transition_field": transition_field,
        "local_attenuation_length": local_attenuation_length,
        "fast_attenuation_length": fast_attenuation_length,
        "max_path_length": max_path_length,
    }
    for name, value in positive.items():
        if not np.isfinite(value) or value <= 0.0:
            raise ValueError(f"{name} must be positive and finite")
    if not 0.0 <= local_fraction <= 1.0:
        raise ValueError("local_fraction must be in [0, 1]")
    if not 0.0 <= survival_floor < 1.0:
        raise ValueError("survival_floor must be in [0, 1)")
    if not r[0] == 0.0 or launch_radius < 0.0 or launch_radius > r[-1]:
        raise ValueError("launch radius must lie in an R-Z grid beginning at r=0")
    if inner_radial_loss is not None and not 0.0 <= inner_radial_loss < launch_radius:
        raise ValueError("inner_radial_loss must lie in [0, launch_radius)")

    z_start = float(z[-1])
    point = np.asarray([launch_radius, z_start], dtype=float)
    physical_points = [np.asarray([abs(point[0]), point[1]])]
    segment_midpoints: list[np.ndarray] = []
    segment_lengths: list[float] = []
    deposited_fractions: list[float] = []
    previous_tangent: np.ndarray | None = None
    local_survival = 1.0
    guided_survival = 1.0
    path_length = 0.0
    entered = False
    termination = "path_limit"

    while path_length < max_path_length:
        remaining_step = min(step_length, max_path_length - path_length)
        try:
            proposal = midpoint_rk2_step(
                point=point,
                previous_field_tangent=previous_tangent,
                step_length=remaining_step,
                r=r,
                z=z,
                br=br,
                bz=bz,
                transition_field=transition_field,
                field_floor=field_floor,
            )
        except ValueError:
            try:
                fallback_direction, fallback_tangent = effective_direction(
                    _bilinear_at(
                        r, z, br, point, axisymmetric_radial_component=True
                    ),
                    _bilinear_at(r, z, bz, point),
                    previous_tangent,
                    transition_field=transition_field,
                    field_floor=field_floor,
                )
            except ValueError:
                termination = "invalid"
                break
            fallback_point = point + remaining_step * fallback_direction
            fallback_crossing = _first_loss_crossing(
                point,
                fallback_point,
                r_max=float(r[-1]),
                z_max=float(z[-1]),
                target_enabled=entered,
                inner_radial_loss=inner_radial_loss,
            )
            if fallback_crossing is None:
                termination = "invalid"
                break
            proposal = RK2Step(
                point=fallback_point,
                midpoint=point + 0.5 * remaining_step * fallback_direction,
                start_direction=fallback_direction,
                midpoint_direction=fallback_direction,
                selected_field_tangent=fallback_tangent,
            )

        crossing = _first_loss_crossing(
            point,
            proposal.point,
            r_max=float(r[-1]),
            z_max=float(z[-1]),
            target_enabled=entered,
            inner_radial_loss=inner_radial_loss,
        )
        fraction, loss_label = crossing if crossing is not None else (1.0, "")
        accepted_length = remaining_step * fraction
        end_point = point + fraction * (proposal.point - point)
        accepted_midpoint = 0.5 * (point + end_point)

        local_next = local_survival * np.exp(
            -accepted_length / local_attenuation_length
        )
        guided_next = guided_survival * np.exp(
            -accepted_length / fast_attenuation_length
        )
        deposited = local_fraction * (local_survival - local_next) + (
            1.0 - local_fraction
        ) * (guided_survival - guided_next)

        segment_midpoints.append(
            np.asarray([abs(accepted_midpoint[0]), accepted_midpoint[1]])
        )
        segment_lengths.append(accepted_length)
        deposited_fractions.append(float(deposited))
        physical_points.append(np.asarray([abs(end_point[0]), end_point[1]]))
        path_length += accepted_length
        local_survival = float(local_next)
        guided_survival = float(guided_next)

        if crossing is not None:
            termination = loss_label
            point = end_point
            break

        point = proposal.point
        previous_tangent = proposal.selected_field_tangent
        entered = entered or point[1] < z_start - max(1.0e-8, 0.1 * step_length)
        combined_survival = local_fraction * local_survival + (
            1.0 - local_fraction
        ) * guided_survival
        if combined_survival <= survival_floor:
            termination = "cutoff"
            break
        if path_length >= max_path_length - 1.0e-14:
            termination = "path_limit"
            break

    remaining_survival = local_fraction * local_survival + (
        1.0 - local_fraction
    ) * guided_survival
    deposited_total = float(np.sum(deposited_fractions))
    return RayTrace(
        points=np.asarray(physical_points),
        segment_midpoints=np.asarray(segment_midpoints),
        segment_lengths=np.asarray(segment_lengths),
        deposited_fractions=np.asarray(deposited_fractions),
        termination=termination,
        remaining_survival=float(remaining_survival),
        deposited_fraction=deposited_total,
        path_length=float(path_length),
    )


def _trapezoid_weights(axis: np.ndarray) -> np.ndarray:
    weights = np.empty_like(axis)
    weights[0] = 0.5 * (axis[1] - axis[0])
    weights[-1] = 0.5 * (axis[-1] - axis[-2])
    weights[1:-1] = 0.5 * (axis[2:] - axis[:-2])
    return weights


def build_launch_ensemble(
    *,
    r_min: float,
    r_max: float,
    launch_radius: float,
    launch_width: float,
    launch_count: int,
    launch_profile: str = DEFAULT_LAUNCH_PROFILE,
    launch_lobe_offset: float = DEFAULT_LAUNCH_LOBE_OFFSET,
    launch_lobe_width: float = DEFAULT_LAUNCH_LOBE_WIDTH,
) -> tuple[np.ndarray, np.ndarray]:
    """Return normalized axisymmetric target-launch radii and weights."""
    values = (r_min, r_max, launch_radius, launch_width)
    if not all(np.isfinite(value) for value in values) or r_max <= r_min:
        raise ValueError("launch-domain values must be finite with r_max > r_min")
    if launch_count < 1:
        raise ValueError("launch_count must be at least one")
    if launch_width <= 0.0:
        raise ValueError("launch_width must be positive")
    if not r_min <= launch_radius <= r_max:
        raise ValueError("launch_radius must lie inside the radial domain")

    if launch_profile == "single_gaussian":
        launch_min = max(r_min, launch_radius - 3.0 * launch_width)
        launch_max = min(r_max, launch_radius + 3.0 * launch_width)
        launch_radii = np.linspace(launch_min, launch_max, launch_count)
        envelope = np.exp(
            -0.5 * ((launch_radii - launch_radius) / launch_width) ** 2
        )
    elif launch_profile == "double_gaussian":
        if not np.isfinite(launch_lobe_offset) or launch_lobe_offset <= 0.0:
            raise ValueError("launch_lobe_offset must be positive and finite")
        if not np.isfinite(launch_lobe_width) or launch_lobe_width <= 0.0:
            raise ValueError("launch_lobe_width must be positive and finite")
        left_center = launch_radius - launch_lobe_offset
        right_center = launch_radius + launch_lobe_offset
        if left_center < r_min or right_center > r_max:
            raise ValueError("both launch lobes must lie inside the radial domain")
        launch_min = max(r_min, left_center - 3.0 * launch_lobe_width)
        launch_max = min(r_max, right_center + 3.0 * launch_lobe_width)
        launch_radii = np.linspace(launch_min, launch_max, launch_count)
        envelope = np.exp(
            -0.5 * ((launch_radii - left_center) / launch_lobe_width) ** 2
        ) + np.exp(
            -0.5 * ((launch_radii - right_center) / launch_lobe_width) ** 2
        )
    else:
        raise ValueError(
            "launch_profile must be 'single_gaussian' or 'double_gaussian'"
        )

    launch_weights = launch_radii * envelope
    total = float(np.sum(launch_weights))
    if total <= 0.0 or not np.isfinite(total):
        raise ValueError("launch ensemble has zero or non-finite annular weight")
    return launch_radii, launch_weights / total


def trace_real_b_source(
    r: np.ndarray,
    z: np.ndarray,
    n_cu: np.ndarray,
    br: np.ndarray,
    bz: np.ndarray,
    *,
    launch_radius: float = 0.15,
    launch_width: float = DEFAULT_LAUNCH_WIDTH,
    launch_count: int = 21,
    launch_profile: str = DEFAULT_LAUNCH_PROFILE,
    launch_lobe_offset: float = DEFAULT_LAUNCH_LOBE_OFFSET,
    launch_lobe_width: float = DEFAULT_LAUNCH_LOBE_WIDTH,
    transition_field: float = 0.001,
    field_floor: float = 1.0e-12,
    step_length: float = 0.0025,
    fast_attenuation_length: float = 0.20,
    local_fraction: float = 0.65,
    local_attenuation_length: float = DEFAULT_LOCAL_ATTENUATION_LENGTH,
    initial_sigma: float = DEFAULT_INITIAL_SIGMA,
    spread_angle_degrees: float = DEFAULT_SPREAD_ANGLE_DEGREES,
    neutral_power: float = 1.0,
    max_path_length: float = 0.90,
    survival_floor: float = 1.0e-4,
    inner_radial_loss: float | None = None,
) -> tuple[np.ndarray, SourceTraceMetadata]:
    """Trace an annular ensemble and construct an unnormalized deposition map."""
    r = np.asarray(r, dtype=float)
    z = np.asarray(z, dtype=float)
    n_cu = np.asarray(n_cu, dtype=float)
    br = np.asarray(br, dtype=float)
    bz = np.asarray(bz, dtype=float)
    shape = (r.size, z.size)
    if n_cu.shape != shape or br.shape != shape or bz.shape != shape:
        raise ValueError("neutral and B arrays must match the R-Z axes")
    if np.any(~np.isfinite(n_cu)) or np.any(n_cu < 0.0):
        raise ValueError("neutral density must be finite and nonnegative")
    if np.any(~np.isfinite(br)) or np.any(~np.isfinite(bz)):
        raise ValueError("B arrays must be finite")
    if initial_sigma <= 0.0 or not np.isfinite(initial_sigma):
        raise ValueError("initial_sigma must be positive and finite")
    if neutral_power < 0.0 or not np.isfinite(neutral_power):
        raise ValueError("neutral_power must be finite and nonnegative")

    launch_radii, launch_weights = build_launch_ensemble(
        r_min=float(r[0]),
        r_max=float(r[-1]),
        launch_radius=launch_radius,
        launch_width=launch_width,
        launch_count=launch_count,
        launch_profile=launch_profile,
        launch_lobe_offset=launch_lobe_offset,
        launch_lobe_width=launch_lobe_width,
    )

    neutral_max = float(np.max(n_cu))
    if neutral_max <= 0.0:
        raise ValueError("neutral density is zero everywhere")
    neutral_weight = (n_cu / neutral_max) ** neutral_power
    quadrature = rz_quadrature_weights(r, z)
    rr, zz = np.meshgrid(r, z, indexing="ij")
    spread_tangent = np.tan(np.deg2rad(spread_angle_degrees))
    raw = np.zeros(shape)
    efficiencies = {
        "plasma": 0.0,
        "target": 0.0,
        "bottom": 0.0,
        "side": 0.0,
        "path_limit": 0.0,
        "invalid": 0.0,
        "cutoff": 0.0,
    }
    termination_counts = {name: 0 for name in efficiencies if name != "plasma"}
    mean_path_length = 0.0

    for launch_r, launch_weight in zip(launch_radii, launch_weights):
        ray = trace_single_ray(
            r=r,
            z=z,
            br=br,
            bz=bz,
            launch_radius=float(launch_r),
            step_length=step_length,
            transition_field=transition_field,
            field_floor=field_floor,
            local_fraction=local_fraction,
            local_attenuation_length=local_attenuation_length,
            fast_attenuation_length=fast_attenuation_length,
            max_path_length=max_path_length,
            survival_floor=survival_floor,
            inner_radial_loss=inner_radial_loss,
        )
        weight = float(launch_weight)
        efficiencies["plasma"] += weight * ray.deposited_fraction
        efficiencies[ray.termination] += weight * ray.remaining_survival
        termination_counts[ray.termination] += 1
        mean_path_length += weight * ray.path_length

        cumulative = 0.0
        for midpoint, ds, deposited in zip(
            ray.segment_midpoints,
            ray.segment_lengths,
            ray.deposited_fractions,
        ):
            sigma = np.sqrt(
                initial_sigma**2
                + (cumulative + 0.5 * ds) ** 2 * spread_tangent**2
            )
            kernel = np.exp(
                -0.5
                * (
                    ((rr - midpoint[0]) / sigma) ** 2
                    + ((zz - midpoint[1]) / sigma) ** 2
                )
            )
            weighted_kernel = kernel * neutral_weight
            integral = float(np.sum(weighted_kernel * quadrature))
            if integral > 0.0:
                raw += weight * deposited * weighted_kernel / integral
            cumulative += float(ds)

    metadata = SourceTraceMetadata(
        plasma_efficiency=float(efficiencies["plasma"]),
        target_efficiency=float(efficiencies["target"]),
        bottom_efficiency=float(efficiencies["bottom"]),
        side_efficiency=float(efficiencies["side"]),
        path_limit_efficiency=float(efficiencies["path_limit"]),
        invalid_efficiency=float(efficiencies["invalid"]),
        cutoff_efficiency=float(efficiencies["cutoff"]),
        mean_path_length=float(mean_path_length),
        termination_counts=termination_counts,
    )
    return raw, metadata


def normalize_to_plasma_efficiency(
    r: np.ndarray,
    z: np.ndarray,
    raw_weight: np.ndarray,
    *,
    target_radius: float,
    plasma_efficiency: float,
) -> np.ndarray:
    """Scale a raw map to eta_plasma times the full target area."""
    if target_radius <= 0.0 or not np.isfinite(target_radius):
        raise ValueError("target_radius must be positive and finite")
    if not 0.0 <= plasma_efficiency <= 1.0 + 1.0e-12:
        raise ValueError("plasma_efficiency must lie in [0, 1]")
    raw_weight = np.asarray(raw_weight, dtype=float)
    if np.any(~np.isfinite(raw_weight)) or np.any(raw_weight < 0.0):
        raise ValueError("raw weight must be finite and nonnegative")
    if plasma_efficiency == 0.0:
        return np.zeros_like(raw_weight)
    integral = rz_volume_integral(r, z, raw_weight)
    if integral <= 0.0:
        raise ValueError("positive plasma efficiency requires a positive raw map")
    desired = plasma_efficiency * np.pi * target_radius**2
    return raw_weight * (desired / integral)


def add_center_bridge_and_column(
    r: np.ndarray,
    z: np.ndarray,
    source_map: np.ndarray,
    *,
    center_radius: float = DEFAULT_SOURCE_ONLY_CENTER_RADIUS,
    bridge_target_ratio: float = DEFAULT_SOURCE_ONLY_BRIDGE_TARGET_RATIO,
    bridge_radial_width: float = DEFAULT_SOURCE_ONLY_BRIDGE_RADIAL_WIDTH,
    bridge_axial_length: float = DEFAULT_SOURCE_ONLY_BRIDGE_AXIAL_LENGTH,
    column_top_ratio: float = DEFAULT_SOURCE_ONLY_COLUMN_TOP_RATIO,
    column_radial_width: float = DEFAULT_SOURCE_ONLY_COLUMN_RADIAL_WIDTH,
    column_attenuation_length: float = DEFAULT_SOURCE_ONLY_COLUMN_ATTENUATION_LENGTH,
) -> np.ndarray:
    """Fill the source-only lobe valley and add a weak axial center column.

    The completed map is rescaled to preserve its original axisymmetric integral,
    so this operation changes only the prescribed spatial distribution.
    """
    r = _validate_axis("r", r)
    z = _validate_axis("z", z)
    source = _validate_field("source_map", source_map, r, z, nonnegative=True)
    if not r[0] <= center_radius <= r[-1]:
        raise ValueError("center_radius must lie inside the radial grid")
    if not 0.0 < bridge_target_ratio < 1.0:
        raise ValueError("bridge_target_ratio must lie in (0, 1)")
    for name, value in (
        ("bridge_radial_width", bridge_radial_width),
        ("bridge_axial_length", bridge_axial_length),
        ("column_radial_width", column_radial_width),
        ("column_attenuation_length", column_attenuation_length),
    ):
        if not np.isfinite(value) or value <= 0.0:
            raise ValueError(f"{name} must be positive and finite")
    if not np.isfinite(column_top_ratio) or column_top_ratio < 0.0:
        raise ValueError("column_top_ratio must be finite and nonnegative")

    original_integral = rz_volume_integral(r, z, source)
    if original_integral <= 0.0:
        raise ValueError("source_map must have a positive axisymmetric integral")
    peak_index = np.unravel_index(np.argmax(source), source.shape)
    peak_value = float(source[peak_index])
    peak_z_index = int(peak_index[1])
    peak_z = float(z[peak_z_index])
    center_index = int(np.argmin(np.abs(r - center_radius)))

    radial_column = np.exp(
        -0.5 * ((r - center_radius) / column_radial_width) ** 2
    )[:, None]
    axial_column = np.exp(-np.abs(z - peak_z) / column_attenuation_length)[
        None, :
    ]
    completed = source + column_top_ratio * peak_value * radial_column * axial_column

    radial_bridge = np.exp(
        -0.5 * ((r - center_radius) / bridge_radial_width) ** 2
    )[:, None]
    axial_bridge = np.exp(-np.abs(z - peak_z) / bridge_axial_length)[None, :]
    bridge_shape = radial_bridge * axial_bridge

    def bridge_ratio(amplitude: float) -> float:
        peak_slice = completed[:, peak_z_index] + amplitude * bridge_shape[:, peak_z_index]
        return float(peak_slice[center_index] / np.max(peak_slice))

    if bridge_ratio(0.0) < bridge_target_ratio:
        lower = 0.0
        upper = peak_value
        while bridge_ratio(upper) < bridge_target_ratio:
            upper *= 2.0
        for _ in range(80):
            midpoint = 0.5 * (lower + upper)
            if bridge_ratio(midpoint) < bridge_target_ratio:
                lower = midpoint
            else:
                upper = midpoint
        completed = completed + upper * bridge_shape

    completed_integral = rz_volume_integral(r, z, completed)
    return completed * (original_integral / completed_integral)


def generate_case(
    case: str,
    *,
    neutral_table: Path = DEFAULT_NEUTRAL_TABLE,
    output_root: Path,
    local_fraction: float,
    trace_options: dict[str, Any],
    center_supplement_options: dict[str, Any] | None = None,
) -> tuple[Path, SourceTraceMetadata]:
    """Generate one original-density map using its signed total B components."""
    if case not in CASE_BFIELD_DIRS:
        raise ValueError(f"unknown B-field case: {case}")
    _validate_fraction("local_fraction", local_fraction)

    neutral_r, neutral_z, table_density = read_moose_table(neutral_table)
    r_mask = neutral_r <= 0.25 + 1.0e-12
    z_mask = neutral_z <= 0.30 + 1.0e-12
    r = neutral_r[r_mask]
    z = neutral_z[z_mask]
    table_density = table_density[np.ix_(r_mask, z_mask)]
    neutral_density = build_original_effective_density(r, z, table_density)

    bfield_dir = CASE_BFIELD_DIRS[case]
    br_r, br_z, br_values = read_moose_table(bfield_dir / "Bx_T.tbl")
    bz_r, bz_z, bz_values = read_moose_table(bfield_dir / "By_T.tbl")
    if not np.array_equal(br_r, bz_r) or not np.array_equal(br_z, bz_z):
        raise ValueError(f"{case} Bx and By tables do not share axes")
    total_br = interpolate_regular_grid(br_r, br_z, br_values, r, z)
    total_bz = interpolate_regular_grid(bz_r, bz_z, bz_values, r, z)

    raw, metadata = trace_real_b_source(
        r,
        z,
        neutral_density,
        total_br,
        total_bz,
        local_fraction=local_fraction,
        **trace_options,
    )
    source_map = normalize_to_plasma_efficiency(
        r,
        z,
        raw,
        target_radius=0.25,
        plasma_efficiency=metadata.plasma_efficiency,
    )
    if center_supplement_options is not None:
        source_map = add_center_bridge_and_column(
            r,
            z,
            source_map,
            **center_supplement_options,
        )
    output_path = output_root / case / "moose_tables/see_spatial_weight_m-1.tbl"
    write_moose_table(output_path, r, z, source_map)
    return output_path, metadata


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--neutral-table", type=Path, default=DEFAULT_NEUTRAL_TABLE)
    parser.add_argument("--output-root", type=Path)
    parser.add_argument(
        "--fraction-mode", choices=("common", "case"), default="common"
    )
    parser.add_argument("--case", action="append", choices=sorted(CASE_BFIELD_DIRS))
    parser.add_argument("--common-local-fraction", type=float, default=0.65)
    parser.add_argument("--source-only-local-fraction", type=float, default=0.95)
    parser.add_argument("--four-coil-local-fraction", type=float, default=0.70)
    parser.add_argument(
        "--four-coil-img3092-local-fraction", type=float, default=0.55
    )
    parser.add_argument("--launch-radius", type=float, default=0.15)
    parser.add_argument("--launch-width", type=float, default=DEFAULT_LAUNCH_WIDTH)
    parser.add_argument("--launch-count", type=int, default=21)
    parser.add_argument(
        "--launch-profile",
        choices=("single_gaussian", "double_gaussian"),
        default=DEFAULT_LAUNCH_PROFILE,
    )
    parser.add_argument(
        "--launch-lobe-offset", type=float, default=DEFAULT_LAUNCH_LOBE_OFFSET
    )
    parser.add_argument(
        "--launch-lobe-width", type=float, default=DEFAULT_LAUNCH_LOBE_WIDTH
    )
    parser.add_argument("--transition-field", type=float, default=0.001)
    parser.add_argument("--field-floor", type=float, default=1.0e-12)
    parser.add_argument("--step-length", type=float, default=0.0025)
    parser.add_argument("--fast-attenuation-length", type=float, default=0.20)
    parser.add_argument(
        "--source-only-fast-attenuation-length",
        type=float,
        default=DEFAULT_SOURCE_ONLY_FAST_ATTENUATION_LENGTH,
    )
    parser.add_argument(
        "--source-only-inner-trace-radius",
        type=float,
        default=DEFAULT_SOURCE_ONLY_INNER_TRACE_RADIUS,
    )
    parser.add_argument(
        "--local-attenuation-length",
        type=float,
        default=DEFAULT_LOCAL_ATTENUATION_LENGTH,
    )
    parser.add_argument("--initial-sigma", type=float, default=DEFAULT_INITIAL_SIGMA)
    parser.add_argument(
        "--spread-angle-degrees",
        type=float,
        default=DEFAULT_SPREAD_ANGLE_DEGREES,
    )
    parser.add_argument("--neutral-power", type=float, default=1.0)
    parser.add_argument("--max-path-length", type=float, default=0.90)
    parser.add_argument("--survival-floor", type=float, default=1.0e-4)
    parser.add_argument(
        "--source-only-center-radius",
        type=float,
        default=DEFAULT_SOURCE_ONLY_CENTER_RADIUS,
    )
    parser.add_argument(
        "--source-only-bridge-target-ratio",
        type=float,
        default=DEFAULT_SOURCE_ONLY_BRIDGE_TARGET_RATIO,
    )
    parser.add_argument(
        "--source-only-bridge-radial-width",
        type=float,
        default=DEFAULT_SOURCE_ONLY_BRIDGE_RADIAL_WIDTH,
    )
    parser.add_argument(
        "--source-only-bridge-axial-length",
        type=float,
        default=DEFAULT_SOURCE_ONLY_BRIDGE_AXIAL_LENGTH,
    )
    parser.add_argument(
        "--source-only-column-top-ratio",
        type=float,
        default=DEFAULT_SOURCE_ONLY_COLUMN_TOP_RATIO,
    )
    parser.add_argument(
        "--source-only-column-radial-width",
        type=float,
        default=DEFAULT_SOURCE_ONLY_COLUMN_RADIAL_WIDTH,
    )
    parser.add_argument(
        "--source-only-column-attenuation-length",
        type=float,
        default=DEFAULT_SOURCE_ONLY_COLUMN_ATTENUATION_LENGTH,
    )
    return parser


def main() -> None:
    args = build_parser().parse_args()
    fractions = fractions_for_mode(
        args.fraction_mode,
        common_fraction=args.common_local_fraction,
        source_only_fraction=args.source_only_local_fraction,
        four_coil_fraction=args.four_coil_local_fraction,
        four_coil_img3092_fraction=args.four_coil_img3092_local_fraction,
    )
    output_root = args.output_root or output_root_for_mode(args.fraction_mode)
    trace_options = {
        "launch_radius": args.launch_radius,
        "launch_width": args.launch_width,
        "launch_count": args.launch_count,
        "launch_profile": args.launch_profile,
        "launch_lobe_offset": args.launch_lobe_offset,
        "launch_lobe_width": args.launch_lobe_width,
        "transition_field": args.transition_field,
        "field_floor": args.field_floor,
        "step_length": args.step_length,
        "fast_attenuation_length": args.fast_attenuation_length,
        "local_attenuation_length": args.local_attenuation_length,
        "initial_sigma": args.initial_sigma,
        "spread_angle_degrees": args.spread_angle_degrees,
        "neutral_power": args.neutral_power,
        "max_path_length": args.max_path_length,
        "survival_floor": args.survival_floor,
    }
    center_supplement_options = {
        "center_radius": args.source_only_center_radius,
        "bridge_target_ratio": args.source_only_bridge_target_ratio,
        "bridge_radial_width": args.source_only_bridge_radial_width,
        "bridge_axial_length": args.source_only_bridge_axial_length,
        "column_top_ratio": args.source_only_column_top_ratio,
        "column_radial_width": args.source_only_column_radial_width,
        "column_attenuation_length": args.source_only_column_attenuation_length,
    }
    for case in args.case or list(CASE_BFIELD_DIRS):
        case_trace_options = dict(trace_options)
        if case == "source_only":
            case_trace_options["fast_attenuation_length"] = (
                args.source_only_fast_attenuation_length
            )
            case_trace_options["inner_radial_loss"] = (
                args.source_only_inner_trace_radius
            )
        output_path, metadata = generate_case(
            case,
            neutral_table=args.neutral_table,
            output_root=output_root,
            local_fraction=fractions[case],
            trace_options=case_trace_options,
            center_supplement_options=(
                center_supplement_options if case == "source_only" else None
            ),
        )
        print(
            f"{case}: f_local={fractions[case]:.3f}, "
            f"eta_plasma={metadata.plasma_efficiency:.6f}, "
            f"target_loss={metadata.target_efficiency:.6f}, "
            f"bottom_loss={metadata.bottom_efficiency:.6f}, "
            f"side_loss={metadata.side_efficiency:.6f}, "
            f"path_limit_loss={metadata.path_limit_efficiency:.6f}, "
            f"map={output_path}"
        )


if __name__ == "__main__":
    main()
