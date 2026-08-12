# Cartesian X–Z B-Field, SEE, Zapdos, and Plotting Workflow

This document is the end-to-end workflow for the full Cartesian chamber:

- Domain: `X = 0–25 cm`, `Z = 0–30 cm`
- Powered target: `X = 19–22 cm`, `Z = 30 cm`
- Wafer: `X = 7–17 cm`, `Z = 0 cm`
- Left and right walls use the same wall-loss model
- The domain is full, nonsymmetric, and is not doubled

The three supported magnetic cases are:

1. `source_only`
2. `four_coil`
3. `four_coil_img3092`

The B-field and SEE table directories must always be selected as a pair from
the same case.

## Quick start

Start in the project root:

```bash
cd /Users/bingqingwang/projects/cu_pvd_hybrid_2d
```

For a source-only run:

```bash
# 1. Generate the paired B-field and SEE tables.
/opt/miniconda3/bin/python3 \
  scripts/generate_cartesian_xz_source_bfield_see.py \
  --see-magnitude-scale 1.0

# 2. Plot the generated B-field and SEE map.
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_source_bfield.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  --output post/cartesian_xz_source_only_bfield.png

MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_see_map.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  --output post/cartesian_xz_source_only_see_map.png

# 3. Check the Zapdos input without solving.
/Users/bingqingwang/projects/zapdos/zapdos-opt \
  --check-input \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables/see_spatial_weight_m-1.tbl \
  Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_source_only

# 4. Run Zapdos after the check reports "Syntax OK".
/Users/bingqingwang/projects/zapdos/zapdos-opt \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables/see_spatial_weight_m-1.tbl \
  Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_source_only
```

After the run, replace the Exodus filename below if MOOSE added a numerical
suffix such as `_1.e`:

```bash
# 5. Plot plasma fields and density.
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_zapdos_last_timestep_diagnostics.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_source_only.e \
  --output post/cartesian_xz_source_only_diagnostics.png \
  --xlabel 'X [cm]' \
  --ylabel 'Z [cm]' \
  --density-style chamber

# 6. Plot Cu+ flux magnitude, direction, signed wafer-directed flux, and profiles.
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_ion_flux.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_source_only.e \
  --output post/cartesian_xz_source_only_cu_ion_flux.png \
  --z-cm 1 5 10 \
  --wafer-x-cm 7 17
```

## 1. Required files

### Map generators

| Purpose | File |
|---|---|
| Source-only B-field and SEE map | `scripts/generate_cartesian_xz_source_bfield_see.py` |
| Standard and IMG_3092 four-coil B-fields and SEE maps | `scripts/generate_cartesian_xz_four_coil_bfield_see.py` |

Each generator writes a complete paired set:

```text
Bx_T.tbl
By_T.tbl
see_spatial_weight_m-1.tbl
generation_metadata.json
```

Do not combine `Bx_T.tbl` from one case with an SEE table from another case.

### Map plotters

| Purpose | File |
|---|---|
| Single `|B|` panel with streamlines | `scripts/plot_cartesian_xz_source_bfield.py` |
| SEE spatial weight with B streamlines | `scripts/plot_cartesian_xz_see_map.py` |

### Simulation input

```text
zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i
```

### Result plotters

| Purpose | File |
|---|---|
| Plasma density, potential, temperature, and field diagnostics | `scripts/plot_zapdos_last_timestep_diagnostics.py` |
| Cartesian Cu+ flux maps and wafer-span profiles | `scripts/plot_cartesian_xz_ion_flux.py` |

## 2. Choose and generate a magnetic case

### Case A: source-only

The source-only generator builds both the translated/compressed two-pole
magnetron B-field and its mostly vertical SEE map:

```bash
/opt/miniconda3/bin/python3 \
  scripts/generate_cartesian_xz_source_bfield_see.py
```

Output directory:

```text
runs/zapdos_cartesian_xz_25x30_source_only/moose_tables/
```

The source-only magnitude can be supplied on the command line without editing
the Python file:

```bash
/opt/miniconda3/bin/python3 \
  scripts/generate_cartesian_xz_source_bfield_see.py \
  --see-magnitude-scale 1.0
```

Other command-line transport controls include:

```text
--local-fraction
--local-attenuation-length
--guided-attenuation-length
--spread-angle-degrees
--transition-field
```

### Case B: standard four-coil

```bash
/opt/miniconda3/bin/python3 \
  scripts/generate_cartesian_xz_four_coil_bfield_see.py \
  --case four_coil
```

Output directory:

```text
runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables/
```

### Case C: IMG_3092 four-coil

```bash
/opt/miniconda3/bin/python3 \
  scripts/generate_cartesian_xz_four_coil_bfield_see.py \
  --case four_coil_img3092
```

Output directory:

```text
runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables/
```

Generate both four-coil cases with:

```bash
/opt/miniconda3/bin/python3 \
  scripts/generate_cartesian_xz_four_coil_bfield_see.py
```

The four-coil case settings are stored in `CASE_CONFIGS` near the beginning of
`scripts/generate_cartesian_xz_four_coil_bfield_see.py`. This includes:

```text
see_magnitude_scale
local_fraction
guided_attenuation_length_m
spread_angle_degrees
bfield_guidance_fraction
```

After changing `CASE_CONFIGS`, rerun the generator. Editing the Python file does
not alter an existing `.tbl` file automatically.

## 3. Check what was actually generated

`generation_metadata.json` records the settings used for the existing tables.
It is more reliable than inspecting the current Python defaults because the
Python file may have been edited after the table was generated.

For example:

```bash
/opt/miniconda3/bin/python3 -m json.tool \
  runs/zapdos_cartesian_xz_25x30_source_only/moose_tables/generation_metadata.json

/opt/miniconda3/bin/python3 -m json.tool \
  runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables/generation_metadata.json

/opt/miniconda3/bin/python3 -m json.tool \
  runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables/generation_metadata.json
```

Check at least:

```text
see_magnitude_scale
see_area_integral_m
see_unscaled_area_integral_m
transport_settings
see_transport_direction
bfield_magnitude_range_t
termination_counts
```

## 4. SEE magnitude and plasma-growth warning

`see_magnitude_scale` changes the integrated physical source, not only the
brightness of the plot.

For this Cartesian model,

```text
target_see_rate = gamma_secondary × target_ion_flux_average × target_length

see_secondary_rate_integral
    = gamma_secondary × see_filtered_target_ion_flux
      × integral(see_spatial_weight dA)
```

When the filtered and instantaneous fluxes are close,

```text
SEE source/target ratio
    approximately equals
    see_area_integral_m / effective_target_length_m
```

The intended target length is `0.03 m`. On the present mesh, the postprocessed
effective target length was measured as approximately `0.03125 m`.

A conservative first run should use an SEE-map integral close to the target
length. A source-only scale near `1.0` gives an integral near `0.03 m`. Values
such as `2`, `3`, `5`, or `10` multiply the total pair-production feedback and
can cause rapid plasma growth.

If a high source peak is desired without increasing the global production
rate, narrow or reshape the profile and renormalize its integral instead of
uniformly increasing `see_magnitude_scale`.

## 5. Plot and inspect the generated B-field

The same plotter can read any of the three table directories.

Source-only:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_source_bfield.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  --output post/cartesian_xz_source_only_bfield.png
```

Standard four-coil:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_source_bfield.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables \
  --output post/cartesian_xz_four_coil_bfield.png
```

IMG_3092:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_source_bfield.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables \
  --output post/cartesian_xz_four_coil_img3092_bfield.png
```

Inspect the following before running Zapdos:

- The domain is exactly `X=0–25 cm`, `Z=0–30 cm`.
- The powered target marker is at `X=19–22 cm`.
- The wafer marker is at `X=7–17 cm`.
- The field is finite everywhere.
- The B-field magnitude and streamlines match the intended case.

## 6. Plot and inspect the generated SEE map

Source-only:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_see_map.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  --output post/cartesian_xz_source_only_see_map.png
```

Standard four-coil:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_see_map.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables \
  --output post/cartesian_xz_four_coil_see_map.png
```

IMG_3092:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_see_map.py \
  --table-dir runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables \
  --output post/cartesian_xz_four_coil_img3092_see_map.png
```

Inspect:

- The two launch lobes begin beneath the powered target.
- The source-only plume is primarily vertical and downward.
- The four-coil tails follow their respective B-fields.
- IMG_3092 remains concentrated enough for the intended comparison.
- The map does not spread outside the chamber.
- The area integral is consistent with the desired physical source strength.

By default, each plot chooses its own maximum. When comparing absolute
magnitudes, supply the same `--absolute-vmax` to every call.

## 7. Select the paired case in Zapdos

The input has two path parameters:

```text
bfield_table_dir
cartesian_see_map_file
```

They can be changed in the input file, but command-line overrides are safer for
case comparisons because they do not modify the baseline input.

### Source-only overrides

```text
bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables
cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables/see_spatial_weight_m-1.tbl
```

### Standard four-coil overrides

```text
bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables
cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables/see_spatial_weight_m-1.tbl
```

### IMG_3092 overrides

```text
bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables
cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables/see_spatial_weight_m-1.tbl
```

The leading `../` is required because MOOSE resolves table paths relative to
the input-file directory, `zapdos_templates/`.

## 8. Check the input before running

Always run `--check-input` after generating tables or changing cases.

Source-only:

```bash
/Users/bingqingwang/projects/zapdos/zapdos-opt \
  --check-input \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables \
  cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_source_only/moose_tables/see_spatial_weight_m-1.tbl \
  Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_source_only
```

Standard four-coil:

```bash
/Users/bingqingwang/projects/zapdos/zapdos-opt \
  --check-input \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables \
  cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables/see_spatial_weight_m-1.tbl \
  Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_four_coil
```

IMG_3092:

```bash
/Users/bingqingwang/projects/zapdos/zapdos-opt \
  --check-input \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables \
  cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_four_coil_img3092/moose_tables/see_spatial_weight_m-1.tbl \
  Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_four_coil_img3092
```

Proceed only after Zapdos prints:

```text
Syntax OK
```

## 9. Run Zapdos

Remove `--check-input` from the corresponding command.

Example, standard four-coil:

```bash
/Users/bingqingwang/projects/zapdos/zapdos-opt \
  -i zapdos_templates/cu_pvd_hybrid_see_30cm_b_guided_original_density.i \
  bfield_table_dir=../runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables \
  cartesian_see_map_file=../runs/zapdos_cartesian_xz_25x30_four_coil/moose_tables/see_spatial_weight_m-1.tbl \
  Outputs/file_base=zapdos_templates/Outputs/cartesian_xz_four_coil
```

Use a distinct `Outputs/file_base` for every case and parameter study. This
prevents one run from overwriting or being confused with another run.

The main Exodus result will be written below:

```text
zapdos_templates/Outputs/
```

If a file already exists, MOOSE may create a suffix such as `_1.e`. Use the
actual generated filename in plotting commands.

Tables are loaded at startup. Regenerating a table while Zapdos is already
running does not update that simulation. Stop and restart Zapdos after every
table change.

## 10. Monitor plasma growth and SEE feedback

The terminal and Exodus global postprocessors include:

```text
electron_density_integral
electron_density_max
cu_ion_density_integral
cu_ion_density_max
target_ion_flux_average
target_see_rate
see_secondary_rate_integral
see_ionization_rate_integral
see_raw_discharge_power
see_power_cap_factor
cu_ion_to_neutral_max_ratio
electron_to_neutral_max_ratio
```

Important diagnostic ratios:

```text
SEE source/target ratio = see_secondary_rate_integral / target_see_rate

relative excess
    = abs(see_secondary_rate_integral - target_see_rate)
      / abs(target_see_rate)
```

Interpretation:

- A source/target ratio near `1` is rate-conservative.
- A ratio of `1.5` means 50% more integrated SEE source than instantaneous
  target SEE production.
- A ratio of `2` means twice the target production and can create strong
  positive feedback.
- `see_power_cap_factor = 1` means the power cap is not reducing the source.
- Rapidly increasing charged-to-neutral ratios indicate that the fixed-neutral
  approximation is approaching its validity limit.

Do not use `see_feedback_response_time` to correct a normalization error. It
changes the lag, not the steady integrated source strength.

## 11. Plot plasma results

### General last-timestep diagnostics

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_zapdos_last_timestep_diagnostics.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil.e \
  --output post/cartesian_xz_four_coil_diagnostics.png \
  --xlabel 'X [cm]' \
  --ylabel 'Z [cm]'
```

Use chamber-focused density scaling:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_zapdos_last_timestep_diagnostics.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil.e \
  --output post/cartesian_xz_four_coil_diagnostics_chamber.png \
  --xlabel 'X [cm]' \
  --ylabel 'Z [cm]' \
  --density-style chamber \
  --density-percentiles 2 98
```

Plot density growth relative to the first populated saved timestep:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_zapdos_last_timestep_diagnostics.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil.e \
  --output post/cartesian_xz_four_coil_density_growth.png \
  --xlabel 'X [cm]' \
  --ylabel 'Z [cm]' \
  --density-growth
```

For a fixed physical time, use seconds:

```text
--time 1.0e-4
```

This selects the saved timestep nearest `100 microseconds`.

### Cu+ flux maps and wafer profiles

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_ion_flux.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil.e \
  --output post/cartesian_xz_four_coil_cu_ion_flux.png \
  --z-cm 1 5 10 \
  --wafer-x-cm 7 17
```

The output contains:

1. Cu+ flux magnitude with direction arrows
2. Signed wafer-directed flux, `-Gamma_z`, positive downward
3. Flux-magnitude profiles over the wafer at the requested Z planes

Compare cases at the same physical time:

```bash
MPLCONFIGDIR=/private/tmp/codex-mpl \
/opt/miniconda3/bin/python3 \
  scripts/plot_cartesian_xz_ion_flux.py \
  --exodus zapdos_templates/Outputs/cartesian_xz_four_coil.e \
  --output post/cartesian_xz_four_coil_cu_ion_flux_100us.png \
  --z-cm 1 5 10 \
  --wafer-x-cm 7 17 \
  --time 1.0e-4
```

Do not compare each case at its own last timestep when the plasma is growing;
different final times can reverse the apparent ordering of the flux profiles.

## 12. Recommended case-by-case sequence

For every case, follow this order:

1. Choose the desired SEE magnitude and transport settings.
2. Run the appropriate generator.
3. Inspect `generation_metadata.json`.
4. Plot and inspect `|B|` with streamlines.
5. Plot and inspect the SEE map.
6. Confirm the SEE area integral is physically intended.
7. Select paired B and SEE paths using command-line overrides.
8. Run Zapdos with `--check-input`.
9. Run the transient simulation with a unique output base.
10. Monitor density, source/target ratio, power, and neutral inventory.
11. Plot density and flux at fixed physical times.
12. Compare the three cases only after using consistent times and plotting
    scales.

## 13. Common problems

### The Python scale changed, but the map did not

Cause: the generator was edited but not rerun.

Action: rerun the generator and inspect `generation_metadata.json`.

### Zapdos still uses an old table

Cause: the table is loaded only at simulation startup.

Action: stop and restart Zapdos after regeneration.

### B-field and SEE behavior do not match

Cause: paths from different cases were paired.

Action: use `bfield_table_dir` and `cartesian_see_map_file` from the same case.

### Plasma grows much faster than expected

Check:

```text
see_secondary_rate_integral / target_see_rate
see_power_cap_factor
see_raw_discharge_power
cu_ion_to_neutral_max_ratio
electron_to_neutral_max_ratio
```

If the source/target ratio is much larger than one, lower the map integral by
reducing `see_magnitude_scale` and regenerate the table.

### Flux profiles change ordering between plots

Cause: plots used different physical times during a transient.

Action: pass the same `--time` value to all plotting commands.

### Matplotlib reports an unwritable cache directory

Prefix plotting commands with:

```text
MPLCONFIGDIR=/private/tmp/codex-mpl
```

## 14. Archive location

Legacy Exodus results were moved, not deleted. They are stored at:

```text
archive/legacy_2026-08-07/
```

Restoration details are recorded in:

```text
archive/legacy_2026-08-07/ARCHIVE_MANIFEST.txt
```

Do not move the active R-Z reference table directories used by the four-coil
generator:

```text
runs/zapdos_hpem_rz_30cm_reference_four_coil/moose_tables/
runs/zapdos_hpem_rz_30cm_reference_four_coil_img3092/moose_tables/
```
