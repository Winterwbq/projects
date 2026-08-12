from __future__ import annotations

import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import numpy as np

from coupling.convergence import ConvergenceState, evaluate_convergence
from coupling.initialize import build_mesh, initial_target_cu_source, load_case
from coupling.relaxation import smooth_then_relax, under_relax
from coupling.source_update import coupled_ionization_source, initial_ionization_source, update_target_source_from_ion_flux
from coupling.zapdos_io import make_boundary_estimates, run_or_read_zapdos, write_zapdos_inputs
from fields.bmap_reader import load_bmaps
from fields.interpolation import CartesianMesh2D
from mc.boundaries import ChamberGeometry
from mc.ion_cu_mc import IonCuMC
from mc.neutral_cu_mc import NeutralCuMC
from post.diagnostics import append_diagnostics_csv, collect_iteration_diagnostics, save_iteration_npz
from post.plots import save_field_plot, save_linear_flux_plot


@dataclass
class DriverResult:
    run_dir: Path
    history: list[dict[str, float]]
    convergence: ConvergenceState


def run_coupled_case(
    config_path: str | Path,
    max_iter_override: int | None = None,
    clean: bool = False,
) -> DriverResult:
    config = load_case(config_path)
    mesh = build_mesh(config)
    run_dir = Path(config.get("output", {}).get("run_dir", "runs/demo"))
    if clean and run_dir.exists():
        shutil.rmtree(run_dir)
    run_dir.mkdir(parents=True, exist_ok=True)

    bmaps = load_bmaps(config, mesh)
    b_total = bmaps["B_total"]
    geometry = ChamberGeometry.from_config(config)
    rng = np.random.default_rng(int(config.get("mc", {}).get("random_seed", 1)))

    target_source, init_info = initial_target_cu_source(config, mesh)
    neutral_mc = NeutralCuMC(mesh, geometry, config, rng)
    ion_mc = IonCuMC(mesh, geometry, config, rng)

    neutral = neutral_mc.run(target_source)
    n_cu = neutral.n_cu
    s_iz = initial_ionization_source(config, mesh, b_total, init_info["total_cu_source_s"])

    history: list[dict[str, float]] = []
    tol = float(config.get("mc", {}).get("convergence_tol", 0.02))
    alpha = float(config.get("mc", {}).get("alpha_relax", 0.1))
    max_iter = int(max_iter_override or config.get("mc", {}).get("max_iter", 100))
    convergence = ConvergenceState(False, {})
    previous_ion = None

    for iteration in range(1, max_iter + 1):
        zapdos_input_dir = (
            Path(config.get("zapdos", {}).get("input_dir", run_dir / "zapdos_input"))
            / f"iter_{iteration:04d}"
        )
        write_zapdos_inputs(
            zapdos_input_dir,
            mesh,
            n_cu,
            s_iz,
            b_total,
            target_source,
            config,
            boundary_estimates=make_boundary_estimates(mesh, neutral=neutral, ion=previous_ion),
        )

        zapdos_fields = run_or_read_zapdos(config, mesh, n_cu=n_cu, s_iz=s_iz)
        neutral_new = neutral_mc.run(target_source)
        ion = ion_mc.run(s_iz, zapdos_fields, b_total)

        target_new, source_info = update_target_source_from_ion_flux(config, mesh, ion.gamma_target)
        dx = mesh.surface_areas_bottom()
        if np.sum(target_new * dx) <= 0.0:
            target_new = target_source.copy()

        s_iz_new = coupled_ionization_source(mesh, neutral_new.n_cu, zapdos_fields)
        if np.sum(s_iz_new * mesh.cell_volumes()) <= 0.0:
            s_iz_new = s_iz.copy()

        n_cu = smooth_then_relax(mesh, n_cu, neutral_new.n_cu, alpha=alpha, smoothing_passes=1)
        s_iz = smooth_then_relax(mesh, s_iz, s_iz_new, alpha=alpha, smoothing_passes=1)
        target_source = under_relax(target_source, target_new, alpha=alpha)
        neutral = neutral_new
        previous_ion = ion

        row = collect_iteration_diagnostics(
            iteration=iteration,
            mesh=mesh,
            target_source=target_source,
            s_iz=s_iz,
            neutral=neutral,
            ion=ion,
            source_info=source_info,
            config=config,
        )
        history.append(row)
        append_diagnostics_csv(run_dir / "diagnostics.csv", row)
        save_iteration_npz(run_dir / f"iteration_{iteration:04d}.npz", mesh, n_cu, s_iz, target_source, neutral, ion)
        _maybe_plot(config, run_dir, iteration, mesh, n_cu, s_iz, zapdos_fields, neutral, ion, target_source)

        convergence = evaluate_convergence(history, tol)
        if convergence.converged:
            break

    return DriverResult(run_dir=run_dir, history=history, convergence=convergence)


def _maybe_plot(
    config: dict[str, Any],
    run_dir: Path,
    iteration: int,
    mesh: CartesianMesh2D,
    n_cu: np.ndarray,
    s_iz: np.ndarray,
    zapdos_fields: Any,
    neutral: Any,
    ion: Any,
    target_source: np.ndarray,
) -> None:
    if not config.get("output", {}).get("save_plots", True):
        return
    plot_dir = run_dir / "plots" / f"iter_{iteration:04d}"
    save_field_plot(mesh, n_cu, plot_dir / "n_Cu.png", "n_Cu [m^-3]", log=False)
    save_field_plot(mesh, s_iz, plot_dir / "S_iz.png", "S_iz [m^-3 s^-1]", log=False)
    save_field_plot(mesh, zapdos_fields.ne, plot_dir / "ne.png", "n_e [m^-3]", log=False)
    save_field_plot(mesh, zapdos_fields.phi, plot_dir / "phi.png", "phi [V]", log=False)
    save_linear_flux_plot(
        mesh,
        {
            "target source Cu0": target_source,
            "wafer Cu0": neutral.gamma_wafer,
            "wafer Cu+": ion.gamma_wafer,
            "target Cu+": ion.gamma_target,
        },
        plot_dir / "linear_fluxes.png",
        "Linear flux profiles (per unit x per unit z-depth)",
    )
