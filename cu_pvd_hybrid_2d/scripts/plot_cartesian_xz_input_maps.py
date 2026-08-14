#!/usr/bin/env python3
"""Plot Cartesian neutral-Cu, magnetic-field, and paired SEE input maps."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
CASE_NAMES = (
    "source_only",
    "four_coil",
    "four_coil_left_bend",
    "four_coil_img3092",
)
CASE_TABLE_DIRS = {
    case: ROOT / "runs" / f"zapdos_cartesian_xz_25x30_{case}" / "moose_tables"
    for case in CASE_NAMES
}
DEFAULT_NEUTRAL_TABLE = (
    ROOT
    / "runs"
    / "zapdos_cartesian_xz_25x30_neutral"
    / "moose_tables"
    / "n_Cu_m3.tbl"
)
CASE_TITLES = {
    "source_only": "Source only",
    "four_coil": "Standard four coil",
    "four_coil_left_bend": "Left-bending four coil",
    "four_coil_img3092": "IMG-3092 four coil",
}
LEGACY_BFIELD_LEVELS_G = np.asarray(
    [
        1.00,
        1.64,
        2.68,
        4.39,
        7.20,
        11.79,
        19.31,
        31.62,
        51.79,
        84.83,
        138.95,
        227.58,
        372.76,
        610.54,
        1000.00,
    ],
    dtype=float,
)


def read_moose_table(path: Path | str) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    sections: dict[str, list[str]] = {}
    current: str | None = None
    for raw_line in Path(path).read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if line.startswith("AXIS ") or line == "DATA":
            current = line
            sections[current] = []
        elif current is not None:
            sections[current].append(line)
    try:
        x = np.fromstring(" ".join(sections["AXIS X"]), sep=" ")
        z = np.fromstring(" ".join(sections["AXIS Y"]), sep=" ")
        rows = [np.fromstring(row, sep=" ") for row in sections["DATA"]]
    except KeyError as error:
        raise ValueError(f"{path} is missing {error.args[0]}") from error
    values_zx = np.vstack(rows)
    if values_zx.shape != (z.size, x.size):
        raise ValueError(
            f"{path} DATA shape {values_zx.shape} does not match {(z.size, x.size)}"
        )
    return x, z, values_zx.T


def _paired_inputs(
    case: str, table_dir: Path, neutral_table: Path
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
    if case not in CASE_NAMES:
        raise ValueError(f"unknown case {case!r}")
    bx_path = table_dir / "Bx_T.tbl"
    bz_path = table_dir / "By_T.tbl"
    see_path = table_dir / "see_spatial_weight_m-1.tbl"
    metadata_path = table_dir / "generation_metadata.json"
    for path in (neutral_table, bx_path, bz_path, see_path, metadata_path):
        if not path.exists():
            raise FileNotFoundError(path)

    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    recorded = metadata.get("bfield_sha256", {})
    for path in (bx_path, bz_path):
        actual = hashlib.sha256(path.read_bytes()).hexdigest()
        if recorded.get(path.name) != actual:
            raise ValueError(
                f"{see_path.name} is stale: rerun the SEE generator for the current {path.name}"
            )

    x, z, bx = read_moose_table(bx_path)
    bz_x, bz_z, bz = read_moose_table(bz_path)
    see_x, see_z, see = read_moose_table(see_path)
    if not (
        np.array_equal(x, bz_x)
        and np.array_equal(z, bz_z)
        and np.array_equal(x, see_x)
        and np.array_equal(z, see_z)
    ):
        raise ValueError("B-field and SEE tables must use one shared X-Z grid")
    neutral_x, neutral_z, neutral = read_moose_table(neutral_table)
    return x, z, bx, bz, see, (neutral_x, neutral_z, neutral)


def _positive_log_norm(values: np.ndarray, *, decades: float = 6.0):
    from matplotlib.colors import LogNorm

    positive = np.asarray(values)[np.isfinite(values) & (values > 0.0)]
    if positive.size == 0:
        raise ValueError("map contains no positive finite values")
    vmax = float(np.max(positive))
    observed_min = float(np.min(positive))
    return LogNorm(vmin=max(observed_min, vmax * 10.0 ** (-decades)), vmax=vmax)


def _annotate_geometry(ax) -> None:
    ax.plot([19.0, 22.0], [30.0, 30.0], color="cyan", linewidth=4.0, label="target")
    ax.plot([7.0, 17.0], [0.0, 0.0], color="lime", linewidth=4.0, label="wafer")
    ax.set_xlim(0.0, 25.0)
    ax.set_ylim(0.0, 30.0)
    ax.set_aspect("equal")
    ax.set_xlabel("X [cm]")
    ax.set_ylabel("Z [cm]")


def plot_input_maps(
    case: str,
    table_dir: Path | str | None = None,
    neutral_table: Path | str = DEFAULT_NEUTRAL_TABLE,
    output: Path | str | None = None,
    *,
    dpi: int = 180,
) -> Path:
    """Render one validated neutral/B/SEE input set as three panels."""
    directory = Path(table_dir) if table_dir is not None else CASE_TABLE_DIRS[case]
    neutral_path = Path(neutral_table)
    output_path = (
        Path(output)
        if output is not None
        else ROOT / "post" / f"cartesian_xz_{case}_input_maps.png"
    )
    x, z, bx, bz, see, neutral_data = _paired_inputs(case, directory, neutral_path)
    neutral_x, neutral_z, neutral = neutral_data

    import matplotlib.pyplot as plt
    from matplotlib.colors import BoundaryNorm

    fig, axes = plt.subplots(1, 3, figsize=(15.8, 5.5), constrained_layout=True)

    neutral_norm = _positive_log_norm(neutral, decades=3.0)
    neutral_image = axes[0].pcolormesh(
        neutral_x * 100.0,
        neutral_z * 100.0,
        np.maximum(neutral.T, neutral_norm.vmin),
        shading="auto",
        cmap="YlGnBu",
        norm=neutral_norm,
    )
    axes[0].set_title("Common neutral Cu density")
    fig.colorbar(neutral_image, ax=axes[0], label=r"$n_{Cu}$ [m$^{-3}$]")

    b_gauss = np.hypot(bx, bz) * 1.0e4
    b_colormap = plt.colormaps["turbo"].resampled(LEGACY_BFIELD_LEVELS_G.size - 1)
    b_norm = BoundaryNorm(
        LEGACY_BFIELD_LEVELS_G,
        b_colormap.N,
        clip=True,
    )
    b_image = axes[1].pcolormesh(
        x * 100.0,
        z * 100.0,
        np.clip(
            b_gauss.T,
            LEGACY_BFIELD_LEVELS_G[0],
            LEGACY_BFIELD_LEVELS_G[-1],
        ),
        shading="auto",
        cmap=b_colormap,
        norm=b_norm,
    )
    axes[1].streamplot(
        x * 100.0,
        z * 100.0,
        bx.T,
        bz.T,
        color="black" if case == "four_coil_left_bend" else "white",
        density=1.2,
        linewidth=0.65,
        arrowsize=0.7,
    )
    axes[1].set_title(f"{CASE_TITLES[case]} magnetic field")
    b_colorbar = fig.colorbar(
        b_image,
        ax=axes[1],
        boundaries=LEGACY_BFIELD_LEVELS_G,
        ticks=LEGACY_BFIELD_LEVELS_G,
        label="|B| [G]",
    )
    b_colorbar.ax.set_yticklabels(
        [f"{level:g}" for level in LEGACY_BFIELD_LEVELS_G]
    )

    see_norm = _positive_log_norm(see, decades=7.0)
    see_image = axes[2].pcolormesh(
        x * 100.0,
        z * 100.0,
        np.maximum(see.T, see_norm.vmin),
        shading="auto",
        cmap="magma",
        norm=see_norm,
    )
    axes[2].streamplot(
        x * 100.0,
        z * 100.0,
        bx.T,
        bz.T,
        color="black" if case == "four_coil_left_bend" else "white",
        density=1.0,
        linewidth=0.55,
        arrowsize=0.65,
    )
    axes[2].set_title("Paired SEE spatial weight")
    fig.colorbar(see_image, ax=axes[2], label=r"SEE weight [m$^{-1}$]")

    for ax in axes:
        _annotate_geometry(ax)
    fig.suptitle(f"Cartesian X-Z inputs — {CASE_TITLES[case]}", fontweight="bold")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=dpi)
    plt.close(fig)
    return output_path


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--case", choices=CASE_NAMES, required=True)
    parser.add_argument("--table-dir", type=Path)
    parser.add_argument("--neutral-table", type=Path, default=DEFAULT_NEUTRAL_TABLE)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--dpi", type=int, default=180)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    output = plot_input_maps(
        args.case,
        args.table_dir,
        args.neutral_table,
        args.output,
        dpi=args.dpi,
    )
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
