#!/usr/bin/env python3
"""Generate the common Cartesian X-Z neutral-Cu table for Zapdos."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = (
    ROOT
    / "runs"
    / "zapdos_cartesian_xz_25x30_neutral"
    / "moose_tables"
    / "n_Cu_m3.tbl"
)

X_MIN_M = 0.0
X_MAX_M = 0.25
Z_MIN_M = 0.0
Z_MAX_M = 0.30
SOURCE_CENTER_X_M = 0.205
DEFAULT_BACKGROUND_DENSITY_M3 = 3.0e18
DEFAULT_PLUME_PEAK_DENSITY_M3 = 4.0e18
DEFAULT_PLUME_WIDTH_X_M = 0.080
DEFAULT_PLUME_WIDTH_Z_M = 0.090


def _validate_axis(name: str, values: np.ndarray) -> np.ndarray:
    axis = np.asarray(values, dtype=float)
    if axis.ndim != 1 or axis.size < 2:
        raise ValueError(f"{name} must be one-dimensional with at least two values")
    if np.any(~np.isfinite(axis)) or np.any(np.diff(axis) <= 0.0):
        raise ValueError(f"{name} must be finite and strictly increasing")
    return axis


def write_moose_table(
    path: Path | str,
    x: np.ndarray,
    z: np.ndarray,
    values: np.ndarray,
) -> None:
    """Write a Cartesian table in MOOSE ``PiecewiseMultilinear`` format."""
    x_axis = _validate_axis("x", x)
    z_axis = _validate_axis("z", z)
    field = np.asarray(values, dtype=float)
    if field.shape != (x_axis.size, z_axis.size):
        raise ValueError("values must have shape (len(x), len(z))")
    if np.any(~np.isfinite(field)):
        raise ValueError("values must be finite")
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    with destination.open("w", encoding="utf-8") as handle:
        handle.write("AXIS X\n")
        handle.write(" ".join(f"{value:.8e}" for value in x_axis) + "\n\n")
        handle.write("AXIS Y\n")
        handle.write(" ".join(f"{value:.8e}" for value in z_axis) + "\n\n")
        handle.write("DATA\n")
        for iz in range(z_axis.size):
            handle.write(" ".join(f"{value:.8e}" for value in field[:, iz]) + "\n")


def read_moose_table(path: Path | str) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Read a two-axis MOOSE table and return ``x, z, values[x,z]``."""
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
    return _validate_axis("x", x), _validate_axis("z", z), values_zx.T


def build_neutral_cu(
    x: np.ndarray,
    z: np.ndarray,
    *,
    background_density: float = DEFAULT_BACKGROUND_DENSITY_M3,
    plume_peak_density: float = DEFAULT_PLUME_PEAK_DENSITY_M3,
    source_center_x: float = SOURCE_CENTER_X_M,
    plume_width_x: float = DEFAULT_PLUME_WIDTH_X_M,
    plume_width_z: float = DEFAULT_PLUME_WIDTH_Z_M,
) -> np.ndarray:
    """Return the common positive Cu-neutral density on the x-z chamber grid."""
    x_axis = _validate_axis("x", x)
    z_axis = _validate_axis("z", z)
    scalars = np.asarray(
        [background_density, plume_peak_density, source_center_x, plume_width_x, plume_width_z],
        dtype=float,
    )
    if np.any(~np.isfinite(scalars)):
        raise ValueError("neutral-density parameters must be finite")
    if background_density <= 0.0 or plume_peak_density < 0.0:
        raise ValueError("neutral densities must be positive/nonnegative")
    if plume_width_x <= 0.0 or plume_width_z <= 0.0:
        raise ValueError("plume widths must be positive")
    if not x_axis[0] <= source_center_x <= x_axis[-1]:
        raise ValueError("source_center_x must lie inside the X domain")

    xx, zz = np.meshgrid(x_axis, z_axis, indexing="ij")
    distance_from_target = z_axis[-1] - zz
    racetrack_plume = np.exp(-0.5 * ((xx - source_center_x) / plume_width_x) ** 2)
    racetrack_plume *= np.exp(-0.5 * (distance_from_target / plume_width_z) ** 2)
    broad_fill = 0.35 * np.exp(
        -0.5 * ((xx - 0.55 * x_axis[-1]) / (0.45 * x_axis[-1])) ** 2
    )
    broad_fill *= np.exp(
        -0.5 * ((zz - 0.45 * z_axis[-1]) / (0.55 * z_axis[-1])) ** 2
    )
    return background_density + plume_peak_density * (racetrack_plume + broad_fill)


def generate_neutral_table(
    output: Path | str = DEFAULT_OUTPUT,
    *,
    nx: int = 101,
    nz: int = 161,
    background_density: float = DEFAULT_BACKGROUND_DENSITY_M3,
    plume_peak_density: float = DEFAULT_PLUME_PEAK_DENSITY_M3,
) -> dict[str, object]:
    if nx < 3 or nz < 3:
        raise ValueError("nx and nz must each be at least three")
    x = np.linspace(X_MIN_M, X_MAX_M, int(nx))
    z = np.linspace(Z_MIN_M, Z_MAX_M, int(nz))
    density = build_neutral_cu(
        x,
        z,
        background_density=background_density,
        plume_peak_density=plume_peak_density,
    )
    destination = Path(output)
    write_moose_table(destination, x, z, density)
    metadata: dict[str, object] = {
        "coordinate_system": "cartesian_x_z",
        "domain_m": {"x": [X_MIN_M, X_MAX_M], "z": [Z_MIN_M, Z_MAX_M]},
        "source_center_x_m": SOURCE_CENTER_X_M,
        "background_density_m-3": float(background_density),
        "plume_peak_density_m-3": float(plume_peak_density),
        "density_range_m-3": [float(np.min(density)), float(np.max(density))],
        "grid": {"nx": int(nx), "nz": int(nz)},
    }
    metadata_path = destination.with_name("neutral_metadata.json")
    metadata_path.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    return metadata


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--nx", type=int, default=101)
    parser.add_argument("--nz", type=int, default=161)
    parser.add_argument("--background-density", type=float, default=DEFAULT_BACKGROUND_DENSITY_M3)
    parser.add_argument("--plume-peak-density", type=float, default=DEFAULT_PLUME_PEAK_DENSITY_M3)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    metadata = generate_neutral_table(
        args.output,
        nx=args.nx,
        nz=args.nz,
        background_density=args.background_density,
        plume_peak_density=args.plume_peak_density,
    )
    print(f"wrote {args.output}")
    print(json.dumps(metadata, indent=2))


if __name__ == "__main__":
    main()
