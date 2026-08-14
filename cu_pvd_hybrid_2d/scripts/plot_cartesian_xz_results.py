#!/usr/bin/env python3
"""Postprocess Cartesian X-Z Zapdos diagnostics and Cu-ion flux results."""
from __future__ import annotations

import argparse
import csv
from pathlib import Path
from typing import TYPE_CHECKING

import numpy as np

if TYPE_CHECKING:
    import matplotlib.pyplot as plt
    import matplotlib.tri as mtri


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_EXODUS = ROOT / "zapdos_templates" / "Outputs"/"cu_pvd_hybrid_hpem_xz_30cm_4coilsBimg3092_guidedSEE_1.e"
DEFAULT_OUTPUT = ROOT / "post" / "zapdos_last_timestep_diagnostics_30cm_9.png"
ION_DENSITY_LOG_RANGE = (1e12 , 1e15)
CHAMBER_DENSITY_PERCENTILES = (2.0, 98.0)
DENSITY_GROWTH_PERCENTILE = 98.0
AVOGADRO = 6.02214076e23
BOLTZMANN = 1.3807e-23
ELEMENTARY_CHARGE = 1.602e-19
ION_TEMPERATURE = 300.0
REDUCED_MOBILITY = 2.2e-4
REFERENCE_PRESSURE = 101325.0
REFERENCE_TEMPERATURE = 273.15


def _decode_names(raw: np.ndarray) -> list[str]:
    names: list[str] = []
    for row in raw:
        names.append(
            b"".join(np.asarray(row).astype("S1"))
            .decode("ascii", errors="ignore")
            .replace("\x00", "")
            .strip()
        )
    return names


def _positive_log_norm(values: np.ndarray):
    from matplotlib.colors import LogNorm

    positive = values[np.isfinite(values) & (values > 0.0)]
    if positive.size == 0:
        return LogNorm(vmin=1.0e-30, vmax=1.0)
    vmin = max(float(np.percentile(positive, 1.0)), 1.0e-30)
    vmax = float(np.nanmax(positive))
    if vmin >= vmax:
        vmax = max(vmin * 1.01, 1.0e-29)
    return LogNorm(vmin=vmin, vmax=vmax)


def _ion_density_log_limits() -> tuple[float, float]:
    return ION_DENSITY_LOG_RANGE


def _ion_density_log_norm():
    from matplotlib.colors import LogNorm

    vmin, vmax = _ion_density_log_limits()
    return LogNorm(vmin=vmin, vmax=vmax)


def _finite_minmax(values: np.ndarray) -> tuple[float, float]:
    finite = values[np.isfinite(values)]
    if finite.size == 0:
        return float("nan"), float("nan")
    return float(np.nanmin(finite)), float(np.nanmax(finite))


def _element_centers(x_cm: np.ndarray, y_cm: np.ndarray, connect: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    return np.mean(x_cm[connect], axis=1), np.mean(y_cm[connect], axis=1)


def _masked(values: np.ndarray, mask: np.ndarray | None) -> np.ndarray:
    if mask is None:
        return values
    return np.where(mask, values, np.nan)


def _select_timestep(
    times: np.ndarray,
    *,
    timestep: int | None = None,
    time: float | None = None,
) -> int:
    times = np.asarray(times)
    if times.size == 0:
        raise ValueError("Exodus file has no saved timesteps.")
    if timestep is not None and time is not None:
        raise ValueError("Use either timestep or time, not both.")
    if timestep is not None:
        step = int(timestep)
        if step < 0:
            step += times.size
        if step < 0 or step >= times.size:
            raise ValueError(f"Saved timestep index {timestep} is outside 0..{times.size - 1}.")
        return step
    if time is not None:
        target_time = float(time)
        finite = np.isfinite(times)
        if not finite.any():
            raise ValueError("Exodus timestep array has no finite times.")
        finite_indices = np.flatnonzero(finite)
        nearest_finite_index = int(np.nanargmin(np.abs(times[finite] - target_time)))
        return int(finite_indices[nearest_finite_index])
    return int(times.size - 1)


def _density_contrast(values: np.ndarray, mask: np.ndarray | None) -> tuple[np.ndarray, TwoSlopeNorm | None]:
    from matplotlib.colors import TwoSlopeNorm

    baseline_values = values if mask is None else values[mask]
    positive = baseline_values[np.isfinite(baseline_values) & (baseline_values > 0.0)]
    if positive.size == 0:
        return values, None

    reference = float(np.nanmedian(positive))
    contrast = np.log10(np.maximum(values, 1.0e-300) / reference)
    contrast = _masked(contrast, mask)
    finite = contrast[np.isfinite(contrast)]
    if finite.size == 0:
        return contrast, None

    span = max(abs(float(np.nanmin(finite))), abs(float(np.nanmax(finite))), 1.0e-6)
    return contrast, TwoSlopeNorm(vmin=-span, vcenter=0.0, vmax=span)


def _validate_density_percentiles(percentiles: tuple[float, float]) -> tuple[float, float]:
    low, high = float(percentiles[0]), float(percentiles[1])
    if not (0.0 <= low < high <= 100.0):
        raise ValueError("density percentiles must satisfy 0 <= low < high <= 100")
    return low, high


def _chamber_density_view(
    values: np.ndarray,
    mask: np.ndarray | None,
    *,
    percentiles: tuple[float, float] = CHAMBER_DENSITY_PERCENTILES,
):
    from matplotlib.colors import Normalize

    percentiles = _validate_density_percentiles(percentiles)
    plot_values = _masked(values, mask)
    baseline_values = values if mask is None else values[mask]
    positive = baseline_values[np.isfinite(baseline_values) & (baseline_values > 0.0)]
    if positive.size == 0:
        return plot_values, Normalize(vmin=0.0, vmax=1.0)

    vmin, vmax = np.percentile(positive, percentiles)
    vmin = float(vmin)
    vmax = float(vmax)
    if vmin >= vmax:
        pad = max(abs(vmin) * 0.01, 1.0)
        vmin -= pad
        vmax += pad
    return plot_values, Normalize(vmin=vmin, vmax=vmax)


def _net_density_growth(
    history: np.ndarray,
    *,
    step: int,
    reference_step: int,
    mask: np.ndarray | None,
) -> np.ndarray:
    values = np.asarray(history)
    if values.ndim < 2:
        raise ValueError("density history must include saved timestep and spatial dimensions")
    return _masked(values[step] - values[reference_step], mask)


def _first_valid_density_step(*density_histories: np.ndarray) -> int:
    if not density_histories:
        raise ValueError("at least one density history is required")

    histories = [np.asarray(history) for history in density_histories]
    if any(history.ndim < 2 for history in histories):
        raise ValueError("density histories must have matching saved timesteps")
    timestep_counts = {history.shape[0] for history in histories}
    if len(timestep_counts) != 1:
        raise ValueError("density histories must have matching saved timesteps")

    for step in range(histories[0].shape[0]):
        if all(np.any(np.isfinite(history[step]) & (history[step] > 0.0)) for history in histories):
            return step
    raise ValueError("density histories contain no populated saved timestep")


def _shared_density_growth_norm(
    *growth_fields: np.ndarray,
    percentile: float = DENSITY_GROWTH_PERCENTILE,
):
    from matplotlib.colors import TwoSlopeNorm

    if not (0.0 < percentile <= 100.0):
        raise ValueError("growth percentile must satisfy 0 < percentile <= 100")

    finite_magnitudes = [
        np.abs(np.asarray(values)[np.isfinite(values)]) for values in growth_fields
    ]
    nonempty = [values for values in finite_magnitudes if values.size]
    if not nonempty:
        limit = 1.0
    else:
        limit = float(np.percentile(np.concatenate(nonempty), percentile))
        if not np.isfinite(limit) or limit <= 0.0:
            limit = 1.0
    return TwoSlopeNorm(vmin=-limit, vcenter=0.0, vmax=limit)


def _validate_density_display_mode(
    *,
    density_growth: bool,
    density_contrast: bool,
    density_style: str,
) -> None:
    if density_growth and (density_contrast or density_style != "default"):
        raise ValueError(
            "--density-growth cannot be combined with --density-contrast or "
            "--density-style chamber"
        )


def _load_exodus(path: Path) -> dict[str, np.ndarray | list[str]]:
    from scipy.io import netcdf_file

    with netcdf_file(path, "r", mmap=False) as exo:
        elem_names = _decode_names(np.asarray(exo.variables["name_elem_var"].data))
        node_names = _decode_names(np.asarray(exo.variables["name_nod_var"].data))

        def elem_var(name: str) -> np.ndarray:
            index = elem_names.index(name) + 1
            return np.asarray(exo.variables[f"vals_elem_var{index}eb1"].data).copy()

        def node_var(name: str) -> np.ndarray:
            index = node_names.index(name) + 1
            return np.asarray(exo.variables[f"vals_nod_var{index}"].data).copy()

        data: dict[str, np.ndarray | list[str]] = {
            "times": np.asarray(exo.variables["time_whole"].data).copy(),
            "x": np.asarray(exo.variables["coordx"].data).copy(),
            "y": np.asarray(exo.variables["coordy"].data).copy(),
            "connect": np.asarray(exo.variables["connect1"].data).copy().astype(int) - 1,
            "elem_names": elem_names,
            "node_names": node_names,
            "em_density": elem_var("em_density"),
            "cu_ion_density": elem_var("Cu+_density"),
            "e_temp": elem_var("e_temp"),
            "efield_x": elem_var("EFieldx"),
            "efield_y": elem_var("EFieldy"),
            "n_cu_effective": node_var("n_Cu_effective"),
            "potential": node_var("potential"),
        }
        if "S_Cu_eff" in node_names:
            data["see_cu_source"] = node_var("S_Cu_eff")
        if "Qe_eff" in node_names:
            data["see_energy_source"] = node_var("Qe_eff")
        if "Ar+_density" in elem_names:
            data["ar_ion_density"] = elem_var("Ar+_density")
        return data


def _elem_face_values(values: np.ndarray) -> np.ndarray:
    return np.concatenate((values, values))


def _plot_element_field(
    ax: "plt.Axes",
    triangulation: "mtri.Triangulation",
    values: np.ndarray,
    title: str,
    cmap: str,
    norm=None,
):
    from matplotlib.colors import LogNorm

    face_values = _elem_face_values(values)
    if isinstance(norm, LogNorm):
        face_values = np.maximum(face_values, norm.vmin)
    pcm = ax.tripcolor(
        triangulation,
        facecolors=face_values,
        shading="flat",
        cmap=cmap,
        norm=norm,
        rasterized=True,
    )
    ax.set_title(title)
    return pcm


def _annotate_chamber(ax: "plt.Axes", xlabel: str, ylabel: str) -> None:
    ax.axhline(0.0, color="white", linewidth=0.9, alpha=0.9)
    ax.axhline(30.0, color="white", linewidth=0.7, alpha=0.55)
    ax.set_aspect("equal")
    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)


def plot_last_timestep(
    exodus_path: Path,
    output_path: Path,
    *,
    timestep: int | None = None,
    time: float | None = None,
    bulk_margin_cm: float = 0.0,
    density_growth: bool = False,
    density_contrast: bool = False,
    density_style: str = "default",
    density_percentiles: tuple[float, float] = CHAMBER_DENSITY_PERCENTILES,
    xlabel: str = "x [cm]",
    ylabel: str = "y [cm]",
) -> None:
    import matplotlib.pyplot as plt
    import matplotlib.tri as mtri
    from matplotlib.colors import LogNorm, TwoSlopeNorm

    _validate_density_display_mode(
        density_growth=density_growth,
        density_contrast=density_contrast,
        density_style=density_style,
    )

    data = _load_exodus(exodus_path)
    times = np.asarray(data["times"])
    step = _select_timestep(times, timestep=timestep, time=time)

    x_cm = np.asarray(data["x"]) * 100.0
    y_cm = np.asarray(data["y"]) * 100.0
    connect = np.asarray(data["connect"])
    triangles = np.vstack((connect[:, [0, 1, 2]], connect[:, [0, 2, 3]]))
    triangulation = mtri.Triangulation(x_cm, y_cm, triangles)
    _, elem_y_cm = _element_centers(x_cm, y_cm, connect)

    element_bulk_mask = None
    node_bulk_mask = None
    if bulk_margin_cm > 0.0:
        y_min = float(np.nanmin(y_cm))
        y_max = float(np.nanmax(y_cm))
        element_bulk_mask = (elem_y_cm >= y_min + bulk_margin_cm) & (elem_y_cm <= y_max - bulk_margin_cm)
        node_bulk_mask = (y_cm >= y_min + bulk_margin_cm) & (y_cm <= y_max - bulk_margin_cm)

    em_history = np.asarray(data["em_density"])
    cup_history = np.asarray(data["cu_ion_density"])
    growth_reference_step = None
    if density_growth:
        growth_reference_step = _first_valid_density_step(em_history, cup_history)
        if step < growth_reference_step:
            raise ValueError(
                f"selected saved step {step} precedes the first populated density step "
                f"{growth_reference_step}"
            )
    em = em_history[step]
    cup = cup_history[step]
    n_cu_effective = np.asarray(data["n_cu_effective"])[step]
    arp = np.asarray(data["ar_ion_density"])[step] if "ar_ion_density" in data else None
    e_temp = np.asarray(data["e_temp"])[step]
    efield_mag = np.hypot(np.asarray(data["efield_x"])[step], np.asarray(data["efield_y"])[step])
    potential = np.asarray(data["potential"])[step]

    plot_em = _masked(em, element_bulk_mask)
    plot_cup = _masked(cup, element_bulk_mask)
    plot_e_temp = _masked(e_temp, element_bulk_mask)
    plot_efield_mag = _masked(efield_mag, element_bulk_mask)
    plot_n_cu_effective = _masked(n_cu_effective, node_bulk_mask)
    plot_potential = _masked(potential, node_bulk_mask)

    em_title = "Electron density $n_e$ [m$^{-3}$]"
    cup_title = "Cu$^+$ density [m$^{-3}$]"
    em_cmap = "turbo"
    cup_cmap = "turbo"
    em_norm = _positive_log_norm(plot_em)
    cup_norm = _ion_density_log_norm()
    if density_growth:
        plot_em = _net_density_growth(
            em_history,
            step=step,
            reference_step=growth_reference_step,
            mask=element_bulk_mask,
        )
        plot_cup = _net_density_growth(
            cup_history,
            step=step,
            reference_step=growth_reference_step,
            mask=element_bulk_mask,
        )
        growth_norm = _shared_density_growth_norm(plot_em, plot_cup)
        em_norm = growth_norm
        cup_norm = growth_norm
        em_title = "Net electron growth $\\Delta n_e$ [m$^{-3}$]"
        cup_title = "Net Cu$^+$ growth $\\Delta n_{Cu+}$ [m$^{-3}$]"
        em_cmap = "coolwarm"
        cup_cmap = "coolwarm"
    elif density_style == "chamber":
        density_percentiles = _validate_density_percentiles(density_percentiles)
        low_pct, high_pct = density_percentiles
        plot_em, em_norm = _chamber_density_view(
            em, element_bulk_mask, percentiles=density_percentiles
        )
        plot_cup, cup_norm = _chamber_density_view(
            cup, element_bulk_mask, percentiles=density_percentiles
        )
        em_title = f"Electron density $n_e$ [m$^{{-3}}$] (linear {low_pct:g}-{high_pct:g}%)"
        cup_title = f"Cu$^+$ density [m$^{{-3}}$] (linear {low_pct:g}-{high_pct:g}%)"
        em_cmap = "turbo"
        cup_cmap = "turbo"
    elif density_contrast:
        plot_em, em_norm = _density_contrast(em, element_bulk_mask)
        plot_cup, cup_norm = _density_contrast(cup, element_bulk_mask)
        em_title = "Electron density contrast log$_{10}(n_e/median)$"
        cup_title = "Cu$^+$ density contrast log$_{10}(n_{Cu+}/median)$"
        em_cmap = "coolwarm"
        cup_cmap = "coolwarm"

    fields: list[tuple[str, np.ndarray, str, object, str]] = [
        (em_title, plot_em, em_cmap, em_norm, "element"),
        (cup_title, plot_cup, cup_cmap, cup_norm, "element"),
        (
            "Effective neutral Cu density $n_{Cu}$ [m$^{-3}$]",
            plot_n_cu_effective,
            "YlGnBu",
            _positive_log_norm(plot_n_cu_effective),
            "node",
        ),
    ]
    if arp is not None:
        plot_arp = arp
        arp_cmap = "turbo"
        arp_norm = _ion_density_log_norm()
        arp_title = "Ar$^+$ density [m$^{-3}$]"
        if density_style == "chamber":
            plot_arp, arp_norm = _chamber_density_view(
                arp, element_bulk_mask, percentiles=density_percentiles
            )
            arp_cmap = "turbo"
            low_pct, high_pct = density_percentiles
            arp_title = f"Ar$^+$ density [m$^{{-3}}$] (linear {low_pct:g}-{high_pct:g}%)"
        fields.append(
            (arp_title, plot_arp, arp_cmap, arp_norm, "element")
        )
    fields.extend(
        [
            ("Electron temperature $T_e$ [eV]", plot_e_temp, "inferno", None, "element"),
            ("|E| [V/m]", plot_efield_mag, "plasma", _positive_log_norm(plot_efield_mag), "element"),
        ]
    )

    vmin, vmax = _finite_minmax(plot_potential)
    if vmin < 0.0 < vmax:
        pnorm = TwoSlopeNorm(vmin=vmin, vcenter=0.0, vmax=vmax)
    else:
        pnorm = None
    fields.append(("Potential [V]", plot_potential, "coolwarm", pnorm, "node"))

    ncols = 3
    nrows = int(np.ceil(len(fields) / ncols))
    fig, axes = plt.subplots(nrows, ncols, figsize=(15.0, 4.8 * nrows), constrained_layout=True)
    axes = np.asarray(axes).reshape(nrows, ncols)

    for ax, (title, values, cmap, norm, field_type) in zip(axes.ravel(), fields):
        if field_type == "node":
            plot_values = np.maximum(values, norm.vmin) if isinstance(norm, LogNorm) else values
            pcm = ax.tripcolor(
                triangulation,
                plot_values,
                shading="gouraud",
                cmap=cmap,
                norm=norm,
                rasterized=True,
            )
        else:
            pcm = _plot_element_field(ax, triangulation, values, title, cmap, norm=norm)
        if field_type == "node":
            ax.set_title(title)
        _annotate_chamber(ax, xlabel, ylabel)
        fig.colorbar(pcm, ax=ax, pad=0.015, shrink=0.9)

    for ax in axes.ravel()[len(fields) :]:
        ax.set_visible(False)

    if step == len(times) - 1 and timestep is None and time is None:
        title = f"Last saved Zapdos timestep: t = {times[step]:.6e} s"
    else:
        title = f"Zapdos saved timestep {step}: t = {times[step]:.6e} s"
    if density_growth:
        title += (
            f"; net density change from t_ref = {times[growth_reference_step]:.6e} s"
        )
    fig.suptitle(title, fontweight="bold")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=220)
    plt.close(fig)

    # Print extrema and locations for the element fields.
    elem_x = np.asarray(data.get("x_position", []))
    if elem_x.size == 0 and "x_position" in data.get("elem_names", []):
        pass
    print(f"Wrote {output_path}")
    print(f"Selected saved step: {step}, time = {times[step]:.6e} s")
    if density_growth:
        print(
            f"Net density growth reference: saved step {growth_reference_step}, "
            f"time = {times[growth_reference_step]:.6e} s"
        )
        print(
            "shared linear growth range: "
            f"[{em_norm.vmin:.3e}, {em_norm.vmax:.3e}] m^-3"
        )
    print(f"potential range: [{np.nanmin(potential):.3e}, {np.nanmax(potential):.3e}] V")
    if bulk_margin_cm > 0.0:
        print(f"bulk diagnostics exclude y within {bulk_margin_cm:.3g} cm of bottom/top boundaries")
    for name, values in [
        ("em_density", em),
        ("Cu+_density", cup),
        ("n_Cu_effective", n_cu_effective),
        ("Ar+_density", arp),
        ("e_temp", e_temp),
        ("|E|", efield_mag),
    ]:
        if values is None:
            continue
        print(f"{name}: min={np.nanmin(values):.3e}, max={np.nanmax(values):.3e}")
    if element_bulk_mask is not None:
        for name, values in [
            ("bulk em_density", em),
            ("bulk Cu+_density", cup),
            ("bulk e_temp", e_temp),
            ("bulk |E|", efield_mag),
        ]:
            bulk_values = values[element_bulk_mask]
            print(f"{name}: min={np.nanmin(bulk_values):.3e}, max={np.nanmax(bulk_values):.3e}")


def _element_log_gradient(
    x: np.ndarray,
    z: np.ndarray,
    connect: np.ndarray,
    log_density: np.ndarray,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the QUAD4 log-density gradient at each element center."""
    if connect.ndim != 2 or connect.shape[1] != 4:
        raise ValueError("Cu+ flux reconstruction requires QUAD4 elements")
    xe = x[connect]
    ze = z[connect]
    ue = log_density[connect]
    dxi = np.asarray([-0.25, 0.25, 0.25, -0.25])
    deta = np.asarray([-0.25, -0.25, 0.25, 0.25])
    dx_dxi = np.sum(xe * dxi, axis=1)
    dz_dxi = np.sum(ze * dxi, axis=1)
    dx_deta = np.sum(xe * deta, axis=1)
    dz_deta = np.sum(ze * deta, axis=1)
    du_dxi = np.sum(ue * dxi, axis=1)
    du_deta = np.sum(ue * deta, axis=1)
    determinant = dx_dxi * dz_deta - dz_dxi * dx_deta
    if np.any(np.isclose(determinant, 0.0)):
        raise ValueError("mesh contains a degenerate QUAD4 element")
    du_dx = (dz_deta * du_dxi - dz_dxi * du_deta) / determinant
    du_dz = (-dx_deta * du_dxi + dx_dxi * du_deta) / determinant
    return du_dx, du_dz


def reconstruct_ion_flux(
    data: dict[str, np.ndarray | float],
) -> dict[str, np.ndarray]:
    """Reconstruct Cartesian Cu+ drift-diffusion flux from one saved timestep."""
    x = np.asarray(data["x"], dtype=float)
    z = np.asarray(data["z"], dtype=float)
    connect = np.asarray(data["connect"], dtype=int)
    log_cup = np.asarray(data["log_cup"], dtype=float)
    pressure = np.asarray(data["pressure"], dtype=float)
    density = np.asarray(data["density"], dtype=float)
    ex = np.asarray(data["ex"], dtype=float)
    ez = np.asarray(data["ez"], dtype=float)
    dlogn_dx, dlogn_dz = _element_log_gradient(x, z, connect, log_cup)
    element_pressure = np.mean(pressure[connect], axis=1)
    if np.any(element_pressure <= 0.0):
        raise ValueError("Cu pressure must be positive")
    mobility = (
        REDUCED_MOBILITY
        * REFERENCE_PRESSURE
        / element_pressure
        * ION_TEMPERATURE
        / REFERENCE_TEMPERATURE
    )
    diffusivity = mobility * ION_TEMPERATURE * BOLTZMANN / ELEMENTARY_CHARGE
    drift_x = mobility * ex * density
    drift_z = mobility * ez * density
    diffusion_x = -diffusivity * density * dlogn_dx
    diffusion_z = -diffusivity * density * dlogn_dz
    gamma_x = drift_x + diffusion_x
    gamma_z = drift_z + diffusion_z
    return {
        "x": np.mean(x[connect], axis=1),
        "z": np.mean(z[connect], axis=1),
        "gamma_x": gamma_x,
        "gamma_z": gamma_z,
        "magnitude": np.hypot(gamma_x, gamma_z),
        "gamma_to_wafer": -gamma_z,
        "drift_to_wafer": -drift_z,
        "diffusion_to_wafer": -diffusion_z,
    }


def load_flux_exodus(
    path: Path | str, *, target_time: float | None = None
) -> dict[str, np.ndarray | float]:
    """Load the fields required for Cartesian Cu+ flux reconstruction."""
    from scipy.io import netcdf_file

    exodus_path = Path(path)
    with netcdf_file(exodus_path, "r", mmap=False) as exodus:
        nodal_names = _decode_names(exodus.variables["name_nod_var"].data)
        element_names = _decode_names(exodus.variables["name_elem_var"].data)
        times = np.asarray(exodus.variables["time_whole"][:]).copy()
        step = int(times.size - 1) if target_time is None else int(
            np.argmin(np.abs(times - target_time))
        )

        def nodal(name: str) -> np.ndarray:
            index = nodal_names.index(name) + 1
            return np.asarray(exodus.variables[f"vals_nod_var{index}"][step]).copy()

        def element(name: str) -> np.ndarray:
            index = element_names.index(name) + 1
            return np.asarray(exodus.variables[f"vals_elem_var{index}eb1"][step]).copy()

        return {
            "time": float(times[step]),
            "x": np.asarray(exodus.variables["coordx"][:]).copy(),
            "z": np.asarray(exodus.variables["coordy"][:]).copy(),
            "connect": np.asarray(exodus.variables["connect1"][:]).astype(int) - 1,
            "log_cup": nodal("Cu+"),
            "pressure": nodal("p_Cu_local"),
            "density": element("Cu+_density"),
            "ex": element("EFieldx"),
            "ez": element("EFieldy"),
        }


def _nearest_z_profiles(
    flux: dict[str, np.ndarray],
    z_planes_cm: tuple[float, ...] | list[float],
    *,
    x_range_cm: tuple[float, float] = (0.0, 25.0),
) -> dict[float, dict[str, np.ndarray | float]]:
    x = np.asarray(flux["x"])
    z = np.asarray(flux["z"])
    rows = np.unique(np.round(z, 12))
    profiles: dict[float, dict[str, np.ndarray | float]] = {}
    for requested_cm in z_planes_cm:
        actual = float(rows[np.argmin(np.abs(rows - requested_cm / 100.0))])
        selected = np.isclose(z, actual, atol=1.0e-11, rtol=0.0)
        selected &= x >= x_range_cm[0] / 100.0
        selected &= x <= x_range_cm[1] / 100.0
        indices = np.flatnonzero(selected)
        indices = indices[np.argsort(x[indices])]
        if indices.size == 0:
            raise ValueError(f"no samples near Z={requested_cm:g} cm")
        profiles[float(requested_cm)] = {
            "actual_z": actual,
            "x": x[indices],
            "gamma_to_wafer": np.asarray(flux["gamma_to_wafer"])[indices],
        }
    return profiles


def _uniform_quiver_indices(
    x: np.ndarray,
    z: np.ndarray,
    *,
    max_arrows: int = 600,
) -> np.ndarray:
    """Select Cartesian cell centers uniformly across both plot dimensions."""
    x = np.asarray(x, dtype=float)
    z = np.asarray(z, dtype=float)
    if x.shape != z.shape:
        raise ValueError("x and z coordinates must have matching shapes")
    if max_arrows < 1:
        raise ValueError("max_arrows must be positive")

    finite_indices = np.flatnonzero(np.isfinite(x) & np.isfinite(z))
    if finite_indices.size == 0:
        return np.asarray([], dtype=int)
    unique_x, x_inverse = np.unique(x[finite_indices], return_inverse=True)
    unique_z, z_inverse = np.unique(z[finite_indices], return_inverse=True)

    x_span = float(np.ptp(unique_x))
    z_span = float(np.ptp(unique_z))
    if x_span > 0.0 and z_span > 0.0:
        aspect_ratio = x_span / z_span
    else:
        aspect_ratio = unique_x.size / unique_z.size
    x_count = min(
        unique_x.size,
        max(1, int(round(np.sqrt(max_arrows * aspect_ratio)))),
    )
    z_count = min(unique_z.size, max(1, max_arrows // x_count))
    x_count = min(unique_x.size, max(1, max_arrows // z_count))
    z_count = min(unique_z.size, max(1, max_arrows // x_count))

    def nearest_axis_indices(values: np.ndarray, count: int) -> np.ndarray:
        if count >= values.size:
            return np.arange(values.size)
        targets = np.linspace(values[0], values[-1], count)
        upper = np.clip(np.searchsorted(values, targets), 0, values.size - 1)
        lower = np.maximum(upper - 1, 0)
        choose_lower = np.abs(targets - values[lower]) <= np.abs(values[upper] - targets)
        return np.unique(np.where(choose_lower, lower, upper))

    selected_x = nearest_axis_indices(unique_x, x_count)
    selected_z = nearest_axis_indices(unique_z, z_count)
    index_grid = np.full((unique_z.size, unique_x.size), -1, dtype=int)
    index_grid[z_inverse, x_inverse] = finite_indices
    selected = index_grid[np.ix_(selected_z, selected_x)].ravel()
    return selected[selected >= 0]


def plot_ion_flux(
    exodus_path: Path | str,
    output_path: Path | str,
    *,
    target_time: float | None = None,
    z_planes_cm: tuple[float, ...] = (1.0, 5.0, 10.0),
    wafer_x_cm: tuple[float, float] = (7.0, 17.0),
) -> None:
    """Plot Cu+ flux magnitude, wafer-directed flux, vectors, and profiles."""
    import matplotlib.pyplot as plt
    import matplotlib.tri as mtri
    from matplotlib.colors import LogNorm, SymLogNorm

    data = load_flux_exodus(exodus_path, target_time=target_time)
    flux = reconstruct_ion_flux(data)
    x_cm = np.asarray(flux["x"]) * 100.0
    z_cm = np.asarray(flux["z"]) * 100.0
    magnitude = np.asarray(flux["magnitude"])
    signed = np.asarray(flux["gamma_to_wafer"])
    triangulation = mtri.Triangulation(x_cm, z_cm)
    positive = magnitude[np.isfinite(magnitude) & (magnitude > 0.0)]
    if positive.size == 0:
        raise ValueError("Cu+ flux magnitude has no positive values")
    vmin, vmax = np.percentile(positive, (1.0, 99.0))
    signed_limit = max(float(np.percentile(np.abs(signed), 99.0)), 1.0)
    linear_width = max(signed_limit * 1.0e-3, 1.0)
    profiles = _nearest_z_profiles(
        flux, z_planes_cm, x_range_cm=wafer_x_cm
    )

    figure, axes = plt.subplots(1, 3, figsize=(16.0, 5.2), constrained_layout=True)
    image = axes[0].tripcolor(
        triangulation,
        magnitude,
        shading="flat",
        cmap="viridis",
        norm=LogNorm(vmin=max(float(vmin), 1.0e-300), vmax=float(vmax)),
    )
    figure.colorbar(image, ax=axes[0], label=r"$|\Gamma_{Cu+}|$ [m$^{-2}$ s$^{-1}$]")
    axes[0].set_title("Cu$^+$ flux magnitude")
    quiver_indices = _uniform_quiver_indices(x_cm, z_cm, max_arrows=600)
    safe = np.maximum(magnitude, np.finfo(float).tiny)
    axes[0].quiver(
        x_cm[quiver_indices],
        z_cm[quiver_indices],
        np.asarray(flux["gamma_x"])[quiver_indices] / safe[quiver_indices],
        np.asarray(flux["gamma_z"])[quiver_indices] / safe[quiver_indices],
        color="white",
        scale=35,
        width=0.002,
    )
    signed_image = axes[1].tripcolor(
        triangulation,
        signed,
        shading="flat",
        cmap="coolwarm",
        norm=SymLogNorm(
            linthresh=linear_width,
            vmin=-signed_limit,
            vmax=signed_limit,
        ),
    )
    figure.colorbar(
        signed_image,
        ax=axes[1],
        label=r"$\Gamma_{Cu+\rightarrow wafer}$ [m$^{-2}$ s$^{-1}$]",
    )
    axes[1].set_title("Signed wafer-directed Cu$^+$ flux")
    for requested, profile in profiles.items():
        axes[2].plot(
            np.asarray(profile["x"]) * 100.0,
            np.asarray(profile["gamma_to_wafer"]),
            linewidth=2.0,
            label=f"Z={requested:g} cm",
        )
    axes[2].axhline(0.0, color="black", linewidth=0.8)
    axes[2].set_xlim(*wafer_x_cm)
    axes[2].set_xlabel("X over wafer [cm]")
    axes[2].set_ylabel(r"$\Gamma_{Cu+\rightarrow wafer}$ [m$^{-2}$ s$^{-1}$]")
    axes[2].set_title("Wafer-span profiles")
    axes[2].legend(frameon=False)
    axes[2].grid(alpha=0.25)
    for axis in axes[:2]:
        axis.plot([7.0, 17.0], [0.0, 0.0], color="lime", linewidth=3.0)
        axis.plot([19.0, 22.0], [30.0, 30.0], color="cyan", linewidth=3.0)
        axis.set_xlim(0.0, 25.0)
        axis.set_ylim(0.0, 30.0)
        axis.set_aspect("equal")
        axis.set_xlabel("X [cm]")
        axis.set_ylabel("Z [cm]")
    figure.suptitle(f"Cartesian Cu$^+$ flux at t={data['time']:.6e} s", fontweight="bold")
    destination = Path(output_path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(destination, dpi=200)
    plt.close(figure)


def compare_ion_flux(
    cases: list[tuple[str, Path]],
    output_path: Path | str,
    csv_path: Path | str,
    *,
    target_time: float | None = None,
    z_planes_cm: tuple[float, ...] = (1.0, 5.0, 10.0),
) -> None:
    """Compare signed wafer-directed Cu+ profiles from multiple x-z cases."""
    import matplotlib.pyplot as plt

    loaded: dict[str, tuple[float, dict[float, dict[str, np.ndarray | float]]]] = {}
    for label, path in cases:
        data = load_flux_exodus(path, target_time=target_time)
        flux = reconstruct_ion_flux(data)
        loaded[label] = (
            float(data["time"]),
            _nearest_z_profiles(flux, z_planes_cm),
        )
    figure, axes = plt.subplots(len(z_planes_cm), 1, figsize=(9.0, 3.4 * len(z_planes_cm)), sharex=True, constrained_layout=True)
    axes = np.atleast_1d(axes)
    colors = ("#0072B2", "#D55E00", "#009E73", "#CC79A7")
    for axis, requested in zip(axes, z_planes_cm):
        for color, (label, (time_value, profiles)) in zip(colors, loaded.items()):
            profile = profiles[float(requested)]
            axis.plot(
                np.asarray(profile["x"]) * 100.0,
                np.asarray(profile["gamma_to_wafer"]),
                color=color,
                linewidth=2.0,
                label=f"{label}, t={time_value:.3e} s",
            )
        axis.axhline(0.0, color="black", linewidth=0.8)
        axis.set_ylabel(r"$\Gamma_{Cu+\rightarrow wafer}$")
        axis.set_title(f"Nearest mesh row to Z={requested:g} cm")
        axis.grid(alpha=0.25)
        axis.legend(frameon=False)
    axes[-1].set_xlabel("X [cm]")
    destination = Path(output_path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(destination, dpi=200)
    plt.close(figure)

    csv_destination = Path(csv_path)
    csv_destination.parent.mkdir(parents=True, exist_ok=True)
    with csv_destination.open("w", newline="") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=("case", "time_s", "requested_z_cm", "actual_z_cm", "x_cm", "gamma_to_wafer_m-2_s-1"),
        )
        writer.writeheader()
        for label, (time_value, profiles) in loaded.items():
            for requested, profile in profiles.items():
                for x_value, gamma in zip(profile["x"], profile["gamma_to_wafer"]):
                    writer.writerow(
                        {
                            "case": label,
                            "time_s": time_value,
                            "requested_z_cm": requested,
                            "actual_z_cm": float(profile["actual_z"]) * 100.0,
                            "x_cm": float(x_value) * 100.0,
                            "gamma_to_wafer_m-2_s-1": float(gamma),
                        }
                    )


def _case_argument(value: str) -> tuple[str, Path]:
    if "=" not in value:
        raise argparse.ArgumentTypeError("case must be LABEL=EXODUS_PATH")
    label, path = value.split("=", 1)
    if not label or not path:
        raise argparse.ArgumentTypeError("case must be LABEL=EXODUS_PATH")
    return label, Path(path)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    diagnostics = subparsers.add_parser("diagnostics", help="Plot plasma-field diagnostics")
    diagnostics.add_argument("--exodus", type=Path, required=True)
    diagnostics.add_argument("--output", type=Path, required=True)
    diagnostics.add_argument("--timestep", type=int)
    diagnostics.add_argument("--time", type=float)
    diagnostics.add_argument("--bulk-margin-cm", type=float, default=0.0)
    diagnostics.add_argument("--density-growth", action="store_true")
    diagnostics.add_argument("--density-contrast", action="store_true")
    diagnostics.add_argument("--density-style", choices=("default", "chamber"), default="default")
    diagnostics.add_argument("--density-percentiles", type=float, nargs=2, default=CHAMBER_DENSITY_PERCENTILES)

    ion_flux = subparsers.add_parser("ion-flux", help="Plot one case's Cu+ flux")
    ion_flux.add_argument("--exodus", type=Path, required=True)
    ion_flux.add_argument("--output", type=Path, required=True)
    ion_flux.add_argument("--time", type=float)
    ion_flux.add_argument("--z-cm", type=float, nargs="+", default=(1.0, 5.0, 10.0))
    ion_flux.add_argument("--wafer-x-cm", type=float, nargs=2, default=(7.0, 17.0))

    comparison = subparsers.add_parser("compare-ion-flux", help="Compare Cu+ flux cases")
    comparison.add_argument("--case", action="append", type=_case_argument, required=True)
    comparison.add_argument("--output", type=Path, required=True)
    comparison.add_argument("--csv", type=Path, required=True)
    comparison.add_argument("--time", type=float)
    comparison.add_argument("--z-cm", type=float, nargs="+", default=(1.0, 5.0, 10.0))
    return parser


def main() -> None:
    args = build_parser().parse_args()
    if args.command == "diagnostics":
        plot_last_timestep(
            args.exodus,
            args.output,
            timestep=args.timestep,
            time=args.time,
            bulk_margin_cm=args.bulk_margin_cm,
            density_growth=args.density_growth,
            density_contrast=args.density_contrast,
            density_style=args.density_style,
            density_percentiles=tuple(args.density_percentiles),
            xlabel="X [cm]",
            ylabel="Z [cm]",
        )
    elif args.command == "ion-flux":
        plot_ion_flux(
            args.exodus,
            args.output,
            target_time=args.time,
            z_planes_cm=tuple(args.z_cm),
            wafer_x_cm=tuple(args.wafer_x_cm),
        )
        print(f"wrote {args.output}")
    else:
        compare_ion_flux(
            args.case,
            args.output,
            args.csv,
            target_time=args.time,
            z_planes_cm=tuple(args.z_cm),
        )
        print(f"wrote {args.output}")
        print(f"wrote {args.csv}")


if __name__ == "__main__":
    main()
