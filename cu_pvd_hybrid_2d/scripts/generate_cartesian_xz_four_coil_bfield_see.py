#!/usr/bin/env python3
"""Generate the two Cartesian X-Z four-coil B-field and B-guided SEE cases."""
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

import generate_cartesian_xz_source_bfield_see as source_generator  # noqa: E402


CASE_CONFIGS: dict[str, dict[str, Any]] = {
    "four_coil": {
        "reference_dir": ROOT
        / "runs"
        / "zapdos_hpem_rz_30cm_reference_four_coil"
        / "moose_tables",
        "output_dir": ROOT
        / "runs"
        / "zapdos_cartesian_xz_25x30_four_coil"
        / "moose_tables",
        "coil_weights": [44.6, 12.6, 3.2, -5.2],
        "see_magnitude_scale": 1.2,
        "local_fraction": 0.95,
        "guided_attenuation_length_m": 0.20,
        "spread_angle_degrees": 8.0,
        "bfield_guidance_fraction": 1.0,
        "reference_name": "standard four-coil R-Z case",
    },
    "four_coil_img3092": {
        "reference_dir": ROOT
        / "runs"
        / "zapdos_hpem_rz_30cm_reference_four_coil_img3092"
        / "moose_tables",
        "output_dir": ROOT
        / "runs"
        / "zapdos_cartesian_xz_25x30_four_coil_img3092"
        / "moose_tables",
        "coil_weights": [44.6, 12.6, 3.2, 13.7],
        "see_magnitude_scale": 1.2,
        "local_fraction": 0.90,
        "guided_attenuation_length_m": 0.20,
        "spread_angle_degrees": 5.0,
        "bfield_guidance_fraction": 0.30,
        "reference_name": "IMG_3092 four-coil R-Z case",
    },
}


def _case_config(case_name: str) -> dict[str, Any]:
    try:
        return CASE_CONFIGS[case_name]
    except KeyError as error:
        choices = ", ".join(CASE_CONFIGS)
        raise ValueError(f"unknown case {case_name!r}; choose one of: {choices}") from error


def read_reference_component(
    case_name: str,
    filename: str,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Read one component from the established R-Z four-coil case."""
    if filename not in {"Bx_T.tbl", "By_T.tbl"}:
        raise ValueError("filename must be Bx_T.tbl or By_T.tbl")
    reference_dir = Path(_case_config(case_name)["reference_dir"])
    return source_generator.rz_see.read_moose_table(reference_dir / filename)


def _limit_bfield_magnitude(
    bx: np.ndarray,
    bz: np.ndarray,
    *,
    minimum_t: float = source_generator.B_FIELD_MIN_T,
    maximum_t: float = source_generator.B_FIELD_MAX_T,
) -> tuple[np.ndarray, np.ndarray]:
    """Apply the common 1--1000 G limits without rotating the field."""
    bx = np.asarray(bx, dtype=float)
    bz = np.asarray(bz, dtype=float)
    if bx.shape != bz.shape or bx.ndim != 2:
        raise ValueError("B components must be matching two-dimensional arrays")
    if np.any(~np.isfinite(bx)) or np.any(~np.isfinite(bz)):
        raise ValueError("B components must be finite")
    if not 0.0 < minimum_t < maximum_t:
        raise ValueError("B limits must satisfy 0 < minimum_t < maximum_t")

    magnitude = np.hypot(bx, bz)
    nonzero = magnitude > np.finfo(float).tiny
    target = np.clip(magnitude, minimum_t, maximum_t)
    scale = np.ones_like(magnitude)
    scale[nonzero] = target[nonzero] / magnitude[nonzero]
    limited_bx = bx * scale
    limited_bz = bz * scale
    limited_bx[~nonzero] = 0.0
    limited_bz[~nonzero] = minimum_t
    return limited_bx, limited_bz


def build_cartesian_four_coil_bfield(
    case_name: str,
    x: np.ndarray,
    z: np.ndarray,
) -> tuple[np.ndarray, np.ndarray]:
    """Combine the current Cartesian source with an established coil field.

    The coil contribution is extracted from the corresponding established
    R-Z total map by subtracting the source field used to construct that map.
    It is then interpolated directly onto the physical X-Z chamber, without
    symmetry reflection, and added vectorially to the translated/compressed
    Cartesian source field.
    """
    _case_config(case_name)
    x = source_generator._validate_axis("x", x)
    z = source_generator._validate_axis("z", z)
    reference_x, reference_z, reference_bx = read_reference_component(
        case_name, "Bx_T.tbl"
    )
    by_x, by_z, reference_bz = read_reference_component(case_name, "By_T.tbl")
    if not np.array_equal(reference_x, by_x) or not np.array_equal(reference_z, by_z):
        raise ValueError("reference Bx and By tables must share axes")

    old_source_bx, old_source_bz = source_generator.rz_source_bfield.build_source_bfield(
        reference_x,
        reference_z,
        source_r_m=0.125,
        source_z_m=0.300,
        min_b_t=source_generator.B_FIELD_MIN_T,
        max_b_t=source_generator.B_FIELD_MAX_T,
    )
    residual_coil_bx = reference_bx - old_source_bx
    residual_coil_bz = reference_bz - old_source_bz
    coil_bx = source_generator._interpolate_points(
        reference_x, reference_z, residual_coil_bx, x, z
    )
    coil_bz = source_generator._interpolate_points(
        reference_x, reference_z, residual_coil_bz, x, z
    )
    source_bx, source_bz = source_generator.build_cartesian_source_bfield(x, z)
    return _limit_bfield_magnitude(source_bx + coil_bx, source_bz + coil_bz)


def generate_cartesian_four_coil_case(
    case_name: str,
    *,
    output_dir: Path | str | None = None,
    neutral_table: Path | str = source_generator.DEFAULT_NEUTRAL_TABLE,
    nx: int = 101,
    nz: int = 161,
    launch_count: int = 41,
    step_length: float = 0.0025,
    max_path_length: float = 0.90,
    survival_floor: float = 1.0e-4,
    local_fraction: float | None = None,
    local_attenuation_length: float = source_generator.LOCAL_ATTENUATION_LENGTH_M,
    guided_attenuation_length: float | None = None,
    spread_angle_degrees: float | None = None,
    transition_field: float = source_generator.TRANSITION_FIELD_T,
    bfield_guidance_fraction: float | None = None,
    see_magnitude_scale: float | None = None,
) -> dict[str, Any]:
    """Write one combined B field and its own B-guided SEE map."""
    config = _case_config(case_name)
    magnitude_scale = (
        float(config["see_magnitude_scale"])
        if see_magnitude_scale is None
        else float(see_magnitude_scale)
    )
    resolved_local_fraction = (
        float(config["local_fraction"])
        if local_fraction is None
        else float(local_fraction)
    )
    resolved_guided_attenuation_length = (
        float(config["guided_attenuation_length_m"])
        if guided_attenuation_length is None
        else float(guided_attenuation_length)
    )
    resolved_spread_angle_degrees = (
        float(config["spread_angle_degrees"])
        if spread_angle_degrees is None
        else float(spread_angle_degrees)
    )
    resolved_bfield_guidance_fraction = (
        float(config["bfield_guidance_fraction"])
        if bfield_guidance_fraction is None
        else float(bfield_guidance_fraction)
    )
    if nx < 3 or nz < 3:
        raise ValueError("nx and nz must each be at least three")
    output_path = Path(output_dir) if output_dir is not None else Path(config["output_dir"])
    x = np.linspace(source_generator.X_MIN_M, source_generator.X_MAX_M, int(nx))
    z = np.linspace(source_generator.Z_MIN_M, source_generator.Z_MAX_M, int(nz))
    bx, bz = build_cartesian_four_coil_bfield(case_name, x, z)
    neutral_density = source_generator._build_effective_neutral_density(
        x, z, Path(neutral_table)
    )
    unscaled_see_weight, trace_metadata = source_generator._trace_cartesian_see(
        x,
        z,
        neutral_density,
        bx,
        bz,
        launch_count=launch_count,
        step_length=step_length,
        max_path_length=max_path_length,
        survival_floor=survival_floor,
        local_fraction=resolved_local_fraction,
        local_attenuation_length=local_attenuation_length,
        guided_attenuation_length=resolved_guided_attenuation_length,
        spread_angle_degrees=resolved_spread_angle_degrees,
        transition_field=transition_field,
        direction_model="bfield_guided",
        bfield_guidance_fraction=resolved_bfield_guidance_fraction,
    )
    see_weight = source_generator.scale_cartesian_see_weight(
        unscaled_see_weight, magnitude_scale
    )

    output_path.mkdir(parents=True, exist_ok=True)
    source_generator.rz_see.write_moose_table(output_path / "Bx_T.tbl", x, z, bx)
    source_generator.rz_see.write_moose_table(output_path / "By_T.tbl", x, z, bz)
    source_generator.rz_see.write_moose_table(
        output_path / "see_spatial_weight_m-1.tbl", x, z, see_weight
    )

    result: dict[str, Any] = {
        "case": case_name,
        "coordinate_system": "cartesian_x_z_per_unit_y_depth",
        "domain_m": {
            "x": [source_generator.X_MIN_M, source_generator.X_MAX_M],
            "z": [source_generator.Z_MIN_M, source_generator.Z_MAX_M],
        },
        "powered_target_x_m": [
            source_generator.SOURCE_X_MIN_M,
            source_generator.SOURCE_X_MAX_M,
        ],
        "source_center_m": source_generator.SOURCE_CENTER_M,
        "launch_lobe_centers_m": [
            round(source_generator.SOURCE_CENTER_M - source_generator.LOBE_OFFSET_M, 12),
            round(source_generator.SOURCE_CENTER_M + source_generator.LOBE_OFFSET_M, 12),
        ],
        "magnetic_pole_centers_m": [
            round(source_generator.SOURCE_CENTER_M - source_generator.LOBE_OFFSET_M, 12),
            round(source_generator.SOURCE_CENTER_M + source_generator.LOBE_OFFSET_M, 12),
        ],
        "coil_weights": config["coil_weights"],
        "reference_four_coil_case": config["reference_name"],
        "field_assembly": "established_rz_total_minus_old_source_plus_current_cartesian_source",
        "boundary_layout_m": {
            "left_wall_x": source_generator.X_MIN_M,
            "right_wall_x": source_generator.X_MAX_M,
            "wafer_x": [source_generator.WAFER_X_MIN_M, source_generator.WAFER_X_MAX_M],
            "powered_target_x": [
                source_generator.SOURCE_X_MIN_M,
                source_generator.SOURCE_X_MAX_M,
            ],
        },
        "transport_settings": {
            "local_fraction": resolved_local_fraction,
            "local_attenuation_length_m": float(local_attenuation_length),
            "guided_attenuation_length_m": resolved_guided_attenuation_length,
            "spread_angle_degrees": resolved_spread_angle_degrees,
            "transition_field_t": float(transition_field),
            "bfield_guidance_fraction": resolved_bfield_guidance_fraction,
        },
        "see_transport_direction": (
            "bfield_guided"
            if resolved_bfield_guidance_fraction == 1.0
            else "mostly_vertical_bfield_guided"
        ),
        "see_magnitude_scale": magnitude_scale,
        "plasma_efficiency": trace_metadata["plasma_efficiency"],
        "see_unscaled_area_integral_m": source_generator.cartesian_area_integral(
            x, z, unscaled_see_weight
        ),
        "see_area_integral_m": source_generator.cartesian_area_integral(
            x, z, see_weight
        ),
        "mean_path_length_m": trace_metadata["mean_path_length_m"],
        "termination_counts": trace_metadata["termination_counts"],
        "bfield_magnitude_range_t": [
            float(np.min(np.hypot(bx, bz))),
            float(np.max(np.hypot(bx, bz))),
        ],
        "source_bfield_axial_compression": {
            "compressed_z_m": [source_generator.B_FIELD_FLOOR_Z_M, source_generator.Z_MAX_M],
            "uniform_lower_source_field_t": source_generator.B_FIELD_MIN_T,
            "mapping": "cubic_smoothstep",
        },
    }
    (output_path / "generation_metadata.json").write_text(
        json.dumps(result, indent=2) + "\n", encoding="utf-8"
    )
    return result


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--case",
        choices=["all", *CASE_CONFIGS],
        default="all",
        help="Generate one case or both cases (default: all)",
    )
    parser.add_argument("--output-dir", type=Path)
    parser.add_argument("--neutral-table", type=Path, default=source_generator.DEFAULT_NEUTRAL_TABLE)
    parser.add_argument("--nx", type=int, default=101)
    parser.add_argument("--nz", type=int, default=161)
    parser.add_argument("--launch-count", type=int, default=41)
    parser.add_argument("--step-length", type=float, default=0.0025)
    parser.add_argument("--max-path-length", type=float, default=0.90)
    parser.add_argument("--survival-floor", type=float, default=1.0e-4)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    if args.case == "all" and args.output_dir is not None:
        raise SystemExit("--output-dir can only be used when generating one --case")
    case_names = tuple(CASE_CONFIGS) if args.case == "all" else (args.case,)
    results = []
    for case_name in case_names:
        result = generate_cartesian_four_coil_case(
            case_name,
            output_dir=args.output_dir,
            neutral_table=args.neutral_table,
            nx=args.nx,
            nz=args.nz,
            launch_count=args.launch_count,
            step_length=args.step_length,
            max_path_length=args.max_path_length,
            survival_floor=args.survival_floor,
        )
        results.append(result)
    print(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
