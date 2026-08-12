#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


def read_moose_table(path: Path) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Read a 2D MOOSE PiecewiseMultilinear table with AXIS X/Y and DATA blocks."""
    lines = path.read_text(encoding="utf-8").splitlines()
    sections: dict[str, list[str]] = {}
    current: str | None = None

    for raw_line in lines:
        line = raw_line.strip()
        if not line:
            continue
        if line.startswith("AXIS "):
            current = line
            sections[current] = []
            continue
        if line == "DATA":
            current = line
            sections[current] = []
            continue
        if current is not None:
            sections[current].append(line)

    try:
        x = np.fromstring(" ".join(sections["AXIS X"]), sep=" ")
        y = np.fromstring(" ".join(sections["AXIS Y"]), sep=" ")
        data_rows = [np.fromstring(row, sep=" ") for row in sections["DATA"]]
    except KeyError as exc:
        raise ValueError(f"{path} is missing section {exc}") from exc

    values_yx = np.vstack(data_rows)
    expected_shape = (len(y), len(x))
    if values_yx.shape != expected_shape:
        raise ValueError(f"{path} data shape {values_yx.shape} does not match {expected_shape}")
    return x, y, values_yx


def positive_log_norm(values: np.ndarray):
    from matplotlib.colors import LogNorm

    positive = values[np.isfinite(values) & (values > 0.0)]
    if positive.size == 0:
        return LogNorm(vmin=1.0e-30, vmax=1.0)
    vmin = max(float(np.percentile(positive, 1.0)), 1.0e-30)
    vmax = float(np.nanmax(positive))
    if vmin >= vmax:
        vmax = max(vmin * 1.01, 1.0e-29)
    return LogNorm(vmin=vmin, vmax=vmax)


def plot_tables(
    table_dir: Path,
    output: Path,
    chamber_radius: float,
    chamber_length: float,
    target_radius: float,
    wafer_radius: float,
) -> None:
    import matplotlib.pyplot as plt

    tables = [
        (
            table_dir / "n_Cu_m3.tbl",
            "Neutral Cu density $n_{Cu}$ [m$^{-3}$]",
            "YlGnBu",
        ),
        (
            table_dir / "S_Cu_eff_m3_s.tbl",
            "Effective Cu ionization source $S_{Cu,eff}$ [m$^{-3}$ s$^{-1}$]",
            "magma",
        ),
    ]

    loaded = []
    for path, title, cmap in tables:
        x, y, values = read_moose_table(path)
        loaded.append((path, title, cmap, x, y, values))

    base_x = loaded[0][3]
    base_y = loaded[0][4]
    for path, _, _, x, y, _ in loaded[1:]:
        if not (np.allclose(base_x, x) and np.allclose(base_y, y)):
            raise ValueError(f"{path} does not use the same grid as {loaded[0][0]}")

    fig, axes = plt.subplots(1, 2, figsize=(13.5, 5.4), constrained_layout=True)
    chamber_radius_cm = chamber_radius * 100.0
    chamber_length_cm = chamber_length * 100.0
    target_radius_cm = target_radius * 100.0
    wafer_radius_cm = wafer_radius * 100.0

    for ax, (path, title, cmap, x, y, values) in zip(axes, loaded):
        norm = positive_log_norm(values)
        clipped = np.maximum(values, norm.vmin)
        pcm = ax.pcolormesh(x * 100.0, y * 100.0, clipped, shading="auto", cmap=cmap, norm=norm)
        fig.colorbar(pcm, ax=ax, pad=0.015, shrink=0.9)
        ax.set_title(title)
        ax.set_xlabel("x [cm]")
        ax.set_ylabel("y [cm]")
        ax.set_xlim(-chamber_radius_cm, chamber_radius_cm)
        ax.set_ylim(0.0, chamber_length_cm)
        ax.set_aspect("equal", adjustable="box")
        ax.plot(
            [-chamber_radius_cm, chamber_radius_cm, chamber_radius_cm, -chamber_radius_cm, -chamber_radius_cm],
            [0.0, 0.0, chamber_length_cm, chamber_length_cm, 0.0],
            color="black",
            linewidth=1.0,
            alpha=0.65,
        )
        ax.plot([-target_radius_cm, target_radius_cm], [0.0, 0.0], color="black", linewidth=4.0)
        ax.plot(
            [-wafer_radius_cm, wafer_radius_cm],
            [chamber_length_cm, chamber_length_cm],
            color="black",
            linewidth=3.0,
        )
        ax.text(
            0.0,
            1.2,
            "target",
            ha="center",
            va="bottom",
            color="black",
            fontsize=9,
        )
        ax.text(
            0.0,
            chamber_length_cm - 1.2,
            "wafer",
            ha="center",
            va="top",
            color="black",
            fontsize=9,
        )
        print(
            f"{path.name}: min={float(np.nanmin(values)):.3e}, "
            f"max={float(np.nanmax(values)):.3e}"
        )

    fig.suptitle(f"Effective source tables from {table_dir}", fontweight="bold")
    output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output, dpi=220)
    plt.close(fig)
    print(f"Saved {output}")


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    default_table_dir = root / "runs" / "zapdos_initial_input" / "moose_tables"
    default_output = root / "post" / "effective_source_tables.png"

    parser = argparse.ArgumentParser(description="Plot n_Cu and S_Cu_eff MOOSE table maps.")
    parser.add_argument("--table-dir", type=Path, default=default_table_dir)
    parser.add_argument("--output", type=Path, default=default_output)
    parser.add_argument("--chamber-radius", type=float, default=0.24)
    parser.add_argument("--chamber-length", type=float, default=0.60)
    parser.add_argument("--target-radius", type=float, default=0.18)
    parser.add_argument("--wafer-radius", type=float, default=0.15)
    args = parser.parse_args()

    plot_tables(
        table_dir=args.table_dir,
        output=args.output,
        chamber_radius=args.chamber_radius,
        chamber_length=args.chamber_length,
        target_radius=args.target_radius,
        wafer_radius=args.wafer_radius,
    )


if __name__ == "__main__":
    main()
