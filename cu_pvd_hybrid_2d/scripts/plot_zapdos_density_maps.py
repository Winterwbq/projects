from __future__ import annotations

import argparse
from pathlib import Path
from typing import NamedTuple

import matplotlib.pyplot as plt
import matplotlib.tri as mtri
import numpy as np
from matplotlib.colors import LogNorm, Normalize
from scipy.io import netcdf_file


DEFAULT_EXODUS = (
    Path(__file__).resolve().parents[1]
    / "zapdos_templates"
    / "cu_pvd_hybrid_template_exodus.e"
)
DEFAULT_OUTPUT = (
    Path(__file__).resolve().parents[1]
    / "post"
    / "zapdos_electron_cuion_density_maps.png"
)
ION_DENSITY_LOG_RANGE = (1.0e13, 1.0e16)
CHAMBER_DENSITY_PERCENTILES = (2.0, 98.0)


class DensityFieldSpec(NamedTuple):
    title: str
    values: np.ndarray
    cmap: str
    norm: Normalize | LogNorm | None


def _decode_names(raw: np.ndarray) -> list[str]:
    names: list[str] = []
    for row in raw:
        text = (
            b"".join(np.asarray(row).astype("S1"))
            .decode("ascii", errors="ignore")
            .replace("\x00", "")
            .strip()
        )
        names.append(text)
    return names


def _pick_representative_steps(times: np.ndarray) -> list[int]:
    if len(times) <= 3:
        return list(range(len(times)))

    nonzero = [i for i, t in enumerate(times) if t > 0.0]
    if len(nonzero) >= 3:
        return [nonzero[0], nonzero[len(nonzero) // 2], nonzero[-1]]

    return [0, len(times) // 2, len(times) - 1]


def _load_exodus(path: Path) -> dict[str, np.ndarray | list[str]]:
    with netcdf_file(path, "r", mmap=False) as exo:
        elem_names = _decode_names(np.asarray(exo.variables["name_elem_var"].data))

        def elem_var(name: str) -> np.ndarray:
            index = elem_names.index(name) + 1
            return np.asarray(exo.variables[f"vals_elem_var{index}eb1"].data).copy()

        data = {
            "times": np.asarray(exo.variables["time_whole"].data).copy(),
            "x": np.asarray(exo.variables["coordx"].data).copy(),
            "y": np.asarray(exo.variables["coordy"].data).copy(),
            "connect": np.asarray(exo.variables["connect1"].data).copy().astype(int) - 1,
            "em_density": elem_var("em_density"),
            "cu_ion_density": elem_var("Cu+_density"),
            "elem_names": elem_names,
        }
        if "Ar+_density" in elem_names:
            data["ar_ion_density"] = elem_var("Ar+_density")
        return data


def _linear_limits(values: np.ndarray, steps: list[int]) -> tuple[float, float]:
    selected = np.asarray(values)[steps].ravel()
    selected = selected[np.isfinite(selected)]
    if selected.size == 0:
        return 0.0, 1.0
    vmin = float(selected.min())
    vmax = float(selected.max())
    if vmin >= vmax:
        pad = max(abs(vmin) * 0.01, 1.0)
        return vmin - pad, vmax + pad
    return vmin, vmax


def _percentile_limits(
    values: np.ndarray,
    steps: list[int],
    percentiles: tuple[float, float] = CHAMBER_DENSITY_PERCENTILES,
) -> tuple[float, float]:
    selected = np.asarray(values)[steps].ravel()
    selected = selected[np.isfinite(selected)]
    selected = selected[selected > 0.0]
    if selected.size == 0:
        return 0.0, 1.0
    vmin, vmax = np.percentile(selected, percentiles)
    vmin = float(vmin)
    vmax = float(vmax)
    if vmin >= vmax:
        pad = max(abs(vmin) * 0.01, 1.0)
        return vmin - pad, vmax + pad
    return vmin, vmax


def _density_limits(values: np.ndarray, steps: list[int], *, style: str) -> tuple[float, float]:
    if style == "chamber":
        return _percentile_limits(values, steps)
    return _linear_limits(values, steps)


def _density_norm(limits: tuple[float, float]) -> Normalize:
    return Normalize(vmin=limits[0], vmax=limits[1])


def _ion_density_log_norm() -> LogNorm:
    return LogNorm(vmin=ION_DENSITY_LOG_RANGE[0], vmax=ION_DENSITY_LOG_RANGE[1])


def _density_field_specs(data: dict[str, np.ndarray | list[str]], *, style: str) -> list[DensityFieldSpec]:
    if style == "chamber":
        fields = [
            DensityFieldSpec("Electron density $n_e$ [m$^{-3}$]", np.asarray(data["em_density"]), "turbo", None),
            DensityFieldSpec("Cu$^+$ density [m$^{-3}$]", np.asarray(data["cu_ion_density"]), "turbo", None),
        ]
        if "ar_ion_density" in data:
            fields.append(
                DensityFieldSpec(
                    "Ar$^+$ density [m$^{-3}$]",
                    np.asarray(data["ar_ion_density"]),
                    "turbo",
                    None,
                )
            )
        return fields

    fields = [
        DensityFieldSpec("Electron density $n_e$ [m$^{-3}$]", np.asarray(data["em_density"]), "viridis", None),
        DensityFieldSpec("Cu$^+$ density [m$^{-3}$]", np.asarray(data["cu_ion_density"]), "magma", _ion_density_log_norm()),
    ]
    if "ar_ion_density" in data:
        fields.append(
            DensityFieldSpec(
                "Ar$^+$ density [m$^{-3}$]",
                np.asarray(data["ar_ion_density"]),
                "cividis",
                _ion_density_log_norm(),
            )
        )
    return fields


def _plot_density_maps(exodus_path: Path, output_path: Path, *, density_style: str = "default") -> None:
    data = _load_exodus(exodus_path)
    times = np.asarray(data["times"])
    steps = _pick_representative_steps(times)

    x_cm = np.asarray(data["x"]) * 100.0
    y_cm = np.asarray(data["y"]) * 100.0
    connect = np.asarray(data["connect"])
    triangles = np.vstack((connect[:, [0, 1, 2]], connect[:, [0, 2, 3]]))
    triangulation = mtri.Triangulation(x_cm, y_cm, triangles)

    fields = _density_field_specs(data, style=density_style)
    limits = [
        _density_limits(spec.values, steps, style=density_style) if spec.norm is None else None
        for spec in fields
    ]

    fig, axes = plt.subplots(
        len(steps),
        len(fields),
        figsize=(5.4 * len(fields), 3.3 * len(steps)),
        sharex=True,
        sharey=True,
        constrained_layout=True,
    )
    if len(steps) == 1:
        axes = np.asarray([axes])
    if len(fields) == 1:
        axes = axes[:, np.newaxis]

    mappables = []
    for col, (spec, limits_or_none) in enumerate(zip(fields, limits)):
        if spec.norm is None:
            assert limits_or_none is not None
            vmin, vmax = limits_or_none
            norm = _density_norm((vmin, vmax))
        else:
            norm = spec.norm
        last = None
        for row, step in enumerate(steps):
            ax = axes[row, col]
            face_values = np.concatenate((spec.values[step], spec.values[step]))
            if isinstance(norm, LogNorm):
                face_values = np.maximum(face_values, norm.vmin)
            last = ax.tripcolor(
                triangulation,
                facecolors=face_values,
                shading="flat",
                cmap=spec.cmap,
                norm=norm,
                rasterized=True,
            )
            ax.set_aspect("equal")
            ax.set_title(f"{spec.title}\nt = {times[step]:.3e} s")
            ax.set_ylabel("y [cm]")
            ax.axhline(0.0, color="white", linewidth=0.9, alpha=0.9)
            ax.axhline(60.0, color="white", linewidth=0.7, alpha=0.55)
        assert last is not None
        mappables.append(last)

    for ax in axes[-1, :]:
        ax.set_xlabel("x [cm]")

    for col, mappable in enumerate(mappables):
        fig.colorbar(mappable, ax=axes[:, col], pad=0.015, shrink=0.92)

    fig.suptitle(
        f"Zapdos density maps from {exodus_path.name} ({density_style})",
        fontsize=14,
        fontweight="bold",
    )
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=220)
    plt.close(fig)

    print(f"Wrote {output_path}")
    print("Selected time steps:")
    for step in steps:
        ne = np.asarray(data["em_density"])[step]
        ni = np.asarray(data["cu_ion_density"])[step]
        ar_text = ""
        if "ar_ion_density" in data:
            nar = np.asarray(data["ar_ion_density"])[step]
            ar_text = f", Ar+=[{nar.min():.3e}, {nar.max():.3e}]"
        print(
            f"  step {step:>3d}, t={times[step]:.6e} s, "
            f"ne=[{ne.min():.3e}, {ne.max():.3e}], "
            f"Cu+=[{ni.min():.3e}, {ni.max():.3e}]"
            f"{ar_text}"
        )


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Plot representative 2D electron and Cu+ density maps from Zapdos Exodus output."
    )
    parser.add_argument("--exodus", type=Path, default=DEFAULT_EXODUS)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument(
        "--density-style",
        choices=("default", "chamber"),
        default="default",
        help=(
            "Color scaling for density maps. 'default' preserves existing mixed linear/log behavior; "
            "'chamber' uses linear 2-98 percentile clipping and a blue-green-yellow-red colormap."
        ),
    )
    args = parser.parse_args()
    _plot_density_maps(args.exodus, args.output, density_style=args.density_style)


if __name__ == "__main__":
    main()
