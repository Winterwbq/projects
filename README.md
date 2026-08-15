# Cartesian X-Z Copper PVD Chamber Simulation

This directory contains a two-dimensional Cartesian X-Z copper physical vapor
deposition (PVD) plasma model. The plasma is solved with a modified version of
[Zapdos](https://shannon-lab.github.io/zapdos/), which is built on the
[MOOSE Framework](https://mooseframework.inl.gov/). The same repository contains
the Python programs and tabulated data used to generate the simulation inputs
and postprocess the results.

This guide provides a complete installation and run procedure for **Ubuntu
22.04 or 24.04 on an x86_64 Linux computer**. Commands are written for Bash.

## Model summary

- Cartesian domain: `X = 0-25 cm`, `Z = 0-30 cm`
- Powered copper target: `X = 19-22 cm`, `Z = 30 cm`
- Wafer: `X = 7-17 cm`, `Z = 0 cm`
- Copper-only plasma chemistry
- Magnetic electron transport
- Target-ion-flux-driven secondary-electron source
- Results reported per unit depth in the unmodeled Cartesian direction

Three magnetic configurations are supported:

| Case | Description | Input-table directory |
|---|---|---|
| `source_only` | Magnetron source field without the four external coils | `runs/zapdos_cartesian_xz_25x30_source_only/moose_tables` |
| `four_coil` | Magnetron source field plus the standard four-coil configuration | `runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables` |
| `four_coil_img3092` | Magnetron source field plus the IMG_3092 four-coil configuration | `runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables` |

The neutral copper table is common to all three cases. Each case's magnetic
field and secondary-electron-emission (SEE) table are a validated pair. Do not
combine the magnetic field from one case with the SEE table from another case.

## Hardware and disk requirements

Recommended minimums are:

- Ubuntu 22.04 or 24.04, x86_64
- 4 CPU cores
- 16 GB RAM; use fewer build or MPI processes on a smaller machine
- At least 40 GB free disk space for the repository, MOOSE environment, build,
  and new simulation results
- Internet access during installation

The repository contains the MOOSE, Crane, and Squirrel source trees needed by
this modified Zapdos build. A separate MOOSE source clone is therefore not
required. The repository also uses Git LFS for three optional reference Exodus
files totaling approximately 2.3 GB. The instructions below skip those files
because they are not needed to build or run a new simulation.

## 1. Install Linux prerequisites

Open a terminal and install the basic tools:

```bash
sudo apt update
sudo apt install -y build-essential ca-certificates curl git git-lfs
git lfs install
```

Do not use `sudo` with Conda commands later in this guide.

## 2. Download this repository

Choose a working location and clone the existing monorepo. The
`GIT_LFS_SKIP_SMUDGE=1` prefix prevents the optional 2.3 GB reference results
from being downloaded during checkout.

```bash
mkdir -p "$HOME/pvd-work"
cd "$HOME/pvd-work"
GIT_LFS_SKIP_SMUDGE=1 git clone https://github.com/Winterwbq/projects.git
cd projects
git lfs install --local
export PVD_REPO="$PWD"
```

For every new terminal, restore the repository variable with:

```bash
export PVD_REPO="$HOME/pvd-work/projects"
```

The important directories are:

```text
projects/
├── zapdos/                   Modified Zapdos application and dependencies
│   ├── include/              Zapdos and custom PVD C++ headers
│   ├── src/                  Zapdos and custom PVD C++ implementations
│   ├── moose/                Compatible MOOSE source tree
│   ├── crane/                Chemical-reaction-network dependency
│   └── squirrel/             Transport/property dependency
└── cu_pvd_hybrid_2d/
    ├── README.md             This guide
    ├── scripts/              Input generators and result plotters
    ├── rate_coefficients_cu/ Cu reaction and electron-transport tables
    ├── runs/                 Neutral, magnetic-field, and SEE input tables
    └── zapdos_templates/     Active Zapdos input and generated results
```

The dependency sources are included as ordinary directories in this monorepo.
Do **not** run `git submodule update` for this checkout.

## 3. Install the compatible MOOSE development environment

MOOSE recommends its Conda development packages for building MOOSE-based
applications. Install Miniforge into your home directory:

```bash
cd "$HOME/pvd-work"
curl -L -O https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh
bash Miniforge3-Linux-x86_64.sh -b -p "$HOME/miniforge"
source "$HOME/miniforge/etc/profile.d/conda.sh"
conda init bash
```

Add the INL package channel and create an environment compatible with the
bundled MOOSE snapshot:

```bash
conda config --add channels https://conda.software.inl.gov/public
conda create -n moose-pvd -y moose-dev=2026.05.08=mpich
conda activate moose-pvd
```

The version is intentionally pinned. The bundled MOOSE tree identifies its
matching development stack as `moose-dev=2026.05.08=mpich`. Do not replace it
with the newest package unless the bundled MOOSE and Zapdos sources are also
updated and retested.

Install the Python packages used for input generation and postprocessing:

```bash
conda install -n moose-pvd -y numpy scipy matplotlib
```

Activate this environment whenever a new terminal is opened:

```bash
source "$HOME/miniforge/etc/profile.d/conda.sh"
conda activate moose-pvd
export PVD_REPO="$HOME/pvd-work/projects"
```

For current general installation information, consult the official
[MOOSE Conda environment guide](https://mooseframework.inl.gov/getting_started/installation/conda.html).
This project uses the pinned package above for reproducibility.

## 4. Build and test the modified Zapdos application

The Zapdos `Makefile` automatically uses the bundled `zapdos/moose`,
`zapdos/crane`, and `zapdos/squirrel` sources.

```bash
cd "$PVD_REPO/zapdos"
make -j4
```

After a successful build, confirm that the executable exists:

```bash
test -x zapdos-opt
./zapdos-opt --version
```

Run the Zapdos regression tests:

```bash
./run_tests -j4
```

Some tests may be skipped because of platform or resource constraints. Skips
are normal; failures are not. If compilation is killed because the machine runs
out of memory, rebuild with fewer concurrent jobs, for example `make -j2`.

There is no need to build another standalone copy of MOOSE: building Zapdos
compiles the required bundled MOOSE framework and electromagnetic module.

## 5. Prepare the simulation inputs

Change to the PVD project root. All subsequent simulation commands must be run
from this directory because paths in the Zapdos input are relative to
`cu_pvd_hybrid_2d/zapdos_templates`.

```bash
cd "$PVD_REPO/cu_pvd_hybrid_2d"
```

The repository contains ready-generated tables, but regenerating them ensures
that they match the checked-out generator code and records metadata for the
current machine. Run the generators in this order:

```bash
python3 scripts/generate_cartesian_xz_neutral_cu.py
python3 scripts/generate_cartesian_xz_bfields.py --case all
python3 scripts/generate_cartesian_xz_see_maps.py --case all
```

The generated inputs are:

```text
runs/zapdos_cartesian_xz_25x30_neutral/moose_tables/
├── n_Cu_m3.tbl
└── neutral_metadata.json

runs/zapdos_cartesian_xz_25x30_<case>/moose_tables/
├── Bx_T.tbl
├── By_T.tbl
├── see_spatial_weight_m-1.tbl
├── bfield_metadata.json
└── generation_metadata.json
```

Plot all three input sets as a pre-run quality check:

```bash
export MPLBACKEND=Agg
for case in source_only four_coil four_coil_img3092; do
  python3 scripts/plot_cartesian_xz_input_maps.py --case "$case"
done
```

This writes:

```text
post/cartesian_xz_source_only_input_maps.png
post/cartesian_xz_four_coil_input_maps.png
post/cartesian_xz_four_coil_img3092_input_maps.png
```

Inspect the figures and confirm that the neutral map fills the chamber, each
magnetic field is finite, and each SEE map remains inside the chamber. The JSON
metadata records the settings and checksums used to produce the tables.

## 6. Check all three Zapdos cases

The common simulation input is:

```text
zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i
```

Create output directories and define the executable path:

```bash
mkdir -p logs post zapdos_templates/Outputs
export ZAPDOS_EXEC="$PVD_REPO/zapdos/zapdos-opt"
test -x "$ZAPDOS_EXEC"
```

Check each case without solving it:

```bash
for case in source_only four_coil four_coil_img3092; do
  "$ZAPDOS_EXEC" --check-input \
    -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
    "bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_${case}/moose_tables" \
    "cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_${case}/moose_tables/see_spatial_weight_m-1.tbl" \
    "Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_${case}"
done
```

Every check must end with:

```text
Syntax OK
```

These commands have been checked against the current repository input. The
explicit overrides ensure that each magnetic field is used with its paired SEE
map and give each case a separate output filename.

## 7. Run the simulations

### Quick start: IMG_3092 four-coil case

Run the IMG_3092 case first if only one result is needed. A four-process MPI run
is recommended on a workstation with at least four cores:

```bash
set -o pipefail
mpiexec -n 4 "$ZAPDOS_EXEC" \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  "bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables" \
  "cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables/see_spatial_weight_m-1.tbl" \
  "Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_four_coil_img3092" \
  --color off 2>&1 | tee logs/cartesian_xz_four_coil_img3092.log
```

For a serial run, remove `mpiexec -n 4` from the beginning of the command.

### Run all three cases

The following loop runs the cases sequentially, using four MPI processes for
each case:

```bash
set -o pipefail
for case in source_only four_coil four_coil_img3092; do
  mpiexec -n 4 "$ZAPDOS_EXEC" \
    -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
    "bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_${case}/moose_tables" \
    "cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_${case}/moose_tables/see_spatial_weight_m-1.tbl" \
    "Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_${case}" \
    --color off 2>&1 | tee "logs/cartesian_xz_${case}.log"
done
```

Do not run the cases concurrently unless the machine has enough memory for all
three jobs. The input integrates to `1.0e-3 s`, uses adaptive time stepping, and
can require substantial compute time.

Expected output names are:

```text
zapdos_templates/Outputs/cartesian_xz_source_only.e
zapdos_templates/Outputs/cartesian_xz_four_coil.e
zapdos_templates/Outputs/cartesian_xz_four_coil_img3092.e
```

MOOSE may append a numerical suffix when an output name already exists. Check
the actual filenames after each run:

```bash
find zapdos_templates/Outputs -maxdepth 1 -name 'cartesian_xz_*.e' -ls
```

The terminal log contains nonlinear convergence information and scalar
postprocessor values. Keep it with the corresponding Exodus file when
archiving a result.

## 8. Postprocess one case

These examples assume the unsuffixed filename shown above. Replace the path if
MOOSE created a suffixed file.

Plot plasma density, potential, temperature, and electric-field diagnostics:

```bash
export MPLBACKEND=Agg
python3 scripts/plot_cartesian_xz_results.py diagnostics \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil_img3092.e \
  --output post/cartesian_xz_four_coil_img3092_diagnostics.png \
  --density-style chamber
```

Plot Cu+ flux magnitude, direction, wafer-directed flux, and profiles at
`Z = 1, 5, and 10 cm`:

```bash
python3 scripts/plot_cartesian_xz_results.py ion-flux \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil_img3092.e \
  --output post/cartesian_xz_four_coil_img3092_cu_ion_flux.png \
  --z-cm 1 5 10 \
  --wafer-x-cm 7 17
```

To select the closest saved timestep to a physical time, add, for example,
`--time 1.0e-4`. Without `--time` or `--timestep`, the diagnostics command uses
the last saved timestep.

## 9. Postprocess and compare all three cases

Generate diagnostics and ion-flux plots for every case:

```bash
export MPLBACKEND=Agg
for case in source_only four_coil four_coil_img3092; do
  python3 scripts/plot_cartesian_xz_results.py diagnostics \
    --exodus "zapdos_templates/Outputs/cartesian_xz_${case}.e" \
    --output "post/cartesian_xz_${case}_diagnostics.png" \
    --density-style chamber

  python3 scripts/plot_cartesian_xz_results.py ion-flux \
    --exodus "zapdos_templates/Outputs/cartesian_xz_${case}.e" \
    --output "post/cartesian_xz_${case}_cu_ion_flux.png" \
    --z-cm 1 5 10 \
    --wafer-x-cm 7 17
done
```

Compare the wafer-directed Cu+ flux profiles:

```bash
python3 scripts/plot_cartesian_xz_results.py compare-ion-flux \
  --case source_only=zapdos_templates/Outputs/cartesian_xz_source_only.e \
  --case four_coil=zapdos_templates/Outputs/cartesian_xz_four_coil.e \
  --case four_coil_img3092=zapdos_templates/Outputs/cartesian_xz_four_coil_img3092.e \
  --output post/cartesian_xz_cu_ion_flux_comparison.png \
  --csv post/cartesian_xz_cu_ion_flux_comparison.csv \
  --z-cm 1 5 10
```

PNG files can be viewed with any image viewer. Exodus files can also be opened
in [ParaView](https://www.paraview.org/).

## Optional: download the bundled reference results

The reference Exodus files are not required for a new run. If needed for
comparison, download them from Git LFS after cloning:

```bash
cd "$PVD_REPO"
git lfs pull --include='cu_pvd_hybrid_2d/zapdos_templates/Outputs/*.e'
```

This downloads approximately 2.3 GB. The reference filenames predate the
shorter output names used in this README; always pass the desired file
explicitly to the plotting program with `--exodus`.

## Reproducibility checklist

For each reported result, record or archive:

- The Git commit from `git -C "$PVD_REPO" rev-parse HEAD`
- The Conda environment from `conda list --explicit`
- The active `.i` input file
- `neutral_metadata.json`
- The selected case's `bfield_metadata.json` and `generation_metadata.json`
- The selected case's `Bx_T.tbl`, `By_T.tbl`, and
  `see_spatial_weight_m-1.tbl`
- `rate_coefficients_cu/reaction1.txt` and
  `rate_coefficients_cu/electron_moments.txt`
- The terminal log and Exodus result
- The exact command-line overrides used for the run

Generated Exodus files, checkpoints, `.jitcache` directories, build products,
and plots are not source inputs and generally should not be committed to Git.

## Troubleshooting

### `conda: command not found`

```bash
source "$HOME/miniforge/etc/profile.d/conda.sh"
conda activate moose-pvd
```

### Conda cannot find the pinned MOOSE package

```bash
conda config --show channels
conda search moose-dev=2026.05.08 --channel https://conda.software.inl.gov/public
```

The expected Linux package build is `moose-dev-2026.05.08-mpich`.

### Zapdos compilation is killed or the machine becomes unresponsive

```bash
cd "$PVD_REPO/zapdos"
make -j2
```

### `zapdos-opt` is missing

```bash
conda activate moose-pvd
cd "$PVD_REPO/zapdos"
make -j2
test -x zapdos-opt
```

### An input table or rate-coefficient file is not found

Run Zapdos from the PVD project root, not from `zapdos_templates`:

```bash
cd "$PVD_REPO/cu_pvd_hybrid_2d"
```

Then regenerate the inputs and repeat `--check-input`.

### A Git LFS file is only a small text pointer

This is expected after cloning with `GIT_LFS_SKIP_SMUDGE=1`. The tracked
reference results are optional. Run the simulation to create new results, or
use the optional `git lfs pull` command above.

### MPI cannot start

```bash
conda activate moose-pvd
which mpiexec
mpiexec --version
```

Test the same Zapdos command without `mpiexec -n 4`. If the serial command
works, investigate the host's MPI or network configuration before retrying the
parallel run.

### A run stops before `1.0e-3 s`

Inspect the corresponding file under `logs/` for the first nonlinear solver or
time-step failure. Do not treat a partially written Exodus file as a completed
case. Record the last accepted time, command, and input metadata before changing
solver or physical parameters.

## External documentation

- [MOOSE installation](https://mooseframework.inl.gov/getting_started/installation/)
- [MOOSE Conda environment](https://mooseframework.inl.gov/getting_started/installation/conda.html)
- [Zapdos installation](https://shannon-lab.github.io/zapdos/getting_started/installation.html)
- [Using Zapdos](https://shannon-lab.github.io/zapdos/getting_started/using_zapdos.html)
- [ParaView](https://www.paraview.org/)

