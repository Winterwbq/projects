#!/usr/bin/env python3
"""Quick post-processing helper for the PVD Zapdos Exodus output.

Examples:
  python3 analyze_pvd_results.py --list
  python3 analyze_pvd_results.py --variable em_density --time-index -1
  python3 analyze_pvd_results.py --variable potential --time-index -1 --line axial
"""

import argparse
from pathlib import Path


def load_pyvista():
    try:
        import pyvista as pv
    except ImportError as exc:
        raise SystemExit(
            "This script needs pyvista. Install it with:\n"
            "  python3 -m pip install pyvista\n"
            "If pip is restricted, use ParaView for now or install pyvista in a conda env."
        ) from exc
    return pv


def read_mesh(filename, time_index):
    pv = load_pyvista()
    reader = pv.get_reader(str(filename))
    if hasattr(reader, "set_active_time_point"):
        reader.set_active_time_point(time_index)
    return pv, reader, reader.read()


def active_block(dataset):
    if hasattr(dataset, "n_blocks"):
        for block in dataset:
            if block is not None and block.n_points:
                return block
    return dataset


def list_contents(filename):
    pv, reader, dataset = read_mesh(filename, -1)
    block = active_block(dataset)
    print(f"file: {filename}")
    if hasattr(reader, "time_values"):
        print(f"time steps: {len(reader.time_values)}")
        if reader.time_values:
            print(f"first time: {reader.time_values[0]}")
            print(f"last time:  {reader.time_values[-1]}")
    print("\npoint arrays:")
    for name in block.point_data:
        print(f"  {name}")
    print("\ncell arrays:")
    for name in block.cell_data:
        print(f"  {name}")


def sample_line(block, variable, line):
    bounds = block.bounds
    xmin, xmax, ymin, ymax, _, _ = bounds
    if line == "axial":
        x = 0.5 * (xmin + xmax)
        start = (x, ymin, 0.0)
        end = (x, ymax, 0.0)
        axis_name = "y_m"
    else:
        y = 0.5 * (ymin + ymax)
        start = (xmin, y, 0.0)
        end = (xmax, y, 0.0)
        axis_name = "x_m"

    sampled = block.sample_over_line(start, end, resolution=400)
    if variable not in sampled.point_data:
        raise SystemExit(f"Variable '{variable}' was not found on the sampled line.")
    axis = sampled.points[:, 1] if line == "axial" else sampled.points[:, 0]
    values = sampled.point_data[variable]
    return axis_name, axis, values


def save_line_csv(path, axis_name, axis, variable, values):
    with path.open("w") as out:
        out.write(f"{axis_name},{variable}\n")
        for x, value in zip(axis, values):
            out.write(f"{x:.16e},{value:.16e}\n")


def save_line_plot(path, axis_name, axis, variable, values):
    try:
        import matplotlib.pyplot as plt
    except ImportError as exc:
        raise SystemExit(
            "Plotting needs matplotlib. Install it with:\n"
            "  python3 -m pip install matplotlib"
        ) from exc

    fig, ax = plt.subplots(figsize=(7, 4), constrained_layout=True)
    ax.plot(axis, values, lw=1.8)
    ax.set_xlabel(axis_name)
    ax.set_ylabel(variable)
    ax.grid(True, alpha=0.25)
    fig.savefig(path, dpi=180)
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--file",
        default="pvd_ar_baseline_60cm_exodus.e",
        help="Exodus result file to read.",
    )
    parser.add_argument("--list", action="store_true", help="List variables and timesteps.")
    parser.add_argument("--variable", default="em_density", help="Variable to extract.")
    parser.add_argument("--time-index", type=int, default=-1, help="Time index to read; -1 is final.")
    parser.add_argument(
        "--line",
        choices=("axial", "radial"),
        default="axial",
        help="Line cut direction. axial uses chamber axis y; radial uses x at mid-gap.",
    )
    args = parser.parse_args()

    filename = Path(args.file)
    if not filename.exists():
        raise SystemExit(f"Could not find result file: {filename}")

    if args.list:
        list_contents(filename)
        return

    pv, reader, dataset = read_mesh(filename, args.time_index)
    block = active_block(dataset)
    axis_name, axis, values = sample_line(block, args.variable, args.line)

    stem = f"{filename.stem}_{args.variable}_{args.line}_t{args.time_index}"
    csv_path = Path(f"{stem}.csv")
    png_path = Path(f"{stem}.png")
    save_line_csv(csv_path, axis_name, axis, args.variable, values)
    save_line_plot(png_path, axis_name, axis, args.variable, values)
    print(f"wrote {csv_path}")
    print(f"wrote {png_path}")


if __name__ == "__main__":
    main()
