from __future__ import annotations

import csv
import subprocess
from pathlib import Path
from typing import Any

import numpy as np
import yaml

from fields.bmap_reader import BMap
from fields.interpolation import CartesianMesh2D
from fields.zapdos_field_reader import ZapdosFields, read_zapdos_csv, synthetic_zapdos_solution


def write_zapdos_inputs(
    out_dir: str | Path,
    mesh: CartesianMesh2D,
    n_cu: np.ndarray,
    s_iz_cu: np.ndarray,
    b_total: BMap,
    gamma_target: np.ndarray,
    config: dict,
    boundary_estimates: dict[str, Any] | None = None,
) -> None:
    """Write the full Zapdos/MOOSE input package for one coupling iteration.

    Files produced
    --------------
    mesh.csv                 — cell-centre coordinates and volumes
    mc_fields.csv            — n_Cu, S_iz, Bx, By, Bmag on the 2D mesh
    target_source.csv        — Cu neutral flux profile at the target (1D in x)
    boundary_fluxes.csv      — MC-estimated surface fluxes (1D in x)
    wall_losses.csv          — total wall loss rates by species
    operation.csv            — scalar operating-point parameters
    zapdos_input_manifest.yaml — human-readable index of the above
    """
    out = Path(out_dir)
    out.mkdir(parents=True, exist_ok=True)
    boundary_estimates = boundary_estimates or {}
    x2d, y2d = mesh.centers_2d()

    # ------------------------------------------------------------------
    # mesh.csv
    # ------------------------------------------------------------------
    with (out / "mesh.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["index_x", "index_y", "x_center_m", "y_center_m", "cell_area_m2"])
        volumes = mesh.cell_volumes()
        for ix in range(mesh.nx):
            for iy in range(mesh.ny):
                writer.writerow([ix, iy, x2d[ix, iy], y2d[ix, iy], volumes[ix, iy]])

    # ------------------------------------------------------------------
    # mc_fields.csv
    # ------------------------------------------------------------------
    with (out / "mc_fields.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_m", "y_m", "n_Cu_m3", "S_iz_Cu_m3_s", "Bx_T", "By_T", "Bmag_T"])
        for ix in range(mesh.nx):
            for iy in range(mesh.ny):
                bmag = np.sqrt(b_total.bx[ix, iy] ** 2 + b_total.by[ix, iy] ** 2)
                writer.writerow([
                    x2d[ix, iy],
                    y2d[ix, iy],
                    n_cu[ix, iy],
                    s_iz_cu[ix, iy],
                    b_total.bx[ix, iy],
                    b_total.by[ix, iy],
                    bmag,
                ])

    # ------------------------------------------------------------------
    # target_source.csv  (1D profile along x)
    # ------------------------------------------------------------------
    with (out / "target_source.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_m", "Gamma_Cu_target_m1_s"])
        for xv, gamma in zip(mesh.x, gamma_target):
            writer.writerow([xv, gamma])

    _write_boundary_fluxes(out, mesh, boundary_estimates)
    _write_wall_losses(out, boundary_estimates)

    # ------------------------------------------------------------------
    # operation.csv
    # ------------------------------------------------------------------
    with (out / "operation.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow([
            "target_bias_V",
            "plasma_potential_guess_V",
            "target_current_A",
            "target_power_W",
            "wafer_rf_amplitude_V",
            "wafer_rf_frequency_Hz",
            "wafer_rf_ramp_time_s",
            "pressure_Cu_Torr",
        ])
        op = config.get("operation", {})
        writer.writerow([
            op.get("target_bias"),
            op.get("plasma_potential_guess"),
            op.get("target_current"),
            op.get("target_power"),
            op.get("wafer_rf_amplitude"),
            op.get("wafer_rf_frequency"),
            op.get("wafer_rf_ramp_time"),
            op.get("pressure_Cu"),
        ])

    _write_manifest(out, mesh, config)


def make_boundary_estimates(
    mesh: CartesianMesh2D,
    neutral: Any | None = None,
    ion: Any | None = None,
) -> dict[str, Any]:
    """Build Zapdos-facing boundary flux estimates from MC tally objects.

    All linear flux arrays are in m⁻¹ s⁻¹ (per unit z-depth).
    Missing estimates are zero-filled so Zapdos reads a complete file.
    """
    zeros = np.zeros(mesh.nx)
    return {
        "Gamma_Cu0_target_return_m1_s": getattr(neutral, "gamma_target_return", zeros),
        "Gamma_Cu0_wafer_m1_s": getattr(neutral, "gamma_wafer", zeros),
        "Gamma_Cuplus_target_m1_s": getattr(ion, "gamma_target", zeros),
        "Gamma_Cuplus_wafer_m1_s": getattr(ion, "gamma_wafer", zeros),
        "Cu0_wall_loss_rate_s": float(getattr(neutral, "gamma_wall_rate", 0.0)),
        "Cuplus_wall_loss_rate_s": float(getattr(ion, "gamma_wall_rate", 0.0)),
    }


def _write_boundary_fluxes(out: Path, mesh: CartesianMesh2D, estimates: dict[str, Any]) -> None:
    zeros = np.zeros(mesh.nx)
    columns = [
        "Gamma_Cu0_target_return_m1_s",
        "Gamma_Cu0_wafer_m1_s",
        "Gamma_Cuplus_target_m1_s",
        "Gamma_Cuplus_wafer_m1_s",
    ]
    with (out / "boundary_fluxes.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_m", *columns])
        for i, xv in enumerate(mesh.x):
            writer.writerow([xv, *[np.asarray(estimates.get(col, zeros), dtype=float)[i] for col in columns]])


def _write_wall_losses(out: Path, estimates: dict[str, Any]) -> None:
    with (out / "wall_losses.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["species", "loss_rate_s"])
        writer.writerow(["Cu0",    float(estimates.get("Cu0_wall_loss_rate_s", 0.0))])
        writer.writerow(["Cuplus", float(estimates.get("Cuplus_wall_loss_rate_s", 0.0))])


def _write_manifest(out: Path, mesh: CartesianMesh2D, config: dict[str, Any]) -> None:
    op = config.get("operation", {})
    geom = config.get("geometry", {})
    manifest = {
        "coordinate_system": "cartesian_x_y",
        "coordinate_units": "m",
        "mesh": {
            "nx": mesh.nx,
            "ny": mesh.ny,
            "x_min_m": float(mesh.x_edges[0]),
            "x_max_m": float(mesh.x_edges[-1]),
            "y_min_m": 0.0,
            "y_max_m": float(mesh.y_edges[-1]),
            "target_y_m": 0.0,
            "wafer_y_m": float(geom.get("target_to_wafer", mesh.y_max)),
            "wall_x_m": float(geom.get("chamber_radius", abs(mesh.x_edges[-1]))),
        },
        "files": {
            "mesh": "mesh.csv",
            "volume_fields": "mc_fields.csv",
            "target_source": "target_source.csv",
            "boundary_fluxes": "boundary_fluxes.csv",
            "wall_losses": "wall_losses.csv",
            "operation": "operation.csv",
        },
        "volume_fields": {
            "n_Cu_m3": "neutral Cu density from MC, units m^-3 per unit z-depth",
            "S_iz_Cu_m3_s": "Cu ionization source, units m^-3 s^-1 per unit z-depth",
            "Bx_T": "x magnetic field component, Tesla",
            "By_T": "y (axial) magnetic field component, Tesla",
            "Bmag_T": "magnetic field magnitude, Tesla",
        },
        "boundary_fluxes": {
            "Gamma_Cu0_target_return_m1_s": "neutral Cu returning to target, m^-1 s^-1",
            "Gamma_Cu0_wafer_m1_s": "neutral Cu collected at wafer, m^-1 s^-1",
            "Gamma_Cuplus_target_m1_s": "Cu+ target return flux, m^-1 s^-1",
            "Gamma_Cuplus_wafer_m1_s": "Cu+ wafer flux, m^-1 s^-1",
        },
        "operation": {
            "target_bias_V": op.get("target_bias"),
            "plasma_potential_guess_V": op.get("plasma_potential_guess"),
            "target_current_A": op.get("target_current"),
            "target_power_W": op.get("target_power"),
            "wafer_rf_amplitude_V": op.get("wafer_rf_amplitude"),
            "wafer_rf_frequency_Hz": op.get("wafer_rf_frequency"),
            "wafer_rf_ramp_time_s": op.get("wafer_rf_ramp_time"),
            "pressure_Cu_Torr": op.get("pressure_Cu"),
        },
    }
    with (out / "zapdos_input_manifest.yaml").open("w", encoding="utf-8") as f:
        yaml.safe_dump(manifest, f, sort_keys=False)


def run_or_read_zapdos(
    config: dict,
    mesh: CartesianMesh2D,
    n_cu: np.ndarray,
    s_iz: np.ndarray,
) -> ZapdosFields:
    zcfg = config.get("zapdos", {})
    mode = zcfg.get("mode", "synthetic")
    output_dir = Path(zcfg.get("output_dir", "runs/demo/zapdos_output"))
    output_csv = output_dir / "zapdos_fields.csv"

    if mode == "synthetic":
        return synthetic_zapdos_solution(mesh, config, n_cu=n_cu, s_iz=s_iz)

    executable = zcfg.get("executable")
    if executable:
        output_dir.mkdir(parents=True, exist_ok=True)
        subprocess.run([executable], cwd=output_dir, check=True)

    if not output_csv.exists():
        raise FileNotFoundError(f"Zapdos output not found: {output_csv}")
    return read_zapdos_csv(output_csv, mesh)
