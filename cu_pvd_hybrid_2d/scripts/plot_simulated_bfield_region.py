#!/usr/bin/env python3
"""Plot the B-field portion sampled by the 25 cm-radius Zapdos meshes."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np

try:
    from plot_effective_source_tables import read_moose_table
except ModuleNotFoundError:
    from scripts.plot_effective_source_tables import read_moose_table

try:
    from generate_reference_source_bfield import (
        GAUSS_TO_TESLA,
        REFERENCE_LEVELS_G,
    )
except ModuleNotFoundError:
    from scripts.generate_reference_source_bfield import (
        GAUSS_TO_TESLA,
        REFERENCE_LEVELS_G,
    )


PLASMA_RADIUS_M = 0.250
CHAMBER_HEIGHT_M = 0.300
ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE_ONLY_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_source_only"
    / "moose_tables"
)
DEFAULT_FOUR_COIL_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_four_coil"
    / "moose_tables"
)
DEFAULT_IMAGE_FOUR_COIL_DIR = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_reference_four_coil_img3092"
    / "moose_tables"
)
DEFAULT_OUTPUT = ROOT / "post" / "bfield_maps_three_cases.png"

PHYSICAL_MARKERS_MM = (
    ("Density/energy seed", 150.0, 285.0, "o", 44.0),
    ("SEE center", 150.0, 300.0, "*", 90.0),
    ("Cu-neutral center", 150.0, 270.0, "D", 38.0),
)


def load_bfield(
    table_dir: Path,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Load paired radial and axial MOOSE B-field component tables."""
    directory = Path(table_dir)
    r_m, z_m, br_zr = read_moose_table(directory / "Bx_T.tbl")
    by_r_m, by_z_m, bz_zr = read_moose_table(directory / "By_T.tbl")

    if (
        r_m.shape != by_r_m.shape
        or z_m.shape != by_z_m.shape
        or not np.allclose(r_m, by_r_m)
        or not np.allclose(z_m, by_z_m)
    ):
        raise ValueError(f"{directory} Bx and By tables must use the same grid")

    return r_m, z_m, br_zr, bz_zr


def crop_field(
    r_m: np.ndarray,
    z_m: np.ndarray,
    br_zr: np.ndarray,
    bz_zr: np.ndarray,
    *,
    r_max_m: float = PLASMA_RADIUS_M,
    z_max_m: float = CHAMBER_HEIGHT_M,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    """Validate and crop a paired field to the inclusive plasma domain."""
    r = np.asarray(r_m, dtype=float)
    z = np.asarray(z_m, dtype=float)
    br = np.asarray(br_zr, dtype=float)
    bz = np.asarray(bz_zr, dtype=float)
    expected_shape = (z.size, r.size)

    if r.ndim != 1 or z.ndim != 1 or r.size < 2 or z.size < 2:
        raise ValueError("R and Z axes must be one-dimensional with at least two points")
    if np.any(np.diff(r) <= 0.0) or np.any(np.diff(z) <= 0.0):
        raise ValueError("R and Z axes must be strictly increasing")
    if br.shape != expected_shape or bz.shape != expected_shape:
        raise ValueError(
            f"B_R and B_Z must both have shape {expected_shape}; "
            f"received {br.shape} and {bz.shape}"
        )
    if not (
        np.all(np.isfinite(r))
        and np.all(np.isfinite(z))
        and np.all(np.isfinite(br))
        and np.all(np.isfinite(bz))
    ):
        raise ValueError("B-field axes and components must contain only finite values")
    if r[0] > 0.0 or z[0] > 0.0 or r[-1] < r_max_m or z[-1] < z_max_m:
        raise ValueError(
            "B-field tables do not cover the requested plasma domain "
            f"R=0..{r_max_m} m, Z=0..{z_max_m} m"
        )

    tolerance = 32.0 * np.finfo(float).eps
    radial_mask = (r >= -tolerance) & (r <= r_max_m + tolerance)
    axial_mask = (z >= -tolerance) & (z <= z_max_m + tolerance)
    cropped_r = r[radial_mask]
    cropped_z = z[axial_mask]

    if (
        not np.isclose(cropped_r[0], 0.0)
        or not np.isclose(cropped_z[0], 0.0)
        or not np.isclose(cropped_r[-1], r_max_m)
        or not np.isclose(cropped_z[-1], z_max_m)
    ):
        raise ValueError("B-field grid must contain all four plasma-domain boundaries")

    return (
        cropped_r,
        cropped_z,
        br[np.ix_(axial_mask, radial_mask)],
        bz[np.ix_(axial_mask, radial_mask)],
    )


def plot_comparison(
    output_path: Path,
    cases: list[tuple[str, Path]],
) -> None:
    """Plot three B-field cases over the exact Zapdos plasma region."""
    import matplotlib

    matplotlib.use("Agg", force=True)
    from matplotlib import pyplot as plt
    from matplotlib.colors import BoundaryNorm

    if len(cases) != 3:
        raise ValueError("the comparison requires exactly three B-field cases")

    loaded_cases = []
    for label, table_dir in cases:
        r_m, z_m, br_zr, bz_zr = crop_field(*load_bfield(table_dir))
        loaded_cases.append((label, Path(table_dir), r_m, z_m, br_zr, bz_zr))

    base_r = loaded_cases[0][2]
    base_z = loaded_cases[0][3]
    for label, _, r_m, z_m, _, _ in loaded_cases[1:]:
        if (
            r_m.shape != base_r.shape
            or z_m.shape != base_z.shape
            or not np.allclose(r_m, base_r)
            or not np.allclose(z_m, base_z)
        ):
            raise ValueError(f"{label} does not use the same cropped grid")

    number_of_bands = REFERENCE_LEVELS_G.size - 1
    colormap = plt.colormaps["turbo"].resampled(number_of_bands)
    normalization = BoundaryNorm(
        REFERENCE_LEVELS_G,
        ncolors=colormap.N,
        clip=True,
    )

    figure, axes = plt.subplots(
        1,
        3,
        figsize=(16.8, 7.6),
        sharex=True,
        sharey=True,
        constrained_layout=True,
    )
    filled = None
    legend_handles = []

    for panel_index, (
        label,
        _,
        r_m,
        z_m,
        br_zr,
        bz_zr,
    ) in enumerate(loaded_cases):
        axis = axes[panel_index]
        r_mm = 1000.0 * r_m
        z_mm = 1000.0 * z_m
        magnitude_g = np.hypot(br_zr, bz_zr) / GAUSS_TO_TESLA
        displayed_magnitude_g = np.clip(
            magnitude_g,
            REFERENCE_LEVELS_G[0],
            REFERENCE_LEVELS_G[-1],
        )

        filled = axis.contourf(
            r_mm,
            z_mm,
            displayed_magnitude_g,
            levels=REFERENCE_LEVELS_G,
            cmap=colormap,
            norm=normalization,
        )
        axis.streamplot(
            r_mm,
            z_mm,
            br_zr,
            bz_zr,
            color="black",
            density=(1.35, 1.35),
            linewidth=0.75,
            arrowsize=0.75,
            arrowstyle="-|>",
            broken_streamlines=True,
        )

        for marker_label, marker_r, marker_z, marker_style, marker_size in (
            PHYSICAL_MARKERS_MM
        ):
            marker = axis.scatter(
                [marker_r],
                [marker_z],
                marker=marker_style,
                s=marker_size,
                facecolor="white",
                edgecolor="black",
                linewidth=1.0,
                zorder=6,
                clip_on=False,
                label=marker_label,
            )
            if panel_index == 0:
                legend_handles.append(marker)

        axis.set_xlim(0.0, 250.0)
        axis.set_ylim(0.0, 300.0)
        axis.set_aspect("equal", adjustable="box")
        axis.set_xlabel("R (mm)")
        axis.set_title(label)
        axis.grid(False)

    axes[0].set_ylabel("Z (mm)")
    if filled is None:
        raise RuntimeError("comparison plot contains no magnitude contours")
    colorbar = figure.colorbar(
        filled,
        ax=axes,
        boundaries=REFERENCE_LEVELS_G,
        ticks=REFERENCE_LEVELS_G,
        spacing="uniform",
        pad=0.025,
        shrink=0.94,
    )
    colorbar.set_label("|B| (G)")
    colorbar.ax.set_yticklabels(
        [f"{level:.2f}" for level in REFERENCE_LEVELS_G]
    )
    figure.legend(
        handles=legend_handles,
        labels=[marker[0] for marker in PHYSICAL_MARKERS_MM],
        loc="outside lower center",
        ncols=3,
        frameon=False,
    )
    figure.suptitle(
        "Magnetic field sampled by the Zapdos plasma mesh",
        fontsize=14,
    )

    destination = Path(output_path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(destination, dpi=180)
    plt.close(figure)


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    """Parse command-line paths for the three input table sets."""
    parser = argparse.ArgumentParser(
        description=(
            "Plot the source-only, original four-coil, and image-3092 "
            "four-coil B-fields over R=0..250 mm and Z=0..300 mm."
        )
    )
    parser.add_argument(
        "--source-only-dir",
        type=Path,
        default=DEFAULT_SOURCE_ONLY_DIR,
    )
    parser.add_argument(
        "--four-coil-dir",
        type=Path,
        default=DEFAULT_FOUR_COIL_DIR,
    )
    parser.add_argument(
        "--image-four-coil-dir",
        type=Path,
        default=DEFAULT_IMAGE_FOUR_COIL_DIR,
    )
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    """Write the simulated-region comparison image."""
    args = parse_args(argv)
    cases = [
        ("Source-only field", args.source_only_dir),
        ("Original four-coil field", args.four_coil_dir),
        ("Image-3092 four-coil field", args.image_four_coil_dir),
    ]
    plot_comparison(args.output, cases)

    for label, table_dir in cases:
        _, _, br_zr, bz_zr = crop_field(*load_bfield(table_dir))
        magnitude_g = np.hypot(br_zr, bz_zr) / GAUSS_TO_TESLA
        print(
            f"{label}: simulated-region |B| "
            f"{np.min(magnitude_g):.2f}..{np.max(magnitude_g):.2f} G"
        )
    print(f"wrote simulated-region comparison: {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
