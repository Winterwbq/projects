#!/usr/bin/env python3
"""Generate the three supported Cartesian X-Z magnetic-field table pairs."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
CASE_NAMES = ("source_only", "four_coil", "four_coil_img3092")
CASE_OUTPUT_DIRS = {
    case: ROOT / "runs" / f"zapdos_cartesian_xz_25x30_{case}" / "moose_tables"
    for case in CASE_NAMES
}

X_MIN_M = 0.0
X_MAX_M = 0.25
Z_MIN_M = 0.0
Z_MAX_M = 0.30
SOURCE_CENTER_M = 0.205
SOURCE_POLE_OFFSET_M = 0.010
SOURCE_DIPOLE_CENTER_Z_M = 0.325
B_FIELD_MIN_T = 1.0e-4
B_FIELD_MAX_T = 0.1
B_FIELD_FLOOR_Z_M = 0.12
GAUSS_TO_TESLA = 1.0e-4

REFERENCE_PROFILE_DISTANCE_M = 1.0e-3 * np.asarray(
    [0.0, 9.0, 13.0, 18.0, 45.0, 65.0, 75.0, 105.0, 135.0, 192.0, 245.0, 300.0]
)
REFERENCE_PROFILE_MAX_G = np.asarray(
    [1000.0, 138.95, 84.83, 51.79, 31.62, 19.31, 11.79, 7.20, 4.39, 2.68, 1.64, 1.0]
)

COIL_RADIUS_M = 0.280
COIL_Z_M = (0.250, 0.150, 0.055, 0.030)
COIL_SOFTENING_M = 0.006
COIL_PEAK_T = 227.58 * GAUSS_TO_TESLA
STANDARD_COIL_WEIGHTS = (44.6, 12.6, 3.2, -5.2)
IMAGE_COIL_WEIGHTS = (44.6, 12.6, 3.2, 13.7)

STANDARD_GUIDE_Z_M = 1.0e-3 * np.asarray(
    [0.0, 25.0, 50.0, 90.0, 135.0, 200.0, 275.0, 300.0]
)
STANDARD_GUIDE_G = np.asarray([3.40, 6.00, 10.30, 29.40, 48.60, 55.00, 55.00, 55.00])
IMAGE_GUIDE_Z_M = np.asarray([0.0, 0.025, 0.055, 0.100, 0.150, 0.200, 0.250, 0.300])
IMAGE_GUIDE_G = np.asarray([51.79, 84.83, 105.0, 115.0, 110.0, 78.0, 68.0, 84.83])


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(f"{name} must be one-dimensional with at least two values")
    if np.any(~np.isfinite(axis)) or np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must be finite and strictly increasing")
    return axis


def write_moose_table(
    path: Path | str, x: np.ndarray, z: np.ndarray, values: np.ndarray
) -> None:
    x_axis = _validate_axis("x", x)
    z_axis = _validate_axis("z", z)
    field = np.asarray(values, dtype=float)
    if field.shape != (x_axis.size, z_axis.size) or np.any(~np.isfinite(field)):
        raise ValueError("values must be a finite array with shape (len(x), len(z))")
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    with destination.open("w", encoding="utf-8") as handle:
        handle.write("AXIS X\n")
        handle.write(" ".join(f"{value:.8e}" for value in x_axis) + "\n\n")
        handle.write("AXIS Y\n")
        handle.write(" ".join(f"{value:.8e}" for value in z_axis) + "\n\n")
        handle.write("DATA\n")
        for iz in range(z_axis.size):
            handle.write(" ".join(f"{value:.8e}" for value in field[:, iz]) + "\n")


def _smooth_interpolate(
    values: np.ndarray, control_x: np.ndarray, control_y: np.ndarray
) -> np.ndarray:
    query = np.asarray(values, dtype=float)
    x = np.asarray(control_x, dtype=float)
    y = np.asarray(control_y, dtype=float)
    segment = np.clip(np.searchsorted(x, query, side="right") - 1, 0, x.size - 2)
    fraction = np.clip(
        (query - x[segment]) / (x[segment + 1] - x[segment]), 0.0, 1.0
    )
    smooth = fraction**2 * (3.0 - 2.0 * fraction)
    return y[segment] + smooth * (y[segment + 1] - y[segment])


def _limit_magnitude(
    bx: np.ndarray,
    bz: np.ndarray,
    minimum_t: float = B_FIELD_MIN_T,
    maximum_t: float = B_FIELD_MAX_T,
) -> tuple[np.ndarray, np.ndarray]:
    x_component = np.asarray(bx, dtype=float).copy()
    z_component = np.asarray(bz, dtype=float).copy()
    if x_component.shape != z_component.shape or x_component.ndim != 2:
        raise ValueError("B components must be matching two-dimensional arrays")
    magnitude = np.hypot(x_component, z_component)
    nonzero = magnitude > np.finfo(float).tiny
    target = np.clip(magnitude, minimum_t, maximum_t)
    scale = np.ones_like(magnitude)
    scale[nonzero] = target[nonzero] / magnitude[nonzero]
    x_component *= scale
    z_component *= scale
    x_component[~nonzero] = 0.0
    z_component[~nonzero] = minimum_t
    return x_component, z_component


def _calibrated_source_magnitude(
    x: np.ndarray,
    z: np.ndarray,
    *,
    source_center: float = SOURCE_CENTER_M,
    pole_offset: float = SOURCE_POLE_OFFSET_M,
) -> np.ndarray:
    """Return the established 1--1000 G source-field magnitude calibration."""
    xx, zz = np.meshgrid(x, z, indexing="ij")
    pole_z = z[-1] + 0.006
    pole_x = np.asarray([source_center - pole_offset, source_center, source_center + pole_offset])

    def pole_field(location: float, qx: np.ndarray, qz: np.ndarray):
        dx = qx - location
        dz = qz - pole_z
        inverse_cube = (dx**2 + dz**2 + 0.010**2) ** (-1.5)
        return dx * inverse_cube, dz * inverse_cube

    null_x = np.asarray([[source_center]])
    null_z = np.asarray([[z[-1] - 0.010]])
    null_fields = [pole_field(location, null_x, null_z) for location in pole_x]
    matrix = np.asarray(
        [
            [null_fields[1][0][0, 0], null_fields[2][0][0, 0]],
            [null_fields[1][1][0, 0], null_fields[2][1][0, 0]],
        ]
    )
    rhs = -np.asarray([null_fields[0][0][0, 0], null_fields[0][1][0, 0]])
    amplitudes = np.concatenate(([1.0], np.linalg.solve(matrix, rhs)))
    fields = [pole_field(location, xx, zz) for location in pole_x]
    raw_x = -sum(weight * field[0] for weight, field in zip(amplitudes, fields))
    raw_z = -sum(weight * field[1] for weight, field in zip(amplitudes, fields))
    raw_magnitude = np.hypot(raw_x, raw_z)
    slice_max = np.maximum(np.max(raw_magnitude, axis=0), np.finfo(float).tiny)
    distance = np.maximum(z[-1] - z, 0.0)
    log_profile = np.interp(
        distance,
        REFERENCE_PROFILE_DISTANCE_M,
        np.log(REFERENCE_PROFILE_MAX_G / REFERENCE_PROFILE_MAX_G[0]),
    )
    desired = B_FIELD_MAX_T * np.exp(log_profile)
    raw_magnitude *= (desired / slice_max)[None, :]
    raw_max = float(np.max(raw_magnitude))
    raw_magnitude *= B_FIELD_MAX_T / raw_max
    return np.maximum(raw_magnitude, B_FIELD_MIN_T)


def _compress_source_magnitude(
    z: np.ndarray, bx: np.ndarray, bz: np.ndarray
) -> tuple[np.ndarray, np.ndarray]:
    magnitude = np.hypot(bx, bz)
    normalized = np.clip(
        (z - B_FIELD_FLOOR_Z_M) / (z[-1] - B_FIELD_FLOOR_Z_M), 0.0, 1.0
    )
    lookup_z = z[0] + (z[-1] - z[0]) * normalized**2 * (3.0 - 2.0 * normalized)
    remapped = np.empty_like(magnitude)
    for ix in range(magnitude.shape[0]):
        remapped[ix] = np.interp(lookup_z, z, magnitude[ix])
    target = np.maximum(remapped, B_FIELD_MIN_T)
    target[:, z <= B_FIELD_FLOOR_Z_M] = B_FIELD_MIN_T
    scale = target / np.maximum(magnitude, np.finfo(float).tiny)
    return bx * scale, bz * scale


def build_source_only_bfield(x: np.ndarray, z: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    """Build the translated two-arch Cartesian source field."""
    x_axis = _validate_axis("x", x)
    z_axis = _validate_axis("z", z)
    if not x_axis[0] <= SOURCE_CENTER_M - SOURCE_POLE_OFFSET_M:
        raise ValueError("source poles lie outside the X domain")
    magnitude = _calibrated_source_magnitude(x_axis, z_axis)
    xx, zz = np.meshgrid(x_axis, z_axis, indexing="ij")
    dx = xx - SOURCE_CENTER_M
    dz = zz - SOURCE_DIPOLE_CENTER_Z_M
    direction_x = 3.0 * dx * dz
    direction_z = 2.0 * dz**2 - dx**2
    norm = np.hypot(direction_x, direction_z)
    bx = magnitude * direction_x / norm
    bz = magnitude * direction_z / norm
    return _compress_source_magnitude(z_axis, bx, bz)


def _raw_coil_field(
    z: np.ndarray,
    weights: tuple[float, float, float, float],
    *,
    quadrature: int,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Evaluate coils on an internal support grid extending beyond the chamber."""
    if quadrature < 32:
        raise ValueError("coil_quadrature must be at least 32")
    support_x = np.linspace(0.0, 0.40, 161)
    centers = np.asarray(COIL_Z_M)
    normalized_weights = np.asarray(weights) / np.max(np.abs(weights))
    rr = support_x[:, None, None]
    raw_bx = np.zeros((support_x.size, z.size))
    raw_bz = np.zeros_like(raw_bx)
    phi = (np.arange(quadrature, dtype=float) + 0.5) * (2.0 * np.pi / quadrature)
    dphi = 2.0 * np.pi / quadrature
    for center, current in zip(centers, normalized_weights):
        axial = z[None, :] - center
        for start in range(0, quadrature, 32):
            cosine = np.cos(phi[start : start + 32])[None, None, :]
            distance_squared = (
                rr**2
                + COIL_RADIUS_M**2
                - 2.0 * rr * COIL_RADIUS_M * cosine
                + axial[:, :, None] ** 2
                + COIL_SOFTENING_M**2
            )
            inverse_cube = distance_squared ** (-1.5)
            raw_bx += (
                current
                * dphi
                * COIL_RADIUS_M
                * axial
                * np.sum(cosine * inverse_cube, axis=2)
            )
            raw_bz += (
                current
                * dphi
                * COIL_RADIUS_M
                * np.sum((COIL_RADIUS_M - rr * cosine) * inverse_cube, axis=2)
            )
    raw_bx[0] = 0.0
    xx, zz = np.meshgrid(support_x, z, indexing="ij")
    neighborhood = (
        (np.abs(xx - COIL_RADIUS_M) <= 2.0 * COIL_SOFTENING_M)
        & (np.abs(zz - COIL_Z_M[0]) <= 2.0 * COIL_SOFTENING_M)
    )
    peak = float(np.max(np.hypot(raw_bx, raw_bz)[neighborhood]))
    if peak <= 0.0 or not np.isfinite(peak):
        raise ValueError("coil calibration region has zero field")
    scale = COIL_PEAK_T / peak
    return support_x, scale * raw_bx, scale * raw_bz


def _interpolate_support(
    support_x: np.ndarray, field: np.ndarray, x: np.ndarray
) -> np.ndarray:
    result = np.empty((x.size, field.shape[1]))
    for iz in range(field.shape[1]):
        result[:, iz] = np.interp(x, support_x, field[:, iz])
    return result


def _build_standard_coil_field(
    x: np.ndarray, z: np.ndarray, *, quadrature: int
) -> tuple[np.ndarray, np.ndarray]:
    support_x, support_bx, support_bz = _raw_coil_field(
        z, STANDARD_COIL_WEIGHTS, quadrature=quadrature
    )
    bx = _interpolate_support(support_x, support_bx, x)
    bz = _interpolate_support(support_x, support_bz, x)
    bulk = (x >= 0.020) & (x <= 0.200)
    median = np.median(np.hypot(bx, bz)[bulk], axis=0)
    desired = GAUSS_TO_TESLA * _smooth_interpolate(z, STANDARD_GUIDE_Z_M, STANDARD_GUIDE_G)
    bx *= (desired / median)[None, :]
    bz *= (desired / median)[None, :]
    xx, zz = np.meshgrid(x, z, indexing="ij")
    dome = 45.0 * GAUSS_TO_TESLA * np.exp(
        -0.5 * (((xx - 0.125) / 0.150) ** 2 + ((zz - 0.245) / 0.055) ** 2)
    )
    magnitude = np.hypot(bx, bz)
    bx += dome * bx / np.maximum(magnitude, np.finfo(float).tiny)
    bz += dome * bz / np.maximum(magnitude, np.finfo(float).tiny)
    magnitude = np.hypot(bx, bz)
    cap = np.minimum(1.0, COIL_PEAK_T / np.maximum(magnitude, np.finfo(float).tiny))
    return bx * cap, bz * cap


def _build_image_coil_field(
    x: np.ndarray, z: np.ndarray, *, quadrature: int
) -> tuple[np.ndarray, np.ndarray]:
    support_x, support_bx, support_bz = _raw_coil_field(
        z, IMAGE_COIL_WEIGHTS, quadrature=quadrature
    )
    bx = _interpolate_support(support_x, support_bx, x)
    bz = _interpolate_support(support_x, support_bz, x)
    bulk = (x >= 0.020) & (x <= 0.180)
    median = np.median(np.hypot(bx, bz)[bulk], axis=0)
    desired = GAUSS_TO_TESLA * _smooth_interpolate(z, IMAGE_GUIDE_Z_M, IMAGE_GUIDE_G)
    bx *= (desired / median)[None, :]
    bz *= (desired / median)[None, :]
    xx, zz = np.meshgrid(x, z, indexing="ij")
    bottom_boost = 160.0 * GAUSS_TO_TESLA * np.exp(
        -0.5 * (((xx - 0.280) / 0.050) ** 2 + ((zz - 0.040) / 0.040) ** 2)
    )
    magnitude = np.hypot(bx, bz)
    bx += bottom_boost * bx / np.maximum(magnitude, np.finfo(float).tiny)
    bz += bottom_boost * bz / np.maximum(magnitude, np.finfo(float).tiny)
    straightening = 0.35 + 0.65 / (1.0 + np.exp(-(x - 0.250) / 0.012))
    bx *= straightening[:, None]
    return bx, bz


def build_case_bfield(
    case: str,
    x: np.ndarray,
    z: np.ndarray,
    *,
    coil_quadrature: int = 256,
) -> tuple[np.ndarray, np.ndarray]:
    """Return ``Bx, Bz`` for one supported Cartesian magnetic case."""
    if case not in CASE_NAMES:
        raise ValueError(f"unknown case {case!r}; choose one of {', '.join(CASE_NAMES)}")
    x_axis = _validate_axis("x", x)
    z_axis = _validate_axis("z", z)
    source_bx, source_bz = build_source_only_bfield(x_axis, z_axis)
    if case == "source_only":
        return _limit_magnitude(source_bx, source_bz)
    if case == "four_coil":
        coil_bx, coil_bz = _build_standard_coil_field(
            x_axis, z_axis, quadrature=coil_quadrature
        )
        bx = source_bx + coil_bx
        bz = source_bz + coil_bz
        xx, zz = np.meshgrid(x_axis, z_axis, indexing="ij")
        bx *= 1.0 + 1.80 * np.exp(-0.5 * ((xx - 0.150) / 0.075) ** 2) * np.exp(
            -zz / 0.080
        )
        magnitude = np.hypot(bx, bz)
        void = np.exp(-0.5 * (((xx - 0.150) / 0.075) ** 2 + (zz / 0.018) ** 2))
        target = np.clip(magnitude * (1.0 - 0.90 * void), B_FIELD_MIN_T, B_FIELD_MAX_T)
        scale = target / np.maximum(magnitude, np.finfo(float).tiny)
        return bx * scale, bz * scale
    coil_bx, coil_bz = _build_image_coil_field(
        x_axis, z_axis, quadrature=coil_quadrature
    )
    return _limit_magnitude(source_bx + coil_bx, source_bz + coil_bz)


def generate_bfield_case(
    case: str,
    output_dir: Path | str | None = None,
    *,
    nx: int = 101,
    nz: int = 161,
    coil_quadrature: int = 256,
) -> dict[str, object]:
    if nx < 3 or nz < 3:
        raise ValueError("nx and nz must each be at least three")
    directory = Path(output_dir) if output_dir is not None else CASE_OUTPUT_DIRS[case]
    x = np.linspace(X_MIN_M, X_MAX_M, int(nx))
    z = np.linspace(Z_MIN_M, Z_MAX_M, int(nz))
    bx, bz = build_case_bfield(case, x, z, coil_quadrature=coil_quadrature)
    directory.mkdir(parents=True, exist_ok=True)
    bx_path = directory / "Bx_T.tbl"
    bz_path = directory / "By_T.tbl"
    write_moose_table(bx_path, x, z, bx)
    write_moose_table(bz_path, x, z, bz)
    magnitude = np.hypot(bx, bz)
    metadata: dict[str, object] = {
        "case": case,
        "coordinate_system": "cartesian_x_z",
        "domain_m": {"x": [X_MIN_M, X_MAX_M], "z": [Z_MIN_M, Z_MAX_M]},
        "grid": {"nx": int(nx), "nz": int(nz)},
        "bfield_magnitude_range_t": [float(np.min(magnitude)), float(np.max(magnitude))],
        "bfield_sha256": {
            "Bx_T.tbl": hashlib.sha256(bx_path.read_bytes()).hexdigest(),
            "By_T.tbl": hashlib.sha256(bz_path.read_bytes()).hexdigest(),
        },
    }
    (directory / "bfield_metadata.json").write_text(
        json.dumps(metadata, indent=2) + "\n", encoding="utf-8"
    )
    return metadata


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", choices=("all", *CASE_NAMES), default="all")
    parser.add_argument("--output-dir", type=Path)
    parser.add_argument("--nx", type=int, default=101)
    parser.add_argument("--nz", type=int, default=161)
    parser.add_argument("--coil-quadrature", type=int, default=256)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    if args.case == "all" and args.output_dir is not None:
        raise SystemExit("--output-dir can only be used with one --case")
    cases = CASE_NAMES if args.case == "all" else (args.case,)
    results = [
        generate_bfield_case(
            case,
            args.output_dir,
            nx=args.nx,
            nz=args.nz,
            coil_quadrature=args.coil_quadrature,
        )
        for case in cases
    ]
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
