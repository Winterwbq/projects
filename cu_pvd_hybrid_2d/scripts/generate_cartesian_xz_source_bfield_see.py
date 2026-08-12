#!/usr/bin/env python3
"""Generate one-sided Cartesian X-Z source B-field and two-lobe SEE tables.

The Cartesian chamber spans x=0..0.25 m and z=0..0.30 m.  The powered
magnetron target is the upper segment x=0.19..0.22 m at z=0.30 m.
The source-only R-Z field remains the calibration reference, but its strength
center is translated from 0.150 m to 0.205 m.  The corresponding magnetic
poles and SEE Gaussian centers are 0.195/0.215 m, both inside the powered
target.  The wafer spans x=0.07..0.17 m at z=0.

Unlike the reference generators, all source-map integrals in this file use
Cartesian dx*dz area per unit out-of-plane depth.  Both x walls are physical
loss boundaries; x=0 is not treated as a symmetry axis.  SEE centerlines move
vertically downward rather than following the divergent B-field arches.  The
retained reduced transport model uses 99.5% local deposition, 6 mm local and
5 cm guided attenuation lengths, 5-degree spreading, and a 1 mT transition
setting retained for compatibility with the paired case metadata.
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import generate_real_b_fieldline_see_source_maps as rz_see  # noqa: E402
import generate_reference_source_bfield as rz_source_bfield  # noqa: E402


DEFAULT_REFERENCE_BFIELD_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_source_only_curved_return"
    / "moose_tables"
)
DEFAULT_NEUTRAL_TABLE = (
    ROOT / "runs" / "zapdos_hpem_rz_30cm" / "moose_tables" / "n_Cu_m3.tbl"
)
DEFAULT_OUTPUT_DIR = (
    ROOT / "runs" / "zapdos_cartesian_xz_25x30_source_only" / "moose_tables"
)

X_MIN_M = 0.0
X_MAX_M = 0.25
Z_MIN_M = 0.0
Z_MAX_M = 0.30
SOURCE_X_MIN_M = 0.19
SOURCE_X_MAX_M = 0.22
WAFER_X_MIN_M = 0.07
WAFER_X_MAX_M = 0.17
REFERENCE_CENTER_M = 0.150
SOURCE_CENTER_M = 0.205
LOBE_OFFSET_M = 0.010
LOBE_WIDTH_M = 0.010
LOCAL_FRACTION = 0.995
LOCAL_ATTENUATION_LENGTH_M = 0.006
GUIDED_ATTENUATION_LENGTH_M = 0.05
SPREAD_ANGLE_DEGREES = 10.0
TRANSITION_FIELD_T = 0.001
SEE_MAGNITUDE_SCALE = 1.2
FIELD_FLOOR_T = 1.0e-12
B_FIELD_FLOOR_Z_M = 0.12
B_FIELD_MIN_T = 1.0e-4
B_FIELD_MAX_T = 0.1


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(f"{name} must be one-dimensional with at least two points")
    if np.any(~np.isfinite(axis)) or np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must be finite and strictly increasing")
    return axis


def _trapezoid_weights(axis: np.ndarray) -> np.ndarray:
    axis = _validate_axis("axis", axis)
    weights = np.empty_like(axis)
    weights[0] = 0.5 * (axis[1] - axis[0])
    weights[-1] = 0.5 * (axis[-1] - axis[-2])
    weights[1:-1] = 0.5 * (axis[2:] - axis[:-2])
    return weights


def cartesian_quadrature_weights(x: np.ndarray, z: np.ndarray) -> np.ndarray:
    """Return trapezoidal Cartesian ``dx*dz`` weights per unit depth."""
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    return _trapezoid_weights(x)[:, None] * _trapezoid_weights(z)[None, :]


def cartesian_area_integral(
    x: np.ndarray, z: np.ndarray, values: np.ndarray
) -> float:
    """Integrate a field over the Cartesian X-Z area."""
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    field = np.asarray(values, dtype=float)
    if field.shape != (x.size, z.size) or np.any(~np.isfinite(field)):
        raise ValueError("values must be a finite array matching the X-Z grid")
    return float(np.sum(field * cartesian_quadrature_weights(x, z)))


def normalize_cartesian_see_weight(
    x: np.ndarray,
    z: np.ndarray,
    raw_weight: np.ndarray,
    *,
    source_x_min: float = SOURCE_X_MIN_M,
    source_x_max: float = SOURCE_X_MAX_M,
    plasma_efficiency: float,
) -> np.ndarray:
    """Normalize SEE weight to efficiency times powered-target width."""
    if not np.isfinite(source_x_min) or not np.isfinite(source_x_max):
        raise ValueError("source bounds must be finite")
    if source_x_max <= source_x_min:
        raise ValueError("source_x_max must exceed source_x_min")
    if not np.isfinite(plasma_efficiency) or not 0.0 <= plasma_efficiency <= 1.0:
        raise ValueError("plasma_efficiency must lie in [0, 1]")
    raw = np.asarray(raw_weight, dtype=float)
    if np.any(~np.isfinite(raw)) or np.any(raw < 0.0):
        raise ValueError("raw_weight must be finite and nonnegative")
    if plasma_efficiency == 0.0:
        return np.zeros_like(raw)
    integral = cartesian_area_integral(x, z, raw)
    if integral <= 0.0:
        raise ValueError("positive efficiency requires a positive SEE map")
    desired = plasma_efficiency * (source_x_max - source_x_min)
    return raw * (desired / integral)


def scale_cartesian_see_weight(
    see_weight: np.ndarray,
    magnitude_scale: float,
) -> np.ndarray:
    """Scale absolute SEE strength without changing its spatial profile."""
    weight = np.asarray(see_weight, dtype=float)
    scale = float(magnitude_scale)
    if np.any(~np.isfinite(weight)) or np.any(weight < 0.0):
        raise ValueError("see_weight must be finite and nonnegative")
    if not np.isfinite(scale) or scale <= 0.0:
        raise ValueError("magnitude_scale must be positive and finite")
    return scale * weight


def build_cartesian_launch_ensemble(
    *,
    source_x_min: float = SOURCE_X_MIN_M,
    source_x_max: float = SOURCE_X_MAX_M,
    center: float = SOURCE_CENTER_M,
    lobe_offset: float = LOBE_OFFSET_M,
    lobe_width: float = LOBE_WIDTH_M,
    count: int = 41,
) -> tuple[np.ndarray, np.ndarray]:
    """Return a line-weighted two-lobe launch ensemble on the powered target."""
    values = (source_x_min, source_x_max, center, lobe_offset, lobe_width)
    if not all(np.isfinite(value) for value in values):
        raise ValueError("launch parameters must be finite")
    if source_x_max <= source_x_min or not source_x_min < center < source_x_max:
        raise ValueError("source bounds must contain the launch center")
    if lobe_offset <= 0.0 or lobe_width <= 0.0 or count < 3:
        raise ValueError("lobe dimensions must be positive and count at least three")
    left_center = center - lobe_offset
    right_center = center + lobe_offset
    if right_center <= source_x_min or left_center >= source_x_max:
        raise ValueError("at least one launch lobe center must overlap the powered target")

    launch_x = np.linspace(source_x_min, source_x_max, int(count))
    envelope = np.exp(-0.5 * ((launch_x - left_center) / lobe_width) ** 2)
    envelope += np.exp(-0.5 * ((launch_x - right_center) / lobe_width) ** 2)
    total = float(np.sum(envelope))
    if total <= 0.0 or not np.isfinite(total):
        raise ValueError("launch ensemble has zero or non-finite weight")
    return launch_x, envelope / total


def _interpolate_points(
    source_x: np.ndarray,
    source_z: np.ndarray,
    source_values: np.ndarray,
    query_x: np.ndarray,
    query_z: np.ndarray,
) -> np.ndarray:
    """Interpolate a regular table at a tensor product of arbitrary X points."""
    source_x = _validate_axis("source_x", source_x)
    source_z = _validate_axis("source_z", source_z)
    values = np.asarray(source_values, dtype=float)
    if values.shape != (source_x.size, source_z.size):
        raise ValueError("source_values does not match its axes")
    if np.any(~np.isfinite(values)):
        raise ValueError("source_values must be finite")
    qx = np.asarray(query_x, dtype=float)
    qz = _validate_axis("query_z", query_z)
    tolerance = 1.0e-12
    if (
        np.any(~np.isfinite(qx))
        or np.any(qx < source_x[0] - tolerance)
        or np.any(qx > source_x[-1] + tolerance)
        or qz[0] < source_z[0] - tolerance
        or qz[-1] > source_z[-1] + tolerance
    ):
        raise ValueError("query coordinates fall outside the reference table")
    qx = np.clip(qx, source_x[0], source_x[-1])
    qz = np.clip(qz, source_z[0], source_z[-1])

    values_at_z = np.empty((source_x.size, qz.size))
    for ix in range(source_x.size):
        values_at_z[ix, :] = np.interp(qz, source_z, values[ix, :])
    result = np.empty((qx.size, qz.size))
    for iz in range(qz.size):
        result[:, iz] = np.interp(qx, source_x, values_at_z[:, iz])
    return result


def translate_reference_bfield(
    reference_x: np.ndarray,
    reference_z: np.ndarray,
    reference_bx: np.ndarray,
    reference_bz: np.ndarray,
    x: np.ndarray,
    z: np.ndarray,
    *,
    reference_center: float = REFERENCE_CENTER_M,
    cartesian_center: float = SOURCE_CENTER_M,
) -> tuple[np.ndarray, np.ndarray]:
    """Translate a centered source field while preserving vector parity.

    The old R-Z table is only tabulated for nonnegative coordinates.  When a
    translated query falls left of that table, its source-centered mirror is
    used: the transverse component is odd and the axial component is even.
    This is an extension of the calibrated source field, not an x=0 symmetry
    boundary condition in the new Cartesian simulation.
    """
    reference_x = _validate_axis("reference_x", reference_x)
    reference_z = _validate_axis("reference_z", reference_z)
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    if not np.isfinite(reference_center) or not np.isfinite(cartesian_center):
        raise ValueError("field centers must be finite")

    shift = cartesian_center - reference_center
    unshifted = x - shift
    reflected = unshifted < reference_x[0]
    lookup_x = unshifted.copy()
    lookup_x[reflected] = 2.0 * reference_center - lookup_x[reflected]
    bx = _interpolate_points(reference_x, reference_z, reference_bx, lookup_x, z)
    bz = _interpolate_points(reference_x, reference_z, reference_bz, lookup_x, z)
    bx[reflected, :] *= -1.0
    return bx, bz


def apply_axial_b_magnitude_cap(
    bx: np.ndarray,
    bz: np.ndarray,
    z: np.ndarray,
    *,
    floor_z: float = B_FIELD_FLOOR_Z_M,
    min_b_t: float = B_FIELD_MIN_T,
    max_b_t: float = B_FIELD_MAX_T,
) -> tuple[np.ndarray, np.ndarray]:
    """Limit |B| logarithmically with Z without changing vector direction.

    The cap rises from ``min_b_t`` at ``floor_z`` to ``max_b_t`` at the
    uppermost Z coordinate.  At and below ``floor_z`` the magnitude is exactly
    ``min_b_t``.  Above it, values below the cap retain their original
    magnitude, preserving as much of the calibrated two-lobe profile as
    possible.
    """
    z = _validate_axis("z", z)
    bx = np.asarray(bx, dtype=float)
    bz = np.asarray(bz, dtype=float)
    if bx.shape != bz.shape or bx.ndim != 2 or bx.shape[1] != z.size:
        raise ValueError("B components must be matching 2-D arrays on the Z axis")
    if np.any(~np.isfinite(bx)) or np.any(~np.isfinite(bz)):
        raise ValueError("B components must be finite")
    if not np.isfinite(floor_z) or not z[0] <= floor_z < z[-1]:
        raise ValueError("floor_z must lie inside the Z domain below its top")
    if not np.isfinite(min_b_t) or not np.isfinite(max_b_t) or not (
        0.0 < min_b_t < max_b_t
    ):
        raise ValueError("B limits must satisfy 0 < min_b_t < max_b_t")

    magnitude = np.hypot(bx, bz)
    if np.any(magnitude <= 0.0):
        raise ValueError("B direction is undefined where its magnitude is zero")
    axial_fraction = np.clip((z - floor_z) / (z[-1] - floor_z), 0.0, 1.0)
    axial_cap = min_b_t * (max_b_t / min_b_t) ** axial_fraction
    capped_magnitude = np.minimum(np.maximum(magnitude, min_b_t), axial_cap[None, :])
    scale = capped_magnitude / magnitude
    return bx * scale, bz * scale


def compress_bfield_magnitude_axially(
    x: np.ndarray,
    z: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    *,
    floor_through_z: float = B_FIELD_FLOOR_Z_M,
    minimum_t: float = B_FIELD_MIN_T,
) -> tuple[np.ndarray, np.ndarray]:
    """Smoothly pack existing magnitude contours into the upper chamber.

    A cubic smoothstep maps the new interval ``floor_through_z..z_max`` onto
    the original full Z interval.  It has zero slope at both ends, so the
    compressed profile joins smoothly to the uniform lower-field region.
    Only magnitude is remapped; the vector direction at every X-Z point is
    preserved exactly.
    """
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    bx = np.asarray(bx, dtype=float)
    bz = np.asarray(bz, dtype=float)
    if bx.shape != (x.size, z.size) or bz.shape != bx.shape:
        raise ValueError("B arrays must match the Cartesian X-Z grid")
    if np.any(~np.isfinite(bx)) or np.any(~np.isfinite(bz)):
        raise ValueError("B arrays must be finite")
    floor_z = float(floor_through_z)
    minimum = float(minimum_t)
    if not np.isfinite(floor_z) or not z[0] < floor_z < z[-1]:
        raise ValueError("floor_through_z must lie strictly inside the Z domain")
    if not np.isfinite(minimum) or minimum <= 0.0:
        raise ValueError("minimum_t must be positive and finite")

    original_magnitude = np.hypot(bx, bz)
    if np.any(original_magnitude <= 0.0):
        raise ValueError("B direction is undefined where its magnitude is zero")
    normalized_height = np.clip((z - floor_z) / (z[-1] - floor_z), 0.0, 1.0)
    smooth_height = normalized_height**2 * (3.0 - 2.0 * normalized_height)
    lookup_z = z[0] + (z[-1] - z[0]) * smooth_height

    remapped_magnitude = np.empty_like(original_magnitude)
    for ix in range(x.size):
        remapped_magnitude[ix, :] = np.interp(
            lookup_z, z, original_magnitude[ix, :]
        )
    target_magnitude = np.maximum(remapped_magnitude, minimum)
    target_magnitude[:, z <= floor_z] = minimum
    scale = target_magnitude / original_magnitude
    return bx * scale, bz * scale


def build_cartesian_source_bfield(
    x: np.ndarray,
    z: np.ndarray,
    *,
    source_center: float = SOURCE_CENTER_M,
    pole_offset: float = LOBE_OFFSET_M,
    dipole_center_z: float = 0.325,
    min_b_t: float = B_FIELD_MIN_T,
    max_b_t: float = B_FIELD_MAX_T,
) -> tuple[np.ndarray, np.ndarray]:
    """Rebuild the calibrated two-arch source field in Cartesian geometry.

    The old R-Z source generator supplies the measured 1--1000 G axial
    strength calibration.  Its three alternating virtual poles are recentered
    at x=0.205 m with the two outer poles at 0.195 and 0.215 m.  A Cartesian
    dipole-loop direction is then applied without the old x=0 axis taper.
    """
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    if not x[0] <= source_center - pole_offset < source_center + pole_offset <= x[-1]:
        raise ValueError("magnetic pole centers must lie inside the X domain")
    if dipole_center_z <= z[-1]:
        raise ValueError("dipole_center_z must lie behind the top target")

    reference_bx, reference_bz = rz_source_bfield.build_source_bfield(
        x,
        z,
        source_r_m=source_center,
        source_z_m=float(z[-1]),
        pole_separation_m=pole_offset,
        min_b_t=min_b_t,
        max_b_t=max_b_t,
    )
    magnitude = np.hypot(reference_bx, reference_bz)

    xx, zz = np.meshgrid(x, z, indexing="ij")
    dx = xx - source_center
    dz = zz - dipole_center_z
    direction_x = 3.0 * dx * dz
    direction_z = 2.0 * dz * dz - dx * dx
    direction_norm = np.hypot(direction_x, direction_z)
    if np.any(direction_norm <= 0.0) or np.any(~np.isfinite(direction_norm)):
        raise ValueError("Cartesian dipole direction contains a zero or non-finite value")
    directed_bx = magnitude * direction_x / direction_norm
    directed_bz = magnitude * direction_z / direction_norm
    return compress_bfield_magnitude_axially(
        x,
        z,
        directed_bx,
        directed_bz,
        floor_through_z=B_FIELD_FLOOR_Z_M,
        minimum_t=min_b_t,
    )


def _bilinear_cartesian(
    x: np.ndarray,
    z: np.ndarray,
    values: np.ndarray,
    point: np.ndarray,
) -> float:
    """Interpolate a scalar field without R-Z reflection or component parity."""
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    field = np.asarray(values, dtype=float)
    point = np.asarray(point, dtype=float)
    if field.shape != (x.size, z.size) or np.any(~np.isfinite(field)):
        raise ValueError("values must be a finite array matching the X-Z grid")
    if point.shape != (2,) or np.any(~np.isfinite(point)):
        raise ValueError("point must be a finite two-vector")
    tolerance = 1.0e-12
    if (
        point[0] < x[0] - tolerance
        or point[0] > x[-1] + tolerance
        or point[1] < z[0] - tolerance
        or point[1] > z[-1] + tolerance
    ):
        raise ValueError("interpolation point lies outside the Cartesian table")

    point_x = float(np.clip(point[0], x[0], x[-1]))
    point_z = float(np.clip(point[1], z[0], z[-1]))
    ix = int(np.clip(np.searchsorted(x, point_x) - 1, 0, x.size - 2))
    iz = int(np.clip(np.searchsorted(z, point_z) - 1, 0, z.size - 2))
    fx = (point_x - x[ix]) / (x[ix + 1] - x[ix])
    fz = (point_z - z[iz]) / (z[iz + 1] - z[iz])
    return float(
        (1.0 - fx) * (1.0 - fz) * field[ix, iz]
        + fx * (1.0 - fz) * field[ix + 1, iz]
        + (1.0 - fx) * fz * field[ix, iz + 1]
        + fx * fz * field[ix + 1, iz + 1]
    )


def first_cartesian_boundary_crossing(
    start: np.ndarray,
    end: np.ndarray,
    *,
    target_enabled: bool,
    x_min: float = X_MIN_M,
    x_max: float = X_MAX_M,
    z_min: float = Z_MIN_M,
    z_max: float = Z_MAX_M,
    target_x_min: float = SOURCE_X_MIN_M,
    target_x_max: float = SOURCE_X_MAX_M,
    wafer_x_min: float = WAFER_X_MIN_M,
    wafer_x_max: float = WAFER_X_MAX_M,
) -> tuple[float, str] | None:
    """Return the first absorbing boundary crossed by a Cartesian segment."""
    start = np.asarray(start, dtype=float)
    end = np.asarray(end, dtype=float)
    if start.shape != (2,) or end.shape != (2,) or np.any(~np.isfinite([start, end])):
        raise ValueError("boundary-crossing points must be finite two-vectors")
    if not x_min < x_max or not z_min < z_max:
        raise ValueError("Cartesian domain bounds must be increasing")
    if not x_min <= target_x_min < target_x_max <= x_max:
        raise ValueError("target bounds must lie inside the X domain")
    if not x_min <= wafer_x_min < wafer_x_max <= x_max:
        raise ValueError("wafer bounds must lie inside the X domain")

    delta = end - start
    tolerance = 1.0e-12
    crossings: list[tuple[float, str]] = []

    def electrode_label(fraction: float, electrode: str, shield: str, low: float, high: float) -> str:
        crossing_x = float(start[0] + fraction * delta[0])
        return electrode if low - tolerance <= crossing_x <= high + tolerance else shield

    if start[1] <= z_min + tolerance and end[1] < start[1]:
        crossings.append(
            (0.0, electrode_label(0.0, "wafer", "bottom_shield", wafer_x_min, wafer_x_max))
        )
    elif end[1] <= z_min < start[1] and delta[1] != 0.0:
        fraction = (z_min - start[1]) / delta[1]
        crossings.append(
            (
                fraction,
                electrode_label(
                    fraction, "wafer", "bottom_shield", wafer_x_min, wafer_x_max
                ),
            )
        )

    if target_enabled:
        if start[1] >= z_max - tolerance and end[1] > start[1]:
            crossings.append(
                (0.0, electrode_label(0.0, "target", "top_shield", target_x_min, target_x_max))
            )
        elif end[1] >= z_max > start[1] and delta[1] != 0.0:
            fraction = (z_max - start[1]) / delta[1]
            crossings.append(
                (
                    fraction,
                    electrode_label(
                        fraction, "target", "top_shield", target_x_min, target_x_max
                    ),
                )
            )

    if start[0] <= x_min + tolerance and end[0] < start[0]:
        crossings.append((0.0, "left_wall"))
    elif end[0] <= x_min < start[0] and delta[0] != 0.0:
        crossings.append(((x_min - start[0]) / delta[0], "left_wall"))

    if start[0] >= x_max - tolerance and end[0] > start[0]:
        crossings.append((0.0, "right_wall"))
    elif end[0] >= x_max > start[0] and delta[0] != 0.0:
        crossings.append(((x_max - start[0]) / delta[0], "right_wall"))

    valid = [(fraction, label) for fraction, label in crossings if 0.0 <= fraction <= 1.0]
    return min(valid, default=None, key=lambda item: item[0])


def _cartesian_midpoint_rk2_step(
    *,
    point: np.ndarray,
    previous_field_tangent: np.ndarray | None,
    step_length: float,
    x: np.ndarray,
    z: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    transition_field: float,
    field_floor: float,
    bfield_guidance_fraction: float = 1.0,
) -> rz_see.RK2Step:
    """Advance one arc-length step using native Cartesian interpolation."""
    start_direction, start_tangent = rz_see.effective_direction(
        _bilinear_cartesian(x, z, bx, point),
        _bilinear_cartesian(x, z, bz, point),
        previous_field_tangent,
        transition_field=transition_field,
        field_floor=field_floor,
    )
    start_direction = _blend_direction_with_downward(
        start_direction, bfield_guidance_fraction
    )
    midpoint = point + 0.5 * step_length * start_direction
    midpoint_direction, midpoint_tangent = rz_see.effective_direction(
        _bilinear_cartesian(x, z, bx, midpoint),
        _bilinear_cartesian(x, z, bz, midpoint),
        start_tangent,
        transition_field=transition_field,
        field_floor=field_floor,
    )
    midpoint_direction = _blend_direction_with_downward(
        midpoint_direction, bfield_guidance_fraction
    )
    return rz_see.RK2Step(
        point=point + step_length * midpoint_direction,
        midpoint=midpoint,
        start_direction=start_direction,
        midpoint_direction=midpoint_direction,
        selected_field_tangent=midpoint_tangent,
    )


def _blend_direction_with_downward(
    field_direction: np.ndarray,
    bfield_guidance_fraction: float,
) -> np.ndarray:
    """Blend a unit field direction with vertical downward transport."""
    if (
        not np.isfinite(bfield_guidance_fraction)
        or not 0.0 <= bfield_guidance_fraction <= 1.0
    ):
        raise ValueError("bfield_guidance_fraction must lie in [0, 1]")
    direction = np.asarray(field_direction, dtype=float)
    if direction.shape != (2,) or np.any(~np.isfinite(direction)):
        raise ValueError("field_direction must be a finite two-vector")
    blended = (
        bfield_guidance_fraction * direction
        + (1.0 - bfield_guidance_fraction) * np.asarray([0.0, -1.0])
    )
    norm = float(np.linalg.norm(blended))
    if norm <= 1.0e-14:
        return np.asarray([0.0, -1.0])
    return blended / norm


def trace_cartesian_ray(
    *,
    x: np.ndarray,
    z: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    launch_x: float,
    step_length: float,
    transition_field: float,
    field_floor: float,
    local_fraction: float,
    local_attenuation_length: float,
    guided_attenuation_length: float,
    max_path_length: float,
    survival_floor: float,
    bfield_guidance_fraction: float = 1.0,
) -> rz_see.RayTrace:
    """Trace one SEE ray through the full Cartesian chamber without reflection."""
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    bx = np.asarray(bx, dtype=float)
    bz = np.asarray(bz, dtype=float)
    if bx.shape != (x.size, z.size) or bz.shape != bx.shape:
        raise ValueError("B arrays must match the Cartesian X-Z grid")
    if np.any(~np.isfinite(bx)) or np.any(~np.isfinite(bz)):
        raise ValueError("B arrays must be finite")
    if not SOURCE_X_MIN_M <= launch_x <= SOURCE_X_MAX_M:
        raise ValueError("launch_x must lie on the powered target")
    positive = {
        "step_length": step_length,
        "transition_field": transition_field,
        "local_attenuation_length": local_attenuation_length,
        "guided_attenuation_length": guided_attenuation_length,
        "max_path_length": max_path_length,
    }
    for name, value in positive.items():
        if not np.isfinite(value) or value <= 0.0:
            raise ValueError(f"{name} must be positive and finite")
    if not np.isfinite(field_floor) or field_floor < 0.0:
        raise ValueError("field_floor must be finite and nonnegative")
    if not 0.0 <= local_fraction <= 1.0:
        raise ValueError("local_fraction must lie in [0, 1]")
    if not 0.0 <= survival_floor < 1.0:
        raise ValueError("survival_floor must lie in [0, 1)")
    if (
        not np.isfinite(bfield_guidance_fraction)
        or not 0.0 <= bfield_guidance_fraction <= 1.0
    ):
        raise ValueError("bfield_guidance_fraction must lie in [0, 1]")

    point = np.asarray([launch_x, z[-1]], dtype=float)
    points = [point.copy()]
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
        trial_step = remaining_step
        proposal: rz_see.RK2Step | None = None
        for _ in range(20):
            try:
                candidate = _cartesian_midpoint_rk2_step(
                    point=point,
                    previous_field_tangent=previous_tangent,
                    step_length=trial_step,
                    x=x,
                    z=z,
                    bx=bx,
                    bz=bz,
                    transition_field=transition_field,
                    field_floor=field_floor,
                    bfield_guidance_fraction=bfield_guidance_fraction,
                )
            except ValueError:
                try:
                    fallback_direction, fallback_tangent = rz_see.effective_direction(
                        _bilinear_cartesian(x, z, bx, point),
                        _bilinear_cartesian(x, z, bz, point),
                        previous_tangent,
                        transition_field=transition_field,
                        field_floor=field_floor,
                    )
                    fallback_direction = _blend_direction_with_downward(
                        fallback_direction, bfield_guidance_fraction
                    )
                except ValueError:
                    break
                fallback_point = point + trial_step * fallback_direction
                fallback_crossing = first_cartesian_boundary_crossing(
                    point, fallback_point, target_enabled=entered
                )
                if fallback_crossing is None:
                    break
                candidate = rz_see.RK2Step(
                    point=fallback_point,
                    midpoint=point + 0.5 * trial_step * fallback_direction,
                    start_direction=fallback_direction,
                    midpoint_direction=fallback_direction,
                    selected_field_tangent=fallback_tangent,
                )

            if not entered and candidate.point[1] > z[-1] + 1.0e-12:
                # A coarse first step can span a very shallow loop and place
                # its endpoint just above the emitting surface. Resolve the
                # initial inward leg before enabling a physical top return.
                trial_step *= 0.5
                continue
            proposal = candidate
            remaining_step = trial_step
            break

        if proposal is None:
            termination = "invalid"
            break

        crossing = first_cartesian_boundary_crossing(
            point, proposal.point, target_enabled=entered
        )
        fraction, loss_label = crossing if crossing is not None else (1.0, "")
        accepted_length = remaining_step * fraction
        end_point = point + fraction * (proposal.point - point)
        accepted_midpoint = 0.5 * (point + end_point)

        local_next = local_survival * np.exp(
            -accepted_length / local_attenuation_length
        )
        guided_next = guided_survival * np.exp(
            -accepted_length / guided_attenuation_length
        )
        deposited = local_fraction * (local_survival - local_next) + (
            1.0 - local_fraction
        ) * (guided_survival - guided_next)
        segment_midpoints.append(accepted_midpoint)
        segment_lengths.append(float(accepted_length))
        deposited_fractions.append(float(deposited))
        points.append(end_point.copy())
        path_length += accepted_length
        local_survival = float(local_next)
        guided_survival = float(guided_next)

        if crossing is not None:
            termination = loss_label
            point = end_point
            break

        point = proposal.point
        previous_tangent = proposal.selected_field_tangent
        # Any resolved inward displacement makes a later top crossing a real
        # target/top-shield return; do not inherit the much larger R-Z launch
        # tolerance that misclassified shallow Cartesian loops as invalid.
        entered = entered or point[1] < z[-1] - 1.0e-12
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
    return rz_see.RayTrace(
        points=np.asarray(points),
        segment_midpoints=np.asarray(segment_midpoints),
        segment_lengths=np.asarray(segment_lengths),
        deposited_fractions=np.asarray(deposited_fractions),
        termination=termination,
        remaining_survival=float(remaining_survival),
        deposited_fraction=float(np.sum(deposited_fractions)),
        path_length=float(path_length),
    )


def trace_vertical_cartesian_ray(
    *,
    x: np.ndarray,
    z: np.ndarray,
    launch_x: float,
    step_length: float,
    transition_field: float,
    local_fraction: float,
    local_attenuation_length: float,
    guided_attenuation_length: float,
    max_path_length: float,
    survival_floor: float,
    bfield_guidance_fraction: float = 1.0,
) -> rz_see.RayTrace:
    """Trace a collimated SEE centerline vertically downward from the target."""
    x = _validate_axis("x", x)
    z = _validate_axis("z", z)
    # A zero guide field activates the tracer's exact weak-field downward
    # direction.  The retained spread angle is applied later to the deposition
    # kernel around this centerline, not by bending the centerline itself.
    zero_field = np.zeros((x.size, z.size), dtype=float)
    return trace_cartesian_ray(
        x=x,
        z=z,
        bx=zero_field,
        bz=zero_field,
        launch_x=launch_x,
        step_length=step_length,
        transition_field=transition_field,
        field_floor=FIELD_FLOOR_T,
        local_fraction=local_fraction,
        local_attenuation_length=local_attenuation_length,
        guided_attenuation_length=guided_attenuation_length,
        max_path_length=max_path_length,
        survival_floor=survival_floor,
        bfield_guidance_fraction=bfield_guidance_fraction,
    )


def _build_effective_neutral_density(
    x: np.ndarray,
    z: np.ndarray,
    neutral_table: Path,
    *,
    source_center: float = SOURCE_CENTER_M,
) -> np.ndarray:
    table_x, table_z, table_density = rz_see.read_moose_table(neutral_table)
    base_density = _interpolate_points(table_x, table_z, table_density, x, z)
    xx, zz = np.meshgrid(x, z, indexing="ij")
    magnetron = 3.0e18 * np.exp(-((xx - source_center) / 0.080) ** 2)
    magnetron *= np.exp(-((zz - 0.270) / 0.090) ** 2)
    return 0.1 * np.maximum(base_density, 1.0e16) + 1.0e17 + magnetron


def _trace_cartesian_see(
    x: np.ndarray,
    z: np.ndarray,
    neutral_density: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    *,
    launch_count: int,
    step_length: float,
    max_path_length: float,
    survival_floor: float,
    local_fraction: float,
    local_attenuation_length: float,
    guided_attenuation_length: float,
    spread_angle_degrees: float,
    transition_field: float,
    direction_model: str = "vertical_downward",
    bfield_guidance_fraction: float = 1.0,
) -> tuple[np.ndarray, dict[str, Any]]:
    if not np.isfinite(spread_angle_degrees) or not 0.0 <= spread_angle_degrees < 90.0:
        raise ValueError("spread_angle_degrees must lie in [0, 90)")
    if direction_model not in {"vertical_downward", "bfield_guided"}:
        raise ValueError(
            "direction_model must be 'vertical_downward' or 'bfield_guided'"
        )
    launch_x, launch_weights = build_cartesian_launch_ensemble(count=launch_count)
    quadrature = cartesian_quadrature_weights(x, z)
    xx, zz = np.meshgrid(x, z, indexing="ij")
    neutral_max = float(np.max(neutral_density))
    if neutral_max <= 0.0:
        raise ValueError("neutral density must be positive")
    neutral_weight = neutral_density / neutral_max
    raw = np.zeros_like(neutral_density)
    plasma_efficiency = 0.0
    termination_counts: dict[str, int] = {}
    mean_path_length = 0.0
    spread_tangent = np.tan(np.deg2rad(spread_angle_degrees))

    for launch_position, launch_weight in zip(launch_x, launch_weights):
        trace_arguments = {
            "x": x,
            "z": z,
            "launch_x": float(launch_position),
            "step_length": step_length,
            "transition_field": transition_field,
            "local_fraction": local_fraction,
            "local_attenuation_length": local_attenuation_length,
            "guided_attenuation_length": guided_attenuation_length,
            "max_path_length": max_path_length,
            "survival_floor": survival_floor,
            "bfield_guidance_fraction": bfield_guidance_fraction,
        }
        if direction_model == "vertical_downward":
            ray = trace_vertical_cartesian_ray(**trace_arguments)
        else:
            ray = trace_cartesian_ray(
                **trace_arguments,
                bx=bx,
                bz=bz,
                field_floor=FIELD_FLOOR_T,
            )
        weight = float(launch_weight)
        plasma_efficiency += weight * ray.deposited_fraction
        mean_path_length += weight * ray.path_length
        termination = ray.termination
        termination_counts[termination] = termination_counts.get(termination, 0) + 1

        cumulative = 0.0
        for midpoint, segment_length, deposited in zip(
            ray.segment_midpoints, ray.segment_lengths, ray.deposited_fractions
        ):
            sigma = np.sqrt(
                0.003**2
                + (cumulative + 0.5 * segment_length) ** 2 * spread_tangent**2
            )
            kernel = np.exp(
                -0.5
                * (
                    ((xx - midpoint[0]) / sigma) ** 2
                    + ((zz - midpoint[1]) / sigma) ** 2
                )
            )
            weighted_kernel = kernel * neutral_weight
            kernel_integral = float(np.sum(weighted_kernel * quadrature))
            if kernel_integral > 0.0:
                raw += weight * deposited * weighted_kernel / kernel_integral
            cumulative += float(segment_length)

    normalized = normalize_cartesian_see_weight(
        x,
        z,
        raw,
        plasma_efficiency=plasma_efficiency,
    )
    metadata: dict[str, Any] = {
        "plasma_efficiency": float(plasma_efficiency),
        "mean_path_length_m": float(mean_path_length),
        "termination_counts": termination_counts,
    }
    return normalized, metadata


def generate_cartesian_case(
    *,
    output_dir: Path | str = DEFAULT_OUTPUT_DIR,
    reference_bfield_dir: Path | str = DEFAULT_REFERENCE_BFIELD_DIR,
    neutral_table: Path | str = DEFAULT_NEUTRAL_TABLE,
    nx: int = 101,
    nz: int = 161,
    launch_count: int = 41,
    step_length: float = 0.0025,
    max_path_length: float = 0.90,
    survival_floor: float = 1.0e-4,
    local_fraction: float = LOCAL_FRACTION,
    local_attenuation_length: float = LOCAL_ATTENUATION_LENGTH_M,
    guided_attenuation_length: float = GUIDED_ATTENUATION_LENGTH_M,
    spread_angle_degrees: float = SPREAD_ANGLE_DEGREES,
    transition_field: float = TRANSITION_FIELD_T,
    see_magnitude_scale: float = SEE_MAGNITUDE_SCALE,
) -> dict[str, Any]:
    """Generate translated B components and a Cartesian two-lobe SEE map."""
    if nx < 3 or nz < 3:
        raise ValueError("nx and nz must each be at least three")
    output_path = Path(output_dir)
    reference_path = Path(reference_bfield_dir)
    x = np.linspace(X_MIN_M, X_MAX_M, int(nx))
    z = np.linspace(Z_MIN_M, Z_MAX_M, int(nz))

    ref_bx_x, ref_bx_z, ref_bx = rz_see.read_moose_table(
        reference_path / "Bx_T.tbl"
    )
    ref_bz_x, ref_bz_z, ref_bz = rz_see.read_moose_table(
        reference_path / "By_T.tbl"
    )
    if not np.array_equal(ref_bx_x, ref_bz_x) or not np.array_equal(ref_bx_z, ref_bz_z):
        raise ValueError("reference Bx and By tables must share axes")
    bx, bz = build_cartesian_source_bfield(x, z)
    neutral_density = _build_effective_neutral_density(
        x, z, Path(neutral_table)
    )
    unscaled_see_weight, trace_metadata = _trace_cartesian_see(
        x,
        z,
        neutral_density,
        bx,
        bz,
        launch_count=launch_count,
        step_length=step_length,
        max_path_length=max_path_length,
        survival_floor=survival_floor,
        local_fraction=local_fraction,
        local_attenuation_length=local_attenuation_length,
        guided_attenuation_length=guided_attenuation_length,
        spread_angle_degrees=spread_angle_degrees,
        transition_field=transition_field,
    )
    see_weight = scale_cartesian_see_weight(
        unscaled_see_weight, see_magnitude_scale
    )

    output_path.mkdir(parents=True, exist_ok=True)
    rz_see.write_moose_table(output_path / "Bx_T.tbl", x, z, bx)
    rz_see.write_moose_table(output_path / "By_T.tbl", x, z, bz)
    rz_see.write_moose_table(
        output_path / "see_spatial_weight_m-1.tbl", x, z, see_weight
    )
    result: dict[str, Any] = {
        "coordinate_system": "cartesian_x_z_per_unit_y_depth",
        "domain_m": {"x": [X_MIN_M, X_MAX_M], "z": [Z_MIN_M, Z_MAX_M]},
        "powered_target_x_m": [SOURCE_X_MIN_M, SOURCE_X_MAX_M],
        "source_center_m": SOURCE_CENTER_M,
        "launch_lobe_centers_m": [
            round(SOURCE_CENTER_M - LOBE_OFFSET_M, 12),
            round(SOURCE_CENTER_M + LOBE_OFFSET_M, 12),
        ],
        "magnetic_pole_centers_m": [
            round(SOURCE_CENTER_M - LOBE_OFFSET_M, 12),
            round(SOURCE_CENTER_M + LOBE_OFFSET_M, 12),
        ],
        "reference_center_m": REFERENCE_CENTER_M,
        "boundary_layout_m": {
            "left_wall_x": X_MIN_M,
            "right_wall_x": X_MAX_M,
            "wafer_x": [WAFER_X_MIN_M, WAFER_X_MAX_M],
            "powered_target_x": [SOURCE_X_MIN_M, SOURCE_X_MAX_M],
        },
        "transport_settings": {
            "local_fraction": float(local_fraction),
            "local_attenuation_length_m": float(local_attenuation_length),
            "guided_attenuation_length_m": float(guided_attenuation_length),
            "spread_angle_degrees": float(spread_angle_degrees),
            "transition_field_t": float(transition_field),
        },
        "see_transport_direction": "vertical_downward",
        "see_magnitude_scale": float(see_magnitude_scale),
        "plasma_efficiency": trace_metadata["plasma_efficiency"],
        "see_unscaled_area_integral_m": cartesian_area_integral(
            x, z, unscaled_see_weight
        ),
        "see_area_integral_m": cartesian_area_integral(x, z, see_weight),
        "mean_path_length_m": trace_metadata["mean_path_length_m"],
        "termination_counts": trace_metadata["termination_counts"],
        "bfield_magnitude_range_t": [
            float(np.min(np.hypot(bx, bz))),
            float(np.max(np.hypot(bx, bz))),
        ],
        "bfield_axial_compression": {
            "original_z_m": [Z_MIN_M, Z_MAX_M],
            "compressed_z_m": [B_FIELD_FLOOR_Z_M, Z_MAX_M],
            "uniform_lower_field_t": B_FIELD_MIN_T,
        },
        "bfield_magnitude_profile": {
            "profile": "smooth_axial_coordinate_compression",
            "original_z_m": [Z_MIN_M, Z_MAX_M],
            "compressed_z_m": [B_FIELD_FLOOR_Z_M, Z_MAX_M],
            "mapping": "cubic_smoothstep",
            "direction": "preserved",
        },
    }
    (output_path / "generation_metadata.json").write_text(
        json.dumps(result, indent=2) + "\n", encoding="utf-8"
    )
    return result


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR)
    parser.add_argument(
        "--reference-bfield-dir", type=Path, default=DEFAULT_REFERENCE_BFIELD_DIR
    )
    parser.add_argument("--neutral-table", type=Path, default=DEFAULT_NEUTRAL_TABLE)
    parser.add_argument("--nx", type=int, default=101)
    parser.add_argument("--nz", type=int, default=161)
    parser.add_argument("--launch-count", type=int, default=41)
    parser.add_argument("--step-length", type=float, default=0.0025)
    parser.add_argument("--max-path-length", type=float, default=0.90)
    parser.add_argument("--survival-floor", type=float, default=1.0e-4)
    parser.add_argument("--local-fraction", type=float, default=LOCAL_FRACTION)
    parser.add_argument(
        "--local-attenuation-length",
        type=float,
        default=LOCAL_ATTENUATION_LENGTH_M,
    )
    parser.add_argument(
        "--guided-attenuation-length",
        type=float,
        default=GUIDED_ATTENUATION_LENGTH_M,
    )
    parser.add_argument(
        "--spread-angle-degrees", type=float, default=SPREAD_ANGLE_DEGREES
    )
    parser.add_argument(
        "--transition-field", type=float, default=TRANSITION_FIELD_T
    )
    parser.add_argument(
        "--see-magnitude-scale", type=float, default=SEE_MAGNITUDE_SCALE
    )
    return parser


def main() -> None:
    args = build_parser().parse_args()
    result = generate_cartesian_case(
        output_dir=args.output_dir,
        reference_bfield_dir=args.reference_bfield_dir,
        neutral_table=args.neutral_table,
        nx=args.nx,
        nz=args.nz,
        launch_count=args.launch_count,
        step_length=args.step_length,
        max_path_length=args.max_path_length,
        survival_floor=args.survival_floor,
        local_fraction=args.local_fraction,
        local_attenuation_length=args.local_attenuation_length,
        guided_attenuation_length=args.guided_attenuation_length,
        spread_angle_degrees=args.spread_angle_degrees,
        transition_field=args.transition_field,
        see_magnitude_scale=args.see_magnitude_scale,
    )
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
