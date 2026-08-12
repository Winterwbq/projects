#!/usr/bin/env python
from __future__ import annotations

import argparse
import sys
from pathlib import Path

import numpy as np


ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from coupling.initialize import build_mesh, initial_target_cu_source, load_case
from coupling.source_update import initial_ionization_source
from coupling.zapdos_io import make_boundary_estimates, write_zapdos_inputs
from fields.bmap_reader import load_bmaps
from fields.fake_magnetron_bmap import write_fake_magnetron_bmap
from mc.boundaries import ChamberGeometry
from mc.neutral_cu_mc import NeutralCuMC


def main() -> int:
    parser = argparse.ArgumentParser(description="Prepare initial MC-to-Zapdos input files")
    parser.add_argument("--config", default=str(ROOT / "config" / "case.yaml"))
    parser.add_argument("--out", default=str(ROOT / "runs" / "zapdos_initial_input"))
    parser.add_argument("--neutral-particles", type=int, default=None)
    args = parser.parse_args()

    config = load_case(args.config)
    if args.neutral_particles is not None:
        config["mc"]["n_particles_neutral"] = args.neutral_particles

    mesh = build_mesh(config)

    # Regenerate the fake magnetron B-field CSV in 2D Cartesian (x, y, Bx, By).
    # The file in the repo may be from an older cylindrical (r, z, Br, Bz) run.
    bcfg = config.get("bfield", {})
    magnetron_csv = bcfg.get("magnetron_csv")
    if magnetron_csv:
        geom = config.get("geometry", {})
        write_fake_magnetron_bmap(
            ROOT / magnetron_csv,
            mesh,
            racetrack_radius=float(geom.get("racetrack_radius", 0.105)),
            racetrack_width=float(geom.get("racetrack_width", 0.025)),
            decay_length=float(bcfg.get("decay_length", 0.090)),
            peak_field_t=float(bcfg.get("synthetic_magnetron_B0_T", 0.055)),
        )
        print(f"regenerated B-field map: {ROOT / magnetron_csv}")
    bmaps = load_bmaps(config, mesh)
    target_source, source_info = initial_target_cu_source(config, mesh)

    rng = np.random.default_rng(int(config.get("mc", {}).get("random_seed", 1)))
    neutral_mc = NeutralCuMC(mesh, ChamberGeometry.from_config(config), config, rng)
    neutral = neutral_mc.run(target_source)

    s_iz = initial_ionization_source(config, mesh, bmaps["B_total"], source_info["total_cu_source_s"])
    write_zapdos_inputs(
        args.out,
        mesh,
        neutral.n_cu,
        s_iz,
        bmaps["B_total"],
        target_source,
        config,
        boundary_estimates=make_boundary_estimates(mesh, neutral=neutral),
    )

    print(f"wrote Zapdos input package: {args.out}")
    print("files:")
    for name in [
        "zapdos_input_manifest.yaml",
        "mesh.csv",
        "mc_fields.csv",
        "target_source.csv",
        "boundary_fluxes.csv",
        "wall_losses.csv",
        "operation.csv",
    ]:
        print(f"  {Path(args.out) / name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

