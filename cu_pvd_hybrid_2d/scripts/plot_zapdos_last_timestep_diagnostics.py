from __future__ import annotations

import argparse
from pathlib import Path
from typing import TYPE_CHECKING

import numpy as np

if TYPE_CHECKING:
    import matplotlib.pyplot as plt
    import matplotlib.tri as mtri


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_EXODUS = ROOT / "zapdos_templates" / "cu_pvd_hybrid_effective_source_30cm_exodus.e"
DEFAULT_OUTPUT = ROOT / "post" / "zapdos_last_timestep_diagnostics_30cm_9.png"
ION_DENSITY_LOG_RANGE = (1e12 , 1e16)
CHAMBER_DENSITY_PERCENTILES = (2.0, 98.0)
DENSITY_GROWTH_PERCENTILE = 98.0


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


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Plot 2D diagnostics at a saved timestep in a Zapdos Exodus file."
    )
    parser.add_argument("--exodus", type=Path, default=DEFAULT_EXODUS)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument(
        "--timestep",
        type=int,
        default=None,
        help="Saved Exodus timestep index to plot. Defaults to the last saved step. Negative indices count from the end.",
    )
    parser.add_argument(
        "--time",
        type=float,
        default=None,
        help="Physical time in seconds; plot the nearest saved Exodus timestep.",
    )
    parser.add_argument(
        "--bulk-margin-cm",
        type=float,
        default=0,
        help="Mask out elements/nodes this close to the bottom and top boundaries.",
    )
    parser.add_argument(
        "--density-growth",
        action="store_true",
        help=(
            "Plot signed electron/Cu+ density change relative to the first populated saved "
            "timestep using a shared linear, zero-centered color scale."
        ),
    )
    parser.add_argument(
        "--density-contrast",
        action="store_true",
        help="Plot electron/Cu+ density as log10(density / bulk median) to reveal weak bulk variation.",
    )
    parser.add_argument(
        "--density-style",
        choices=("default", "chamber"),
        default="default",
        help=(
            "Density color scaling. 'default' keeps log density panels unless --density-contrast "
            "is set; 'chamber' uses linear 2-98 percentile clipping with the turbo colormap."
        ),
    )
    parser.add_argument(
        "--density-percentiles",
        type=float,
        nargs=2,
        metavar=("LOW", "HIGH"),
        default=CHAMBER_DENSITY_PERCENTILES,
        help=(
            "Percentile range for --density-style chamber. Lower HIGH values saturate the source "
            "hotspot and reveal more chamber/bulk variation, e.g. '--density-percentiles 5 90'."
        ),
    )
    parser.add_argument("--xlabel", default="x [cm]")
    parser.add_argument("--ylabel", default="y [cm]")
    args = parser.parse_args()
    try:
        _validate_density_display_mode(
            density_growth=args.density_growth,
            density_contrast=args.density_contrast,
            density_style=args.density_style,
        )
    except ValueError as error:
        parser.error(str(error))
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
        xlabel=args.xlabel,
        ylabel=args.ylabel,
    )


if __name__ == "__main__":
    main()
