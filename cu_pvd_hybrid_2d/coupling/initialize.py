from __future__ import annotations

from pathlib import Path
from typing import Any

import numpy as np
import yaml

from fields.interpolation import CartesianMesh2D
from mc.particles import ELEMENTARY_CHARGE


def load_case(path: str | Path) -> dict[str, Any]:
    with Path(path).open("r", encoding="utf-8") as f:
        return yaml.safe_load(f)


def build_mesh(config: dict[str, Any]) -> CartesianMesh2D:
    mesh_cfg = config.get("mesh", {})
    geom = config.get("geometry", {})
    return CartesianMesh2D.from_bounds(
        nx=int(mesh_cfg.get("nx", 128)),
        ny=int(mesh_cfg.get("ny", 96)),
        x_min=float(mesh_cfg.get("x_min", -geom.get("chamber_radius", 0.24))),
        x_max=float(mesh_cfg.get("x_max",  geom.get("chamber_radius", 0.24))),
        y_max=float(mesh_cfg.get("y_max",  geom.get("target_to_wafer", 0.60))),
    )


def initial_target_cu_source(
    config: dict[str, Any],
    mesh: CartesianMesh2D,
) -> tuple[np.ndarray, dict[str, float]]:
    """Compute the initial Cu neutral flux profile at the target.

    Returns ``gamma_target`` (shape nx, m⁻¹ s⁻¹) and an info dict.
    """
    op = config.get("operation", {})
    phys = config.get("physics", {})
    geom = config.get("geometry", {})

    target_current = float(op.get("target_current", 1.0))
    v_plasma = float(op.get("plasma_potential_guess", 10.0))
    v_target = float(op.get("target_bias", -500.0))
    gamma_secondary = float(phys.get("gamma_secondary", 0.0))
    # erosion_area is the effective 1D target width (m) per unit z-depth
    erosion_area = float(
        geom.get("erosion_area", 2.0 * geom.get("target_radius", 0.18))
    )

    ion_energy_ev = max(v_plasma - v_target, 0.0)
    gamma_i_target = target_current / (ELEMENTARY_CHARGE * erosion_area * (1.0 + gamma_secondary))
    sputter_yield = copper_sputter_yield(ion_energy_ev, phys.get("sputter_yield_model", "yamamura_like_fit"))
    gamma_cu_area_avg = sputter_yield * gamma_i_target
    total_cu_rate = gamma_cu_area_avg * erosion_area

    profile = racetrack_profile(config, mesh)
    gamma_target = _normalize_linear_profile(mesh, profile, total_cu_rate)
    info = {
        "ion_energy_eV": ion_energy_ev,
        "gamma_i_target_m2_s": gamma_i_target,
        "sputter_yield": sputter_yield,
        "total_cu_source_s": total_cu_rate,
    }
    return gamma_target, info


def racetrack_profile(config: dict[str, Any], mesh: CartesianMesh2D) -> np.ndarray:
    """Return the unnormalised racetrack sputtering profile on the target (1D in x).

    By default this represents one instantaneous rolling-magnet location at
    x = +r0. Set geometry.racetrack_mode: symmetric to use mirror-image lobes
    at x = +/-r0.
    """
    geom = config.get("geometry", {})
    r0 = float(geom.get("racetrack_radius", 0.105))
    width = float(geom.get("racetrack_width", 0.025))
    target_radius = float(geom.get("target_radius", 0.18))
    mode = str(geom.get("racetrack_mode", "one_sided")).lower()
    x = mesh.x
    profile = np.exp(-0.5 * ((x - r0) / max(width, 1.0e-9)) ** 2)
    if mode in {"symmetric", "two_sided", "two-sided", "mirror"}:
        profile = profile + np.exp(-0.5 * ((x + r0) / max(width, 1.0e-9)) ** 2)
    profile = np.where(np.abs(x) <= target_radius, profile, 0.0)
    return profile


def copper_sputter_yield(ion_energy_ev: float, model: str = "yamamura_like_fit") -> float:
    """Yamamura-like fit for Cu self-sputtering (Cu+ → Cu target).

    Threshold ~17 eV. Yield exceeds 1 above ~250 eV, enabling a self-sustaining
    discharge. Coefficients are tuned to match Yamamura & Tawara (1996) tabulated
    values for Cu/Cu at normal incidence:
        E=200 eV → Y ≈ 1.1, E=500 eV → Y ≈ 2.1, E=1000 eV → Y ≈ 3.2.

    Replace with a validated measurement table when available.
    """
    threshold = 17.0   # eV, Cu+→Cu (lower than Ar+→Cu at ~25 eV)
    e = max(float(ion_energy_ev), 0.0)
    if e <= threshold:
        return 0.0
    reduced = 1.0 - threshold / e
    if model == "constant":
        return 1.0
    # Normalisation reference 100 eV; exponent 1.6 matches Yamamura shape for Cu/Cu.
    return float(np.clip(0.90 * np.sqrt(e / 100.0) * reduced ** 1.6, 0.0, 5.0))


def _normalize_linear_profile(
    mesh: CartesianMesh2D,
    profile: np.ndarray,
    total_rate: float,
) -> np.ndarray:
    """Return flux density (m⁻¹ s⁻¹) such that sum(flux * dx) == total_rate."""
    weights = np.maximum(profile, 0.0) * mesh.surface_areas_bottom()
    integral = float(np.sum(weights))
    if integral <= 0.0:
        raise ValueError("racetrack source profile integrates to zero")
    return np.maximum(profile, 0.0) * (float(total_rate) / integral)
