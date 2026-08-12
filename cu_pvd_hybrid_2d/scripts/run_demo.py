#!/usr/bin/env python
from __future__ import annotations

import argparse
import sys
from pathlib import Path

import yaml


ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from coupling.driver import run_coupled_case


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Cu PVD Zapdos-MC hybrid demo")
    parser.add_argument("--config", default=str(ROOT / "config" / "case.yaml"))
    parser.add_argument("--iterations", type=int, default=3)
    parser.add_argument("--neutral-particles", type=int, default=None)
    parser.add_argument("--ion-particles", type=int, default=None)
    parser.add_argument("--no-plots", action="store_true")
    parser.add_argument("--clean", action="store_true")
    args = parser.parse_args()

    config_path = Path(args.config)
    if args.neutral_particles is not None or args.ion_particles is not None or args.no_plots:
        with config_path.open("r", encoding="utf-8") as f:
            config = yaml.safe_load(f)
        if args.neutral_particles is not None:
            config["mc"]["n_particles_neutral"] = args.neutral_particles
        if args.ion_particles is not None:
            config["mc"]["n_particles_ion"] = args.ion_particles
        if args.no_plots:
            config["output"]["save_plots"] = False
        scratch = ROOT / "runs" / "_scratch_case.yaml"
        scratch.parent.mkdir(parents=True, exist_ok=True)
        with scratch.open("w", encoding="utf-8") as f:
            yaml.safe_dump(config, f, sort_keys=False)
        config_path = scratch

    result = run_coupled_case(config_path, max_iter_override=args.iterations, clean=args.clean)
    last = result.history[-1] if result.history else {}
    print(f"run_dir={result.run_dir}")
    print(f"iterations={len(result.history)} converged={result.convergence.converged}")
    if last:
        print(f"total_cu_source_rate_s={last['total_cu_source_rate_s']:.6e}")
        print(f"total_ionization_rate_s={last['total_ionization_rate_s']:.6e}")
        print(f"cu_total_wafer_rate_s={last['cu_total_wafer_rate_s']:.6e}")
        print(f"wafer_ion_fraction={last['wafer_ion_fraction']:.6e}")
        print(f"I_target_sim_A={last['I_target_sim_A']:.6e}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
