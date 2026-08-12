#!/usr/bin/env python3
"""Generate a low-density prescribed Cu map from the original effective map."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT = (
    ROOT / "runs" / "zapdos_hpem_rz_30cm" / "moose_tables" / "n_Cu_m3.tbl"
)
DEFAULT_OUTPUT = (
    ROOT
    / "runs"
    / "zapdos_hpem_rz_30cm_low_density"
    / "moose_tables"
    / "n_Cu_m3.tbl"
)

ORIGINAL_DENSITY_FLOOR = 1.0e16
ORIGINAL_DENSITY_MULTIPLIER = 0.1
ORIGINAL_BACKGROUND_DENSITY = 1.0e17
ORIGINAL_MAGNETRON_PEAK = 3.0e18
ORIGINAL_MAGNETRON_CENTER_R = 0.150
ORIGINAL_MAGNETRON_CENTER_Z = 0.270
ORIGINAL_MAGNETRON_WIDTH_R = 0.080
ORIGINAL_MAGNETRON_WIDTH_Z = 0.090

DEFAULT_MIN_DENSITY = 4.0e12
DEFAULT_MAX_DENSITY = 3.0e13


def read_moose_table(path: Path) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Read an AXIS X/Y and DATA PiecewiseMultilinear table."""
    sections: dict[str, list[str]] = {}
    current: str | None = None

    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line:
            continue
        if line.startswith("AXIS ") or line == "DATA":
            current = line
            sections[current] = []
        elif current is not None:
            sections[current].append(line)

    try:
        r = np.fromstring(" ".join(sections["AXIS X"]), sep=" ")
        z = np.fromstring(" ".join(sections["AXIS Y"]), sep=" ")
        density = np.vstack(
            [np.fromstring(row, sep=" ") for row in sections["DATA"]]
        )
    except KeyError as exc:
        raise ValueError(f"{path} is missing section {exc}") from exc

    expected_shape = (z.size, r.size)
    if density.shape != expected_shape:
        raise ValueError(
            f"{path} data shape {density.shape} does not match {expected_shape}"
        )
    return r, z, density


def write_moose_table(
    path: Path,
    r: np.ndarray,
    z: np.ndarray,
    density: np.ndarray,
) -> None:
    """Write an R-Z density field as a PiecewiseMultilinear table."""
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as handle:
        handle.write("AXIS X\n")
        handle.write(" ".join(f"{value:.8e}" for value in r) + "\n\n")
        handle.write("AXIS Y\n")
        handle.write(" ".join(f"{value:.8e}" for value in z) + "\n\n")
        handle.write("DATA\n")
        for row in density:
            handle.write(" ".join(f"{value:.8e}" for value in row) + "\n")


def build_original_effective_density(
    r: np.ndarray,
    z: np.ndarray,
    table_density: np.ndarray,
) -> np.ndarray:
    """Reconstruct the complete effective Cu field used by the original input."""
    rr, zz = np.meshgrid(r, z, indexing="xy")
    floored_table = np.maximum(table_density, ORIGINAL_DENSITY_FLOOR)
    magnetron = np.exp(
        -((rr - ORIGINAL_MAGNETRON_CENTER_R) / ORIGINAL_MAGNETRON_WIDTH_R) ** 2
        -((zz - ORIGINAL_MAGNETRON_CENTER_Z) / ORIGINAL_MAGNETRON_WIDTH_Z) ** 2
    )
    return (
        ORIGINAL_DENSITY_MULTIPLIER * floored_table
        + ORIGINAL_BACKGROUND_DENSITY
        + ORIGINAL_MAGNETRON_PEAK * magnetron
    )


def affine_rescale(
    density: np.ndarray,
    target_min: float,
    target_max: float,
) -> np.ndarray:
    """Map the sampled density extrema to the requested target extrema."""
    source_min = float(np.min(density))
    source_max = float(np.max(density))
    if source_max <= source_min:
        raise ValueError("The source Cu map must have a nonzero density range")
    if target_min <= 0.0 or target_max <= target_min:
        raise ValueError("Target densities must satisfy 0 < minimum < maximum")

    normalized = (density - source_min) / (source_max - source_min)
    return target_min + (target_max - target_min) * normalized


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Affine-rescale the complete original effective Cu map."
    )
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--min-density", type=float, default=DEFAULT_MIN_DENSITY)
    parser.add_argument("--max-density", type=float, default=DEFAULT_MAX_DENSITY)
    args = parser.parse_args()

    r, z, table_density = read_moose_table(args.input)
    effective_density = build_original_effective_density(r, z, table_density)
    prescribed_density = affine_rescale(
        effective_density,
        target_min=args.min_density,
        target_max=args.max_density,
    )
    write_moose_table(args.output, r, z, prescribed_density)


if __name__ == "__main__":
    main()
