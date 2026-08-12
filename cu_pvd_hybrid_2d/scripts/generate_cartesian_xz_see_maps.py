#!/usr/bin/env python3
"""Generate SEE maps paired to existing Cartesian X-Z B-field tables."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
CASE_NAMES = ("source_only", "four_coil", "four_coil_img3092")
CASE_TABLE_DIRS = {
    case: ROOT / "runs" / f"zapdos_cartesian_xz_25x30_{case}" / "moose_tables"
    for case in CASE_NAMES
}
DEFAULT_NEUTRAL_TABLE = (
    ROOT
    / "runs"
    / "zapdos_cartesian_xz_25x30_neutral"
    / "moose_tables"
    / "n_Cu_m3.tbl"
)

SOURCE_X_MIN_M = 0.19
SOURCE_X_MAX_M = 0.22
SOURCE_CENTER_M = 0.205
LOBE_OFFSET_M = 0.010
FIELD_FLOOR_T = 1.0e-12

CASE_SETTINGS: dict[str, dict[str, float | str]] = {
    "source_only": {
        "direction_model": "vertical_downward",
        "local_fraction": 0.995,
        "local_attenuation_length_m": 0.006,
        "guided_attenuation_length_m": 0.05,
        "spread_angle_degrees": 10.0,
        "bfield_guidance_fraction": 0.0,
        "transition_field_t": 0.001,
        "see_magnitude_scale": 1.2,
    },
    "four_coil": {
        "direction_model": "bfield_guided",
        "local_fraction": 0.95,
        "local_attenuation_length_m": 0.006,
        "guided_attenuation_length_m": 0.20,
        "spread_angle_degrees": 8.0,
        "bfield_guidance_fraction": 1.0,
        "transition_field_t": 0.001,
        "see_magnitude_scale": 1.2,
    },
    "four_coil_img3092": {
        "direction_model": "mostly_vertical_bfield_guided",
        "local_fraction": 0.90,
        "local_attenuation_length_m": 0.006,
        "guided_attenuation_length_m": 0.20,
        "spread_angle_degrees": 5.0,
        "bfield_guidance_fraction": 0.30,
        "transition_field_t": 0.001,
        "see_magnitude_scale": 1.2,
    },
}


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(f"{name} must be one-dimensional with at least two values")
    if np.any(~np.isfinite(axis)) or np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must be finite and strictly increasing")
    return axis


def read_moose_table(path: Path | str) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    sections: dict[str, list[str]] = {}
    current: str | None = None
    for raw_line in Path(path).read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if line.startswith("AXIS ") or line == "DATA":
            current = line
            sections[current] = []
        elif current is not None:
            sections[current].append(line)
    try:
        x = np.fromstring(" ".join(sections["AXIS X"]), sep=" ")
        z = np.fromstring(" ".join(sections["AXIS Y"]), sep=" ")
        rows = [np.fromstring(row, sep=" ") for row in sections["DATA"]]
    except KeyError as error:
        raise ValueError(f"{path} is missing {error.args[0]}") from error
    values_zx = np.vstack(rows)
    if values_zx.shape != (z.size, x.size):
        raise ValueError(
            f"{path} DATA shape {values_zx.shape} does not match {(z.size, x.size)}"
        )
    return _validate_axis("x", x), _validate_axis("z", z), values_zx.T


def write_moose_table(
    path: Path | str, x: np.ndarray, z: np.ndarray, values: np.ndarray
) -> None:
    x_axis = _validate_axis("x", x)
    z_axis = _validate_axis("z", z)
    field = np.asarray(values, dtype=float)
    if field.shape != (x_axis.size, z_axis.size) or np.any(~np.isfinite(field)):
        raise ValueError("values must be a finite array with shape (len(x), len(z))")
    # MOOSE's GriddedData parser treats floating-point underflow during string
    # conversion as an error. Values below the smallest normal float carry no
    # useful SEE weight, so serialize them as exact zero.
    field = np.where(np.abs(field) < np.finfo(float).tiny, 0.0, field)
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


def _trapezoid_weights(axis: np.ndarray) -> np.ndarray:
    values = _validate_axis("axis", axis)
    weights = np.empty_like(values)
    weights[0] = 0.5 * (values[1] - values[0])
    weights[-1] = 0.5 * (values[-1] - values[-2])
    weights[1:-1] = 0.5 * (values[2:] - values[:-2])
    return weights


def cartesian_area_integral(x: np.ndarray, z: np.ndarray, values: np.ndarray) -> float:
    field = np.asarray(values, dtype=float)
    if field.shape != (x.size, z.size):
        raise ValueError("values must match the X-Z grid")
    quadrature = _trapezoid_weights(x)[:, None] * _trapezoid_weights(z)[None, :]
    return float(np.sum(field * quadrature))


def _bilinear(
    x: np.ndarray, z: np.ndarray, values: np.ndarray, point_x: float, point_z: float
) -> float:
    px = float(np.clip(point_x, x[0], x[-1]))
    pz = float(np.clip(point_z, z[0], z[-1]))
    ix = int(np.clip(np.searchsorted(x, px) - 1, 0, x.size - 2))
    iz = int(np.clip(np.searchsorted(z, pz) - 1, 0, z.size - 2))
    fx = (px - x[ix]) / (x[ix + 1] - x[ix])
    fz = (pz - z[iz]) / (z[iz + 1] - z[iz])
    return float(
        (1.0 - fx) * (1.0 - fz) * values[ix, iz]
        + fx * (1.0 - fz) * values[ix + 1, iz]
        + (1.0 - fx) * fz * values[ix, iz + 1]
        + fx * fz * values[ix + 1, iz + 1]
    )


def _interpolate_grid(
    source_x: np.ndarray,
    source_z: np.ndarray,
    values: np.ndarray,
    target_x: np.ndarray,
    target_z: np.ndarray,
) -> np.ndarray:
    along_x = np.empty((target_x.size, source_z.size))
    for iz in range(source_z.size):
        along_x[:, iz] = np.interp(target_x, source_x, values[:, iz])
    result = np.empty((target_x.size, target_z.size))
    for ix in range(target_x.size):
        result[ix] = np.interp(target_z, source_z, along_x[ix])
    return result


def build_effective_neutral_density(
    x: np.ndarray, z: np.ndarray, neutral_table: Path | str
) -> np.ndarray:
    neutral_x, neutral_z, base_density = read_moose_table(neutral_table)
    base = _interpolate_grid(neutral_x, neutral_z, base_density, x, z)
    xx, zz = np.meshgrid(x, z, indexing="ij")
    magnetron = 3.0e18 * np.exp(-((xx - SOURCE_CENTER_M) / 0.080) ** 2)
    magnetron *= np.exp(-((zz - 0.270) / 0.090) ** 2)
    return 0.1 * np.maximum(base, 1.0e16) + 1.0e17 + magnetron


def _launch_ensemble(count: int) -> tuple[np.ndarray, np.ndarray]:
    if count < 2:
        raise ValueError("launch_count must be at least two")
    half = max(1, count // 2)
    left = np.linspace(SOURCE_X_MIN_M, SOURCE_CENTER_M, half, endpoint=False)
    right = np.linspace(SOURCE_CENTER_M, SOURCE_X_MAX_M, count - half)
    points = np.concatenate((left, right))
    weights = np.exp(-0.5 * ((points - (SOURCE_CENTER_M - LOBE_OFFSET_M)) / 0.010) ** 2)
    weights += np.exp(-0.5 * ((points - (SOURCE_CENTER_M + LOBE_OFFSET_M)) / 0.010) ** 2)
    weights /= np.sum(weights)
    return points, weights


def _selected_direction(
    x: np.ndarray,
    z: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    point: np.ndarray,
    *,
    guidance_fraction: float,
    transition_field: float,
) -> np.ndarray:
    local_bx = _bilinear(x, z, bx, point[0], point[1])
    local_bz = _bilinear(x, z, bz, point[0], point[1])
    magnitude = float(np.hypot(local_bx, local_bz))
    if magnitude <= FIELD_FLOOR_T:
        field_direction = np.asarray([0.0, -1.0])
    else:
        field_direction = np.asarray([local_bx, local_bz]) / magnitude
        if field_direction[1] > 0.0:
            field_direction *= -1.0
        magnetic_fraction = magnitude / (magnitude + max(transition_field, FIELD_FLOOR_T))
        field_direction = magnetic_fraction * field_direction + (1.0 - magnetic_fraction) * np.asarray([0.0, -1.0])
        field_direction /= np.linalg.norm(field_direction)
    blended = guidance_fraction * field_direction + (1.0 - guidance_fraction) * np.asarray([0.0, -1.0])
    norm = float(np.linalg.norm(blended))
    return blended / norm if norm > 1.0e-14 else np.asarray([0.0, -1.0])


def _deposit(
    output: np.ndarray,
    x: np.ndarray,
    z: np.ndarray,
    quadrature: np.ndarray,
    neutral_weight: np.ndarray,
    point: np.ndarray,
    amount: float,
    sigma: float,
) -> None:
    width = max(sigma, 1.0e-6)
    xx, zz = np.meshgrid(x, z, indexing="ij")
    kernel = np.exp(
        -0.5
        * (
            ((xx - point[0]) / width) ** 2
            + ((zz - point[1]) / width) ** 2
        )
    )
    shape = kernel * neutral_weight
    normalization = float(np.sum(shape * quadrature))
    if normalization > 0.0:
        output += amount * shape / normalization


def _trace_component(
    output: np.ndarray,
    x: np.ndarray,
    z: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    quadrature: np.ndarray,
    neutral_weight: np.ndarray,
    *,
    launch_x: float,
    launch_weight: float,
    component_fraction: float,
    attenuation_length: float,
    guidance_fraction: float,
    transition_field: float,
    spread_angle_degrees: float,
    step_length: float,
    max_path_length: float,
    survival_floor: float,
) -> tuple[float, str]:
    if component_fraction <= 0.0:
        return 0.0, "disabled"
    point = np.asarray([launch_x, z[-1] - 0.25 * step_length], dtype=float)
    survival = component_fraction
    deposited = 0.0
    distance = 0.0
    initial_sigma = 0.003
    spread = np.tan(np.deg2rad(spread_angle_degrees))
    termination = "max_path"
    while distance < max_path_length and survival > component_fraction * survival_floor:
        direction = _selected_direction(
            x,
            z,
            bx,
            bz,
            point,
            guidance_fraction=guidance_fraction,
            transition_field=transition_field,
        )
        next_point = point + step_length * direction
        midpoint = 0.5 * (point + next_point)
        if not (x[0] <= midpoint[0] <= x[-1] and z[0] <= midpoint[1] <= z[-1]):
            termination = "boundary"
            break
        fraction = 1.0 - np.exp(-step_length / attenuation_length)
        amount = survival * fraction
        _deposit(
            output,
            x,
            z,
            quadrature,
            neutral_weight,
            midpoint,
            launch_weight * amount,
            np.sqrt(
                initial_sigma**2
                + ((distance + 0.5 * step_length) * spread) ** 2
            ),
        )
        deposited += amount
        survival -= amount
        point = next_point
        distance += step_length
        if point[1] <= z[0] or point[0] <= x[0] or point[0] >= x[-1]:
            termination = "boundary"
            break
    return deposited, termination


def build_see_weight(
    case: str,
    x: np.ndarray,
    z: np.ndarray,
    bx: np.ndarray,
    bz: np.ndarray,
    neutral_density: np.ndarray,
    *,
    launch_count: int = 41,
    step_length: float = 0.0025,
    max_path_length: float = 0.90,
    survival_floor: float = 1.0e-4,
) -> tuple[np.ndarray, dict[str, object]]:
    if case not in CASE_SETTINGS:
        raise ValueError(f"unknown case {case!r}")
    settings = CASE_SETTINGS[case]
    quadrature = _trapezoid_weights(x)[:, None] * _trapezoid_weights(z)[None, :]
    neutral_weight = neutral_density / float(np.max(neutral_density))
    output = np.zeros_like(neutral_density)
    launch_points, launch_weights = _launch_ensemble(launch_count)
    local_fraction = float(settings["local_fraction"])
    deposited_total = 0.0
    terminations: dict[str, int] = {}
    for launch_x, launch_weight in zip(launch_points, launch_weights):
        for fraction, attenuation, guidance in (
            (local_fraction, float(settings["local_attenuation_length_m"]), 0.0),
            (1.0 - local_fraction, float(settings["guided_attenuation_length_m"]), float(settings["bfield_guidance_fraction"])),
        ):
            deposited, termination = _trace_component(
                output,
                x,
                z,
                bx,
                bz,
                quadrature,
                neutral_weight,
                launch_x=float(launch_x),
                launch_weight=float(launch_weight),
                component_fraction=fraction,
                attenuation_length=attenuation,
                guidance_fraction=guidance,
                transition_field=float(settings["transition_field_t"]),
                spread_angle_degrees=float(settings["spread_angle_degrees"]),
                step_length=step_length,
                max_path_length=max_path_length,
                survival_floor=survival_floor,
            )
            deposited_total += float(launch_weight) * deposited
            terminations[termination] = terminations.get(termination, 0) + 1
    integral = cartesian_area_integral(x, z, output)
    if integral <= 0.0 or not np.isfinite(integral):
        raise ValueError("SEE tracing deposited no finite weight in the chamber")
    target_integral = (SOURCE_X_MAX_M - SOURCE_X_MIN_M) * deposited_total
    unscaled = output * target_integral / integral
    scaled = unscaled * float(settings["see_magnitude_scale"])
    trace_metadata: dict[str, object] = {
        "plasma_efficiency": deposited_total,
        "termination_counts": terminations,
        "see_unscaled_area_integral_m": cartesian_area_integral(x, z, unscaled),
        "see_area_integral_m": cartesian_area_integral(x, z, scaled),
    }
    return scaled, trace_metadata


def generate_see_case(
    case: str,
    table_dir: Path | str | None = None,
    neutral_table: Path | str = DEFAULT_NEUTRAL_TABLE,
    *,
    launch_count: int = 41,
    step_length: float = 0.0025,
    max_path_length: float = 0.90,
    survival_floor: float = 1.0e-4,
) -> dict[str, object]:
    if case not in CASE_NAMES:
        raise ValueError(f"unknown case {case!r}; choose one of {', '.join(CASE_NAMES)}")
    directory = Path(table_dir) if table_dir is not None else CASE_TABLE_DIRS[case]
    bx_path = directory / "Bx_T.tbl"
    bz_path = directory / "By_T.tbl"
    if not bx_path.exists() or not bz_path.exists():
        raise FileNotFoundError(
            f"generate the paired B-field tables first: {bx_path} and {bz_path}"
        )
    x, z, bx = read_moose_table(bx_path)
    bz_x, bz_z, bz = read_moose_table(bz_path)
    if not np.array_equal(x, bz_x) or not np.array_equal(z, bz_z):
        raise ValueError("Bx_T.tbl and By_T.tbl must use the same X-Z grid")
    neutral_density = build_effective_neutral_density(x, z, neutral_table)
    weight, trace_metadata = build_see_weight(
        case,
        x,
        z,
        bx,
        bz,
        neutral_density,
        launch_count=launch_count,
        step_length=step_length,
        max_path_length=max_path_length,
        survival_floor=survival_floor,
    )
    see_path = directory / "see_spatial_weight_m-1.tbl"
    write_moose_table(see_path, x, z, weight)
    metadata: dict[str, object] = {
        "case": case,
        "coordinate_system": "cartesian_x_z_per_unit_y_depth",
        "paired_bfield_directory": str(directory.resolve()),
        "bfield_sha256": {
            "Bx_T.tbl": hashlib.sha256(bx_path.read_bytes()).hexdigest(),
            "By_T.tbl": hashlib.sha256(bz_path.read_bytes()).hexdigest(),
        },
        "neutral_table": str(Path(neutral_table).resolve()),
        "neutral_sha256": hashlib.sha256(Path(neutral_table).read_bytes()).hexdigest(),
        "transport_settings": CASE_SETTINGS[case],
        **trace_metadata,
    }
    (directory / "generation_metadata.json").write_text(
        json.dumps(metadata, indent=2) + "\n", encoding="utf-8"
    )
    return metadata


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", choices=("all", *CASE_NAMES), default="all")
    parser.add_argument("--table-dir", type=Path)
    parser.add_argument("--neutral-table", type=Path, default=DEFAULT_NEUTRAL_TABLE)
    parser.add_argument("--launch-count", type=int, default=41)
    parser.add_argument("--step-length", type=float, default=0.0025)
    parser.add_argument("--max-path-length", type=float, default=0.90)
    parser.add_argument("--survival-floor", type=float, default=1.0e-4)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    if args.case == "all" and args.table_dir is not None:
        raise SystemExit("--table-dir can only be used with one --case")
    cases = CASE_NAMES if args.case == "all" else (args.case,)
    results = [
        generate_see_case(
            case,
            args.table_dir,
            args.neutral_table,
            launch_count=args.launch_count,
            step_length=args.step_length,
            max_path_length=args.max_path_length,
            survival_floor=args.survival_floor,
        )
        for case in cases
    ]
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
