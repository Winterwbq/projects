#!/usr/bin/env python3
"""Extract and compare the actual bottom-boundary Cu+ loss flux from Exodus.

The evaluated flux is the signed particle flux represented by the
``LymberopoulosIonBC`` drift-loss term on the bottom boundary.  This differs
from reconstructing a bulk drift-diffusion flux on a nearby horizontal plane.
"""
from __future__ import annotations

import argparse
import csv
from pathlib import Path
from typing import Any

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_PLOT = ROOT / "post/bottom_cu_ion_boundary_flux.png"
DEFAULT_CSV = ROOT / "post/bottom_cu_ion_boundary_flux.csv"

AVOGADRO = 6.02214076e23
ION_TEMPERATURE = 300.0
REDUCED_MOBILITY = 2.2e-4
REFERENCE_PRESSURE = 101325.0
REFERENCE_TEMPERATURE = 273.15


def cu_ion_mobility(pressure: np.ndarray | float) -> np.ndarray:
    """Return the input model's pressure-scaled Cu+ mobility [m^2/(V s)]."""
    pressure_array = np.asarray(pressure, dtype=float)
    if np.any(~np.isfinite(pressure_array)) or np.any(pressure_array <= 0.0):
        raise ValueError("pressure must be positive and finite")
    return (
        REDUCED_MOBILITY
        * REFERENCE_PRESSURE
        / pressure_array
        * ION_TEMPERATURE
        / REFERENCE_TEMPERATURE
    )


def lymberopoulos_bottom_particle_flux(
    log_molar_density: np.ndarray | float,
    pressure: np.ndarray | float,
    ez: np.ndarray | float,
    loss_scale: float,
    *,
    time: float | None = None,
    ramp_time: float = 0.0,
) -> np.ndarray:
    """Evaluate the bottom outward Cu+ particle flux [m^-2 s^-1].

    The bottom outward normal is ``-z``, so ``E dot n = -Ez``.  Positive output
    is loss from the plasma; negative output means the field term points into
    the plasma.  No clipping is applied.
    """
    log_density = np.asarray(log_molar_density, dtype=float)
    field_z = np.asarray(ez, dtype=float)
    if np.any(~np.isfinite(log_density)) or np.any(~np.isfinite(field_z)):
        raise ValueError("density and electric field must be finite")
    if not np.isfinite(loss_scale) or loss_scale < 0.0:
        raise ValueError("loss_scale must be finite and nonnegative")
    loss_factor = float(loss_scale)
    if ramp_time < 0.0 or not np.isfinite(ramp_time):
        raise ValueError("ramp_time must be finite and nonnegative")
    if ramp_time > 0.0:
        if time is None or not np.isfinite(time):
            raise ValueError("a finite time is required when ramp_time is positive")
        loss_factor *= float(np.tanh(time / ramp_time))
    return (
        loss_factor
        * cu_ion_mobility(pressure)
        * np.exp(log_density)
        * AVOGADRO
        * (-field_z)
    )


def select_bottom_edges(
    x: np.ndarray, z: np.ndarray, connect: np.ndarray, *, tolerance: float | None = None
) -> dict[str, np.ndarray]:
    """Locate QUAD4 edges on ``z_min`` and return them in radial order."""
    x = np.asarray(x, dtype=float)
    z = np.asarray(z, dtype=float)
    connect = np.asarray(connect, dtype=int)
    if x.ndim != 1 or z.shape != x.shape:
        raise ValueError("x and z must be same-length one-dimensional arrays")
    if connect.ndim != 2 or connect.shape[1] != 4:
        raise ValueError("bottom-boundary extraction requires QUAD4 connectivity")
    if np.any(connect < 0) or np.any(connect >= x.size):
        raise ValueError("connectivity contains an invalid node index")
    z_min = float(np.min(z))
    if tolerance is None:
        scale = max(float(np.ptp(z)), 1.0)
        tolerance = 1.0e-10 * scale
    candidate_edges: list[tuple[float, int, int, int]] = []
    local_edges = ((0, 1), (1, 2), (2, 3), (3, 0))
    for element, nodes in enumerate(connect):
        for local_a, local_b in local_edges:
            node_a = int(nodes[local_a])
            node_b = int(nodes[local_b])
            if abs(z[node_a] - z_min) <= tolerance and abs(z[node_b] - z_min) <= tolerance:
                if x[node_b] < x[node_a]:
                    node_a, node_b = node_b, node_a
                candidate_edges.append(
                    (0.5 * (x[node_a] + x[node_b]), element, node_a, node_b)
                )
    if not candidate_edges:
        raise ValueError("mesh has no edges on its minimum-z boundary")
    candidate_edges.sort(key=lambda item: item[0])
    return {
        "r_mid": np.asarray([item[0] for item in candidate_edges]),
        "elements": np.asarray([item[1] for item in candidate_edges], dtype=int),
        "nodes": np.asarray([[item[2], item[3]] for item in candidate_edges], dtype=int),
    }


def evaluate_bottom_profile(
    data: dict[str, Any],
    *,
    loss_scale: float = 0.5,
    ramp_time: float = 5.0e-8,
) -> dict[str, np.ndarray | float]:
    """Evaluate the midpoint bottom-boundary profile for one saved state."""
    edges = select_bottom_edges(data["x"], data["z"], data["connect"])
    nodes = edges["nodes"]
    elements = edges["elements"]
    log_density = np.mean(np.asarray(data["log_cup"])[nodes], axis=1)
    pressure = np.mean(np.asarray(data["pressure"])[nodes], axis=1)
    ez = np.asarray(data["ez"])[elements]
    saved_time = float(data.get("time", 0.0))
    signed_flux = lymberopoulos_bottom_particle_flux(
        log_density,
        pressure,
        ez,
        loss_scale,
        time=saved_time,
        ramp_time=ramp_time,
    )
    return {
        "time": saved_time,
        "r": edges["r_mid"],
        "r_inner": np.asarray(data["x"])[nodes[:, 0]],
        "r_outer": np.asarray(data["x"])[nodes[:, 1]],
        "log_molar_density": log_density,
        "pressure": pressure,
        "ez": ez,
        "signed_flux": signed_flux,
        "absolute_flux": np.abs(signed_flux),
    }


def wafer_mask(r: np.ndarray, wafer_radius: float = 0.15) -> np.ndarray:
    r = np.asarray(r, dtype=float)
    return r <= wafer_radius


def integrate_axisymmetric_profile(
    r: np.ndarray, flux: np.ndarray, *, radius_limit: float
) -> float:
    """Integrate ``2*pi*r*flux`` from the axis to ``radius_limit``.

    Midpoint profiles are extended with their nearest value to the axis and to
    the requested outer radius.  The radius-limit point is interpolated when
    it lies inside the sampled profile.
    """
    r = np.asarray(r, dtype=float)
    flux = np.asarray(flux, dtype=float)
    if r.ndim != 1 or flux.shape != r.shape or r.size == 0:
        raise ValueError("r and flux must be nonempty same-length 1D arrays")
    if np.any(~np.isfinite(r)) or np.any(~np.isfinite(flux)):
        raise ValueError("r and flux must be finite")
    if radius_limit <= 0.0 or not np.isfinite(radius_limit):
        raise ValueError("radius_limit must be positive and finite")
    order = np.argsort(r)
    r_sorted = r[order]
    flux_sorted = flux[order]
    if np.any(np.diff(r_sorted) <= 0.0):
        raise ValueError("r values must be unique")
    interior = r_sorted < radius_limit
    sample_r = r_sorted[interior]
    sample_flux = flux_sorted[interior]
    boundary_flux = float(
        np.interp(radius_limit, r_sorted, flux_sorted, left=flux_sorted[0], right=flux_sorted[-1])
    )
    sample_r = np.concatenate(([0.0], sample_r, [radius_limit]))
    sample_flux = np.concatenate(([flux_sorted[0]], sample_flux, [boundary_flux]))
    return float(np.trapezoid(2.0 * np.pi * sample_r * sample_flux, sample_r))


def _decode_names(raw: np.ndarray) -> list[str]:
    return [
        b"".join(np.asarray(row).astype("S1"))
        .decode("ascii", errors="ignore")
        .replace("\x00", "")
        .strip()
        for row in raw
    ]


def select_saved_timestep(times: np.ndarray, target_time: float | None) -> int:
    times = np.asarray(times, dtype=float)
    if times.size == 0:
        raise ValueError("Exodus file contains no saved timesteps")
    if target_time is None:
        return int(times.size - 1)
    return int(np.argmin(np.abs(times - target_time)))


def load_exodus_bottom_fields(
    path: Path | str, *, target_time: float | None = None
) -> dict[str, Any]:
    """Load the mesh, Cu+, pressure, and axial field needed by the BC formula."""
    from scipy.io import netcdf_file

    exodus_path = Path(path)
    with netcdf_file(exodus_path, "r", mmap=False) as exodus:
        nodal_names = _decode_names(exodus.variables["name_nod_var"].data)
        element_names = _decode_names(exodus.variables["name_elem_var"].data)
        times = np.asarray(exodus.variables["time_whole"][:]).copy()
        step = select_saved_timestep(times, target_time)

        def nodal(name: str) -> np.ndarray:
            if name not in nodal_names:
                raise ValueError(f"{exodus_path} has no nodal variable {name!r}")
            index = nodal_names.index(name) + 1
            return np.asarray(exodus.variables[f"vals_nod_var{index}"][step]).copy()

        def element(name: str) -> np.ndarray:
            if name not in element_names:
                raise ValueError(f"{exodus_path} has no element variable {name!r}")
            index = element_names.index(name) + 1
            variable_name = f"vals_elem_var{index}eb1"
            if variable_name not in exodus.variables:
                raise ValueError(
                    f"{exodus_path} does not store {name!r} on element block 1"
                )
            return np.asarray(exodus.variables[variable_name][step]).copy()

        return {
            "path": exodus_path,
            "time": float(times[step]),
            "x": np.asarray(exodus.variables["coordx"][:]).copy(),
            "z": np.asarray(exodus.variables["coordy"][:]).copy(),
            "connect": np.asarray(exodus.variables["connect1"][:]).astype(int) - 1,
            "log_cup": nodal("Cu+"),
            "pressure": nodal("p_Cu_local"),
            "ez": element("EFieldy"),
        }


def summarize_profile(
    profile: dict[str, Any], *, wafer_radius: float, chamber_radius: float
) -> dict[str, float]:
    r = np.asarray(profile["r"])
    signed_flux = np.asarray(profile["signed_flux"])
    wafer_rate = integrate_axisymmetric_profile(
        r, signed_flux, radius_limit=wafer_radius
    )
    whole_rate = integrate_axisymmetric_profile(
        r, signed_flux, radius_limit=chamber_radius
    )
    fraction = wafer_rate / whole_rate if whole_rate != 0.0 else float("nan")
    return {
        "wafer_rate_s-1": wafer_rate,
        "whole_bottom_rate_s-1": whole_rate,
        "wafer_fraction": fraction,
    }


def write_profiles_csv(
    path: Path,
    profiles: dict[str, dict[str, Any]],
    summaries: dict[str, dict[str, float]],
    *,
    wafer_radius: float,
) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fields = [
        "case",
        "time_s",
        "r_m",
        "r_cm",
        "on_wafer",
        "signed_flux_m-2_s-1",
        "absolute_flux_m-2_s-1",
        "normalized_absolute_flux",
        "wafer_rate_s-1",
        "whole_bottom_rate_s-1",
        "wafer_fraction",
    ]
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        for label, profile in profiles.items():
            absolute = np.asarray(profile["absolute_flux"])
            peak = float(np.max(absolute))
            normalized = absolute / peak if peak > 0.0 else np.zeros_like(absolute)
            summary = summaries[label]
            for radius, signed, magnitude, scaled in zip(
                profile["r"], profile["signed_flux"], absolute, normalized
            ):
                writer.writerow(
                    {
                        "case": label,
                        "time_s": profile["time"],
                        "r_m": radius,
                        "r_cm": 100.0 * radius,
                        "on_wafer": bool(radius <= wafer_radius),
                        "signed_flux_m-2_s-1": signed,
                        "absolute_flux_m-2_s-1": magnitude,
                        "normalized_absolute_flux": scaled,
                        **summary,
                    }
                )


def plot_profiles(
    path: Path,
    profiles: dict[str, dict[str, Any]],
    *,
    wafer_radius: float,
    chamber_radius: float,
) -> None:
    import matplotlib.pyplot as plt

    path.parent.mkdir(parents=True, exist_ok=True)
    figure, axes = plt.subplots(2, 1, figsize=(8.0, 8.0), sharex=True)
    for label, profile in profiles.items():
        radius_cm = 100.0 * np.asarray(profile["r"])
        absolute = np.asarray(profile["absolute_flux"])
        peak = float(np.max(absolute))
        normalized = absolute / peak if peak > 0.0 else np.zeros_like(absolute)
        axes[0].plot(radius_cm, absolute, linewidth=2.0, label=label)
        axes[1].plot(radius_cm, normalized, linewidth=2.0, label=label)
    for axis in axes:
        axis.axvspan(0.0, 100.0 * wafer_radius, color="#d9edf7", alpha=0.35)
        axis.axvline(100.0 * wafer_radius, color="black", linestyle=":", linewidth=1.2)
        axis.grid(alpha=0.25)
        axis.set_xlim(0.0, 100.0 * chamber_radius)
    axes[0].set_ylabel(r"$|\Gamma_{\mathrm{Cu}^+}|$ (m$^{-2}$ s$^{-1}$)")
    axes[1].set_ylabel("Peak-normalized magnitude")
    axes[1].set_xlabel("Radius on bottom boundary (cm)")
    axes[0].legend(frameon=False)
    figure.tight_layout()
    figure.savefig(path, dpi=200)
    plt.close(figure)


def _case_argument(value: str) -> tuple[str, Path]:
    if "=" not in value:
        raise argparse.ArgumentTypeError("case must be LABEL=EXODUS_PATH")
    label, raw_path = value.split("=", 1)
    if not label or not raw_path:
        raise argparse.ArgumentTypeError("case must be LABEL=EXODUS_PATH")
    return label, Path(raw_path)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--case",
        action="append",
        type=_case_argument,
        required=True,
        metavar="LABEL=EXODUS_PATH",
        help="Case label and Exodus file; repeat to compare cases.",
    )
    parser.add_argument("--time", type=float, help="Nearest saved physical time (s).")
    parser.add_argument("--wafer-radius", type=float, default=0.15)
    parser.add_argument("--chamber-radius", type=float, default=0.25)
    parser.add_argument("--loss-scale", type=float, default=0.5)
    parser.add_argument("--loss-ramp-time", type=float, default=5.0e-8)
    parser.add_argument("--plot", type=Path, default=DEFAULT_PLOT)
    parser.add_argument("--csv", type=Path, default=DEFAULT_CSV)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    profiles: dict[str, dict[str, Any]] = {}
    summaries: dict[str, dict[str, float]] = {}
    for label, path in args.case:
        data = load_exodus_bottom_fields(path, target_time=args.time)
        profile = evaluate_bottom_profile(
            data, loss_scale=args.loss_scale, ramp_time=args.loss_ramp_time
        )
        profiles[label] = profile
        summaries[label] = summarize_profile(
            profile,
            wafer_radius=args.wafer_radius,
            chamber_radius=args.chamber_radius,
        )
    write_profiles_csv(
        args.csv, profiles, summaries, wafer_radius=args.wafer_radius
    )
    plot_profiles(
        args.plot,
        profiles,
        wafer_radius=args.wafer_radius,
        chamber_radius=args.chamber_radius,
    )
    for label, summary in summaries.items():
        print(
            f"{label}: wafer={summary['wafer_rate_s-1']:.6e} s^-1, "
            f"whole_bottom={summary['whole_bottom_rate_s-1']:.6e} s^-1, "
            f"fraction={summary['wafer_fraction']:.6f}"
        )
    print(f"plot: {args.plot}")
    print(f"csv: {args.csv}")


if __name__ == "__main__":
    main()
