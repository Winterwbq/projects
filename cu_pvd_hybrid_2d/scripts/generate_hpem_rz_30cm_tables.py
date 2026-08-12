#!/usr/bin/env python3
"""Generate HPEM-like 30 cm R-Z tables for the Zapdos Cu PVD surrogate.

Coordinates:
    x = r [m], 0 <= r <= chamber_radius
    y = z [m], 0 <= z <= chamber_length

The tables are intentionally analytic, not imported from HPEM. They provide a
30 cm R-Z chamber with a top/right annular magnetron source, Cu neutral plume,
a localized magnetron field plus a four-coil bulk guide field, and HPEM-like
SEE/fast-electron deposition that travels from the powered target into the
chamber.
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUT_DIR = ROOT / "runs" / "zapdos_hpem_rz_30cm" / "moose_tables"
DEFAULT_EXTERNAL_COIL_Z = (0.030, 0.110, 0.190, 0.260)
# Effective current filament at the inner edge of the external winding pack.
# The pack center is near 0.325 m, but 0.310 m reproduces its chamber-side
# localization and the weak bands between neighboring coils.
DEFAULT_EXTERNAL_COIL_RADIUS = 0.310
DEFAULT_EXTERNAL_COIL_WEIGHTS = (1.0, 1.0, 1.0, 1.0)
DEFAULT_EXTERNAL_COIL_QUADRATURE = 512
DEFAULT_MAGNETRON_AXIAL_LENGTH = 0.012
DEFAULT_SOURCE_STEERING_FRACTION = 1.20
DEFAULT_EXTERNAL_FULL_BELOW_Z = 0.25
DEFAULT_EXTERNAL_ZERO_ABOVE_Z = 0.30
DEFAULT_TWO_LOOP_SOURCE_CENTER = 0.244
DEFAULT_TWO_LOOP_POLE_SEPARATION = 0.055
DEFAULT_TWO_LOOP_POLE_WIDTH = 0.008
DEFAULT_TWO_LOOP_SOURCE_DEPTH = 0.001
DEFAULT_TWO_LOOP_BACK_DEPTH = 0.002
DEFAULT_TWO_LOOP_CORE_RADIUS = 0.005
DEFAULT_TWO_LOOP_NULL_R = 0.232
DEFAULT_TWO_LOOP_NULL_Z = 0.190
DEFAULT_TWO_LOOP_RADIAL_QUADRATURE = 5
DEFAULT_TWO_LOOP_AZIMUTHAL_QUADRATURE = 256
DEFAULT_TWO_LOOP_POLARITY = -1.0


def smootherstep01(value: np.ndarray) -> np.ndarray:
    """Map values to [0, 1] with zero first and second endpoint derivatives."""
    clipped = np.clip(value, 0.0, 1.0)
    return clipped**3 * (clipped * (6.0 * clipped - 15.0) + 10.0)


def smootherstep01_derivative(value: np.ndarray) -> np.ndarray:
    """Return the derivative of smootherstep01 with respect to its input."""
    clipped = np.clip(value, 0.0, 1.0)
    return 30.0 * clipped**2 * (1.0 - clipped) ** 2


def write_moose_table(path: Path, x: np.ndarray, y: np.ndarray, values: np.ndarray) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        handle.write("AXIS X\n")
        handle.write(" ".join(f"{v:.8e}" for v in x) + "\n\n")
        handle.write("AXIS Y\n")
        handle.write(" ".join(f"{v:.8e}" for v in y) + "\n\n")
        handle.write("DATA\n")
        for iy in range(len(y)):
            handle.write(" ".join(f"{v:.8e}" for v in values[:, iy]) + "\n")


def positive_normalize(values: np.ndarray, floor: float = 0.0) -> np.ndarray:
    arr = np.maximum(np.asarray(values, dtype=float), floor)
    vmax = float(np.nanmax(arr))
    if not np.isfinite(vmax) or vmax <= 0.0:
        return np.zeros_like(arr)
    return arr / vmax


def normalized_gaussian(points: np.ndarray, center: float, width: float) -> np.ndarray:
    sigma = max(float(width), 1.0e-12)
    weights = np.exp(-0.5 * ((points - center) / sigma) ** 2)
    total = float(np.sum(weights))
    if not np.isfinite(total) or total <= 0.0:
        raise ValueError("Gaussian weights are zero")
    return weights / total


def build_neutral_cu(
    r: np.ndarray,
    z: np.ndarray,
    *,
    chamber_length: float,
    racetrack_radius: float,
    background_density: float,
    plume_peak_density: float,
    radial_width: float,
    axial_width: float,
) -> np.ndarray:
    rr, zz = np.meshgrid(r, z, indexing="ij")
    axial_distance_from_target = chamber_length - zz
    racetrack_plume = np.exp(-0.5 * ((rr - racetrack_radius) / radial_width) ** 2)
    racetrack_plume *= np.exp(-0.5 * (axial_distance_from_target / axial_width) ** 2)
    broad_chamber_fill = 0.35 * np.exp(-0.5 * ((rr - 0.55 * r[-1]) / (0.45 * r[-1])) ** 2)
    broad_chamber_fill *= np.exp(-0.5 * ((zz - 0.45 * chamber_length) / (0.55 * chamber_length)) ** 2)
    return background_density + plume_peak_density * (racetrack_plume + broad_chamber_fill)


def build_magnetron_bfield(
    r: np.ndarray,
    z: np.ndarray,
    *,
    chamber_length: float,
    racetrack_radius: float,
    peak_b_t: float,
    radial_width: float,
    axial_width: float,
    loop_fraction: float = 0.50,
    open_guide_fraction: float = 0.05,
    open_guide_width: float = 0.070,
    source_tail_fraction: float = 0.006,
    source_tail_decay_length: float = 0.160,
    source_tail_radial_width: float = 0.240,
) -> tuple[np.ndarray, np.ndarray]:
    rr, zz = np.meshgrid(r, z, indexing="ij")
    zc = chamber_length + 0.015
    radial = (rr - racetrack_radius) / max(radial_width, 1.0e-12)
    axial = (zz - zc) / max(axial_width, 1.0e-12)
    envelope = np.exp(-0.5 * (radial**2 + axial**2))

    # Bx is radial Br, By is axial Bz. Both the loop and its open guide are
    # localized to the target-side axial scale. Chamber-scale guidance is added
    # separately by build_external_coil_bfield().
    loop = max(float(loop_fraction), 0.0)
    br = loop * (-axial * envelope)
    bz = loop * (radial * envelope)

    distance_from_target = np.maximum(chamber_length - zz, 0.0)
    guide_center = racetrack_radius - 0.020
    guide_radial = np.exp(
        -0.5 * ((rr - guide_center) / max(open_guide_width, 1.0e-12)) ** 2
    )
    guide_axial = np.exp(-distance_from_target / max(axial_width, 1.0e-12))
    bz += max(float(open_guide_fraction), 0.0) * guide_radial * guide_axial

    # Very weak source-directed guide field: it is too small to dominate the
    # magnitude map, but it makes low-field streamlines bend toward the upper
    # target magnet instead of rising as nearly straight vertical lines.
    target_dr = racetrack_radius - rr
    target_dz = distance_from_target + 0.6 * axial_width
    target_distance = np.hypot(target_dr, target_dz)
    tail_radial = 0.45 + 0.55 * np.exp(
        -0.5 * ((rr - racetrack_radius) / max(source_tail_radial_width, 1.0e-12)) ** 2
    )
    tail_axial = np.exp(
        -distance_from_target / max(source_tail_decay_length, 1.0e-12)
    )
    tail = max(float(source_tail_fraction), 0.0) * tail_radial * tail_axial
    br += tail * target_dr / np.maximum(target_distance, 1.0e-12)
    bz += tail * target_dz / np.maximum(target_distance, 1.0e-12)

    bmag = np.hypot(br, bz)
    bmax = float(np.nanmax(bmag))
    if bmax <= 0.0 or not np.isfinite(bmax):
        raise ValueError("synthetic B field is zero")
    scale = peak_b_t / bmax
    return br * scale, bz * scale


def _annular_pole_band_field(
    r: np.ndarray,
    z: np.ndarray,
    *,
    pole_radius: float,
    front_z: float,
    back_z: float,
    pole_width: float,
    core_radius: float,
    radial_quadrature: int,
    azimuthal_quadrature: int,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the field of one finite annular magnet column.

    The front and back rings carry opposite virtual magnetic charge. All
    virtual charge therefore remains behind the target and each basis has no
    magnetic-monopole far field.
    """
    r_values = np.asarray(r, dtype=float)
    z_values = np.asarray(z, dtype=float)
    rr = r_values[:, None, None]
    zz = z_values[None, :, None]
    br = np.zeros((len(r_values), len(z_values)), dtype=float)
    bz = np.zeros_like(br)

    radial_nodes, radial_weights = np.polynomial.legendre.leggauss(
        radial_quadrature
    )
    band_radii = pole_radius + 3.0 * pole_width * radial_nodes
    band_weights = radial_weights * np.exp(
        -0.5 * ((band_radii - pole_radius) / pole_width) ** 2
    )
    # The annular area element contributes a factor of radius.
    band_weights *= band_radii
    band_weights /= np.sum(band_weights)

    phi = (
        np.arange(azimuthal_quadrature, dtype=float) + 0.5
    ) * (2.0 * np.pi / azimuthal_quadrature)
    cos_phi = np.cos(phi)
    chunk_size = 32
    for ring_radius, radial_weight in zip(band_radii, band_weights):
        for sign, source_z in ((1.0, front_z), (-1.0, back_z)):
            axial_delta = zz - source_z
            for start in range(0, azimuthal_quadrature, chunk_size):
                cosine = cos_phi[start : start + chunk_size][None, None, :]
                distance_squared = (
                    rr**2
                    + ring_radius**2
                    - 2.0 * rr * ring_radius * cosine
                    + axial_delta**2
                    + core_radius**2
                )
                inverse_cube = distance_squared ** (-1.5)
                weight = radial_weight * sign / azimuthal_quadrature
                br += weight * np.sum(
                    (rr - ring_radius * cosine) * inverse_cube, axis=2
                )
                bz += weight * np.sum(axial_delta * inverse_cube, axis=2)

    if len(r_values) > 0 and np.isclose(r_values[0], 0.0):
        br[0, :] = 0.0
    return br, bz


def build_two_loop_magnetron_bfield(
    r: np.ndarray,
    z: np.ndarray,
    *,
    chamber_length: float,
    chamber_radius: float,
    peak_b_t: float,
    source_center: float = DEFAULT_TWO_LOOP_SOURCE_CENTER,
    pole_separation: float = DEFAULT_TWO_LOOP_POLE_SEPARATION,
    pole_width: float = DEFAULT_TWO_LOOP_POLE_WIDTH,
    source_depth: float = DEFAULT_TWO_LOOP_SOURCE_DEPTH,
    back_depth: float = DEFAULT_TWO_LOOP_BACK_DEPTH,
    core_radius: float = DEFAULT_TWO_LOOP_CORE_RADIUS,
    null_r: float = DEFAULT_TWO_LOOP_NULL_R,
    null_z: float = DEFAULT_TWO_LOOP_NULL_Z,
    radial_quadrature: int = DEFAULT_TWO_LOOP_RADIAL_QUADRATURE,
    azimuthal_quadrature: int = DEFAULT_TWO_LOOP_AZIMUTHAL_QUADRATURE,
    polarity: float = DEFAULT_TWO_LOOP_POLARITY,
) -> tuple[np.ndarray, np.ndarray]:
    """Build two source arches from three virtual magnets behind the target."""
    peak = float(peak_b_t)
    radius = float(chamber_radius)
    center = float(source_center)
    separation = float(pole_separation)
    width = float(pole_width)
    front_depth = float(source_depth)
    rear_depth = float(back_depth)
    core = float(core_radius)
    desired_null_r = float(null_r)
    desired_null_z = float(null_z)
    radial_order = int(radial_quadrature)
    azimuthal_order = int(azimuthal_quadrature)
    field_polarity = float(polarity)
    pole_radii = np.asarray(
        [center - separation, center, center + separation], dtype=float
    )

    if peak <= 0.0 or not np.isfinite(peak):
        raise ValueError("two-loop magnetron peak must be positive")
    if radius <= 0.0 or not np.isfinite(radius):
        raise ValueError("two-loop chamber radius must be positive")
    if pole_radii[0] <= 3.0 * width or pole_radii[-1] >= radius:
        raise ValueError("two-loop pole centers must lie inside the radial domain")
    if width <= 0.0 or core <= 0.0:
        raise ValueError("two-loop pole width and core radius must be positive")
    if front_depth <= 0.0 or rear_depth <= front_depth:
        raise ValueError("two-loop virtual poles must be ordered behind the target")
    if not 0.0 < desired_null_r < radius:
        raise ValueError("two-loop null radius must lie inside the chamber")
    if not 0.0 < desired_null_z < float(chamber_length):
        raise ValueError("two-loop null height must lie inside the chamber")
    if radial_order < 3 or azimuthal_order < 32:
        raise ValueError("two-loop quadrature orders are too small")
    if field_polarity not in (-1.0, 1.0):
        raise ValueError("two-loop polarity must be either -1 or +1")

    front_z = float(chamber_length) + front_depth
    back_z = float(chamber_length) + rear_depth
    field_arguments = {
        "front_z": front_z,
        "back_z": back_z,
        "pole_width": width,
        "core_radius": core,
        "radial_quadrature": radial_order,
        "azimuthal_quadrature": azimuthal_order,
    }
    basis_fields = [
        _annular_pole_band_field(
            r,
            z,
            pole_radius=float(pole_radius),
            **field_arguments,
        )
        for pole_radius in pole_radii
    ]
    null_fields = [
        _annular_pole_band_field(
            np.asarray([desired_null_r]),
            np.asarray([desired_null_z]),
            pole_radius=float(pole_radius),
            **field_arguments,
        )
        for pole_radius in pole_radii
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
        raise ValueError("two-loop null calibration matrix is ill-conditioned")
    amplitudes = np.concatenate(
        ([1.0], np.linalg.solve(calibration_matrix, calibration_rhs))
    )
    if not (amplitudes[0] > 0.0 and amplitudes[1] < 0.0 and amplitudes[2] > 0.0):
        raise ValueError("two-loop null calibration lost alternating pole signs")

    br = sum(
        amplitude * field[0]
        for amplitude, field in zip(amplitudes, basis_fields)
    )
    bz = sum(
        amplitude * field[1]
        for amplitude, field in zip(amplitudes, basis_fields)
    )
    null_br = float(
        sum(
            amplitude * field[0][0, 0]
            for amplitude, field in zip(amplitudes, null_fields)
        )
    )
    null_bz = float(
        sum(
            amplitude * field[1][0, 0]
            for amplitude, field in zip(amplitudes, null_fields)
        )
    )
    response_scale = max(
        max(abs(field[0][0, 0]), abs(field[1][0, 0]))
        for field in null_fields
    )
    if np.hypot(null_br, null_bz) > 1.0e-10 * response_scale:
        raise ValueError("two-loop virtual poles failed to produce the requested null")

    bmax = float(np.nanmax(np.hypot(br, bz)))
    if bmax <= 0.0 or not np.isfinite(bmax):
        raise ValueError("two-loop magnetron field is zero")
    scale = peak / bmax
    return field_polarity * br * scale, field_polarity * bz * scale


def build_external_coil_bfield(
    r: np.ndarray,
    z: np.ndarray,
    *,
    chamber_radius: float,
    bulk_b_t: float,
    coil_enhancement_t: float,
    wafer_b_t: float = 0.0006,
    source_radius: float = 0.235,
    source_steering_fraction: float = DEFAULT_SOURCE_STEERING_FRACTION,
    coil_z: tuple[float, ...] = DEFAULT_EXTERNAL_COIL_Z,
    coil_radius: float = DEFAULT_EXTERNAL_COIL_RADIUS,
    coil_weights: tuple[float, ...] = DEFAULT_EXTERNAL_COIL_WEIGHTS,
    azimuthal_quadrature: int = DEFAULT_EXTERNAL_COIL_QUADRATURE,
    coil_radial_offset: float = 0.025,
    radial_width: float = 0.070,
    axial_width: float = 0.020,
    wafer_flat_length: float = 0.0,
    wafer_transition_length: float = 0.120,
) -> tuple[np.ndarray, np.ndarray]:
    """Evaluate four circular current loops by periodic Biot-Savart quadrature.

    The common current and mu_0/(4*pi) factor are absorbed into one scale chosen
    from ``bulk_b_t``. Legacy shaping arguments remain accepted so older run
    commands continue to parse, but they do not modify the physical loop field.
    """
    r_values = np.asarray(r, dtype=float)
    z_values = np.asarray(z, dtype=float)
    if r_values.ndim != 1 or z_values.ndim != 1:
        raise ValueError("external-coil coordinates must be one-dimensional")
    if len(r_values) < 2 or len(z_values) < 2:
        raise ValueError("external-coil coordinates require at least two points")

    loop_radius = float(coil_radius)
    centers = np.asarray(coil_z, dtype=float)
    weights = np.asarray(coil_weights, dtype=float)
    quadrature = int(azimuthal_quadrature)
    bulk_field = float(bulk_b_t)
    if loop_radius <= float(np.max(r_values)) or not np.isfinite(loop_radius):
        raise ValueError("external coil radius must lie outside the radial domain")
    if centers.ndim != 1 or len(centers) == 0 or not np.all(np.isfinite(centers)):
        raise ValueError("external coil heights must be a finite one-dimensional sequence")
    if weights.shape != centers.shape or not np.all(np.isfinite(weights)):
        raise ValueError("external coil weights must match the coil heights")
    if quadrature < 32:
        raise ValueError("external coil azimuthal quadrature must be at least 32")
    if bulk_field < 0.0 or not np.isfinite(bulk_field):
        raise ValueError("external bulk field must be finite and nonnegative")

    rr = r_values[:, None, None]
    zz = z_values[None, :, None]
    raw_br = np.zeros((len(r_values), len(z_values), 1), dtype=float)
    raw_bz = np.zeros_like(raw_br)
    phi = (np.arange(quadrature, dtype=float) + 0.5) * (2.0 * np.pi / quadrature)
    dphi = 2.0 * np.pi / quadrature
    chunk_size = 32
    for center_z, current_weight in zip(centers, weights):
        axial_delta = zz - float(center_z)
        for start in range(0, quadrature, chunk_size):
            cosine = np.cos(phi[start : start + chunk_size])[None, None, :]
            distance_squared = (
                rr**2
                + loop_radius**2
                - 2.0 * rr * loop_radius * cosine
                + axial_delta**2
            )
            inverse_cube = distance_squared ** (-1.5)
            raw_br += (
                current_weight
                * dphi
                * loop_radius
                * axial_delta
                * np.sum(cosine * inverse_cube, axis=2, keepdims=True)
            )
            raw_bz += (
                current_weight
                * dphi
                * loop_radius
                * np.sum(
                    (loop_radius - rr * cosine) * inverse_cube,
                    axis=2,
                    keepdims=True,
                )
            )

    raw_br = raw_br[:, :, 0]
    raw_bz = raw_bz[:, :, 0]
    raw_br[0, :] = 0.0
    normalized_z = (z_values[None, :] - z_values[0]) / max(
        float(z_values[-1] - z_values[0]), 1.0e-12
    )
    bulk_calibration_region = (
        (r_values[:, None] < 0.5 * float(chamber_radius))
        & (normalized_z > 0.23)
        & (normalized_z < 0.77)
    )
    raw_bulk_median = float(
        np.nanmedian(np.hypot(raw_br, raw_bz)[bulk_calibration_region])
    )
    if raw_bulk_median <= 0.0 or not np.isfinite(raw_bulk_median):
        if bulk_field > 0.0:
            raise ValueError("external four-coil field is zero in the bulk")
        scale = 0.0
    else:
        scale = bulk_field / raw_bulk_median
    br = scale * raw_br
    bz = scale * raw_bz
    return br, bz


def _blend_external_field_below_source(
    z: np.ndarray,
    external_br: np.ndarray,
    external_bz: np.ndarray,
    source_br: np.ndarray,
    source_bz: np.ndarray,
    *,
    full_below_t: float,
    zero_above_t: float,
    full_below_z: float,
    zero_above_z: float,
) -> tuple[np.ndarray, np.ndarray]:
    """Blend external components into the weak-source middle/lower chamber."""
    full_threshold = float(full_below_t)
    zero_threshold = float(zero_above_t)
    if full_threshold < 0.0:
        raise ValueError("external full-field threshold must be nonnegative")
    if zero_threshold <= full_threshold:
        raise ValueError(
            "external zero-field threshold must be greater than the full-field threshold"
        )
    full_z = float(full_below_z)
    zero_z = float(zero_above_z)
    if zero_z <= full_z:
        raise ValueError("external zero-field z must be greater than full-field z")

    source_bmag = np.hypot(source_br, source_bz)
    transition_width = zero_threshold - full_threshold
    source_u = np.clip(
        (zero_threshold - source_bmag) / transition_width,
        0.0,
        1.0,
    )
    source_weight = smootherstep01(source_u)

    zz = np.broadcast_to(np.asarray(z, dtype=float)[None, :], source_bmag.shape)
    axial_u = np.clip((zero_z - zz) / (zero_z - full_z), 0.0, 1.0)
    axial_weight = smootherstep01(axial_u)
    protection_progress = 1.0 - axial_weight
    effective_source_weight = 1.0 - protection_progress * (1.0 - source_weight)
    weight = axial_weight * effective_source_weight

    protected_br = weight * external_br
    protected_bz = weight * external_bz
    return protected_br, protected_bz


def build_combined_bfield(
    r: np.ndarray,
    z: np.ndarray,
    *,
    chamber_length: float,
    chamber_radius: float,
    racetrack_radius: float,
    magnetron_peak_b_t: float,
    magnetron_axial_length: float,
    external_bulk_b_t: float,
    external_coil_enhancement_t: float,
    magnetron_model: str = "single-loop",
    external_wafer_b_t: float = 0.0006,
    source_steering_fraction: float = DEFAULT_SOURCE_STEERING_FRACTION,
    external_coil_z: tuple[float, ...] = DEFAULT_EXTERNAL_COIL_Z,
    external_coil_radius: float = DEFAULT_EXTERNAL_COIL_RADIUS,
    external_coil_weights: tuple[float, ...] = DEFAULT_EXTERNAL_COIL_WEIGHTS,
    external_coil_quadrature: int = DEFAULT_EXTERNAL_COIL_QUADRATURE,
    magnetron_loop_fraction: float = 0.50,
    magnetron_open_guide_fraction: float = 0.05,
    external_full_below_t: float = 0.0016,
    external_zero_above_t: float = 0.0064,
    external_full_below_z: float = DEFAULT_EXTERNAL_FULL_BELOW_Z,
    external_zero_above_z: float = DEFAULT_EXTERNAL_ZERO_ABOVE_Z,
    two_loop_source_center: float = DEFAULT_TWO_LOOP_SOURCE_CENTER,
    two_loop_pole_separation: float = DEFAULT_TWO_LOOP_POLE_SEPARATION,
    two_loop_pole_width: float = DEFAULT_TWO_LOOP_POLE_WIDTH,
    two_loop_source_depth: float = DEFAULT_TWO_LOOP_SOURCE_DEPTH,
    two_loop_back_depth: float = DEFAULT_TWO_LOOP_BACK_DEPTH,
    two_loop_core_radius: float = DEFAULT_TWO_LOOP_CORE_RADIUS,
    two_loop_null_r: float = DEFAULT_TWO_LOOP_NULL_R,
    two_loop_null_z: float = DEFAULT_TWO_LOOP_NULL_Z,
    two_loop_radial_quadrature: int = DEFAULT_TWO_LOOP_RADIAL_QUADRATURE,
    two_loop_azimuthal_quadrature: int = DEFAULT_TWO_LOOP_AZIMUTHAL_QUADRATURE,
    two_loop_polarity: float = DEFAULT_TWO_LOOP_POLARITY,
) -> tuple[np.ndarray, np.ndarray]:
    if magnetron_model == "single-loop":
        magnetron_br, magnetron_bz = build_magnetron_bfield(
            r,
            z,
            chamber_length=chamber_length,
            racetrack_radius=racetrack_radius,
            peak_b_t=magnetron_peak_b_t,
            radial_width=0.060,
            axial_width=magnetron_axial_length,
            loop_fraction=magnetron_loop_fraction,
            open_guide_fraction=magnetron_open_guide_fraction,
        )
    elif magnetron_model == "two-loop":
        magnetron_br, magnetron_bz = build_two_loop_magnetron_bfield(
            r,
            z,
            chamber_length=chamber_length,
            chamber_radius=chamber_radius,
            peak_b_t=magnetron_peak_b_t,
            source_center=two_loop_source_center,
            pole_separation=two_loop_pole_separation,
            pole_width=two_loop_pole_width,
            source_depth=two_loop_source_depth,
            back_depth=two_loop_back_depth,
            core_radius=two_loop_core_radius,
            null_r=two_loop_null_r,
            null_z=two_loop_null_z,
            radial_quadrature=two_loop_radial_quadrature,
            azimuthal_quadrature=two_loop_azimuthal_quadrature,
            polarity=two_loop_polarity,
        )
    else:
        raise ValueError(f"unknown magnetron model: {magnetron_model}")
    external_br, external_bz = build_external_coil_bfield(
        r,
        z,
        chamber_radius=chamber_radius,
        bulk_b_t=external_bulk_b_t,
        wafer_b_t=external_wafer_b_t,
        source_radius=racetrack_radius,
        source_steering_fraction=source_steering_fraction,
        coil_enhancement_t=external_coil_enhancement_t,
        coil_z=external_coil_z,
        coil_radius=external_coil_radius,
        coil_weights=external_coil_weights,
        azimuthal_quadrature=external_coil_quadrature,
    )
    external_br, external_bz = _blend_external_field_below_source(
        z,
        external_br,
        external_bz,
        magnetron_br,
        magnetron_bz,
        full_below_t=external_full_below_t,
        zero_above_t=external_zero_above_t,
        full_below_z=external_full_below_z,
        zero_above_z=external_zero_above_z,
    )
    return magnetron_br + external_br, magnetron_bz + external_bz


def build_fast_electron_sources(
    r: np.ndarray,
    z: np.ndarray,
    n_cu: np.ndarray,
    br: np.ndarray,
    bz: np.ndarray,
    *,
    chamber_length: float,
    racetrack_radius: float,
    source_peak: float,
    energy_per_pair: float,
    launch_width: float,
    launch_r_count: int,
    angle_count: int,
    max_angle_deg: float,
    angle_spread_deg: float,
    ionization_mfp_at_peak: float,
    energy_attenuation_length: float,
    lateral_scatter_sqrt_m: float,
    lateral_scatter_linear: float,
    magnetic_residence_factor: float,
    wafer_cutoff_z: float,
    wafer_cutoff_width: float,
) -> tuple[np.ndarray, np.ndarray]:
    dr = float(np.median(np.diff(r)))
    dz = float(np.median(np.diff(z)))
    source_shape = np.zeros((len(r), len(z)), dtype=float)

    neutral_weight = positive_normalize(n_cu) ** 0.7
    b_weight = positive_normalize(np.hypot(br, bz))
    collisionality = neutral_weight * (1.0 + magnetic_residence_factor * b_weight)

    launch_r = np.linspace(
        max(0.0, racetrack_radius - 3.0 * launch_width),
        min(r[-1], racetrack_radius + 3.0 * launch_width),
        launch_r_count,
    )
    launch_r_weights = normalized_gaussian(launch_r, racetrack_radius, launch_width)

    max_angle = np.deg2rad(max_angle_deg)
    angle_spread = np.deg2rad(angle_spread_deg)
    angles = np.linspace(-max_angle, max_angle, angle_count)
    angle_weights = normalized_gaussian(angles, 0.0, angle_spread)

    z_top = chamber_length + 0.5 * dz
    ion_mfp = max(ionization_mfp_at_peak, 1.0e-12)
    energy_loss_length = max(energy_attenuation_length, 1.0e-12)
    wafer_cutoff = 0.5 * (1.0 + np.tanh((z - wafer_cutoff_z) / max(wafer_cutoff_width, 1.0e-12)))

    for r_launch, wr in zip(launch_r, launch_r_weights):
        for angle, wa in zip(angles, angle_weights):
            slope = np.tan(angle)
            geometric_factor = float(np.sqrt(1.0 + slope * slope))
            survival = 1.0
            previous_z = z_top

            for iz in range(len(z) - 1, -1, -1):
                z_cell = float(z[iz])
                distance_from_target = max(z_top - z_cell, 0.0)
                step_z = max(previous_z - z_cell, dz)
                ds = step_z * geometric_factor
                previous_z = z_cell

                center_r = r_launch + slope * distance_from_target
                sigma_r = max(
                    0.75 * dr,
                    lateral_scatter_sqrt_m * np.sqrt(distance_from_target + dz),
                    lateral_scatter_linear * distance_from_target,
                )
                lateral = np.exp(-0.5 * ((r - center_r) / sigma_r) ** 2)
                lateral[r < 0.0] = 0.0
                lateral_sum = float(np.sum(lateral))
                if lateral_sum <= 0.0 or not np.isfinite(lateral_sum):
                    continue
                lateral /= lateral_sum

                row_collision = np.maximum(collisionality[:, iz], 0.0) / ion_mfp
                row_loss_rate = float(np.sum(lateral * row_collision))
                if row_loss_rate <= 0.0 or not np.isfinite(row_loss_rate):
                    survival *= np.exp(-ds / energy_loss_length)
                    continue

                deposit_fraction = survival * (1.0 - np.exp(-row_loss_rate * ds))
                spatial_pdf = lateral * row_collision / row_loss_rate
                source_shape[:, iz] += float(wr * wa) * deposit_fraction * spatial_pdf * wafer_cutoff[iz]

                survival *= np.exp(-row_loss_rate * ds)
                survival *= np.exp(-ds / energy_loss_length)
                if survival < 1.0e-6:
                    break

    shape_max = float(np.nanmax(source_shape))
    if shape_max <= 0.0 or not np.isfinite(shape_max):
        raise ValueError("fast-electron source is zero")
    s_cu_eff = source_peak * source_shape / shape_max
    return s_cu_eff, energy_per_pair * s_cu_eff


def build_hpem_like_see_source(
    r: np.ndarray,
    z: np.ndarray,
    n_cu: np.ndarray,
    *,
    chamber_length: float,
    racetrack_radius: float,
    source_peak: float,
    energy_per_pair: float,
    wafer_cutoff_z: float,
    wafer_cutoff_width: float,
) -> tuple[np.ndarray, np.ndarray]:
    """Build an HPEM-like SEE source map with a top/right trajectory.

    This is an analytic surrogate for the HPEM SEB-E pattern: a hot source
    near the upper racetrack/right side, a broad upper band, and a diagonal
    downstream lobe produced by fast electrons leaving the target region.
    """
    rr, zz = np.meshgrid(r, z, indexing="ij")
    distance_from_target = np.maximum(chamber_length - zz, 0.0)
    neutral_weight = 0.35 + 0.65 * positive_normalize(n_cu) ** 0.45

    top_hot = np.exp(-0.5 * ((rr - (racetrack_radius + 0.020)) / 0.035) ** 2)
    top_hot *= np.exp(-0.5 * ((zz - (chamber_length - 0.020)) / 0.020) ** 2)

    top_band = np.exp(-0.5 * ((zz - (chamber_length - 0.040)) / 0.040) ** 2)
    top_band *= 0.40 + 0.60 * np.exp(-0.5 * ((rr - racetrack_radius) / 0.130) ** 2)

    trajectory_center = racetrack_radius + 0.020 - 0.42 * distance_from_target
    trajectory_width = 0.030 + 0.22 * distance_from_target
    trajectory = np.exp(-0.5 * ((rr - trajectory_center) / trajectory_width) ** 2)
    trajectory *= np.exp(-distance_from_target / 0.26)
    trajectory *= 1.0 / (1.0 + np.exp((distance_from_target - 0.22) / 0.025))

    right_side_lobe = np.exp(-0.5 * ((rr - (r[-1] - 0.018)) / 0.040) ** 2)
    right_side_lobe *= np.exp(-0.5 * ((zz - 0.70 * chamber_length) / 0.20) ** 2)

    broad_fill = np.exp(-0.5 * ((rr - 0.60 * r[-1]) / (0.35 * r[-1])) ** 2)
    broad_fill *= np.exp(-0.5 * ((zz - 0.45 * chamber_length) / (0.45 * chamber_length)) ** 2)
    left_suppression = 0.18 + 0.82 / (1.0 + np.exp(-(rr - 0.115) / 0.018))

    wafer_cutoff = 0.5 * (
        1.0 + np.tanh((zz - wafer_cutoff_z) / max(wafer_cutoff_width, 1.0e-12))
    )
    source_shape = (
        2.2 * top_hot
        + 1.10 * top_band
        + 1.05 * trajectory
        + 0.22 * right_side_lobe
        + 0.010 * broad_fill
    )
    source_shape *= left_suppression
    source_shape *= neutral_weight * wafer_cutoff

    shape_max = float(np.nanmax(source_shape))
    if shape_max <= 0.0 or not np.isfinite(shape_max):
        raise ValueError("HPEM-like SEE source is zero")
    s_cu_eff = source_peak * source_shape / shape_max
    return s_cu_eff, energy_per_pair * s_cu_eff


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate HPEM-like R-Z 30 cm MOOSE tables.")
    parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    parser.add_argument("--chamber-radius", type=float, default=0.30)
    parser.add_argument("--chamber-length", type=float, default=0.30)
    parser.add_argument("--nx", type=int, default=121)
    parser.add_argument("--ny", type=int, default=161)
    parser.add_argument("--racetrack-radius", type=float, default=0.235)
    parser.add_argument("--source-peak", type=float, default=1.0e22)
    parser.add_argument("--energy-per-pair", type=float, default=20.0)
    parser.add_argument(
        "--source-map",
        choices=("hpem-see", "transport"),
        default="hpem-see",
        help=(
            "Source-map shape for S_Cu_eff. 'hpem-see' gives a top/right SEE trajectory "
            "like the HPEM SEB-E map; 'transport' keeps the older ray-deposition surrogate."
        ),
    )
    parser.add_argument("--peak-b-t", type=float, default=0.10)
    parser.add_argument(
        "--magnetron-model",
        choices=("single-loop", "two-loop"),
        default="two-loop",
        help="Source-magnet topology; the image-calibrated two-loop model is the default.",
    )
    parser.add_argument(
        "--magnetron-axial-length",
        type=float,
        default=DEFAULT_MAGNETRON_AXIAL_LENGTH,
        help=(
            "Axial localization scale of the strong target magnetron field in meters "
            "(default: 0.012 m)."
        ),
    )
    parser.add_argument(
        "--two-loop-source-center",
        type=float,
        default=DEFAULT_TWO_LOOP_SOURCE_CENTER,
        help="Radial center rc of the three target pole bands in meters.",
    )
    parser.add_argument(
        "--two-loop-pole-separation",
        type=float,
        default=DEFAULT_TWO_LOOP_POLE_SEPARATION,
        help="Radial spacing d between adjacent target pole bands in meters.",
    )
    parser.add_argument(
        "--two-loop-pole-width",
        type=float,
        default=DEFAULT_TWO_LOOP_POLE_WIDTH,
        help="Gaussian one-sigma width of each target pole band in meters.",
    )
    parser.add_argument(
        "--two-loop-source-depth",
        type=float,
        default=DEFAULT_TWO_LOOP_SOURCE_DEPTH,
        help="Depth of the front virtual pole behind the target plane in meters.",
    )
    parser.add_argument(
        "--two-loop-back-depth",
        type=float,
        default=DEFAULT_TWO_LOOP_BACK_DEPTH,
        help="Depth of the opposite back pole behind the target plane in meters.",
    )
    parser.add_argument(
        "--two-loop-core-radius",
        type=float,
        default=DEFAULT_TWO_LOOP_CORE_RADIUS,
        help="Finite regularization radius of each virtual pole ring in meters.",
    )
    parser.add_argument(
        "--two-loop-null-r",
        type=float,
        default=DEFAULT_TWO_LOOP_NULL_R,
        help="Requested radial coordinate of the source-field null in meters.",
    )
    parser.add_argument(
        "--two-loop-null-z",
        type=float,
        default=DEFAULT_TWO_LOOP_NULL_Z,
        help="Requested axial coordinate of the source-field null in meters.",
    )
    parser.add_argument(
        "--two-loop-radial-quadrature",
        type=int,
        default=DEFAULT_TWO_LOOP_RADIAL_QUADRATURE,
        help="Gauss-Legendre order across each finite pole band.",
    )
    parser.add_argument(
        "--two-loop-azimuthal-quadrature",
        type=int,
        default=DEFAULT_TWO_LOOP_AZIMUTHAL_QUADRATURE,
        help="Periodic quadrature count around each virtual annular pole.",
    )
    parser.add_argument(
        "--two-loop-polarity",
        type=float,
        choices=(-1.0, 1.0),
        default=DEFAULT_TWO_LOOP_POLARITY,
        help="Global source-magnet polarity; -1 points the bulk fan toward the target.",
    )
    parser.add_argument(
        "--two-loop-source-radii",
        type=float,
        nargs=3,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-source-height",
        type=float,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-source-strengths",
        type=float,
        nargs=3,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-left-center",
        type=float,
        nargs=2,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-right-center",
        type=float,
        nargs=2,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-cusp",
        type=float,
        nargs=2,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-radial-width",
        type=float,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-axial-width",
        type=float,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-return-radial-width",
        type=float,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--two-loop-return-axial-width",
        type=float,
        default=None,
        help=argparse.SUPPRESS,
    )
    parser.add_argument(
        "--bfield-loop-fraction",
        type=float,
        default=0.50,
        help="Relative strength of the closed magnetron loop component before peak normalization.",
    )
    parser.add_argument(
        "--bfield-open-guide-fraction",
        type=float,
        default=0.05,
        help="Relative strength of the localized magnetron axial guide before normalization.",
    )
    parser.add_argument(
        "--external-bulk-b-t",
        type=float,
        default=0.003,
        help="Target median magnitude of the four-coil central bulk field in Tesla.",
    )
    parser.add_argument(
        "--external-wafer-b-t",
        type=float,
        default=0.0006,
        help="Legacy option retained for command compatibility; physical coils ignore it.",
    )
    parser.add_argument(
        "--source-steering-fraction",
        type=float,
        default=DEFAULT_SOURCE_STEERING_FRACTION,
        help="Legacy option retained for command compatibility; physical coils ignore it.",
    )
    parser.add_argument(
        "--external-coil-enhancement-t",
        type=float,
        default=0.0,
        help="Legacy option retained for command compatibility; physical coils ignore it.",
    )
    parser.add_argument(
        "--external-full-below-source-b-t",
        type=float,
        default=0.0016,
        help=(
            "Use the complete external field where the source-only magnitude is at or "
            "below this value in Tesla (default: 16 G)."
        ),
    )
    parser.add_argument(
        "--external-zero-above-source-b-t",
        type=float,
        default=0.0064,
        help=(
            "Suppress the external field where the source-only magnitude is at or "
            "above this value in Tesla (default: 64 G)."
        ),
    )
    parser.add_argument(
        "--external-full-below-z",
        type=float,
        default=DEFAULT_EXTERNAL_FULL_BELOW_Z,
        help="Use the complete external field below this axial position in meters.",
    )
    parser.add_argument(
        "--external-zero-above-z",
        type=float,
        default=DEFAULT_EXTERNAL_ZERO_ABOVE_Z,
        help="Suppress the external field above this axial position in meters.",
    )
    parser.add_argument(
        "--external-coil-z",
        type=float,
        nargs=4,
        default=DEFAULT_EXTERNAL_COIL_Z,
        metavar=("Z1", "Z2", "Z3", "Z4"),
        help="Axial centers in meters of the four circular current loops.",
    )
    parser.add_argument(
        "--external-coil-radius",
        type=float,
        default=DEFAULT_EXTERNAL_COIL_RADIUS,
        help="Common circular-loop radius in meters; it must lie outside the chamber.",
    )
    parser.add_argument(
        "--external-coil-weights",
        type=float,
        nargs=4,
        default=DEFAULT_EXTERNAL_COIL_WEIGHTS,
        metavar=("W1", "W2", "W3", "W4"),
        help="Relative currents of the four loops; equal positive weights are the default.",
    )
    parser.add_argument(
        "--external-coil-quadrature",
        type=int,
        default=DEFAULT_EXTERNAL_COIL_QUADRATURE,
        help="Periodic azimuthal quadrature points per circular loop.",
    )
    parser.add_argument("--neutral-background", type=float, default=3.0e18)
    parser.add_argument("--neutral-plume-peak", type=float, default=4.0e18)
    parser.add_argument("--ionization-mfp-at-peak", type=float, default=0.12)
    parser.add_argument("--energy-attenuation-length", type=float, default=0.30)
    parser.add_argument("--lateral-scatter-sqrt-m", type=float, default=0.055)
    parser.add_argument("--lateral-scatter-linear", type=float, default=0.20)
    args = parser.parse_args()

    r = np.linspace(0.0, args.chamber_radius, args.nx)
    z = np.linspace(0.0, args.chamber_length, args.ny)

    n_cu = build_neutral_cu(
        r,
        z,
        chamber_length=args.chamber_length,
        racetrack_radius=args.racetrack_radius,
        background_density=args.neutral_background,
        plume_peak_density=args.neutral_plume_peak,
        radial_width=0.080,
        axial_width=0.090,
    )
    br, bz = build_combined_bfield(
        r,
        z,
        chamber_length=args.chamber_length,
        chamber_radius=args.chamber_radius,
        racetrack_radius=args.racetrack_radius,
        magnetron_peak_b_t=args.peak_b_t,
        magnetron_axial_length=args.magnetron_axial_length,
        magnetron_model=args.magnetron_model,
        external_bulk_b_t=args.external_bulk_b_t,
        external_wafer_b_t=args.external_wafer_b_t,
        source_steering_fraction=args.source_steering_fraction,
        external_coil_enhancement_t=args.external_coil_enhancement_t,
        external_coil_z=tuple(args.external_coil_z),
        external_coil_radius=args.external_coil_radius,
        external_coil_weights=tuple(args.external_coil_weights),
        external_coil_quadrature=args.external_coil_quadrature,
        magnetron_loop_fraction=args.bfield_loop_fraction,
        magnetron_open_guide_fraction=args.bfield_open_guide_fraction,
        external_full_below_t=args.external_full_below_source_b_t,
        external_zero_above_t=args.external_zero_above_source_b_t,
        external_full_below_z=args.external_full_below_z,
        external_zero_above_z=args.external_zero_above_z,
        two_loop_source_center=args.two_loop_source_center,
        two_loop_pole_separation=args.two_loop_pole_separation,
        two_loop_pole_width=args.two_loop_pole_width,
        two_loop_source_depth=args.two_loop_source_depth,
        two_loop_back_depth=args.two_loop_back_depth,
        two_loop_core_radius=args.two_loop_core_radius,
        two_loop_null_r=args.two_loop_null_r,
        two_loop_null_z=args.two_loop_null_z,
        two_loop_radial_quadrature=args.two_loop_radial_quadrature,
        two_loop_azimuthal_quadrature=args.two_loop_azimuthal_quadrature,
        two_loop_polarity=args.two_loop_polarity,
    )
    if args.source_map == "hpem-see":
        s_cu_eff, qe_eff = build_hpem_like_see_source(
            r,
            z,
            n_cu,
            chamber_length=args.chamber_length,
            racetrack_radius=args.racetrack_radius,
            source_peak=args.source_peak,
            energy_per_pair=args.energy_per_pair,
            wafer_cutoff_z=0.035,
            wafer_cutoff_width=0.015,
        )
    else:
        s_cu_eff, qe_eff = build_fast_electron_sources(
            r,
            z,
            n_cu,
            br,
            bz,
            chamber_length=args.chamber_length,
            racetrack_radius=args.racetrack_radius,
            source_peak=args.source_peak,
            energy_per_pair=args.energy_per_pair,
            launch_width=0.035,
            launch_r_count=49,
            angle_count=65,
            max_angle_deg=78.0,
            angle_spread_deg=42.0,
            ionization_mfp_at_peak=args.ionization_mfp_at_peak,
            energy_attenuation_length=args.energy_attenuation_length,
            lateral_scatter_sqrt_m=args.lateral_scatter_sqrt_m,
            lateral_scatter_linear=args.lateral_scatter_linear,
            magnetic_residence_factor=2.0,
            wafer_cutoff_z=0.035,
            wafer_cutoff_width=0.015,
        )

    args.out_dir.mkdir(parents=True, exist_ok=True)
    write_moose_table(args.out_dir / "n_Cu_m3.tbl", r, z, n_cu)
    write_moose_table(args.out_dir / "S_iz_Cu_m3_s.tbl", r, z, s_cu_eff)
    write_moose_table(args.out_dir / "S_Cu_eff_m3_s.tbl", r, z, s_cu_eff)
    write_moose_table(args.out_dir / "Qe_eff_eV_m3_s.tbl", r, z, qe_eff)
    write_moose_table(args.out_dir / "Bx_T.tbl", r, z, br)
    write_moose_table(args.out_dir / "By_T.tbl", r, z, bz)

    print(f"wrote HPEM-like R-Z tables to {args.out_dir}")
    print(f"source map: {args.source_map}")
    magnetron_geometry = (
        f"magnetron length={args.magnetron_axial_length:.3e} m, "
        if args.magnetron_model == "single-loop"
        else ""
    )
    print(
        "B-field components: "
        f"magnetron model={args.magnetron_model}, "
        f"magnetron peak={args.peak_b_t:.3e} T, "
        f"{magnetron_geometry}"
        f"external bulk={args.external_bulk_b_t:.3e} T, "
        f"coil radius={args.external_coil_radius:.3f} m, "
        f"coil weights={tuple(args.external_coil_weights)}, "
        f"coil quadrature={args.external_coil_quadrature}, "
        f"full external below={args.external_full_below_source_b_t / 1.0e-4:.1f} G, "
        f"zero external above={args.external_zero_above_source_b_t / 1.0e-4:.1f} G, "
        f"axial blend={args.external_full_below_z:.3f}.."
        f"{args.external_zero_above_z:.3f} m"
    )
    if args.magnetron_model == "two-loop":
        print(
            "two-loop calibration: "
            f"center={args.two_loop_source_center:.4f} m, "
            f"separation={args.two_loop_pole_separation:.4f} m, "
            f"width={args.two_loop_pole_width:.4f} m, "
            f"depths=({args.two_loop_source_depth:.4f}, "
            f"{args.two_loop_back_depth:.4f}) m, "
            f"core={args.two_loop_core_radius:.4f} m, "
            f"null=({args.two_loop_null_r:.3f}, {args.two_loop_null_z:.3f}) m, "
            f"polarity={args.two_loop_polarity:+.0f}, "
            f"quadrature=({args.two_loop_radial_quadrature}, "
            f"{args.two_loop_azimuthal_quadrature})"
        )
    print(f"n_Cu range: {np.nanmin(n_cu):.3e} .. {np.nanmax(n_cu):.3e} m^-3")
    print(f"S_Cu_eff range: {np.nanmin(s_cu_eff):.3e} .. {np.nanmax(s_cu_eff):.3e} m^-3 s^-1")
    print(f"|B| range: {np.nanmin(np.hypot(br, bz)):.3e} .. {np.nanmax(np.hypot(br, bz)):.3e} T")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
