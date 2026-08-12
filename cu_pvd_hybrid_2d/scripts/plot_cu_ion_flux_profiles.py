#!/usr/bin/env python3
"""Plot final-timestep Cu+ flux toward the wafer on selected axial planes."""
from __future__ import annotations

import argparse
import csv
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE_B = (
    ROOT
    / "zapdos_templates"
    / "Outputs"
    / "cu_pvd_hybrid_hpem_rz_30cm_newsourceB_guidedSEE_1.e"
)
DEFAULT_FOUR_COIL = (
    ROOT
    / "zapdos_templates"
    / "Outputs"
    / "cu_pvd_hybrid_hpem_rz_30cm_4coilsB1_guidedSEE_1.e"
)
DEFAULT_IMAGE_FOUR_COIL = (
    ROOT
    / "zapdos_templates"
    / "Outputs"
    / "cu_pvd_hybrid_hpem_rz_30cm_4coilsB2_guidedSEE_1.e"
)
DEFAULT_OUTPUT = ROOT / "post" / "cu_ion_flux_profiles_three_b_1ms_2.png"
DEFAULT_CSV = ROOT / "post" / "cu_ion_flux_profiles_three_b_1ms_2.csv"
DEFAULT_MAP_OUTPUT = ROOT / "post" / "cu_ion_flux_2d_map_three_b_1ms_2.png"

AVOGADRO = 6.02214076e23
BOLTZMANN = 1.3807e-23
ELEMENTARY_CHARGE = 1.602e-19
ION_TEMPERATURE = 300.0
REDUCED_MOBILITY = 2.2e-4
REFERENCE_PRESSURE = 101325.0
REFERENCE_TEMPERATURE = 273.15


def case_styles(number_of_cases: int) -> list[dict[str, str | float]]:
    """Return stable, distinguishable styles for up to three comparison cases."""
    styles: list[dict[str, str | float]] = [
        {"color": "#0072B2", "linewidth": 2.0, "linestyle": "-"},
        {"color": "#D55E00", "linewidth": 2.0, "linestyle": "--"},
        {"color": "#009E73", "linewidth": 2.0, "linestyle": "-."},
    ]
    if number_of_cases < 1 or number_of_cases > len(styles):
        raise ValueError(f"comparison supports 1..{len(styles)} cases")
    return styles[:number_of_cases]


def signed_flux_colorbar_ticks(limit: float) -> np.ndarray:
    """Return symmetric decade ticks without crowding the linear region."""
    if not np.isfinite(limit) or limit <= 0.0:
        raise ValueError("signed flux colorbar limit must be positive and finite")
    outer_tick = 10.0 ** np.floor(np.log10(limit))
    inner_tick = outer_tick / 10.0
    return np.asarray([-outer_tick, -inner_tick, 0.0, inner_tick, outer_tick])


def _decode_names(raw: np.ndarray) -> list[str]:
    return [
        b"".join(np.asarray(row).astype("S1"))
        .decode("ascii", errors="ignore")
        .replace("\x00", "")
        .strip()
        for row in raw
    ]


def select_saved_timestep(times: np.ndarray, target_time: float | None) -> int:
    """Return the final saved step or the step nearest a requested physical time."""
    times = np.asarray(times)
    if times.size == 0:
        raise ValueError("Exodus file contains no saved timesteps.")
    if target_time is None:
        return int(times.size - 1)
    return int(np.argmin(np.abs(times - float(target_time))))


def load_final_exodus(
    path: Path, target_time: float | None = None
) -> dict[str, np.ndarray | float]:
    """Read the mesh and selected fields needed for the flux reconstruction."""
    from scipy.io import netcdf_file

    with netcdf_file(path, "r", mmap=True) as exo:
        nodal_names = _decode_names(exo.variables["name_nod_var"].data)
        element_names = _decode_names(exo.variables["name_elem_var"].data)
        times = np.asarray(exo.variables["time_whole"][:]).copy()
        step = select_saved_timestep(times, target_time)

        def nodal(name: str) -> np.ndarray:
            index = nodal_names.index(name) + 1
            return np.asarray(exo.variables[f"vals_nod_var{index}"][step]).copy()

        def element(name: str) -> np.ndarray:
            index = element_names.index(name) + 1
            return np.asarray(exo.variables[f"vals_elem_var{index}eb1"][step]).copy()

        return {
            "time": float(times[step]),
            "x": np.asarray(exo.variables["coordx"][:]).copy(),
            "z": np.asarray(exo.variables["coordy"][:]).copy(),
            "connect": np.asarray(exo.variables["connect1"][:]).astype(int) - 1,
            "log_cup": nodal("Cu+"),
            "pressure": nodal("p_Cu_local"),
            "density": element("Cu+_density"),
            "er": element("EFieldx"),
            "ez": element("EFieldy"),
        }


def _element_log_gradient(
    x: np.ndarray,
    z: np.ndarray,
    connect: np.ndarray,
    log_density: np.ndarray,
) -> tuple[np.ndarray, np.ndarray]:
    """Return the QUAD4 log-density gradient at each element center."""
    if connect.ndim != 2 or connect.shape[1] != 4:
        raise ValueError("Cu+ flux reconstruction currently requires QUAD4 elements.")

    xe = x[connect]
    ze = z[connect]
    ue = log_density[connect]
    dshape_dxi = np.array([-0.25, 0.25, 0.25, -0.25])
    dshape_deta = np.array([-0.25, -0.25, 0.25, 0.25])

    # Explicit reductions avoid spurious floating-point warnings from the
    # large-matrix matmul path in NumPy 2.2.x.
    dx_dxi = np.sum(xe * dshape_dxi, axis=1)
    dz_dxi = np.sum(ze * dshape_dxi, axis=1)
    dx_deta = np.sum(xe * dshape_deta, axis=1)
    dz_deta = np.sum(ze * dshape_deta, axis=1)
    du_dxi = np.sum(ue * dshape_dxi, axis=1)
    du_deta = np.sum(ue * dshape_deta, axis=1)
    determinant = dx_dxi * dz_deta - dz_dxi * dx_deta
    if np.any(np.isclose(determinant, 0.0)):
        raise ValueError("Mesh contains a degenerate QUAD4 element.")

    dlogn_dr = (dz_deta * du_dxi - dz_dxi * du_deta) / determinant
    dlogn_dz = (-dx_deta * du_dxi + dx_dxi * du_deta) / determinant
    return dlogn_dr, dlogn_dz


def reconstruct_ion_flux(
    data: dict[str, np.ndarray | float],
) -> dict[str, np.ndarray]:
    """Reconstruct the radial and axial Cu+ drift-diffusion flux."""
    x = np.asarray(data["x"])
    z = np.asarray(data["z"])
    connect = np.asarray(data["connect"])
    log_cup = np.asarray(data["log_cup"])
    pressure = np.asarray(data["pressure"])
    density = np.asarray(data["density"])
    er = np.asarray(data["er"])
    ez = np.asarray(data["ez"])

    dlogn_dr, dlogn_dz = _element_log_gradient(x, z, connect, log_cup)
    element_pressure = np.mean(pressure[connect], axis=1)
    mobility = (
        REDUCED_MOBILITY
        * REFERENCE_PRESSURE
        / element_pressure
        * ION_TEMPERATURE
        / REFERENCE_TEMPERATURE
    )
    diffusivity = mobility * ION_TEMPERATURE * BOLTZMANN / ELEMENTARY_CHARGE

    drift_r = mobility * er * density
    drift_z = mobility * ez * density
    diffusion_r = -diffusivity * density * dlogn_dr
    diffusion_z = -diffusivity * density * dlogn_dz
    gamma_r = drift_r + diffusion_r
    gamma_z = drift_z + diffusion_z
    return {
        "r": np.mean(x[connect], axis=1),
        "z": np.mean(z[connect], axis=1),
        "gamma_r": gamma_r,
        "gamma_z": gamma_z,
        "magnitude": np.hypot(gamma_r, gamma_z),
        "gamma_to_wafer": -gamma_z,
        "drift_to_wafer": -drift_z,
        "diffusion_to_wafer": -diffusion_z,
    }


def reconstruct_axial_flux(
    data: dict[str, np.ndarray | float],
) -> dict[str, np.ndarray]:
    """Backward-compatible alias for the full ion-flux reconstruction."""
    return reconstruct_ion_flux(data)


def select_plane_profile(
    flux: dict[str, np.ndarray], requested_z: float
) -> dict[str, np.ndarray | float]:
    """Select the mesh row nearest requested_z and sort it radially."""
    z = np.asarray(flux["z"])
    axial_rows = np.unique(np.round(z, 12))
    actual_z = float(axial_rows[np.argmin(np.abs(axial_rows - requested_z))])
    mask = np.isclose(z, actual_z, atol=1.0e-11, rtol=0.0)
    order = np.argsort(np.asarray(flux["r"])[mask])

    profile: dict[str, np.ndarray | float] = {"actual_z": actual_z}
    for name, values in flux.items():
        profile[name] = np.asarray(values)[mask][order]
    return profile


def _write_csv(
    path: Path,
    profiles: dict[str, dict[float, dict[str, np.ndarray | float]]],
    times: dict[str, float],
) -> None:
    fieldnames = [
        "case",
        "time_s",
        "requested_z_cm",
        "actual_z_cm",
        "r_cm",
        "gamma_to_wafer_m-2_s-1",
        "drift_to_wafer_m-2_s-1",
        "diffusion_to_wafer_m-2_s-1",
    ]
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        for label, case_profiles in profiles.items():
            for requested_z_cm, profile in case_profiles.items():
                for radius, total, drift, diffusion in zip(
                    np.asarray(profile["r"]),
                    np.asarray(profile["gamma_to_wafer"]),
                    np.asarray(profile["drift_to_wafer"]),
                    np.asarray(profile["diffusion_to_wafer"]),
                ):
                    writer.writerow(
                        {
                            "case": label,
                            "time_s": times[label],
                            "requested_z_cm": requested_z_cm,
                            "actual_z_cm": float(profile["actual_z"]) * 100.0,
                            "r_cm": radius * 100.0,
                            "gamma_to_wafer_m-2_s-1": total,
                            "drift_to_wafer_m-2_s-1": drift,
                            "diffusion_to_wafer_m-2_s-1": diffusion,
                        }
                    )


def _plot_profiles(
    path: Path,
    profiles: dict[str, dict[float, dict[str, np.ndarray | float]]],
    planes_cm: list[float],
    times: dict[str, float],
) -> None:
    import matplotlib.pyplot as plt

    figure, axes = plt.subplots(
        1, len(planes_cm), figsize=(4.7 * len(planes_cm), 4.4), sharey=True, constrained_layout=True
    )
    axes = np.atleast_1d(axes)
    styles = case_styles(len(profiles))
    for axis, requested_z_cm in zip(axes, planes_cm):
        for (label, case_profiles), style in zip(profiles.items(), styles):
            profile = case_profiles[requested_z_cm]
            axis.plot(
                np.asarray(profile["r"]) * 100.0,
                np.asarray(profile["gamma_to_wafer"]),
                label=rf"{label} ($t={times[label] * 1.0e6:.1f}\ \mu$s)",
                **style,
            )
        first_profile = next(iter(profiles.values()))[requested_z_cm]
        axis.axhline(0.0, color="0.35", linewidth=0.8)
        axis.set_title(f"z = {float(first_profile['actual_z']) * 100.0:.3f} cm")
        axis.set_xlabel("Radius r [cm]")
        axis.grid(True, alpha=0.22)
        axis.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))
    axes[0].set_ylabel(r"Cu$^+$ flux toward wafer $-\Gamma_z$ [m$^{-2}$ s$^{-1}$]")
    axes[0].legend(frameon=False, loc="best")
    time_values = np.asarray(list(times.values()))
    if np.ptp(time_values) <= 1.0e-12:
        title = rf"Signed axial Cu$^+$ flux at $t={time_values[0] * 1.0e6:g}\ \mu$s"
    else:
        title = r"Signed axial Cu$^+$ flux at each case's selected time"
    figure.suptitle(title)
    path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(path, dpi=180)
    plt.close(figure)


def _plot_flux_maps(
    path: Path,
    cases: dict[str, tuple[dict[str, np.ndarray | float], dict[str, np.ndarray]]],
) -> None:
    import matplotlib.pyplot as plt
    import matplotlib.tri as mtri
    from matplotlib.colors import LogNorm, SymLogNorm

    magnitudes = np.concatenate(
        [np.asarray(flux["magnitude"]) for _, flux in cases.values()]
    )
    positive = magnitudes[np.isfinite(magnitudes) & (magnitudes > 0.0)]
    magnitude_min, magnitude_max = np.percentile(positive, (1.0, 99.0))
    magnitude_norm = LogNorm(vmin=float(magnitude_min), vmax=float(magnitude_max))

    toward_values = np.concatenate(
        [np.asarray(flux["gamma_to_wafer"]) for _, flux in cases.values()]
    )
    toward_limit = float(np.percentile(np.abs(toward_values[np.isfinite(toward_values)]), 99.0))
    toward_norm = SymLogNorm(
        linthresh=max(toward_limit * 0.01, 1.0),
        linscale=1.0,
        vmin=-toward_limit,
        vmax=toward_limit,
    )

    figure, axes = plt.subplots(
        2, len(cases), figsize=(5.5 * len(cases), 9.0), constrained_layout=True
    )
    axes = np.asarray(axes).reshape(2, len(cases))
    magnitude_plot = None
    toward_plot = None
    for column, (label, (data, flux)) in enumerate(cases.items()):
        x_cm = np.asarray(data["x"]) * 100.0
        z_cm = np.asarray(data["z"]) * 100.0
        connect = np.asarray(data["connect"])
        triangles = np.vstack((connect[:, [0, 1, 2]], connect[:, [0, 2, 3]]))
        triangulation = mtri.Triangulation(x_cm, z_cm, triangles)

        magnitude_plot = axes[0, column].tripcolor(
            triangulation,
            facecolors=np.concatenate(
                (np.asarray(flux["magnitude"]), np.asarray(flux["magnitude"]))
            ),
            shading="flat",
            cmap="viridis",
            norm=magnitude_norm,
            rasterized=True,
        )
        toward_plot = axes[1, column].tripcolor(
            triangulation,
            facecolors=np.concatenate(
                (
                    np.asarray(flux["gamma_to_wafer"]),
                    np.asarray(flux["gamma_to_wafer"]),
                )
            ),
            shading="flat",
            cmap="coolwarm",
            norm=toward_norm,
            rasterized=True,
        )

        center_r = np.asarray(flux["r"])
        center_z = np.asarray(flux["z"])
        radial_rows = np.unique(np.round(center_r, 12))
        axial_rows = np.unique(np.round(center_z, 12))
        radial_index = np.searchsorted(radial_rows, np.round(center_r, 12))
        axial_index = np.searchsorted(axial_rows, np.round(center_z, 12))
        radial_stride = max(1, len(radial_rows) // 18)
        axial_stride = max(1, len(axial_rows) // 18)
        arrow_mask = (
            (radial_index % radial_stride == radial_stride // 2)
            & (axial_index % axial_stride == axial_stride // 2)
        )
        flux_magnitude = np.maximum(np.asarray(flux["magnitude"]), 1.0e-300)
        direction_r = np.asarray(flux["gamma_r"]) / flux_magnitude
        direction_z = np.asarray(flux["gamma_z"]) / flux_magnitude
        for row, color in ((0, "white"), (1, "0.15")):
            axes[row, column].quiver(
                center_r[arrow_mask] * 100.0,
                center_z[arrow_mask] * 100.0,
                direction_r[arrow_mask],
                direction_z[arrow_mask],
                color=color,
                angles="xy",
                scale_units="xy",
                scale=1.1,
                width=0.003,
                alpha=0.85,
                pivot="middle",
            )

        axes[0, column].set_title(
            rf"{label}" + "\n" + rf"$t={float(data['time']) * 1.0e6:.1f}\ \mu$s"
        )
        for row in range(2):
            axes[row, column].set_aspect("equal")
            axes[row, column].set_xlim(float(np.min(x_cm)), float(np.max(x_cm)))
            axes[row, column].set_ylim(float(np.min(z_cm)), float(np.max(z_cm)))
            axes[row, column].set_xlabel("R [cm]")
        axes[0, column].set_ylabel("Z [cm]")
        axes[1, column].set_ylabel("Z [cm]")

    assert magnitude_plot is not None
    assert toward_plot is not None
    magnitude_colorbar = figure.colorbar(
        magnitude_plot, ax=axes[0, :].tolist(), shrink=0.92, pad=0.02
    )
    magnitude_colorbar.set_label(r"$|\boldsymbol{\Gamma}_{Cu^+}|$ [m$^{-2}$ s$^{-1}$]")
    toward_colorbar = figure.colorbar(
        toward_plot, ax=axes[1, :].tolist(), shrink=0.92, pad=0.02
    )
    toward_colorbar.set_label(
        r"$-\Gamma_z$ [m$^{-2}$ s$^{-1}$] (positive toward wafer)"
    )
    toward_colorbar.set_ticks(signed_flux_colorbar_ticks(toward_limit))
    figure.suptitle(r"Cu$^+$ flux at selected times; arrows show direction")
    path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(path, dpi=180)
    plt.close(figure)


def plot_flux_profiles(
    cases: dict[str, Path],
    planes_cm: list[float],
    output_path: Path,
    csv_path: Path,
    map_output_path: Path,
    comparison_time: float | None = None,
) -> None:
    profiles: dict[str, dict[float, dict[str, np.ndarray | float]]] = {}
    times: dict[str, float] = {}
    map_cases: dict[
        str, tuple[dict[str, np.ndarray | float], dict[str, np.ndarray]]
    ] = {}
    for label, exodus_path in cases.items():
        data = load_final_exodus(exodus_path, comparison_time)
        times[label] = float(data["time"])
        flux = reconstruct_ion_flux(data)
        map_cases[label] = (data, flux)
        profiles[label] = {
            plane_cm: select_plane_profile(flux, plane_cm / 100.0) for plane_cm in planes_cm
        }

    _write_csv(csv_path, profiles, times)
    _plot_profiles(output_path, profiles, planes_cm, times)
    _plot_flux_maps(map_output_path, map_cases)

    print(f"Wrote {output_path}")
    print(f"Wrote {csv_path}")
    print(f"Wrote {map_output_path}")
    for plane_cm in planes_cm:
        print(f"z = {plane_cm:g} cm")
        for label, case_profiles in profiles.items():
            profile = case_profiles[plane_cm]
            radius = np.asarray(profile["r"])
            flux = np.asarray(profile["gamma_to_wafer"])
            rate = 2.0 * np.pi * np.trapezoid(flux * radius, radius)
            print(
                f"  {label}: actual z = {float(profile['actual_z']) * 100.0:.6f} cm, "
                f"integrated signed rate = {rate:.6e} s^-1"
            )


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Plot final-timestep axial Cu+ flux toward the wafer."
    )
    parser.add_argument("--source-b", type=Path, default=DEFAULT_SOURCE_B)
    parser.add_argument("--four-coil", type=Path, default=DEFAULT_FOUR_COIL)
    parser.add_argument(
        "--image-four-coil",
        type=Path,
        default=DEFAULT_IMAGE_FOUR_COIL,
    )
    parser.add_argument("--z-cm", type=float, nargs="+", default=[1.0, 5.0, 10.0])
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--csv", type=Path, default=DEFAULT_CSV)
    parser.add_argument("--map-output", type=Path, default=DEFAULT_MAP_OUTPUT)
    parser.add_argument(
        "--time",
        type=float,
        default=None,
        help="Physical time in seconds; use the nearest saved timestep in both files.",
    )
    args = parser.parse_args()

    plot_flux_profiles(
        {
            "Source B only": args.source_b,
            "Original four-coil B": args.four_coil,
            "Image-3092 four-coil B": args.image_four_coil,
        },
        args.z_cm,
        args.output,
        args.csv,
        args.map_output,
        args.time,
    )


if __name__ == "__main__":
    main()
