from __future__ import annotations

from functools import lru_cache
from pathlib import Path

import numpy as np

from fields.bmap_reader import BMap
from fields.interpolation import CartesianMesh2D, normalize_volume_source
from fields.zapdos_field_reader import ZapdosFields
from mc.particles import ELEMENTARY_CHARGE
from coupling.initialize import copper_sputter_yield


ROOT = Path(__file__).resolve().parents[1]
CU_IONIZATION_RATE_TABLE = ROOT / "rate_coefficients_cu" / "cu_ionization_rate_bolsig.csv"
CU_IONIZATION_FALLBACK_TABLE = ROOT / "rate_coefficients_cu" / "cu_ionization_rate_maxwellian.csv"


def initial_ionization_source(
    config: dict,
    mesh: CartesianMesh2D,
    b_total: BMap,
    total_cu_source_rate: float,
) -> np.ndarray:
    """Build the first-iteration Cu ionization source on the 2D Cartesian mesh.

    The shape is a product of (i) the local B-magnitude normalised to its
    maximum, (ii) a racetrack Gaussian at the instantaneous magnet position,
    and (iii) an exponential decay from target to wafer. Set
    geometry.racetrack_mode: symmetric to add a mirror lobe at x = -r0.
    The result is normalised so that ∫∫ S dx dy = f_ion * total_cu_source_rate.
    """
    phys = config.get("physics", {})
    geom = config.get("geometry", {})
    f_ion = float(phys.get("initial_f_ion", 0.1))
    l_ion = float(phys.get("initial_L_ion", 0.05))
    r0 = float(geom.get("racetrack_radius", 0.105))
    width = float(geom.get("racetrack_width", 0.025))
    mode = str(geom.get("racetrack_mode", "one_sided")).lower()

    x, y = mesh.centers_2d()
    bmag = b_total.magnitude().values
    f_b = bmag / max(float(np.max(bmag)), 1.0e-30)

    f_x = np.exp(-0.5 * ((x - r0) / max(width, 1.0e-9)) ** 2)
    if mode in {"symmetric", "two_sided", "two-sided", "mirror"}:
        f_x = f_x + np.exp(-0.5 * ((x + r0) / max(width, 1.0e-9)) ** 2)

    source_shape = (0.10 + 0.90 * f_b) * f_x * np.exp(-y / max(l_ion, 1.0e-9))
    return normalize_volume_source(mesh, source_shape, f_ion * total_cu_source_rate)


def coupled_ionization_source(
    mesh: CartesianMesh2D,
    n_cu: np.ndarray,
    zapdos: ZapdosFields,
) -> np.ndarray:
    """S_iz = n_Cu × n_e × k_iz(Te)  [m⁻³ s⁻¹ per unit z-depth]."""
    return np.maximum(n_cu, 0.0) * np.maximum(zapdos.ne, 0.0) * ionization_rate_coeff_cu(zapdos.te)


def ionization_rate_coeff_cu(te_ev: np.ndarray) -> np.ndarray:
    """Cu electron-impact ionization rate coefficient k_iz(Te) [m³ s⁻¹]."""
    te = np.maximum(np.asarray(te_ev, dtype=float), 1.0e-3)
    table = _load_cu_ionization_rate_table()
    if table is not None:
        return np.interp(te, table[:, 0], table[:, 1])
    threshold = 7.73
    return 2.5e-14 * np.sqrt(te) * np.exp(-threshold / te)


@lru_cache(maxsize=1)
def _load_cu_ionization_rate_table() -> np.ndarray | None:
    path = CU_IONIZATION_RATE_TABLE if CU_IONIZATION_RATE_TABLE.exists() else CU_IONIZATION_FALLBACK_TABLE
    if not path.exists():
        return None
    table = np.genfromtxt(path, delimiter=",", names=True)
    if table.size == 0:
        return None
    te = np.asarray(table["Te_eV"], dtype=float)
    kiz = np.asarray(table["k_iz_m3_s"], dtype=float)
    return np.column_stack((te, kiz))


def update_target_source_from_ion_flux(
    config: dict,
    mesh: CartesianMesh2D,
    gamma_cu_ion_target: np.ndarray,
    residual_ar_sputter: np.ndarray | None = None,
) -> tuple[np.ndarray, dict[str, float]]:
    """Recompute the target Cu source from the Cu+ ion flux hitting the target.

    Applies the sputter yield, optional Ar+ residual contribution, and a
    current-normalisation scale factor so the simulated target current matches
    the measured value.

    Parameters
    ----------
    gamma_cu_ion_target:
        Cu+ flux at target, shape (nx,), m⁻¹ s⁻¹.

    Returns
    -------
    gamma_new:
        Updated Cu neutral source, shape (nx,), m⁻¹ s⁻¹.
    info:
        Dictionary with diagnostic scalars.
    """
    op = config.get("operation", {})
    phys = config.get("physics", {})
    v_plasma = float(op.get("plasma_potential_guess", 10.0))
    v_target = float(op.get("target_bias", -500.0))
    gamma_secondary = float(phys.get("gamma_secondary", 0.0))
    ion_energy_ev = max(v_plasma - v_target, 0.0)
    y_cu = copper_sputter_yield(ion_energy_ev, phys.get("sputter_yield_model", "yamamura_like_fit"))

    gamma_new = y_cu * np.maximum(gamma_cu_ion_target, 0.0)
    if residual_ar_sputter is not None:
        gamma_new = gamma_new + np.maximum(residual_ar_sputter, 0.0)

    # sum(gamma * dx) gives total particle rate [s⁻¹ per unit z-depth]
    dx = mesh.surface_areas_bottom()
    i_target_sim = (
        ELEMENTARY_CHARGE
        * float(np.sum(gamma_cu_ion_target * dx))
        * (1.0 + gamma_secondary)
    )
    i_target_measured = float(op.get("target_current", i_target_sim))
    scale = 1.0
    if i_target_sim > 0.0:
        scale = i_target_measured / i_target_sim
        gamma_new = gamma_new * scale

    return gamma_new, {
        "ion_energy_eV": ion_energy_ev,
        "sputter_yield": y_cu,
        "I_target_sim_A": i_target_sim,
        "target_current_scale": scale,
    }
