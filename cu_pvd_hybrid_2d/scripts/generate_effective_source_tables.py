#!/usr/bin/env python
"""Generate fast-electron effective Cu ionization and energy source tables.

This script reads the MC field package written by ``prepare_zapdos_inputs.py``
and writes MOOSE ``PiecewiseMultilinear`` tables:

    S_Cu_eff_m3_s.tbl       Cu ionization pair source [m^-3 s^-1]
    Qe_eff_eV_m3_s.tbl      electron energy source [eV m^-3 s^-1]

The default model is a deterministic fast-electron deposition surrogate:
electron bundles are launched from the target racetrack, spread by pitch angle
and anomalous cross-field broadening, then attenuate as they deposit ionization
through the neutral Cu/B-field maps. This is still a reduced model, but it is
closer to a nonlocal fast-electron deposition map than the old local
``n_Cu * B * Gaussian`` source shape.
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MC_FIELDS = ROOT / "runs" / "zapdos_initial_input" / "mc_fields.csv"
DEFAULT_OUT_DIR = ROOT / "runs" / "zapdos_initial_input" / "moose_tables"


def write_moose_table(path: Path, x: np.ndarray, y: np.ndarray, values: np.ndarray) -> None:
    """Write a 2D field to MOOSE PiecewiseMultilinear table format."""
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        handle.write("AXIS X\n")
        handle.write(" ".join(f"{v:.8e}" for v in x) + "\n\n")
        handle.write("AXIS Y\n")
        handle.write(" ".join(f"{v:.8e}" for v in y) + "\n\n")
        handle.write("DATA\n")
        for iy in range(len(y)):
            handle.write(" ".join(f"{v:.8e}" for v in values[:, iy]) + "\n")


def load_mc_fields(path: Path) -> tuple[np.ndarray, np.ndarray, dict[str, np.ndarray]]:
    data = np.genfromtxt(path, delimiter=",", names=True)
    required = {"x_m", "y_m", "n_Cu_m3", "Bx_T", "By_T"}
    missing = required.difference(data.dtype.names or ())
    if missing:
        raise ValueError(f"{path} is missing required columns: {sorted(missing)}")

    x_all = np.asarray(data["x_m"], dtype=float)
    y_all = np.asarray(data["y_m"], dtype=float)
    x = np.unique(x_all)
    y = np.unique(y_all)
    nx, ny = x.size, y.size
    if nx * ny != data.size:
        raise ValueError(f"Expected {nx} x {ny} = {nx * ny} rows, found {data.size}")

    order = np.lexsort((y_all, x_all))

    def reshape(name: str) -> np.ndarray:
        return np.asarray(data[name], dtype=float)[order].reshape(nx, ny)

    fields = {
        "n_Cu_m3": reshape("n_Cu_m3"),
        "Bx_T": reshape("Bx_T"),
        "By_T": reshape("By_T"),
    }
    return x, y, fields


def positive_normalize(values: np.ndarray, floor: float = 0.0) -> np.ndarray:
    arr = np.maximum(np.asarray(values, dtype=float), floor)
    vmax = float(np.nanmax(arr))
    if not np.isfinite(vmax) or vmax <= 0.0:
        return np.zeros_like(arr)
    return arr / vmax


def normalized_gaussian(points: np.ndarray, center: float, width: float) -> np.ndarray:
    """Return discrete Gaussian weights normalized to unit sum."""
    sigma = max(float(width), 1.0e-12)
    weights = np.exp(-0.5 * ((points - center) / sigma) ** 2)
    total = float(np.sum(weights))
    if not np.isfinite(total) or total <= 0.0:
        raise ValueError("Gaussian launch weights are zero")
    return weights / total


def cell_width(values: np.ndarray) -> float:
    if len(values) < 2:
        return 1.0
    return float(np.median(np.diff(values)))


def build_local_effective_sources(
    x: np.ndarray,
    y: np.ndarray,
    n_cu: np.ndarray,
    bx: np.ndarray,
    by: np.ndarray,
    *,
    source_peak: float,
    energy_per_pair: float,
    racetrack_center_x: float,
    racetrack_width_x: float,
    axial_peak_y: float,
    axial_rise_length: float,
    axial_decay_length: float,
    wafer_cutoff_y: float,
    wafer_cutoff_width: float,
    neutral_power: float,
    b_power: float,
) -> tuple[np.ndarray, np.ndarray]:
    x2d, y2d = np.meshgrid(x, y, indexing="ij")
    bmag = np.sqrt(bx**2 + by**2)

    neutral_weight = positive_normalize(n_cu) ** neutral_power
    b_weight = positive_normalize(bmag) ** b_power
    racetrack = np.exp(-0.5 * ((x2d - racetrack_center_x) / max(racetrack_width_x, 1.0e-12)) ** 2)
    dy_from_peak = y2d - axial_peak_y
    axial = np.where(
        dy_from_peak < 0.0,
        np.exp(-0.5 * (dy_from_peak / max(axial_rise_length, 1.0e-12)) ** 2),
        np.exp(-dy_from_peak / max(axial_decay_length, 1.0e-12)),
    )
    wafer_cutoff = 0.5 * (
        1.0 - np.tanh((y2d - wafer_cutoff_y) / max(wafer_cutoff_width, 1.0e-12))
    )

    shape = neutral_weight * b_weight * racetrack * axial * wafer_cutoff
    shape_max = float(np.nanmax(shape))
    if not np.isfinite(shape_max) or shape_max <= 0.0:
        raise ValueError("Effective source shape is zero everywhere")

    s_cu_eff = float(source_peak) * shape / shape_max
    qe_eff = float(energy_per_pair) * s_cu_eff
    return s_cu_eff, qe_eff


def build_fast_electron_deposition_sources(
    x: np.ndarray,
    y: np.ndarray,
    n_cu: np.ndarray,
    bx: np.ndarray,
    by: np.ndarray,
    *,
    source_peak: float,
    energy_per_pair: float,
    racetrack_center_x: float,
    racetrack_width_x: float,
    wafer_cutoff_y: float,
    wafer_cutoff_width: float,
    neutral_power: float,
    b_power: float,
    launch_x_count: int,
    launch_angle_count: int,
    launch_span_sigma: float,
    emission_angle_spread_deg: float,
    max_emission_angle_deg: float,
    ionization_mfp_at_peak: float,
    energy_attenuation_length: float,
    lateral_scatter_sqrt_m: float,
    lateral_scatter_linear: float,
    exb_drift_slope: float,
    magnetic_residence_factor: float,
    survival_floor: float,
) -> tuple[np.ndarray, np.ndarray]:
    """Build nonlocal source maps from a reduced fast-electron deposition model.

    The quantity being transported is an effective fast-electron flux, not the
    cold bulk electron density. Each bundle starts at the powered target
    racetrack and deposits ionization along a slanted/scattered path. Local Cu
    neutral density controls collision probability, and the B-field magnitude
    controls residence-time enhancement. The final field is normalized by peak
    because the absolute fast-electron current is supplied by ``source_peak``.
    """
    if launch_x_count < 1:
        raise ValueError("--launch-x-count must be at least 1")
    if launch_angle_count < 1:
        raise ValueError("--launch-angle-count must be at least 1")

    dx = cell_width(x)
    dy = cell_width(y)
    source_shape = np.zeros((len(x), len(y)), dtype=float)

    bmag = np.sqrt(bx**2 + by**2)
    neutral_weight = positive_normalize(n_cu) ** neutral_power
    b_weight = positive_normalize(bmag) ** b_power
    residence = 1.0 + max(float(magnetic_residence_factor), 0.0) * b_weight
    collisionality = neutral_weight * residence

    x_span = max(float(launch_span_sigma), 0.1) * max(float(racetrack_width_x), dx)
    launch_x = np.linspace(
        racetrack_center_x - x_span,
        racetrack_center_x + x_span,
        int(launch_x_count),
    )
    launch_x_weights = normalized_gaussian(launch_x, racetrack_center_x, racetrack_width_x)

    max_angle = np.deg2rad(max(float(max_emission_angle_deg), 1.0))
    angle_spread = np.deg2rad(max(float(emission_angle_spread_deg), 1.0))
    launch_angles = np.linspace(-max_angle, max_angle, int(launch_angle_count))
    launch_angle_weights = normalized_gaussian(launch_angles, 0.0, angle_spread)

    y0 = max(float(np.min(y)) - 0.5 * dy, 0.0)
    wafer_cutoff = 0.5 * (
        1.0 - np.tanh((y - wafer_cutoff_y) / max(wafer_cutoff_width, 1.0e-12))
    )
    ion_mfp = max(float(ionization_mfp_at_peak), 1.0e-12)
    energy_loss_length = max(float(energy_attenuation_length), 1.0e-12)
    min_survival = max(float(survival_floor), 0.0)
    scatter_sqrt = max(float(lateral_scatter_sqrt_m), 0.0)
    scatter_linear = max(float(lateral_scatter_linear), 0.0)

    for x_launch, wx in zip(launch_x, launch_x_weights):
        for angle, wa in zip(launch_angles, launch_angle_weights):
            ray_weight = float(wx * wa)
            slope = float(np.tan(angle) + exb_drift_slope)
            geometric_factor = float(np.sqrt(1.0 + slope * slope))
            survival = 1.0
            previous_y = y0

            for iy, y_cell in enumerate(y):
                axial_distance = max(float(y_cell) - y0, 0.0)
                step_y = max(float(y_cell) - previous_y, dy)
                ds = step_y * geometric_factor
                previous_y = float(y_cell)

                centerline_x = float(x_launch) + slope * axial_distance
                sigma_x = max(
                    0.75 * dx,
                    scatter_sqrt * np.sqrt(axial_distance + dy),
                    scatter_linear * axial_distance,
                )
                lateral = np.exp(-0.5 * ((x - centerline_x) / sigma_x) ** 2)
                lateral_sum = float(np.sum(lateral))
                if not np.isfinite(lateral_sum) or lateral_sum <= 0.0:
                    continue
                lateral /= lateral_sum

                row_collision = np.maximum(collisionality[:, iy], 0.0) / ion_mfp
                row_loss_rate = float(np.sum(lateral * row_collision))
                if not np.isfinite(row_loss_rate) or row_loss_rate <= 0.0:
                    survival *= np.exp(-ds / energy_loss_length)
                    if survival < min_survival:
                        break
                    continue

                deposit_fraction = survival * (1.0 - np.exp(-row_loss_rate * ds))
                spatial_pdf = lateral * row_collision / row_loss_rate
                source_shape[:, iy] += ray_weight * deposit_fraction * spatial_pdf * wafer_cutoff[iy]

                survival *= np.exp(-row_loss_rate * ds)
                survival *= np.exp(-ds / energy_loss_length)
                if survival < min_survival:
                    break

    shape_max = float(np.nanmax(source_shape))
    if not np.isfinite(shape_max) or shape_max <= 0.0:
        raise ValueError("Fast-electron deposition source is zero everywhere")

    s_cu_eff = float(source_peak) * source_shape / shape_max
    qe_eff = float(energy_per_pair) * s_cu_eff
    return s_cu_eff, qe_eff


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate table-driven S_Cu_eff and Qe_eff maps from MC/B-field fields."
    )
    parser.add_argument("--mc-fields", type=Path, default=DEFAULT_MC_FIELDS)
    parser.add_argument("--out-dir", type=Path, default=DEFAULT_OUT_DIR)
    parser.add_argument("--model", choices=("fast-electron", "local"), default="fast-electron")
    parser.add_argument("--source-peak", type=float, default=1.0e21)
    parser.add_argument("--energy-per-pair", type=float, default=15.0)
    parser.add_argument("--racetrack-center-x", type=float, default=0.105)
    parser.add_argument("--racetrack-width-x", type=float, default=0.050)
    parser.add_argument("--axial-peak-y", type=float, default=0.030)
    parser.add_argument("--axial-rise-length", type=float, default=0.015)
    parser.add_argument("--axial-decay-length", type=float, default=0.060)
    parser.add_argument("--wafer-cutoff-y", type=float, default=None)
    parser.add_argument("--wafer-cutoff-width", type=float, default=None)
    parser.add_argument("--neutral-power", type=float, default=0.7)
    parser.add_argument("--b-power", type=float, default=1.0)
    parser.add_argument("--launch-x-count", type=int, default=41)
    parser.add_argument("--launch-angle-count", type=int, default=61)
    parser.add_argument("--launch-span-sigma", type=float, default=3.0)
    parser.add_argument("--emission-angle-spread-deg", type=float, default=35.0)
    parser.add_argument("--max-emission-angle-deg", type=float, default=70.0)
    parser.add_argument("--ionization-mfp-at-peak", type=float, default=0.10)
    parser.add_argument("--energy-attenuation-length", type=float, default=0.30)
    parser.add_argument("--lateral-scatter-sqrt-m", type=float, default=0.035)
    parser.add_argument("--lateral-scatter-linear", type=float, default=0.10)
    parser.add_argument("--exb-drift-slope", type=float, default=0.0)
    parser.add_argument("--magnetic-residence-factor", type=float, default=2.0)
    parser.add_argument("--survival-floor", type=float, default=1.0e-5)
    args = parser.parse_args()

    x, y, fields = load_mc_fields(args.mc_fields)
    y_max = float(np.max(y))
    wafer_cutoff_y = args.wafer_cutoff_y
    if wafer_cutoff_y is None:
        wafer_cutoff_y = 0.50 * y_max
    wafer_cutoff_width = args.wafer_cutoff_width
    if wafer_cutoff_width is None:
        wafer_cutoff_width = max(0.04 * y_max, 1.0e-3)

    if args.model == "fast-electron":
        s_cu_eff, qe_eff = build_fast_electron_deposition_sources(
            x,
            y,
            fields["n_Cu_m3"],
            fields["Bx_T"],
            fields["By_T"],
            source_peak=args.source_peak,
            energy_per_pair=args.energy_per_pair,
            racetrack_center_x=args.racetrack_center_x,
            racetrack_width_x=args.racetrack_width_x,
            wafer_cutoff_y=wafer_cutoff_y,
            wafer_cutoff_width=wafer_cutoff_width,
            neutral_power=args.neutral_power,
            b_power=args.b_power,
            launch_x_count=args.launch_x_count,
            launch_angle_count=args.launch_angle_count,
            launch_span_sigma=args.launch_span_sigma,
            emission_angle_spread_deg=args.emission_angle_spread_deg,
            max_emission_angle_deg=args.max_emission_angle_deg,
            ionization_mfp_at_peak=args.ionization_mfp_at_peak,
            energy_attenuation_length=args.energy_attenuation_length,
            lateral_scatter_sqrt_m=args.lateral_scatter_sqrt_m,
            lateral_scatter_linear=args.lateral_scatter_linear,
            exb_drift_slope=args.exb_drift_slope,
            magnetic_residence_factor=args.magnetic_residence_factor,
            survival_floor=args.survival_floor,
        )
    else:
        s_cu_eff, qe_eff = build_local_effective_sources(
            x,
            y,
            fields["n_Cu_m3"],
            fields["Bx_T"],
            fields["By_T"],
            source_peak=args.source_peak,
            energy_per_pair=args.energy_per_pair,
            racetrack_center_x=args.racetrack_center_x,
            racetrack_width_x=args.racetrack_width_x,
            axial_peak_y=args.axial_peak_y,
            axial_rise_length=args.axial_rise_length,
            axial_decay_length=args.axial_decay_length,
            wafer_cutoff_y=wafer_cutoff_y,
            wafer_cutoff_width=wafer_cutoff_width,
            neutral_power=args.neutral_power,
            b_power=args.b_power,
        )

    args.out_dir.mkdir(parents=True, exist_ok=True)
    s_path = args.out_dir / "S_Cu_eff_m3_s.tbl"
    q_path = args.out_dir / "Qe_eff_eV_m3_s.tbl"
    write_moose_table(s_path, x, y, s_cu_eff)
    write_moose_table(q_path, x, y, qe_eff)

    print(f"wrote {s_path}")
    print(f"wrote {q_path}")
    print(f"S_Cu_eff range: {np.nanmin(s_cu_eff):.3e} .. {np.nanmax(s_cu_eff):.3e} m^-3 s^-1")
    print(f"Qe_eff range: {np.nanmin(qe_eff):.3e} .. {np.nanmax(qe_eff):.3e} eV m^-3 s^-1")
    print(f"source model = {args.model}")
    print(f"wafer cutoff center y = {wafer_cutoff_y:.6e} m, width = {wafer_cutoff_width:.6e} m")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
