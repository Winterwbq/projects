#!/usr/bin/env python3
"""Compare signed downward Cu+ flux profiles for three Cartesian B-map cases."""
from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from plot_cu_ion_flux_profiles import (  # noqa: E402
    case_styles,
    load_final_exodus,
    reconstruct_ion_flux,
)


OUTPUT_DIR = ROOT / "zapdos_templates" / "Outputs"
DEFAULT_CASES = {
    "Source B only": (
        OUTPUT_DIR / "cu_pvd_hybrid_hpem_xz_30cm_newsourceB_guidedSEE_1.e"
    ),
    "Four-coil B": (
        OUTPUT_DIR / "cu_pvd_hybrid_hpem_xz_30cm_4coilsB_guidedSEE_1.e"
    ),
    "IMG-3092 four-coil B": (
        OUTPUT_DIR / "cu_pvd_hybrid_hpem_xz_30cm_4coilsBimg3092_guidedSEE_1.e"
    ),
}
DEFAULT_OUTPUT = ROOT / "post" / "cartesian_xz_cu_ion_flux_comparison_1ms.png"
DEFAULT_CSV = ROOT / "post" / "cartesian_xz_cu_ion_flux_comparison_1ms.csv"
DEFAULT_Z_PLANES_CM = (1.0, 5.0, 10.0)
DEFAULT_X_RANGE_CM = (0.0, 25.0)


def select_x_profiles(
    flux: dict[str, np.ndarray],
    z_planes_cm: list[float] | tuple[float, ...],
    x_range_cm: tuple[float, float] = DEFAULT_X_RANGE_CM,
) -> dict[float, dict[str, np.ndarray | float]]:
    """Select the nearest mesh row at each requested Z over an X interval."""
    x = np.asarray(flux["r"], dtype=float)
    z = np.asarray(flux["z"], dtype=float)
    if x.ndim != 1 or z.shape != x.shape or x.size == 0:
        raise ValueError("flux X and Z coordinates must be nonempty matching arrays")

    xmin_cm, xmax_cm = map(float, x_range_cm)
    if not np.isfinite([xmin_cm, xmax_cm]).all() or xmin_cm >= xmax_cm:
        raise ValueError("x_range_cm must be a finite increasing pair")
    requested_planes = [float(value) for value in z_planes_cm]
    if not requested_planes or not np.isfinite(requested_planes).all():
        raise ValueError("z_planes_cm must contain finite values")

    axial_rows = np.unique(np.round(z, 12))
    profiles: dict[float, dict[str, np.ndarray | float]] = {}
    for requested_cm in requested_planes:
        requested_m = requested_cm / 100.0
        actual_z = float(axial_rows[np.argmin(np.abs(axial_rows - requested_m))])
        row_mask = np.isclose(z, actual_z, atol=1.0e-11, rtol=0.0)
        x_mask = (x >= xmin_cm / 100.0 - 1.0e-12) & (
            x <= xmax_cm / 100.0 + 1.0e-12
        )
        indices = np.flatnonzero(row_mask & x_mask)
        if indices.size == 0:
            raise ValueError(
                f"no samples found at Z={actual_z * 100.0:g} cm over "
                f"X={xmin_cm:g}-{xmax_cm:g} cm"
            )
        indices = indices[np.argsort(x[indices])]
        profiles[requested_cm] = {
            "actual_z": actual_z,
            "x": x[indices],
            "gamma_to_wafer": np.asarray(flux["gamma_to_wafer"])[indices],
        }
    return profiles


def _write_csv(
    path: Path,
    profiles: dict[str, dict[float, dict[str, np.ndarray | float]]],
    times: dict[str, float],
) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(
            handle,
            fieldnames=[
                "case",
                "time_s",
                "requested_z_cm",
                "actual_z_cm",
                "x_cm",
                "gamma_toward_wafer_m-2_s-1",
            ],
        )
        writer.writeheader()
        for label, case_profiles in profiles.items():
            for requested_z_cm, profile in case_profiles.items():
                for x_value, flux_value in zip(
                    np.asarray(profile["x"]),
                    np.asarray(profile["gamma_to_wafer"]),
                ):
                    writer.writerow(
                        {
                            "case": label,
                            "time_s": times[label],
                            "requested_z_cm": requested_z_cm,
                            "actual_z_cm": float(profile["actual_z"]) * 100.0,
                            "x_cm": x_value * 100.0,
                            "gamma_toward_wafer_m-2_s-1": flux_value,
                        }
                    )


def _plot_profiles(
    path: Path,
    profiles: dict[str, dict[float, dict[str, np.ndarray | float]]],
    times: dict[str, float],
    planes_cm: list[float],
    x_range_cm: tuple[float, float],
    dpi: int,
) -> None:
    try:
        import matplotlib
    except ModuleNotFoundError as error:
        raise ModuleNotFoundError(
            "Matplotlib is required. Run with /opt/miniconda3/bin/python."
        ) from error

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt

    figure, axes = plt.subplots(
        1,
        len(planes_cm),
        figsize=(5.0 * len(planes_cm), 4.8),
        sharey=True,
        constrained_layout=True,
    )
    axes = np.atleast_1d(axes)
    styles = case_styles(len(profiles))
    for axis, requested_z_cm in zip(axes, planes_cm):
        for (label, case_profiles), style in zip(profiles.items(), styles):
            profile = case_profiles[requested_z_cm]
            axis.plot(
                np.asarray(profile["x"]) * 100.0,
                np.asarray(profile["gamma_to_wafer"]),
                label=rf"{label} ($t={times[label] * 1.0e6:g}\ \mu$s)",
                **style,
            )
        actual_values = [
            float(case_profiles[requested_z_cm]["actual_z"]) * 100.0
            for case_profiles in profiles.values()
        ]
        actual_z_cm = float(np.mean(actual_values))
        axis.axhline(0.0, color="0.35", linewidth=0.8)
        axis.set_title(rf"$Z={actual_z_cm:.3f}$ cm")
        axis.set_xlim(x_range_cm)
        axis.set_xlabel("X [cm]")
        axis.grid(True, alpha=0.22)
        axis.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

    axes[0].set_ylabel(
        r"Cu$^+$ flux toward wafer $-\Gamma_z$ [m$^{-2}$ s$^{-1}$]"
    )
    axes[0].legend(frameon=False, loc="best")
    time_values = np.asarray(list(times.values()))
    if np.ptp(time_values) <= 1.0e-12:
        title = rf"Cartesian X–Z signed Cu$^+$ flux at $t={time_values[0] * 1.0e6:g}\ \mu$s"
    else:
        title = r"Cartesian X–Z signed Cu$^+$ flux at each case's selected time"
    figure.suptitle(title, fontsize=15)

    path.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(path, dpi=dpi, facecolor="white")
    plt.close(figure)


def plot_cartesian_flux_comparison(
    cases: dict[str, Path],
    output_path: Path = DEFAULT_OUTPUT,
    csv_path: Path = DEFAULT_CSV,
    *,
    planes_cm: list[float] | tuple[float, ...] = DEFAULT_Z_PLANES_CM,
    x_range_cm: tuple[float, float] = DEFAULT_X_RANGE_CM,
    comparison_time: float | None = None,
    dpi: int = 180,
) -> tuple[Path, Path]:
    """Load three results and write their signed Cu+ flux profile comparison."""
    if not cases:
        raise ValueError("at least one case is required")
    if dpi <= 0:
        raise ValueError("dpi must be positive")
    planes = [float(value) for value in planes_cm]
    profiles: dict[str, dict[float, dict[str, np.ndarray | float]]] = {}
    times: dict[str, float] = {}

    for label, exodus_path in cases.items():
        path = Path(exodus_path)
        if not path.is_file():
            raise FileNotFoundError(f"Exodus result not found: {path}")
        data = load_final_exodus(path, comparison_time)
        times[label] = float(data["time"])
        profiles[label] = select_x_profiles(
            reconstruct_ion_flux(data), planes, x_range_cm
        )

    output = Path(output_path)
    csv_output = Path(csv_path)
    _plot_profiles(output, profiles, times, planes, x_range_cm, int(dpi))
    _write_csv(csv_output, profiles, times)

    print(f"Wrote {output}")
    print(f"Wrote {csv_output}")
    for label, time_s in times.items():
        print(f"{label}: selected time = {time_s:.9e} s")
    for requested_z_cm in planes:
        print(f"Z={requested_z_cm:g} cm")
        for label, case_profiles in profiles.items():
            profile = case_profiles[requested_z_cm]
            values = np.asarray(profile["gamma_to_wafer"])
            print(
                f"  {label}: mesh Z={float(profile['actual_z']) * 100.0:.6f} cm, "
                f"range=[{np.min(values):.6e}, {np.max(values):.6e}] m^-2 s^-1"
            )
    return output, csv_output


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-b", type=Path, default=DEFAULT_CASES["Source B only"])
    parser.add_argument("--four-coil", type=Path, default=DEFAULT_CASES["Four-coil B"])
    parser.add_argument(
        "--img3092",
        type=Path,
        default=DEFAULT_CASES["IMG-3092 four-coil B"],
    )
    parser.add_argument("--z-cm", type=float, nargs="+", default=DEFAULT_Z_PLANES_CM)
    parser.add_argument(
        "--x-range-cm",
        type=float,
        nargs=2,
        metavar=("XMIN", "XMAX"),
        default=DEFAULT_X_RANGE_CM,
    )
    parser.add_argument("--time", type=float, default=None)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--csv", type=Path, default=DEFAULT_CSV)
    parser.add_argument("--dpi", type=int, default=180)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    plot_cartesian_flux_comparison(
        {
            "Source B only": args.source_b,
            "Four-coil B": args.four_coil,
            "IMG-3092 four-coil B": args.img3092,
        },
        args.output,
        args.csv,
        planes_cm=args.z_cm,
        x_range_cm=tuple(args.x_range_cm),
        comparison_time=args.time,
        dpi=args.dpi,
    )


if __name__ == "__main__":
    main()
