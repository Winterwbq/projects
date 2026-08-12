#!/usr/bin/env python3
"""Plot last-timestep diagnostics for the HPEM-like R-Z 30 cm Zapdos case."""
from __future__ import annotations

import argparse
from pathlib import Path

from plot_zapdos_last_timestep_diagnostics import plot_last_timestep
from plot_zapdos_last_timestep_diagnostics import CHAMBER_DENSITY_PERCENTILES
from plot_zapdos_last_timestep_diagnostics import _validate_density_display_mode


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_EXODUS = ROOT / "zapdos_templates" / "Outputs"/"cu_pvd_hybrid_hpem_rz_30cm_newsourceB_guidedSEE_1.e"
# DEFAULT_EXODUS = ROOT / "zapdos_templates" / "Outputs"/"cu_pvd_hybrid_hpem_rz_30cm_newSourceB_2.e"
DEFAULT_OUTPUT = ROOT / "post" / "hpem_rz_30cm_last_timestep_newsourceB_guidedSEE_1.png"
# DEFAULT_OUTPUT = ROOT / "post" / "hpem_rz_30cm_last_timestep_newSourceB_2.png"
 

def main() -> None:
    parser = argparse.ArgumentParser(description="Plot HPEM-like R-Z 30 cm Zapdos diagnostics.")
    parser.add_argument("--exodus", type=Path, default=DEFAULT_EXODUS)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument(
        "--timestep",
        type=int,
        default=None,
        help="Saved Exodus timestep index to plot. Defaults to the last saved step. Negative indices count from the end.",
    )
    parser.add_argument(
        "--time",
        type=float,
        default=None,
        help="Physical time in seconds; plot the nearest saved Exodus timestep.",
    )
    parser.add_argument("--bulk-margin-cm", type=float, default=0.0)
    parser.add_argument(
        "--density-growth",
        action="store_true",
        help=(
            "Plot signed electron/Cu+ density change relative to the first populated saved "
            "timestep using a shared linear, zero-centered color scale."
        ),
    )
    parser.add_argument("--density-contrast", action="store_true")
    parser.add_argument(
        "--density-style",
        choices=("default", "chamber"),
        default="default",
        help=(
            "Density color scaling. 'chamber' uses linear 2-98 percentile clipping "
            "with the turbo colormap."
        ),
    )
    parser.add_argument(
        "--density-percentiles",
        type=float,
        nargs=2,
        metavar=("LOW", "HIGH"),
        default=CHAMBER_DENSITY_PERCENTILES,
        help=(
            "Percentile range for --density-style chamber. Use a lower HIGH, such as 90, "
            "to saturate the source hotspot and emphasize chamber density variation."
        ),
    )
    args = parser.parse_args()
    try:
        _validate_density_display_mode(
            density_growth=args.density_growth,
            density_contrast=args.density_contrast,
            density_style=args.density_style,
        )
    except ValueError as error:
        parser.error(str(error))

    plot_last_timestep(
        args.exodus,
        args.output,
        timestep=args.timestep,
        time=args.time,
        bulk_margin_cm=args.bulk_margin_cm,
        density_growth=args.density_growth,
        density_contrast=args.density_contrast,
        density_style=args.density_style,
        density_percentiles=tuple(args.density_percentiles),
        xlabel="R [cm]",
        ylabel="Z [cm]",
    )


if __name__ == "__main__":
    main()
